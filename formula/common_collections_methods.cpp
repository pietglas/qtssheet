#include "common_collections_methods.h"

#include <set>
#include <QString>
#include <QChar>

namespace misc {
	QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";	// for column index

	std::set<QChar> operations {
		'-', '+', '*', '/', '^'
	};
	std::set<QString> soperations {
		"-", "+", "*", "/", "^"
	};
	std::set<QString> predef_formulas {
		"sum", "average", "median", "min", "max"
	};
	std::set<QChar> capitals {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
		'U', 'V', 'W', 'X', 'Y', 'X'
	};
	std::set<QChar> letters {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y', 'z'
	};
	std::set<QChar> numbers {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'.'
	};
	std::set<QChar> punctuations {
		'(', ')', ':'
	};
	std::set<QString> spunctuations {
		"(", ")", ":"
	};
}	// end namespace misc