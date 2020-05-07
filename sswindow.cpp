/* author: Piet Glas
 */
#include "sswindow.h"
#include "ssview.h"
#include "ssmodel.h"
#include <QObject>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QCoreApplication>
#include <QInputDialog>
#include <QDir>
#include <QLineEdit>
#include <QRect>
#include <QStyle>
#include <QKeySequence>
#include <QDesktopWidget>

SSWindow::SSWindow(int rows, int cols, QWidget * parent): QMainWindow(parent),
			sheetview_(new SSView(this)), sheetmodel_(new SSModel(rows, cols, this)) {
	// set sheetview_ to be the central widget
	setCentralWidget(sheetview_);

	// set up the position and size of the window 
	QPoint fhd_center = QRect(0, 0, 1920, 1080).center();
	QPoint left_upper_corner = QPoint(fhd_center.x() - 500, fhd_center.y() - 300);
	setGeometry(QRect(left_upper_corner, QSize(1000, 600)));
	
	// connect view with model
	sheetview_->setModel(sheetmodel_);
	
	// create actions, set up menu bar
	createActions();
	setupMenuBar();
}

SSWindow::~SSWindow() {
	delete sheetview_;
	delete sheetmodel_;
	delete open_;
	delete save_to_file_;
	delete save_;
	delete clear_;
	delete exit_;
	delete add_formula_;
}

void SSWindow::showWindowTitle(const QString & title) {
	setWindowTitle(tr("New File"));
}

void SSWindow::clear() {
	sheetmodel_->clearData();
}

void SSWindow::loadFromFile() {
	bool ok;
	file_name_ = QInputDialog::getText(this, tr("Open File"),
						tr("Enter File Name"), QLineEdit::Normal,
						QDir::home().dirName(), &ok);
	sheetmodel_->getDataFromFile(file_name_);
	setWindowTitle(file_name_);
}

void SSWindow::saveToFile() {
	bool ok;
	file_name_ = QInputDialog::getText(this, tr("Save File"),
						tr("Enter File Name"), QLineEdit::Normal,
						QDir::home().dirName(), &ok);
	sheetmodel_->saveData(file_name_);
	setWindowTitle(file_name_);
}

void SSWindow::save() {
	sheetmodel_->saveData(file_name_);
}

void SSWindow::addFormula() {
	bool ok;
	QString formula = QInputDialog::getText(this, tr("Enter Formula"),
						tr("Enter Formula"), QLineEdit::Normal,
						QDir::home().dirName(), &ok);
	sheetmodel_->setFormula(formula);
}

void SSWindow::createActions() {
	clear_ = new QAction(tr("Clear"), this);
	clear_->setShortcut(Qt::Key_Delete);
	connect(clear_, &QAction::triggered, this, &SSWindow::clear);

	exit_ = new QAction(tr("Exit"), this);
	connect(exit_, &QAction::triggered, qApp, &QCoreApplication::quit);

	open_ = new QAction(tr("Open File"), this);
	open_->setShortcut(QKeySequence(tr("Ctrl+O")));
	connect(open_, &QAction::triggered, this, &SSWindow::loadFromFile);

	save_to_file_ = new QAction(tr("Save As"), this);
	connect(save_to_file_, &QAction::triggered, this, &SSWindow::saveToFile);

	save_ = new QAction(tr("Save"), this);
	save_->setShortcut(QKeySequence(tr("Ctrl+S")));
	connect(save_, &QAction::triggered, this, &SSWindow::save);

	add_formula_ = new QAction(tr("Add Formula"), this);
	connect(add_formula_, &QAction::triggered, this, &SSWindow::addFormula);
}

void SSWindow::setupMenuBar() {
	// file menu
	QMenu * filemenu = menuBar()->addMenu(tr("File"));
	filemenu->addAction(open_);
	filemenu->addAction(save_to_file_);
	filemenu->addAction(save_);
	filemenu->addAction(clear_);
	filemenu->addAction(exit_);
	// add formula
	QMenu * formula_menu = menuBar()->addMenu(tr("Cell"));
	formula_menu->addAction(add_formula_);
}