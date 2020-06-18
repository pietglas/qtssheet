/* Author: Piet Glas
 *
 * TODO:
 * - deal properly with the case where an empty formula
 *	 is entered, or even just a number as formula. 
 */

#pragma once

#include <QMainWindow>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QLabel>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QModelIndex>
#include "ssview.h"
#include "ssmodel.h"
#include "qformulaaction.h"

// Sets the main window of the spreadsheet. 
// Contains a pointer to SSView which inherits QTableView; 
// this is set to be the central widget. 
// Also contains a pointer to the model, SSModel, which
// inherits QAbstractTableModel. The actions allow
// the main window to communicate changes to the model.
class SSWindow : public QMainWindow {
	Q_OBJECT
public:
	SSWindow(int rows, int cols, QWidget * parent=nullptr);
	~SSWindow();
public slots:
	void loadFromFile();
	void saveToFile();
	void save();	
	void clear();
	void showWindowTitle();
	void addFormula();
	void currentCell(const QModelIndex& index);
signals:
	void setLabel(const QString& index);
protected:
	void setupMenuBar();
	void createActions();
	void createToolBars();
private:
	QString current_cell_;
	QString file_name_;

	SSView * sheetview_;
	SSModel * sheetmodel_;

	QToolBar * formula_toolbar_;
	QLineEdit * formula_editor_;
	QLabel * cell_label_;

	QFormulaAction * sum_;
	QFormulaAction * average_;
	QFormulaAction * median_;
	QFormulaAction * min_;
	QFormulaAction * max_;

	QAction * open_;
	QAction * save_to_file_;
	QAction * save_;
	QAction * clear_;
	QAction * exit_;	
	QAction * add_formula_;
};