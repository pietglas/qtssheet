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
			sheetview_(new SSView(this)), sheetmodel_(new SSModel(rows, cols, this)) 
{
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

	// make sure the correct info is displayed when switching cells
	connect(sheetview_, &SSView::cellSelected, sheetmodel_, &SSModel::getFormula);
	connect(sheetview_, &SSView::cellSelected, this, &SSWindow::currentCell);
	connect(this, &SSWindow::setLabel, cell_label_, &QLabel::setText);
	connect(sheetmodel_, &SSModel::sendFormula, 
		formula_editor_, &QLineEdit::setText);

	// Connect formula options from menu bar to the formula editor
	connectFormulaToEditor(sum_);
	connectFormulaToEditor(average_);
	connectFormulaToEditor(median_);
	connectFormulaToEditor(min_);
	connectFormulaToEditor(max_);
}

SSWindow::~SSWindow() 
{
	delete sheetview_;
	delete sheetmodel_;
	delete open_;
	delete save_to_file_;
	delete save_;
	delete clear_;
	delete exit_;
	delete add_formula_;
	delete formula_toolbar_;
}

void SSWindow::showWindowTitle() 
{
	setWindowTitle(tr("New File"));
}

void SSWindow::clear() 
{
	sheetmodel_->clearData();
}

void SSWindow::loadFromFile() 
{
	bool ok;
	file_name_ = QInputDialog::getText(this, tr("Open File"),
						tr("Enter File Name"), QLineEdit::Normal,
						QDir::home().dirName(), &ok);
	sheetmodel_->getDataFromFile(file_name_);
	setWindowTitle(file_name_);
}

void SSWindow::saveToFile() 
{
	bool ok;
	file_name_ = QInputDialog::getText(this, tr("Save File"),
						tr("Enter File Name"), QLineEdit::Normal,
						QDir::home().dirName(), &ok);
	sheetmodel_->saveData(file_name_);
	setWindowTitle(file_name_);
}

void SSWindow::save() 
{
	sheetmodel_->saveData(file_name_);
}

void SSWindow::addFormula() 
{
	QString formula;
	QLineEdit * emitter = qobject_cast<QLineEdit *>(sender());
	if (emitter == nullptr) 
	{
		bool ok;
		formula = QInputDialog::getText(this, tr("Enter Formula"),
							tr("Enter Formula"), QLineEdit::Normal,
							QDir::home().dirName(), &ok);
	}
	else 
		formula = formula_editor_->text();
	
	if (!(sheetmodel_->setFormula(formula, current_cell_))) 
	{
		QErrorMessage error(this);
		error.showMessage("Invalid syntax or circular dependency");
		error.exec();
	}
}

void SSWindow::currentCell(const QModelIndex& index) 
{
	current_cell_ = sheetmodel_->convertIndexToString(index);
	emit setLabel(current_cell_);
}


void SSWindow::createActions() 
{
	// set the file options in the menu bar
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

	// create formula options in the menu bar
	sum_ = new QFormulaAction(tr("sum"), this);

	average_ = new QFormulaAction(tr("average"), this);

	median_ = new QFormulaAction(tr("average"), this);

	min_ = new QFormulaAction(tr("min"), this);

	max_ = new QFormulaAction(tr("max"), this);
}

void SSWindow::createToolBars() 
{
	// add a toolbar for editing formulas
	formula_toolbar_ = new QToolBar(this);
	addToolBar(formula_toolbar_);

	// add cell label to the toolbar 
	cell_label_ = new QLabel(formula_toolbar_);
	formula_toolbar_->addWidget(cell_label_);	

	// add formula editor to the toolbar
	formula_editor_ = new QLineEdit(formula_toolbar_);
	formula_toolbar_->addWidget(formula_editor_);

	// connect the toolbar to the model
	connect(formula_editor_, &QLineEdit::editingFinished, 
		this, &SSWindow::addFormula);
}

void SSWindow::setupMenuBar() 
{
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

void SSWindow::connectFormulaToEditor(QFormulaAction *& formula)
{
	// Connect formula options from menu bar to the formula editor
	connect(formula, &QFormulaAction::triggered, 
		formula, &QFormulaAction::isTriggered);
	connect(formula, &QFormulaAction::formulaTriggered, 
			formula_editor_, &QLineEdit::setText);	// sets text formula editor
	connect(formula, &QFormulaAction::triggered, 
		formula_editor_, &QLineEdit::selectAll);	// selects the text
}