/* TODO:
 * - implement the validation proces with a grammer, the current approach
 * 	 seems prone to failures
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
	// with the indices if the formula is valid, otherwise it
	// returns an empty vector. 
	std::set<QString> validate() const;
	// returns the vector with tokens.
	QVector<QString> tokenized() const;
	bool predefined() const; 
	QString getPredefined() const;
private:
	QString formula_;
	QString predefined_formula_ = "";
	QVector<QString> tokenized_;

	bool getNewToken(std::set<QChar>& category, QString& token, int& pos);
	bool numberOrIndex(int pos) const;
};