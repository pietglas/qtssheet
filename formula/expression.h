#pragma once

#include "tokenizer.h"
#include <memory>

// Constructs an expression tree from a qvector of 
// tokens. Assumes that the formula is syntactically 
// correct. 
class Expression {
public:
	Expression(QVector<QString> & tokens);
	std::shared_ptr<Expression> lhs();
	std::shared_ptr<Expression> rhs();
	QString token() const;
	void print() const;
private:
	QString token_;
	std::shared_ptr<Expression> lhs_;
	std::shared_ptr<Expression> rhs_;
};