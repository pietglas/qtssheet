/* author: Piet Glas
 */
#include "ssview.h"
#include <QTableView>

SSView::SSView(QWidget * parent) : QTableView(parent) {}


void SSView::currentChanged(const QModelIndex & current, 
		const QModelIndex & previous) {
	emit SSView::cellSelected(current);
}