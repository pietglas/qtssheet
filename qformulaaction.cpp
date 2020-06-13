#include "qformulaaction.h"

QFormulaAction::QFormulaAction(const QString& text, QObject * parent) :
	QAction(text, parent) {}

QFormulaAction::~QFormulaAction() {}

void QFormulaAction::isTriggered(bool checked) {
	QString add_formula = "=" + this->text() + "(A1:A2)";
	emit formulaTriggered(add_formula);
}