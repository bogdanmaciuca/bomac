#ifndef AST_H
#define AST_H

#include "util.h"
#include "token.h"

enum class NodeType {
	PRINT_STMT = 0,
	BLOCK_STMT,
	EXPR_STMT,
	VAR_DECL_STMT,
	ASSIGN_EXPR,
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
		free(expr);
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
		for (Stmt* stmt : statements)
			stmt->Destroy();
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
		free(expr);
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
		free(expr);
	}
	std::string Str() {
		return "(decl " + identifier.lexeme + " " + (expr ? expr->Str() : "") + ")";
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
		free(expr);
	}
	std::string Str() {
		return "(assign " + identifier.lexeme + " " + expr->Str() + ")";
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
		if (left != 0) {
			left->Destroy();
			free(left);
		}
		if (right != 0) {
			right->Destroy();
			free(right);
		}
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
		free(expr);
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
		free(right);
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
