/* author: Piet Glas
 */
#include "ssmodel.h"
#include "formula/tokenizer.h"
#include "formula/expression.h"
#include <math.h>
#include <QDebug>
#include <QDebug>
#include <QBrush>
#include <QFont>
#include <QList>
#include <QString>
#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <QFile>
#include <QDataStream>

SSModel::SSModel(int rows, int cols, QObject * parent) : 
				QAbstractTableModel(parent), rows_{rows}, cols_{cols} {}

SSModel::~SSModel() {}

int SSModel::rowCount(const QModelIndex & /*parent*/) const {
	return rows_;
}

int SSModel::columnCount(const QModelIndex & /*parent*/) const {
	return cols_; 
}

QVariant SSModel::data(const QModelIndex & index, int role) const {
	QPair<int, int> normindex = qMakePair(index.row(), index.column());
	QString strindex = convertIndexToStr(normindex);
	bool index_found = data_.contains(strindex);
	if (role == Qt::DisplayRole && index.isValid() && index_found)
		return data_[strindex].first;
	return QVariant();
}

QVariant SSModel::headerData(int section, Qt::Orientation orientation,
						int role) const {
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		return QString(alph_[section]);
	}
	if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
		return section + 1;
	}
	return QVariant();
}

bool SSModel::setData(const QModelIndex & index, 
						const QVariant & value, int role) {
	if (role == Qt::EditRole && index.isValid()) {
		bool is_double;
		double dvalue = value.toDouble(&is_double);
		// save value from editor to model
		QPair<int, int> normindex = qMakePair(index.row(), index.column());
		QString strindex = convertIndexToStr(normindex);
		QVector<QString> empty_formula;
		QPair<QVariant, QVector<QString>> val;
		if (is_double) 
			val = qMakePair(dvalue, empty_formula);
		else
			val = qMakePair(value, empty_formula);
		data_.insert(strindex, val);
		// update dependent data
		updateDependentValues(strindex);

		return true;
	}	
	return false;
}

void SSModel::clearData() {
	data_.clear();
}

bool SSModel::getDataFromFile(const QString& file_name) {
	QFile file(file_name);
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream input(&file);
		// set rows_ and cols
		QString rowsstr;
		QString colstr;
		input >> rowsstr >> colstr;
		rows_ = rowsstr.toInt();
		cols_ = colstr.toInt();
		// set strings for input
		QString index; 
		QString separator;
		QString formula;
		QString data;
		while (!input.atEnd()) {
			bool has_formula = true;
			input >> index >> separator;
			if (separator == ":")
				has_formula = false;
			// set the data
			if (has_formula) {
				formula = input.readLine();
				QString equation = index + " = " + formula;
				SSModel::setFormula(equation);
			}
			else {
				input >> data;
				QPair<QVariant,QVector<QString>> single_value;
				QVector<QString> empty_formula;
				bool ok;
				double val = data.toDouble(&ok);
				if (ok)
					single_value = qMakePair(val, empty_formula);
				else
					single_value = qMakePair(data, empty_formula);
				data_.insert(index, single_value);
			}
		}
		file.close();
		return true;
	}
	return false;
}

bool SSModel::saveData(const QString & file_name) const {
	QFile file(file_name);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QTextStream output(&file);
		output << QString::number(rows_) << " " << QString::number(cols_) << "\n";
		auto it = data_.begin();
		while (it != data_.end()) {
			if (it.value().first != "") {
				if (it.value().second.empty()) {	// if no formula
					output << it.key() << " : " << 
					it.value().first.toString() << "\n";
				}
				else {
					// convert formula to string
					QString rhs;
					for (auto token : it.value().second)
						rhs += token;	
					output << it.key() << " = " << 
					rhs << "\n";
				}
			}
			++it;
		}
		file.close();
		return true;
	}
	return false;
}

bool SSModel::setFormula(const QString & formula) {
	Tokenizer tokenizer;
	if (tokenizer.tokenize(formula)) {	// turn string into vector of tokens
		QSet<QString> indices = tokenizer.validate();
		if (!indices.empty()) {	// check for correct syntax
			// update existing formula or add a new one
			QString key = tokenizer.tokenized()[0];

			// check for circularity
			if (!checkCircularity(key, indices)) {
				QVector<QString> tokens = tokenizer.tokenized().mid(2, -1);

				// set data displayed
				auto formula_ptr = std::make_shared<Expression>(tokens);
				double val = SSModel::calculateFormula(formula_ptr);
				QPair<QVariant, QVector<QString>> value = qMakePair(val, tokens);
				data_.insert(key, value);

				// update depending cells
				for (auto index : indices) 
					has_effect_on_[index].insert(key);
				updateDependentValues(key);
				
				// update statusbar
				emit sendFormula(formula);
				
				return true;
			}
		}
	}
	return false;
}

double SSModel::calculateFormula(std::shared_ptr<Expression> formula) {
	if (formula->lhs() == nullptr) {
		bool ok;
		double try_conv = formula->token().toDouble(&ok);
		if (ok)	// in case of a number
			return try_conv;
		else	// in case of an index
			return data_[formula->token()].first.toDouble();
	}
	else {
		if (formula->token() == "+")
			return calculateFormula(formula->lhs()) + 
					calculateFormula(formula->rhs());
		else if (formula->token() == "-")
			return calculateFormula(formula->lhs()) - 
					calculateFormula(formula->rhs());
		else if (formula->token() == "*")
			return calculateFormula(formula->lhs()) * 
					calculateFormula(formula->rhs());
		else if (formula->token() == "/")
			return calculateFormula(formula->lhs()) / 
					calculateFormula(formula->rhs());
		else
			return pow(calculateFormula(formula->lhs()),
						calculateFormula(formula->rhs()));
	}
}

void SSModel::getFormula(const QModelIndex & current) {
	if (current.isValid()) {
		QPair<int, int> normindex = qMakePair(current.row(), current.column());
		QString strindex = convertIndexToStr(normindex);
		if (data_.contains(strindex) && !data_[strindex].second.isEmpty()) {
			QString formula = strindex + " = ";
			for (auto token : data_[strindex].second)
				formula += token;
			emit SSModel::sendFormula(formula);
		}
		else
			emit SSModel::sendFormula("");
	}
}

QPair<int, int> SSModel::convertStrToIndex(const QString & index) const {
	int col = 0;
	while (alph_[col] != index[0]) 
		++col;
	int row = index.mid(1, -1).toInt() - 1;
	return qMakePair(row, col);
}

QString SSModel::convertIndexToStr(const QPair<int, int> & index) const {
	int row = index.first + 1;
	int col = index.second;
	QString dindex;
	dindex += alph_[col];
	dindex += QString::number(row);
	return dindex;
}

bool SSModel::checkCircularity(const QString & lhs, 
					const QSet<QString> & indices_rhs) {
	QMap<QString, QSet<QString>> new_depends = depends_on_;
	new_depends.insert(lhs, indices_rhs);
	if (!SSModel::checkCircularityHelper(lhs, indices_rhs)) {
		depends_on_ = new_depends;
		return false;
	}
	return true;
}

bool SSModel::checkCircularityHelper(const QString & lhs,
	const QSet<QString> & depends_on) {
	// see if current index depends on lhs
	if (depends_on.contains(lhs))
		return true;
	for (auto ind : depends_on) {
		if (SSModel::checkCircularityHelper(lhs, depends_on_[ind]))
			return true;
	}
	return false;
}

void SSModel::updateDependentValues(const QString & index) {
	for (auto ind : has_effect_on_[index]) {
		// calculate the formula 
		QVector<QString> tokens = data_[ind].second;
		auto formula_ptr = std::make_shared<Expression>(tokens);
		double val = SSModel::calculateFormula(formula_ptr);
		data_[ind].first = val;
		// update values depending on the value we just updated
		SSModel::updateDependentValues(ind);
	}
}


Qt::ItemFlags SSModel::flags(const QModelIndex & index) const {
	return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}