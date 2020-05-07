#include <memory>
#include <set>
#include <QDebug>
#include "expression.h"

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

