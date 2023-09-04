#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "util.h"
#include "AST.h"

static std::unordered_map<std::string, float> var_stack;

float PrintNode::evaluate() {
	float expr_val = expr->evaluate();
	std::cout << expr_val << "\n";
	return 0;
}
float AssignNode::evaluate() {
	var_stack[identifier] = expr->evaluate();
	return 0;
}
float BinaryNode::evaluate() {
	switch(op) {
	case TokenType::PLUS:
		return left->evaluate() + right->evaluate();
	case TokenType::MINUS:
		return left->evaluate() - right->evaluate();
	case TokenType::STAR:
		return left->evaluate() * right->evaluate();
	case TokenType::SLASH:
		return left->evaluate() / right->evaluate();
	default:
		return 0;
	}
}
float GroupingNode::evaluate() {
	return expr->evaluate();
}
float LiteralNode::evaluate() {
	return value;
}
float UnaryNode::evaluate() {
	switch(op) {
	case TokenType::MINUS:
		return -right->evaluate();
	default:
		return 0;
	}
}
float VarNode::evaluate() {
	if (var_stack.count(identifier.lexeme))
		return var_stack[identifier.lexeme];
	Error(identifier.line, "Runtime error: Undefined identifier: '" + identifier.lexeme + "'");
	exit(0);
}

#endif
