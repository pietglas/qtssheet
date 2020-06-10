/* Author: Piet Glas
 *
 * TODO:
 * - 
 */

#pragma once

#include "formula/expression.h"

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
	// Add a formula.
	bool setFormula(const QString & formula);

	Qt::ItemFlags flags(const QModelIndex & index) const override;
public slots:
	// receives signal from ssview, which cell is currently selected
	void getFormula(const QModelIndex & current);
signals:
	// sends a signal with the formula to sswindow,
	// so it can be displayed in the statusbar
	void sendFormula(const QString & formula);

private:
	int rows_;
	int cols_;
	QMap<QString, QPair<QVariant,QVector<QString>>> data_;
	QMap<QString, std::set<QString>> depends_on_;
	QMap<QString, std::set<QString>> has_effect_on_;
	QString alph_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";	// for column index
	std::set<QString> predefined_formulas_ {
		"sum", "average", "median", "min", "max"
	};

	// converts displayed index (i.e. `A2`) to model index (i.e. `<1, 0>`)
	QPair<int, int> convertStrToIndex(const QString & index) const;
	// reverse of the previous
	QString convertIndexToStr(const QPair<int, int> & index) const;
	// calculate a formula.
	double calculateFormula(std::shared_ptr<Expression> formula);
	// checks if a formula causes circular dependencies to occur
	double calculatePredefinedFormula(const QVector<QString>& tokens);
	bool checkCircularity(const QString & lhs, 
					const std::set<QString> & indices_rhs);
	bool checkCircularityHelper(const QString & lhs,
			const std::set<QString> & depends_on);
	// updates dependent values
	void updateDependentValues(const QString & index);

	std::set<QString> getIndices(const QString& index1, const QString& index2) const;

	double getSum(const std::set<QString>& indices);
	double getAverage(const std::set<QString>& indices);
	double getMedian(const std::set<QString>& indices);
	double getMin(const std::set<QString>& indices);
	double getMax(const std::set<QString>& indices);
	
};