#include "tokenizer.h"

Tokenizer::Tokenizer(){}

bool Tokenizer::tokenize(const QString & formula) {
	// reset tokenized_
	tokenized_.clear();

	int pos = 0;
	while (pos != formula.length()) {
		QString token;
		// skip whitespaces
		if (formula[pos] == " ")
			++pos;
		else if (capitals_.find(formula[pos]) != capitals_.end()) {
			token += formula[pos];
			++pos;
			while (numbers_.find(formula[pos]) != numbers_.end()) {
				if (formula[pos] == '.')
					return false;
				token += formula[pos];
				++pos;
			}	
			tokenized_.push_back(token);
			token = "";
		}
		else if (numbers_.find(formula[pos]) != numbers_.end()) {
			while (numbers_.find(formula[pos]) != numbers_.end()) {
				token += formula[pos];
				++pos;
			}	
			tokenized_.push_back(token);
			token = "";
		}
		else if (punctuations_.find(formula[pos]) != punctuations_.end()) {
			token = QString(formula[pos]);
			tokenized_.push_back(token);
			token = "";
			++pos;
		}
		else if (operations_.find(formula[pos]) != operations_.end()) {
			token = QString(formula[pos]);
			tokenized_.push_back(token);
			token = "";
			++pos;
		}
		else
			return false;
	}
	return true;
}

bool Tokenizer::validate() const { 
	int leftbrace_count = 0;
	int rightbrace_count = 0;
	for (int pos = 0; pos != tokenized_.length(); pos++) {
		if (pos == 0) {
			if (soperations_.find(tokenized_[pos]) != soperations_.end())
				return false;	// operator at start of expression
		}
		if (soperations_.find(tokenized_[pos]) != soperations_.end()) {
			if (pos == tokenized_.length() - 1)
				return false;	// operator at end of expression
			if (soperations_.find(tokenized_[pos + 1]) != soperations_.end() ||
				tokenized_[pos + 1] == ")")
				return false;	// operation next to operation or )
		}
		else if (spunctuations_.find(tokenized_[pos]) != spunctuations_.end()) {
			if (tokenized_[pos] == "(")
				leftbrace_count++;
			else
				rightbrace_count++;
			if (rightbrace_count > leftbrace_count)
				return false;	// more right than left braces
		}
		else {
			if (pos != tokenized_.length() - 1) {
				if (soperations_.find(tokenized_[pos + 1]) == soperations_.end() &&
					spunctuations_.find(tokenized_[pos + 1]) == spunctuations_.end())
					return false; // two numbers next to each others
			}
		}
	}
	return true;
}

QVector<QString> Tokenizer::tokenized() const {
	return tokenized_;
}