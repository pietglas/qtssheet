#include "tokenizer.h"

Tokenizer::Tokenizer(QString formula) : formula_{formula} {}

bool Tokenizer::tokenize() {
	// reset tokenized_
	tokenized_.clear();

	int pos = 0;
	while (pos != formula_.length()) {
		QString token = "";
		std::set<QChar> category;
		// skip whitespaces
		if (formula_[pos] == " ") {
			++pos;
			continue;
		}
		// find the token type
		else if (letters_.find(formula_[pos]) != letters_.end())
			category = letters_;
		else if (capitals_.find(formula_[pos]) != capitals_.end()) {
			category = capitals_;	
		}
		else if (numbers_.find(formula_[pos]) != numbers_.end()) {
			category = numbers_;
		}
		else if (punctuations_.find(formula_[pos]) != punctuations_.end())
			category = punctuations_;
		else if (operations_.find(formula_[pos]) != operations_.end())
			category = operations_;			
		else
			return false;
		
		if (!getNewToken(category, token, pos))
			return false;
		tokenized_.push_back(token);
	}
	return true;
}

std::set<QString> Tokenizer::validate() const { 
	std::set<QString> indices;
	std::set<QString> emptyset;
	int leftbrace_count = 0;
	int rightbrace_count = 0;
	for (int pos = 0; pos != tokenized_.length(); pos++) {
		if (pos == 0) {
			if (capitals_.find(tokenized_[pos][0]) == capitals_.end()) 
				return emptyset;	// formula starts with a cell index
		}
		if (soperations_.find(tokenized_[pos]) != soperations_.end()) {
			if (pos == tokenized_.length() - 1) 
				return emptyset;	// operator at end of expression
			if (soperations_.find(tokenized_[pos + 1]) != soperations_.end() ||
					tokenized_[pos + 1] == ")") 
				return emptyset;	// operation next to operation or )
		}
		else if (spunctuations_.find(tokenized_[pos]) != spunctuations_.end()) {
			if (tokenized_[pos] == "(")
				leftbrace_count++;
			else if (tokenized_[pos] == ")")
				rightbrace_count++;
			if (rightbrace_count > leftbrace_count)
				return emptyset;	// more right than left braces
		}
		else if (predefined_formulas_.find(tokenized_[pos]) != 
				predefined_formulas_.end()) {
			if (tokenized_[pos + 1] != "(" || tokenized_[pos - 1] != "=")
				return emptyset;
		}
		else {	// number followed by number, cellindex or predefined formula
			if (pos != tokenized_.length() - 1) {
				if (soperations_.find(tokenized_[pos + 1]) == soperations_.end() &&
					spunctuations_.find(tokenized_[pos + 1]) == spunctuations_.end())
					return emptyset;
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
	if (category == capitals_) {
		category = numbers_;
		getNewToken(category, token, pos);
	}
	else if (category == letters_) {
		if (predefined_formulas_.find(token) == predefined_formulas_.end())
			return false;
		predefined_formula_ = *predefined_formulas_.find(token);
	}
	return true;
}
