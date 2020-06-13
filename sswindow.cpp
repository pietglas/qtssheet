/* author: Piet Glas
 */
#include "sswindow.h"
#include "ssview.h"
#include "ssmodel.h"
#include "qformulaaction.h"
#include <QObject>
#include <QMenu>
#include <QDebug>
#include <QMenuBar>
#include <QAction>
#include <QCoreApplication>
#include <QInputDialog>
#include <QDir>
#include <QLineEdit>
#include <QRect>
#include <QErrorMessage>
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
	createToolBars();

	statusBar();
	connect(sheetview_, &SSView::cellSelected, sheetmodel_, &SSModel::getFormula);
	connect(sheetmodel_, &SSModel::sendFormula, this, &SSWindow::updateStatus);
	connect(sheetview_, &SSView::cellSelected, this, &SSWindow::currentCell);
	connect(this, &SSWindow::setLabel, cell_label_, &QLabel::setText);

	// formula actions
	connect(sum_, &QFormulaAction::triggered, 
		sum_, &QFormulaAction::isTriggered);
	connect(sum_, &QFormulaAction::formulaTriggered, 
			formula_editor_, &QLineEdit::setText);
	connect(average_, &QFormulaAction::triggered, 
		average_, &QFormulaAction::isTriggered);
	connect(average_, &QFormulaAction::formulaTriggered, 
			formula_editor_, &QLineEdit::setText);
	connect(median_, &QFormulaAction::triggered, 
		median_, &QFormulaAction::isTriggered);
	connect(median_, &QFormulaAction::formulaTriggered, 
			formula_editor_, &QLineEdit::setText);
	connect(min_, &QFormulaAction::triggered, 
		min_, &QFormulaAction::isTriggered);
	connect(min_, &QFormulaAction::formulaTriggered, 
			formula_editor_, &QLineEdit::setText);
	connect(max_, &QFormulaAction::triggered, 
		max_, &QFormulaAction::isTriggered);
	connect(max_, &QFormulaAction::formulaTriggered, 
			formula_editor_, &QLineEdit::setText);
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
	// delete formula_editor_;	// actually, toolbar is responsible for this
	// delete cell_label_;	// dito 
	delete formula_toolbar_;
}

void SSWindow::showWindowTitle() {
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
	QString formula;
	QLineEdit * emitter = qobject_cast<QLineEdit *>(sender());
	if (emitter == nullptr) {
		bool ok;
		formula = QInputDialog::getText(this, tr("Enter Formula"),
							tr("Enter Formula"), QLineEdit::Normal,
							QDir::home().dirName(), &ok);
	}
	else {
		formula = formula_editor_->text();
	}
	if (!(sheetmodel_->setFormula(formula, current_cell_))) {
		QErrorMessage error(this);
		error.showMessage("Invalid syntax or circular dependency");
		error.exec();
	}
}

void SSWindow::updateStatus(const QString & formula) {
	QString message = "Formula: " + formula;
	statusBar()->showMessage(message);
}

void SSWindow::currentCell(const QModelIndex& index) {
	current_cell_ = sheetmodel_->convertIndexToString(index);
	emit setLabel(current_cell_);
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
	save_to_file_->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
	connect(save_to_file_, &QAction::triggered, this, &SSWindow::saveToFile);

	save_ = new QAction(tr("Save"), this);
	save_->setShortcut(QKeySequence(tr("Ctrl+S")));
	connect(save_, &QAction::triggered, this, &SSWindow::save);

	add_formula_ = new QAction(tr("Add Formula"), this);
	connect(add_formula_, &QAction::triggered, this, &SSWindow::addFormula);

	sum_ = new QFormulaAction(tr("sum"), this);

	average_ = new QFormulaAction(tr("average"), this);

	median_ = new QFormulaAction(tr("average"), this);

	min_ = new QFormulaAction(tr("min"), this);

	max_ = new QFormulaAction(tr("max"), this);
}

void SSWindow::createToolBars() {
	// add a toolbar for editing formulas
	formula_toolbar_ = new QToolBar(this);
	addToolBar(formula_toolbar_);

	// add cell label to the toolbar 
	cell_label_ = new QLabel(formula_toolbar_);
	formula_toolbar_->addWidget(cell_label_);	

	// add formula editor to the toolbar
	formula_editor_ = new QLineEdit(formula_toolbar_);
	formula_toolbar_->addWidget(formula_editor_);
	connect(formula_editor_, &QLineEdit::editingFinished, 
		this, &SSWindow::addFormula);
}

void SSWindow::setupMenuBar() {
	// file menu
	QMenu * filemenu = menuBar()->addMenu(tr("File"));
	filemenu->addAction(open_);
	filemenu->addAction(save_to_file_);
	filemenu->addAction(save_);
	filemenu->addAction(clear_);
	filemenu->addAction(exit_);
	// cell menu
	QMenu * cell_menu = menuBar()->addMenu(tr("Cell"));
	cell_menu->addAction(add_formula_);
	// formula menu
	QMenu * formula_menu = menuBar()->addMenu(tr("Formula"));
	formula_menu->addAction(sum_);
	formula_menu->addAction(average_);
	formula_menu->addAction(median_);
	formula_menu->addAction(min_);
	formula_menu->addAction(max_);
}