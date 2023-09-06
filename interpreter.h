#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "util.h"
#include "AST.h"
#include <cmath>

class Environment {
public:
	Environment* enclosing = 0;
	Environment() {}
	Environment(Environment* enclosing) : enclosing(enclosing) {}
	std::unordered_map<std::string, Object> values;
	void Destroy() {
		if (enclosing)
			enclosing->Destroy();
	}
	Object Get(const Token& name) {
		if (values.count(name.lexeme))
			return values[name.lexeme];
		
		if (enclosing)
			return enclosing->Get(name);

		ErrorRT(name.line, "Undefined variable '" + name.lexeme + "'.");
		return Object(0.0f); // Unreachable
	}
	void Define(std::string name, Object value) {
		values[name] = value;
	}
	Object Assign(Token name, Object value) {
		if (values.count(name.lexeme)) {
			values[name.lexeme] = value;
			return value;
		}
		if (enclosing) {
			enclosing->Assign(name, value);
			return value;
		}
		ErrorRT(name.line, "Undefined variable '" + name.lexeme + "'.");
	}
};

Environment* environment = new Environment();

bool ObjIsTruthy(Object obj) {
	switch(obj.index()) {
	case TYPE_BOOLEAN:
		return std::get<TYPE_BOOLEAN>(obj);
	case TYPE_NUMBER:
		return std::get<TYPE_NUMBER>(obj) != 0;
	case TYPE_STRING:
		return std::get<TYPE_STRING>(obj).size() != 0;
	}
	return false; // Unreachable
}

bool ObjEqual(Object l, Object r) {
	if (l.index() != r.index()) return false;
	if (l.index() == TYPE_BOOLEAN && r.index() == TYPE_BOOLEAN)
		return std::get<TYPE_BOOLEAN>(l) == std::get<TYPE_BOOLEAN>(r);
	if (l.index() == TYPE_NUMBER && r.index() == TYPE_NUMBER)
		return std::get<TYPE_NUMBER>(l) == std::get<TYPE_NUMBER>(r);
	if (l.index() == TYPE_STRING && r.index() == TYPE_STRING)
		return std::get<TYPE_STRING>(l) == std::get<TYPE_STRING>(r);
	return false;
}

void CheckNumberOperand(Token op, Object right) {
	if (right.index() == TYPE_NUMBER) return;
	ErrorRT(op.line, "Expected the operand following '-' to be a number.");
}
void CheckNumberOperands(Token op, Object left, Object right) {
	if (left.index() == TYPE_NUMBER && right.index() == TYPE_NUMBER) return;
	ErrorRT(op.line, "Expected both operands of the '" + op.lexeme + "' operator to be numbers.");
}

void PrintStmt::Evaluate() {
	std::cout << ObjToStr(expr->Evaluate()) << "\n";
}

void BlockStmt::Evaluate() {
	Environment *prev = environment;
	environment = new Environment(environment);
	
	for (Stmt* stmt : statements)
		stmt->Evaluate();
	environment = prev;
}

void ExprStmt::Evaluate() {
	expr->Evaluate();
}

void VarDeclStmt::Evaluate() {
	environment->Define(identifier.lexeme, expr ? expr->Evaluate() : Object(0.0f));
}

void IfStmt::Evaluate() {
	if (ObjIsTruthy(condition->Evaluate()))
		then_branch->Evaluate();
	else if (else_branch)
		else_branch->Evaluate();
}

Object AssignExpr::Evaluate() {
	return environment->Assign(identifier, expr->Evaluate());
}

Object IfExpr::Evaluate() {
	if (ObjIsTruthy(condition->Evaluate()))
		return then_branch->Evaluate();
	else
		return else_branch->Evaluate();
}

Object LogicExpr::Evaluate() {
	Object l = left->Evaluate();
	if (op.type == TokenType::OR) {
		if (ObjIsTruthy(l)) return left;
	}
	else {
		if (!ObjIsTruthy(l)) return left;
	}
	return right->Evaluate();
}

Object BinaryExpr::Evaluate() {
	Object l = left->Evaluate();
	Object r = right->Evaluate();

	switch(op.type) {
	case TokenType::PLUS:
		if (l.index() == TYPE_NUMBER && r.index() == TYPE_NUMBER)
			return std::get<TYPE_NUMBER>(l) + std::get<TYPE_NUMBER>(r);
		if (l.index() == TYPE_STRING && r.index() == TYPE_STRING)
			return std::get<TYPE_STRING>(l) + std::get<TYPE_STRING>(r);
	case TokenType::MINUS:
		CheckNumberOperands(op, l, r);
		return std::get<TYPE_NUMBER>(l) - std::get<TYPE_NUMBER>(r);
	case TokenType::STAR:
		CheckNumberOperands(op, l, r);
		return std::get<TYPE_NUMBER>(l) * std::get<TYPE_NUMBER>(r);
	case TokenType::SLASH:
		CheckNumberOperands(op, l, r);
		return std::get<TYPE_NUMBER>(l) / std::get<TYPE_NUMBER>(r);
	case TokenType::STAR_STAR:
		CheckNumberOperands(op, l, r);
		return std::pow(std::get<TYPE_NUMBER>(l), std::get<TYPE_NUMBER>(r));

	case TokenType::EQUAL_EQUAL:
		return ObjEqual(l, r);
	case TokenType::BANG_EQUAL:
		return !ObjEqual(l, r);
	case TokenType::LESS:
		CheckNumberOperands(op, l, r);
		return std::get<TYPE_NUMBER>(l) < std::get<TYPE_NUMBER>(r);
	case TokenType::LESS_EQUAL:
		CheckNumberOperands(op, l, r);
		return std::get<TYPE_NUMBER>(l) <= std::get<TYPE_NUMBER>(r);
	case TokenType::GREATER:
		CheckNumberOperands(op, l, r);
		return std::get<TYPE_NUMBER>(l) > std::get<TYPE_NUMBER>(r);
	case TokenType::GREATER_EQUAL:
		CheckNumberOperands(op, l, r);
		return std::get<TYPE_NUMBER>(l) >= std::get<TYPE_NUMBER>(r);
	}
	return Object(); // Unreachable
}

Object VarExpr::Evaluate() {
	return environment->Get(identifier);
}

Object UnaryExpr::Evaluate() {
	Object r = right->Evaluate();
	switch(op.type) {
	case TokenType::BANG:
		return !ObjIsTruthy(r);
	case TokenType::MINUS:
		CheckNumberOperand(op, r);
		return -std::get<TYPE_NUMBER>(r);
	}
	return Object(); // Unreachable
}

Object GroupExpr::Evaluate() {
	return expr->Evaluate();
}

Object LiteralExpr::Evaluate() {
	return value;
}

#endif
