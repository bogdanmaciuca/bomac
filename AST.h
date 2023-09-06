#ifndef AST_H
#define AST_H

#include "util.h"
#include "token.h"

enum class NodeType {
	PRINT_STMT = 0,
	BLOCK_STMT,
	EXPR_STMT,
	VAR_DECL_STMT,
	IF_STMT,
	ASSIGN_EXPR,
	IF_EXPR,
	LOGIC_EXPR,
	BINARY_EXPR,
	GROUP_EXPR,
	UNARY_EXPR,
	VAR_EXPR,
	LITERAL_EXPR
};

class Expr {
public:
	virtual NodeType Type() = 0;
	virtual std::string Str() = 0;
	virtual Object Evaluate() = 0;
	virtual void Destroy() = 0;
};

class Stmt {
public:
	virtual NodeType Type() = 0;
	virtual std::string Str() = 0;
	virtual void Evaluate() = 0;
	virtual void Destroy() = 0;
};

class PrintStmt : public Stmt {
public:
	Expr* expr = 0;
	PrintStmt(Expr *expr) : expr(expr) {}
	NodeType Type() { return NodeType::PRINT_STMT; }
	void Destroy() {
		if (expr == 0) return;
		expr->Destroy();
		delete expr;
	}
	std::string Str() {
		return "(print " + (expr ? expr->Str() : "") + ")";
	}
	void Evaluate();
};

class BlockStmt : public Stmt {
public:
	std::vector<Stmt*> statements;
	BlockStmt(const std::vector<Stmt*>& statements) : statements(statements) {}
	NodeType Type() { return NodeType::BLOCK_STMT; }
	void Destroy() {
		for (Stmt* stmt : statements) {
			stmt->Destroy();
			delete stmt;
		}
	}
	std::string Str() {
		std::string result = "(block";
		for (Stmt* stmt : statements)
			result += " " + stmt->Str();
		result += ")";
		return result;
	}
	void Evaluate();
};

class ExprStmt : public Stmt {
public:
	Expr* expr = 0;
	ExprStmt(Expr *expr) : expr(expr) {}
	NodeType Type() { return NodeType::EXPR_STMT; }
	void Destroy() {
		if (expr == 0) return;
		expr->Destroy();
		delete expr;
	}
	std::string Str() {
		return "(exprStatement " + (expr ? expr->Str() : "") + ")";
	}
	void Evaluate();
};

class VarDeclStmt : public Stmt {
public:
	Token identifier;
	Expr* expr = 0;
	VarDeclStmt(Token identifier, Expr* expr) : identifier(identifier), expr(expr) {}
	NodeType Type() { return NodeType::VAR_DECL_STMT; }
	void Destroy() {
		if (expr == 0) return;
		expr->Destroy();
		delete expr;
	}
	std::string Str() {
		return "(decl " + identifier.lexeme + " " + (expr ? expr->Str() : "") + ")";
	}
	void Evaluate();
};

class IfStmt : public Stmt {
public:
	Expr* condition = 0;
	Stmt* then_branch = 0;
	Stmt* else_branch = 0;
	IfStmt(Expr* condition, Stmt* then_branch, Stmt* else_branch)
		: condition(condition), then_branch(then_branch), else_branch(else_branch) {}
	NodeType Type() { return NodeType::IF_STMT; }
	void Destroy() {
		if (condition) { condition->Destroy(); delete condition; }
		if (then_branch) { then_branch->Destroy(); delete then_branch; }
		if (else_branch) { else_branch->Destroy(); delete else_branch; }
	}
	std::string Str() {
		return condition->Str();
	}
	void Evaluate();
};


class AssignExpr : public Expr {
public:
	Token identifier;
	Expr* expr = 0;
	AssignExpr(Token identifier, Expr* expr) : identifier(identifier), expr(expr) {}
	NodeType Type() { return NodeType::ASSIGN_EXPR; }
	void Destroy() {
		if (expr == 0) return;
		expr->Destroy();
		delete expr;
	}
	std::string Str() {
		return "(assign " + identifier.lexeme + " " + expr->Str() + ")";
	}
	Object Evaluate();
};

class IfExpr : public Expr {
public:
	Expr* condition = 0;
	Expr* then_branch = 0;
	Expr* else_branch = 0;
	IfExpr(Expr* condition, Expr* then_branch, Expr* else_branch)
		: condition(condition), then_branch(then_branch), else_branch(else_branch) {}
	NodeType Type() { return NodeType::IF_EXPR; }
	void Destroy() {
		if (condition) { condition->Destroy(); delete condition; }
		if (then_branch) { then_branch->Destroy(); delete then_branch; }
		if (else_branch) { else_branch->Destroy(); delete else_branch; }
	}
	std::string Str() {
		return condition->Str();
	}
	Object Evaluate();
};

class LogicExpr : public Expr {
public:
	Token op;
	Expr* left = 0;
	Expr* right = 0;
	LogicExpr(Token op, Expr* left, Expr* right)
		: op(op), left(left), right(right) {}
	NodeType Type() { return NodeType::LOGIC_EXPR; }
	void Destroy() {
		if (left) { left->Destroy(); delete left; }
		if (right) { right->Destroy(); delete right; }
	}
	std::string Str() {
		return "LOGIC";
	}
	Object Evaluate();
};

class BinaryExpr : public Expr {
public:
	Token op;
	Expr* left = 0;
	Expr* right = 0;
	BinaryExpr(Token op, Expr* left, Expr* right) : op(op), left(left), right(right) {}
	NodeType Type() { return NodeType::BINARY_EXPR; }
	void Destroy() {
		if (left) { left->Destroy(); delete left; }
		if (right) { right->Destroy(); delete right; }
	}
	std::string Str() {
		return "(" + op.TypeStr() + " " + left->Str() + " " + right->Str() + ")";
	}
	Object Evaluate();
};

class GroupExpr : public Expr {
public:
	Expr* expr = 0;
	GroupExpr(Expr* expr) : expr(expr) {}
	NodeType Type() { return NodeType::GROUP_EXPR; }
	void Destroy() {
		if (expr == 0) return;
		expr->Destroy();
		delete expr;
	}
	std::string Str() {
		return "(group " + expr->Str() + ")";
	}
	Object Evaluate();
};

class UnaryExpr : public Expr {
public:
	Token op;
	Expr* right = 0;
	UnaryExpr(Token op, Expr* right) : op(op), right(right) {}
	NodeType Type() { return NodeType::UNARY_EXPR; }
	void Destroy() {
		if (right == 0) return;
		right->Destroy();
		delete right;
	}
	std::string Str() {
		return "(" + op.TypeStr() + " " + right->Str() + ")";
	}
	Object Evaluate();
};

class VarExpr : public Expr {
public:
	Token identifier;
	VarExpr(Token identifier) : identifier(identifier) {}
	NodeType Type() { return NodeType::VAR_EXPR; }
	void Destroy() {}
	std::string Str() {
		return identifier.lexeme;
	}
	Object Evaluate();
};

class LiteralExpr : public Expr {
public:
	Object value;
	LiteralExpr(Object value) : value(value) {}
	NodeType Type() { return NodeType::LITERAL_EXPR; }
	void Destroy() {}
	std::string Str() {
		return ObjToStr(value);
	}
	Object Evaluate();
};
#endif
