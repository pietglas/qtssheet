Files for a simple spreadsheet in Qt. 

This project is mainly there for learning purposes and in progress. As such, the functionality is still very limited and there might be several bugs.

To build and run the code, clone the repository. From the command line, go to /qtssheet and run `qmake -project`. Open `qtssheet.pro` and add the line `QT += widgets`. Then run `make` from the command line, followed by `./qtssheet`.

As usual, one can enter alphanumeric data. One can also save the data to a .txt-file and reopen it later, see the menu bar. 

The text bar below the menu bar is for entering formulas (functionality will be expanded), currently one can use 'sum', 'min', 'max', 'average', 'median'. The syntax for entering a formula is similar to Excel, i.e. `=sum(index_1:index_2)` (with index_1 <= index_2). The formula is calculated for the currently selected cell. 

Sources used:
- https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/itemviews/spreadsheet?h=5.14
- https://doc.qt.io/qt-5/modelview.html 
- http://web.stanford.edu/class/cs106x/res/reader/CS106BX-Reader.pdf (for file parsing)

To do: 
- Add more functionality to the formula toolbar
- Add Help button and corresponding documentation 

Build with Qt version 5.9.5, on platform Ubuntu 18.04