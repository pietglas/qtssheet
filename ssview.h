/* author: Piet Glas
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
signals:
	QModelIndexList getSelectedIndex();	// for displaying cell info in statusbar
};