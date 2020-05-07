#pragma once

#include <QVector>
#include <QString>
#include <QChar>
#include <set>

// Converts a string into a vector of tokens. Valid tokens
// are defined as private members. The same tokenizer can
// be used for several strings.
class Tokenizer {
public:
	Tokenizer();
	// tokenizes a string. Returns false if the string 
	// contains invalid tokens. 
	bool tokenize(const QString & formula);
	// check if the formula is syntactically valid
	bool validate() const;
	// returns the vector with tokens.
	QVector<QString> tokenized() const;
private:
	std::set<QChar> capitals_ {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
		'U', 'V', 'W', 'X', 'Y', 'X'
	};
	std::set<QChar> numbers_ {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'.'
	};
	std::set<QChar> punctuations_ {
		'(', ')' 
	};
	std::set<QString> spunctuations_ {
		"(", ")"
	};
	std::set<QChar> operations_ {
		'=', '-', '+', '*', '/', '^'
	};
	std::set<QString> soperations_ {
		"=", "-", "+", "*", "/", "^"
	};
	QVector<QString> tokenized_;
};