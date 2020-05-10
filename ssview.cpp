/* author: Piet Glas
 */
#include "ssview.h"
#include <QTableView>

SSView::SSView(QWidget * parent) : QTableView(parent) {}

QModelIndexList SSView::getSelectedIndex() {
	return this->selectedIndexes();
}