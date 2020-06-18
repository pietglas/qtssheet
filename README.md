Files for a simple spreadsheet in Qt. 

The purpose of this project is for me to get more experience with C++, to familiarize myself with Qt and to have fun building something. As such, the functionality is rather limited and there might be several bugs.

To build and run the code, clone the repository. From the command line, go to /qtssheet and run `qmake -project`. Open qtssheet.pro and add the line `QT += widgets`. Then run `make` from the command line, followed by `./qtssheet`.

As usual, one can enter alphanumeric data. One can also save the data to a .txt-file and reopen it later, see the menu bar. 

The text bar below the menu bar is for entering formulas (functionality will be expanded). There are some predefined formulas like sum, average etc that can be selected in the menu. The syntax for entering a formula is similar to Excel, for example `=sum(index_1:index_2)` (with index_1 <= index_2). The formula is calculated for the currently selected cell. 

Sources used:
- https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/itemviews/spreadsheet?h=5.14
- https://doc.qt.io/qt-5/modelview.html 
- http://web.stanford.edu/class/cs106x/res/reader/CS106BX-Reader.pdf (for line parsing)

To do: 
- Add more functionality to the formula toolbar
- Add Help documentation 

Build with Qt version 5.9.5, on platform Ubuntu 18.04