/* author: Piet Glas
 */
#pragma once

#include <QTableView>
#include <QWidget>

// Just renames the QTableView class, to make the 
// view-model design more apparent. 
class SSView : public QTableView {
public:
	SSView(QWidget * parent = nullptr);
};