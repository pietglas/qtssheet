/* author: Piet Glas
 */
#include "ssview.h"
#include "ssmodel.h"
#include "sswindow.h"
#include <QApplication>
#include <string>
#include <iostream>

using std::cout;	using std::endl;

int main(int argc, char **argv) {
	QApplication a(argc, argv);
	int rows = 100;
	int cols = 26;
	// when given, convert cmd input to int, to obtain row and column size
	if (argc > 1) {
		std::string rowss(1, argv[1][0]);
		std::string colss(1, argv[2][0]);
		rows = std::stoi(rowss);
		cols = std::stoi(colss);
	}
	SSWindow window(rows, cols);
	window.show();
	return a.exec();
}