#pragma once

#include <QString>
#include <QAction>
#include <QObject>

/* Derives the QAction class. Converts the triggered() signal into
 * a signal sending the text of the QAction object, which is a 
 * name for a predefined formula. Upon triggering, the formula
 * textbox is conveniently filled with the syntax for the 
 * corresponding predefined formula.
 */
class QFormulaAction : public QAction {
	Q_OBJECT
public:
	QFormulaAction(const QString& text, QObject * parent=nullptr);
	~QFormulaAction();
public slots:
	void isTriggered(bool checked);
signals:
	void formulaTriggered(const QString& text);
};