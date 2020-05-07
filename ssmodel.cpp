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
		QString empty_formula;
		QPair<QVariant, QString> val;
		if (is_double) 
			val = qMakePair(dvalue, empty_formula);
		else
			val = qMakePair(value, empty_formula);
		data_.insert(strindex, val);

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
		bool has_formula = true;
		while (!input.atEnd()) {
			input >> index >> separator;
			qDebug() << index;
			qDebug() << separator;
			if (separator == ":")
				has_formula = false;
			else
				has_formula = true;
			// set the data
			if (has_formula) {
				formula = input.readLine();
				QString equation = index + " = " + formula;
				SSModel::setFormula(equation);
			}
			else {
				input >> data;
				QPair<QVariant,QString> single_value;
				QString empty_formula;
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
				if (it.value().second == "") {
					output << it.key() << " : " << 
					it.value().first.toString() << "\n";
				}
				else {
					output << it.key() << " = " << 
					it.value().second << "\n";
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
		if (tokenizer.validate()) {	// check for correct syntax
			// update existing formula or add a new one
			QString key = tokenizer.tokenized()[0];
			QVector<QString> tokens = tokenizer.tokenized().mid(2, -1);

			// get formula without lhs
			QString without_lhs;
			int pos = 0;
			while (formula[pos] != '=')
				++pos;
			++pos;
			while (formula[pos] == ' ')
				++pos;
			without_lhs = formula.mid(pos, -1);

			// set data displayed
			auto formula_ptr = std::make_shared<Expression>(tokens);
			double val = SSModel::calculateFormula(formula_ptr);
			QPair<QVariant, QString> value = qMakePair(val, without_lhs);

			data_.insert(key, value);
			
			return true;
		}
		return false;
	}
	return false;
}

double SSModel::calculateFormula(std::shared_ptr<Expression> formula) {
	if (formula->lhs() == nullptr) {
		bool ok;
		double try_conv = formula->token().toDouble(&ok);
		if (ok)
			return try_conv;
		else
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


Qt::ItemFlags SSModel::flags(const QModelIndex & index) const {
	return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}