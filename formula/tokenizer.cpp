#include "tokenizer.h"
#include "common_collections_methods.h"

Tokenizer::Tokenizer(QString formula) : formula_{formula} {}

bool Tokenizer::tokenize() {
	// reset tokenized_
	tokenized_.clear();

	int pos = 1;	// skip '=' sign
	while (pos != formula_.length()) {
		QString token = "";
		std::set<QChar> category;
		// skip whitespaces
		if (formula_[pos] == " ") {
			++pos;
			continue;
		}
		// find the token type
		else if (misc::letters.find(formula_[pos]) != misc::letters.end())
			category = misc::letters;
		else if (misc::capitals.find(formula_[pos]) != misc::capitals.end()) {
			category = misc::capitals;	
		}
		else if (misc::numbers.find(formula_[pos]) != misc::numbers.end()) {
			category = misc::numbers;
		}
		else if (misc::punctuations.find(formula_[pos]) != misc::punctuations.end())
			category = misc::punctuations;
		else if (misc::operations.find(formula_[pos]) != misc::operations.end())
			category = misc::operations;			
		else
			return false;
		
		if (!getNewToken(category, token, pos))
			return false;

		qDebug() << "new token: " << token;

		tokenized_.push_back(token);
	}
	return true;
}

std::set<QString> Tokenizer::validate() const { 
	std::set<QString> indices;
	std::set<QString> emptyset;
	int leftbrace_count = 0;
	int rightbrace_count = 0;
	int end = tokenized_.length() - 1;
	for (int pos = 0; pos != end + 1; pos++) {
		if (misc::soperations.find(tokenized_[pos]) != 
			misc::soperations.end()) {
			// invalid if two operators are adjecent, operator precedes
			// ')', or operator is at begin or end of the expression
			if (pos == end || pos == 0 ||
				misc::soperations.find(tokenized_[pos + 1]) != 
					misc::soperations.end() ||
					tokenized_[pos + 1] == ")") {
				// qDebug() << "operator at end of expression";
				return emptyset;	
			}
		}
		else if (misc::spunctuations.find(tokenized_[pos]) != 
				misc::spunctuations.end()) {
			if (tokenized_[pos] == "(") {
				if (pos != 0) {
					if (numberOrIndex(pos - 1))
						return emptyset;	// '(' following number
				}
				leftbrace_count++;
			}
			else if (tokenized_[pos] == ")") {
				if (pos != end) {
					if (numberOrIndex(pos + 1))
						return emptyset;	// number following ')'
				}
				rightbrace_count++;
			}
			if (rightbrace_count > leftbrace_count) {
				// qDebug() << "more right than left braces";
				return emptyset;	// more right than left braces
			}
		}
		else if (misc::predef_formulas.find(tokenized_[pos]) != 
				misc::predef_formulas.end()) {
			if (tokenized_[pos + 1] != "(" || pos != 0) {
				// qDebug() << "predef formula not followed by '('";
				return emptyset;	// formula not at begin, or followed by '('
			}
		}
		else {	// number or cellindex
			if (pos != end) {
				if (numberOrIndex(pos + 1)) {
					// qDebug() << "number followed by number, index or predef form";
					return emptyset;	// number followed by number or cellindex
				}
			}
			// if the token is an index, add it to indices for later use
			bool ok;
			double index_or_nr = tokenized_[pos].toDouble(&ok);
			if (!ok && pos != 0)
				indices.insert(tokenized_[pos]);
		}
	}
	return indices;
}

QVector<QString> Tokenizer::tokenized() const {
	return tokenized_;
}

bool Tokenizer::predefined() const {
	return predefined_formula_.length() != 0;
}

QString Tokenizer::getPredefined() const {
	return predefined_formula_;
}

bool Tokenizer::getNewToken(std::set<QChar>& category, 
		QString& token, int& pos) {
	while (category.find(formula_[pos]) != category.end()) {
		token += formula_[pos];
		++pos;
	}
	if (category == misc::capitals) {
		category = misc::numbers;
		getNewToken(category, token, pos);
	}
	else if (category == misc::letters) {
		if (misc::predef_formulas.find(token) == misc::predef_formulas.end()) {
			// qDebug() << "predef formula doesn't exist";
			return false;
		}
		predefined_formula_ = *misc::predef_formulas.find(token);
	}
	return true;
}

bool Tokenizer::numberOrIndex(int pos) const {
	return misc::soperations.find(tokenized_[pos]) ==
		misc::soperations.end() &&
		misc::spunctuations.find(tokenized_[pos]) ==
		misc::soperations.end() &&
		misc::predef_formulas.find(tokenized_[pos]) ==
		misc::predef_formulas.end();
}
