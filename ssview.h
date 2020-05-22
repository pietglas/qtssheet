/* author: Piet Glas
 * TODO
 * - use QAbstractItemView::currentChanged() slot for displaying cell info
 	 in status bar. 
 */
#pragma once

#include <QTableView>
#include <QModelIndexList>
#include <QWidget>

// Just renames the QTableView class, to make the 
// view-model design more apparent. 
class SSView : public QTableView {
	Q_OBJECT
public:
	SSView(QWidget * parent = nullptr);
protected:
	void currentChanged(const QModelIndex & current, const QModelIndex & previous);
signals:
	// sends a signal to ssmodel if another cell is selected
	void cellSelected(const QModelIndex & current);
};