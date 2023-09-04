#ifndef AST_H
#define AST_H

#include "util.h"
#include "token.h"

class Node {
private:
public:
	virtual std::string str() = 0;
	virtual float evaluate() = 0;
};

class PrintNode : public Node {
public:
	Node* expr = 0;
	std::string str() {
		return "(print " + (expr ? expr->str() : "") + ")";
	}
	float evaluate();
};

class AssignNode : public Node {
public:
	std::string identifier;
	Node* expr;
	std::string str() {
		return "(assign " + identifier + " " + expr->str() + ")";
	}
	float evaluate();
};

class BinaryNode : public Node {
public:
	TokenType op;
	Node* left = 0;
	Node* right = 0;
	std::string str() {
		return "(" + Token::TypeStr(op) + " " + left->str() + " " + right->str() + ")";
	}
	float evaluate();
};

class GroupingNode : public Node {
public:
	Node* expr = 0;
	std::string str() {
		return "(group " + expr->str() + ")";
	}
	float evaluate();
};

class LiteralNode : public Node {
public:
	float value;
	std::string str() {
		return std::to_string(value);
	}
	float evaluate();
};

class UnaryNode : public Node {
public:
	TokenType op;
	Node* right = 0;
	std::string str() {
		return "(" + Token::TypeStr(op) + " " + right->str() + ")";
	}
	float evaluate();
};

class VarNode : public Node {
public:
	Token identifier;
	std::string str() {
		return identifier.lexeme;
	}
	float evaluate();
};

#endif
