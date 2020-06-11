#pragma once

#include <set>
#include <QString>
#include <QChar>

namespace misc {
	extern QString alphabet;	// for column index

	extern std::set<QChar> operations;
	extern std::set<QString> soperations;
	extern std::set<QString> predef_formulas;
	extern std::set<QChar> capitals;
	extern std::set<QChar> letters;
	extern std::set<QChar> numbers;
	extern std::set<QChar> punctuations;
	extern std::set<QString> spunctuations;
}	// end namespace misc