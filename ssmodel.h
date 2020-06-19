/* Author: Piet Glas
 *
 * TODO:
 * - 
 */

#pragma once

#include "formula/expression.h"
#include "formula/tokenizer.h"
#include "formula/common_collections_methods.h"

#include <QAbstractTableModel>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QPair>
#include <set>
#include <QObject>
#include <QMap>

// Model containing the data for our spreadsheet, and an interface which
// SSView and MainWindow use for communicating changes made by the user.
class SSModel : public QAbstractTableModel {
	Q_OBJECT
public:
	SSModel(int rows, int cols, QObject * parent=nullptr);
	~SSModel();
	// following methods return the number of rows and the number of columns;
	// mainly used by the display class SSView
	int rowCount(const QModelIndex & parent=QModelIndex()) const override;
	int columnCount(const QModelIndex & parent=QModelIndex()) const override;
	// SSView obtains data by calling this method
	QVariant data(const QModelIndex & index, 
					int role = Qt::DisplayRole) const override;
	// SSView obtains headerdata by calling this method
	QVariant headerData(int section, Qt::Orientation orientation,
						int role) const override;
	// SSView communicates updates in the data by calling this method
	bool setData(const QModelIndex & index, const QVariant & value, 
	 			int role = Qt::EditRole) override;
	// clears the data
	void clearData();
	// Load data from a .txt file.
	bool getDataFromFile(const QString& file_name);
	// Save data in a .txt file. 
	bool saveData(const QString & file_name) const;
	// Adds a formula to the cell with index key, and calls 
	// calculateFormula to set the value of the cell. Returns
	// true if succesfull, i.e. if no circular dependencies
	// were caused
	bool setFormula(const QString & formula, const QString& key);

	// converts a qmodelindex into the 'displayed' index, and vice versa
	QString convertIndexToString(const QModelIndex& index) const;
	QModelIndex convertStringToIndex(const QString& index) const;

	Qt::ItemFlags flags(const QModelIndex & index) const override;
public slots:
	// receives signal from ssview, which cell is currently selected
	void getFormula(const QModelIndex & current);
signals:
	// sends a signal with the formula to sswindow,
	// so the correct formula is displayed in the formula editor
	void sendFormula(const QString & formula);

private:
	int rows_;
	int cols_;
	// contains the data as <index, <data, formula>>
	QMap<QString, QPair<QVariant,QVector<QString>>> data_;
	// keeps track of the cells that each cell depends on
	QMap<QString, std::set<QString>> depends_on_;
	// keeps track of the cells that each cell has influnce on
	QMap<QString, std::set<QString>> has_effect_on_;

	// calculate a custom formula, returns the value
	double calculateFormula(std::shared_ptr<Expression> formula);
	// calculate a predifined formula, returns the value
	double calculatePredefinedFormula(const QVector<QString>& tokens);
	// returns true if rhs depends on lhs, which would cause 
	// a circular dependency
	bool checkCircularity(const QString & lhs, 
					const std::set<QString> & indices_rhs);
	bool checkCircularityHelper(const QString & lhs,
			const std::set<QString> & depends_on);
	// updates the cells that depend on index
	void updateDependentValues(const QString & index);

	// returns all indices between index1 and index2, in the following way:
	// - getIndices(A1:A6) returns {A1, A2, A3, A4, A5, A6}
	// - getIndices(A1:C2) returns {A1, A2, B1, B2, C1, C2}
	std::set<QString> getIndices(const QString& index1, const QString& index2) const;

	// functions to calculate predefined formulas
	double getSum(const std::set<QString>& indices);
	double getAverage(const std::set<QString>& indices);
	double getMedian(const std::set<QString>& indices);
	double getMin(const std::set<QString>& indices);
	double getMax(const std::set<QString>& indices);
	
};