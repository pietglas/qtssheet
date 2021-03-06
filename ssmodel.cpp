/* author: Piet Glas
 *
 */
#include "ssmodel.h"

#include <math.h>
#include <iostream>
#include <algorithm>	// for std::set_difference, std::sort
#include <vector>
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
	QString strindex = convertIndexToString(index);
	bool index_found = data_.contains(strindex);
	if (role == Qt::DisplayRole && index.isValid() && index_found)
		return data_[strindex].first;
	return QVariant();
}

QVariant SSModel::headerData(int section, Qt::Orientation orientation,
						int role) const {
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		return QString(misc::alphabet[section]);
	}
	if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
		return section + 1;
	}
	return QVariant();
}

bool SSModel::setData(const QModelIndex & index, 
						const QVariant & value, int role) {
	if (role == Qt::EditRole && index.isValid()) {
		// save value from editor to model
		QString strindex = convertIndexToString(index);
		QVector<QString> empty_formula;
		QPair<QVariant, QVector<QString>> val = qMakePair(value, empty_formula);
		// if the cell contained a formula, remove those dependencies
		std::set<QString> no_dependencies;
		updateDependencies(strindex, no_dependencies);

		data_.insert(strindex, val);
		// update dependent data
		updateDependentValues(strindex);
		// report changed index to ssview
		emit dataChanged(index, index);

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
			input >> index >> separator;
			if (separator != ":") {	// by convention, ':' indicates absence formula
				formula = "=" + input.readLine();
				setFormula(formula, index);
			}
			else {
				input >> data;
				QVector<QString> empty_formula;
				QPair<QVariant,QVector<QString>> single_value = 
					qMakePair(data, empty_formula);
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

bool SSModel::setFormula(const QString & formula, const QString& key) {
	QModelIndex index = convertStringToIndex(key);
	// check if a formula was entered. If not, simply modify data
	if (formula[0] != '=') {
		setData(index, formula, Qt::EditRole);
		return true;
	}
	// continue if a formula was entered
	Tokenizer tokenizer(formula);
	if (tokenizer.tokenize()) {	// turn string into vector of tokens
		std::set<QString> indices = tokenizer.validate();

		if (!indices.empty()) {	// check for correct syntax
			// check for circularitar dependencies
			if (!checkCircularity(key, indices)) {
				double val;
				QVector<QString> tokens = tokenizer.tokenized();

				// set data displayed
				if (tokenizer.predefined()) {
					val = calculatePredefinedFormula(tokens);
					// get all indices that the cell depends on
					auto it = indices.begin();
					QString index1 = *it;
					QString index2 = *(++it);
					indices = getIndices(index1, index2);
				}
				else {
					auto formula_ptr = std::make_shared<Expression>(tokens);
					val = calculateFormula(formula_ptr);
				}
				QPair<QVariant, QVector<QString>> value = qMakePair(val, tokens);
				data_.insert(key, value);

				// update depending cells
				updateDependencies(key, indices);
				
				// update statusbar
				emit sendFormula(formula);
				// update ssview
				emit dataChanged(index, index);
				
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

double SSModel::calculatePredefinedFormula(const QVector<QString>& tokens) {
	std::set<QString> indices = getIndices(tokens[2], tokens[4]);
	if (tokens[0] == "average")
		return getAverage(indices);
	else if (tokens[0] == "sum")
		return getSum(indices);
	else if (tokens[0] == "median")
		return getMedian(indices);
	else if (tokens[0] == "min")
		return getMin(indices);
	else if (tokens[0] == "max")
		return getMax(indices);
	else
		return 0;
}

void SSModel::getFormula(const QModelIndex & current) {
	if (current.isValid()) {
		QString strindex = convertIndexToString(current);
		if (data_.contains(strindex) && !data_[strindex].second.isEmpty()) {
			QString formula = " = ";
			for (auto token : data_[strindex].second)
				formula += token;
			emit sendFormula(formula);
		}
		else
			emit sendFormula("");
	}
}

QString SSModel::convertIndexToString(const QModelIndex& index) const {
	int row = index.row() + 1;
	int col = index.column();
	QString dindex;
	dindex += misc::alphabet[col];
	dindex += QString::number(row);
	return dindex;
}

QModelIndex SSModel::convertStringToIndex(const QString& index) const {
	int column = 0;
	while (index[0] != misc::alphabet[column])
		column++;
	int row = index.right(index.length() - 1).toInt() - 1;
	QModelIndex qindex = this->index(row, column);
	return qindex;
}

bool SSModel::checkCircularity(const QString & lhs, 
					const std::set<QString> & indices_rhs) {
	QMap<QString, std::set<QString>> new_depends = depends_on_;
	new_depends.insert(lhs, indices_rhs);
	if (!checkCircularityHelper(lhs, indices_rhs)) {
		depends_on_ = new_depends;
		return false;
	}
	return true;
}

bool SSModel::checkCircularityHelper(const QString & lhs,
	const std::set<QString> & depends_on) {
	// see if current index depends on lhs
	if (depends_on.find(lhs) != depends_on.end())
		return true;
	for (auto ind : depends_on) {
		if (checkCircularityHelper(lhs, depends_on_[ind]))
			return true;
	}
	return false;
}

void SSModel::updateDependentValues(const QString & index) {
	for (auto ind : has_effect_on_[index]) {
		// calculate the formula 
		QVector<QString> tokens = data_[ind].second;
		double val;
		if (misc::predef_formulas.find(tokens[0]) != misc::predef_formulas.end())
			val = calculatePredefinedFormula(tokens);
		else {
			auto formula_ptr = std::make_shared<Expression>(tokens);
			val = calculateFormula(formula_ptr);
		}
		data_[ind].first = val;

		// update values depending on the value we just updated
		updateDependentValues(ind);
	}
}

void SSModel::updateDependencies(const QString& index, 
		const std::set<QString>& depends_on) {
	if (data_.contains(index)) {
		if (!data_[index].second.empty()) {
			std::set<QString> old_minus_new;
			std::set<QString> new_minus_old;
			std::set<QString> old_depends_on = depends_on_[index];
			// get set difference of old dependecies minus new dependencies
			std::set_difference(old_depends_on.begin(), old_depends_on.end(),
				depends_on.begin(), depends_on.end(),
				std::inserter(old_minus_new, old_minus_new.begin()));

			// get set difference of new dependencies minus old dependencies
			std::set_difference(depends_on.begin(), depends_on.end(),
				old_depends_on.begin(), old_depends_on.end(),
				std::inserter(new_minus_old, new_minus_old.begin()));

			// remove cells that index does not depend on anymore
			for (const QString& old_item : old_minus_new) {
				has_effect_on_[old_item].erase(index);
				depends_on_[index].erase(old_item);
			}
			// add new cells that index did not previously depend on
			for (const QString& new_item : new_minus_old) {
				has_effect_on_[new_item].emplace(index);
				depends_on_[index].emplace(new_item);
			}

			updateDependentValues(index);
			return;
		}
	}
	depends_on_.insert(index, depends_on);
	for (auto ind : depends_on) 
		has_effect_on_[ind].insert(index);
	updateDependentValues(index);
}

std::set<QString> SSModel::getIndices(const QString& index1, const QString& index2) const {
	std::set<QString> indices;
	QString first_column_c = QString(index1.at(0));
	QString second_column_c = QString(index2.at(0));
	int first_column = first_column_c.toInt();
	int second_column = second_column_c.toInt();
	int first_row = index1.right(index1.length() -1).toInt();
	int second_row = index2.right(index2.length() -1).toInt();
	for (int i = 0; i != misc::alphabet.length(); i++) {
		if (misc::alphabet[i] == index1[0])
			first_column = i;
		else if (misc::alphabet[i] == index2[0])
			second_column = i;
	}
	for (int i = first_column; i != second_column + 1; i++) {
		for (int j = first_row; j != second_row + 1; j++) {
			QString index = QString(misc::alphabet[i]) + QString::number(j);
			indices.insert(index);
		}
	}
	return indices;
}

double SSModel::getSum(const std::set<QString>& indices) {
	double sum = 0; 
	for (auto index : indices)
		sum += data_[index].first.toDouble();
	return sum;
}

double SSModel::getAverage(const std::set<QString>& indices) {
	double sum = 0;
	int ctr = 0; 
	for (auto index : indices) {
		bool ok;
		double value = data_[index].first.toDouble(&ok);
		if (ok) {
			sum += value;
			ctr++;
		}
	}
	return sum / ctr;
}

double SSModel::getMedian(const std::set<QString>& indices) {
	size_t size = indices.size();
	std::vector<double> values;
	values.resize(size);
	int i = 0;
	for (auto index : indices) {
		values.at(i) = data_[index].first.toDouble();
		i++;
	}
	std::sort(values.begin(), values.end());
	double median;
	if (indices.size() % 2 == 0)
		median = (values[values.size() / 2 - 1] + values[values.size() / 2]) /2;
	else
		median = values[values.size() / 2];
	return median;
}

double SSModel::getMin(const std::set<QString>& indices) {
	double minvalue = data_[*indices.begin()].first.toDouble();
	for (auto index: indices) {
		if (data_[index].first.toDouble() < minvalue)
			minvalue = data_[index].first.toDouble();
	}
	return minvalue;
}

double SSModel::getMax(const std::set<QString>& indices) {
	double maxvalue = data_[*indices.begin()].first.toDouble();
	for (auto index: indices) {
		if (data_[index].first.toDouble() > maxvalue)
			maxvalue = data_[index].first.toDouble();
	}
	return maxvalue;
}


Qt::ItemFlags SSModel::flags(const QModelIndex & index) const {
	return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}