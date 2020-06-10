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
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QModelIndex>
#include "ssview.h"
#include "ssmodel.h"

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
	void updateStatus(const QString & formula);
protected:
	void setupMenuBar();
	void createActions();
private:
	QString file_name_;
	SSView * sheetview_;
	SSModel * sheetmodel_;
	QAction * open_;
	QAction * save_to_file_;
	QAction * save_;
	QAction * clear_;
	QAction * exit_;	
	QAction * add_formula_;
};