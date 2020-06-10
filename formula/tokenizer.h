/* TODO:
 * - implement the validation proces with a grammer, the current approach
 * 	 doesn't prevent incorrect behaviour
 */

#pragma once

#include <QVector>
#include <set>
#include <QDebug>
#include <QString>
#include <QChar>
#include <set>

// Converts a string into a vector of tokens. Valid tokens
// are defined as private members. The same tokenizer can
// be used for several strings.
class Tokenizer {
public:
	Tokenizer(QString formula);
	// tokenizes a string. Returns false if the string 
	// contains invalid tokens. 
	bool tokenize();
	// check if the formula is syntactically valid. Returns a vector
	// with the indexes on rhs if the formula is valid, otherwise it
	// returns an empty vector. 
	std::set<QString> validate() const;
	// returns the vector with tokens.
	QVector<QString> tokenized() const;
	bool predefined() const; 
	QString getPredefined() const;
private:
	QString formula_;
	QString predefined_formula_ = "";
	std::set<QChar> capitals_ {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
		'U', 'V', 'W', 'X', 'Y', 'X'
	};
	std::set<QChar> letters_ {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y', 'z'
	};
	std::set<QChar> numbers_ {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'.'
	};
	std::set<QChar> punctuations_ {
		'(', ')', ':'
	};
	std::set<QString> spunctuations_ {
		"(", ")", ":"
	};
	std::set<QChar> operations_ {
		'=', '-', '+', '*', '/', '^'
	};
	std::set<QString> soperations_ {
		"=", "-", "+", "*", "/", "^"
	};
	std::set<QString> predefined_formulas_ {
		"sum", "average", "median", "min", "max"
	};
	QVector<QString> tokenized_;

	bool getNewToken(std::set<QChar>& category, QString& token, int& pos);
};