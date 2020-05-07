#include <QVector>
#include <QString>
#include <QMap>
#include <QPair>
#include <QChar>
#include <QDebug>
#include <set>
#include <math.h>
#include "/home/piet/Projects/cs106bx/qtssheet/tokenizer.h"
#include "/home/piet/Projects/cs106bx/qtssheet/expression.h"

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

int precedence(const QString & oper);
// Returns the position of the operator with the least
// precedence. If there are several, it returns the 
// leftmost one. 
// Assumes the the expression corresponding to `tokens`
// is not enclosed in braces, otherwise it doesn't behave
// well. 
int least_precedence_operator(const QVector<QString> & tokens);

Expression::Expression(QVector<QString> & tokens) {
	if (tokens.size() == 1) {
		token_ = tokens[0];
		rhs_ = nullptr;
		lhs_ = nullptr;
	}
	else {
		// check for enclosing braces first
		if ((tokens[0] == "(") && (tokens.last() == ")"))
			tokens = tokens.mid(1, tokens.length() - 2);
		int pos = least_precedence_operator(tokens);
		token_ = tokens[pos];
		QVector<QString> lhs = tokens.mid(0, pos);
		QVector<QString> rhs = tokens.mid(pos + 1, -1);
		lhs_ = std::make_shared<Expression>(lhs);
		rhs_ = std::make_shared<Expression>(rhs);
	}
}

void Expression::print() const {
	qDebug() << token_;
	if (lhs_ != nullptr) {	// either both are null, or neither
		lhs_->print();
		rhs_->print();
	}
}

std::shared_ptr<Expression> Expression::lhs() {
	return lhs_;
}

std::shared_ptr<Expression> Expression::rhs() {
	return rhs_;
}

QString Expression::token() const {
	return token_;
}

int precedence(const QString & oper) {
	if (oper == "=")
		return 0;
	else if (oper == "+" || oper == "-")
		return 1;
	else if (oper == "*" || oper == "/")
		return 2;
	else	// if oper == ^
		return 3;
}

int least_precedence_operator(const QVector<QString> & tokens) {
	int pos = -1;
	int current_precedence = 1e6;
	int depth = 0;
	std::set<QString> operators{"=", "+", "-", "/", "*", "^"};
	for (int i = 0; i != tokens.length(); i++) {
		if (tokens[i] == "(")
			depth += 10;	// operators within braces have less precedence
		else if (tokens[i] == ")")
			depth -= 10;
		else if (operators.find(tokens[i]) != operators.end()) {
			if (precedence(tokens[i]) + depth < current_precedence) {
				pos = i;
				current_precedence = precedence(tokens[pos]);
				if (current_precedence == 0)
					break; 
			}
		}
		else
			continue;
	}
	return pos;
}

QString alph_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";	// for column index

QVector<QVector<double>> m_grid_data_;

QMap<QString, QVector<QString>> formulas_;

QPair<int, int> convertStrToIndex(const QString & index) {
	int col = 0;
	while (alph_[col] != index[0]) 
		++col;
	int row = index.mid(1, -1).toInt() - 1;
	return qMakePair(row, col);
}

double calculateFormula(std::shared_ptr<Expression> formula) {
	if (formula->lhs() == nullptr) {
		bool ok;
		double try_conv = formula->token().toDouble(&ok);
		if (ok) 
			return try_conv;
		else {
			QPair<int, int> index = convertStrToIndex(formula->token());
			return m_grid_data_[index.first][index.second];
		}
	}
	else {
		if (formula->token() == "+")
			return calculateFormula(formula->lhs()) + 
					calculateFormula(formula->rhs());
		else if (formula->token() == "-")
			return calculateFormula(formula->lhs()) - 
					calculateFormula(formula->rhs());
		else if (formula->token() == "*")
			return calculateFormula(formula->lhs()) * 
					calculateFormula(formula->rhs());
		else if (formula->token() == "/")
			return calculateFormula(formula->lhs()) / 
					calculateFormula(formula->rhs());
		else
			return std::pow(calculateFormula(formula->lhs()),
						calculateFormula(formula->rhs()));
	}
}

bool setFormula(const QString & formula) {
	Tokenizer tokenizer;
	if (tokenizer.tokenize(formula)) {
		if (tokenizer.validate()) {
			// update existing formula or add a new one
			QString key = tokenizer.tokenized()[0];
			QVector<QString> value = tokenizer.tokenized().mid(2, -1);
			formulas_.insert(key, value);

			// update the actual data displayed
			auto formula_ptr = std::make_shared<Expression>(value);
			double val = calculateFormula(formula_ptr);
			QPair<int, int> index = convertStrToIndex(key);
			m_grid_data_[index.first][index.second] = val;

			return true;
		}
		return false;
	}
	return false;
}



int main() {
	m_grid_data_.resize(3);
	for (int i = 0; i != 3; i++) {
		m_grid_data_[i].resize(3);
		for (int j = 0; j != 3; j++)
			m_grid_data_[i][j] = i;
	}

	QString formula1 = "A2 = A1 + B2 * C2";
	Tokenizer get_tokens;

	if (setFormula(formula1))
		qDebug() << m_grid_data_;
	else
		qDebug() << "something went wrong during calculation";


	QString formula2 = "B2 = C2*(B3^C3 + 0.2)/ 3";
	if (get_tokens.tokenize(formula2)) { 
		if (get_tokens.validate()) {
			QVector<QString> tokens_2 = get_tokens.tokenized();
			qDebug() << tokens_2;
			Expression expression2(tokens_2);
			expression2.print();
			setFormula(formula2);
		}
		else
			qDebug() << "formula syntactically incorrect";
	}

	QString formula3 = "A1 ^ (2) + (24 +)";
	if (get_tokens.tokenize(formula3)) {
		if (get_tokens.validate()) {
			QVector<QString> tokens_3 = get_tokens.tokenized();
			qDebug() << tokens_3;
		}
		else
			qDebug() << "formula syntactically incorrect";
	}
	else
		qDebug() << "a non-token was entered";

	QString formula4 = "A3 + B2*";
	if (get_tokens.tokenize(formula4)) { 
		if (get_tokens.validate()) {
			QVector<QString> tokens = get_tokens.tokenized();
			qDebug() << tokens;
		}
		else
			qDebug() << "formula syntactically incorrect";
	}
	else
		qDebug() << "a non-token was entered";



}