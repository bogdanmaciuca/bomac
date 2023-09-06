#ifndef PARSER_H
#define PARSER_H

#include "util.h"
#include "AST.h"
#include <initializer_list>

class Parser {
private:
	std::vector<Token> tokens;
	u32 current = 0;
	bool had_error = false;
public:
	bool HadError() { return had_error; }
	std::vector<Stmt*> statements;
	// TODO: eliminate copying of the vector
	void Parse(const std::vector<Token> &toks) {
		tokens = toks;
		current = 0;
		had_error = false;
		statements.clear();
		while(!AtEnd()) {
			statements.push_back(Declaration());
			if (had_error) Synchronize();
		}
	}
private:
	void Synchronize() {
		Advance();
		while (!AtEnd()) {
			if (Prev().type == TokenType::SEMICOLON)
				return;
			switch (Peek().type) {
			case TokenType::CLASS:
			case TokenType::FN:
			case TokenType::VAR:
			case TokenType::FOR:
			case TokenType::IF:
			case TokenType::WHILE:
			case TokenType::PRINT:
			case TokenType::RETURN:
				return;
			default:
				break;
			}
			Advance();
		}
	}
	Expr* Expression() {
		return Assignment();
	}
	Expr* Assignment() {
		Expr *expr = Equality();

		if (Match({TokenType::EQUAL})) {
			Token equals = Prev();
			Expr *value = Assignment();

			if (expr->Type() == NodeType::VAR_EXPR) {
				Token identifier = ((VarExpr*)expr)->identifier;
				return new AssignExpr(identifier, value);
			}

			Error(equals.line, "Invalid l-value."); 
		}
		return expr;
	}
	Expr* Equality() {
		Expr* expr = Comparison();
		while (Match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
			Token op = Prev();
			Expr* right = Comparison();
			expr = new BinaryExpr(op, expr, right);
		}
		return expr;
	}
	Expr* Comparison() {
		Expr* expr = Term();
		while (Match({TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL})) {
			Token op = Prev();
			Expr* right = Term();
			expr = new BinaryExpr(op, expr, right);
		}
		return expr;
	}
	Expr* Term() {
		Expr* expr = Factor();
		while (Match({TokenType::PLUS, TokenType::MINUS})) {
			Token op = Prev();
			Expr* right = Factor();
			expr = new BinaryExpr(op, expr, right);
		}
		return expr;
	}
	Expr* Factor() {
		Expr* expr = Power();
		while (Match({TokenType::STAR, TokenType::SLASH})) {
			Token op = Prev();
			Expr* right = Power();
			expr = new BinaryExpr(op, expr, right);
		}
		return expr;
	}
	Expr* Power() {
		Expr* expr = Unary();
		while (Match({TokenType::STAR_STAR})) {
			Token op = Prev();
			Expr* right = Power();
			expr = new BinaryExpr(op, expr, right);
		}
		return expr;
	}
	Expr* Unary() {
		if (Match({TokenType::BANG, TokenType::MINUS})) {
			Token op = Prev();
			Expr* right = Unary();
			return new UnaryExpr(op, right);
		}
		return Primary();
	}
	Expr* Primary() {
		if (Match({TokenType::FALSE})) return new LiteralExpr(false);
		if (Match({TokenType::TRUE})) return new LiteralExpr(true);

		if (Match({TokenType::NUMBER, TokenType::STRING}))
			return new LiteralExpr(Prev().literal);
		
		if (Match({TokenType::IDENTIFIER}))
			return new VarExpr(Prev());

		if (Match({TokenType::LEFT_PAREN})) {
			Expr *expr = Expression();
			Consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
			return new GroupExpr(expr);
		}
		return 0; // Unreachable unless an error is made by the user
	}
	Stmt* Declaration() {
		if (Match({TokenType::VAR}))
			return VarDecl();
		return Statement();
	}
	Stmt* VarDecl() {
		Consume(TokenType::IDENTIFIER, "Expected an identifier.");
		Token identifier = Prev();
		Expr* expr = 0;
		if (Match({TokenType::EQUAL}))
			expr = Expression();
		Consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
		return new VarDeclStmt(identifier, expr);
	}
	Stmt* Statement() {
		if (Match({TokenType::PRINT}))
			return Print();
		if (Match({TokenType::LEFT_BRACE}))
			return new BlockStmt(Block());
		return ExpressionStmt();
	}
	std::vector<Stmt*> Block() {
		std::vector<Stmt*> statements;
		while (!Check(TokenType::RIGHT_BRACE) && !AtEnd())
			statements.push_back(Declaration());
		Consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
	    return statements;
	}
	Stmt* Print() {
		Expr* expr = Expression();
		Consume(TokenType::SEMICOLON, "Expected ';' after print statement.");
		return new PrintStmt(expr);
	}
	Stmt* ExpressionStmt() {
		Expr *expr = Expression();
		Consume(TokenType::SEMICOLON, "Expected ';' after expression.");
		return new ExprStmt(expr);
	}
	bool AtEnd() {
		return Peek().type == TokenType::EOF;
	}
	Token Peek() {
		return tokens[current];
	}
	Token Prev() {
		return tokens[current-1];
	}
	bool Check(TokenType type) {
		if (AtEnd())
			return false;
		return Peek().type == type;
	}
	Token Advance() {
		if (!AtEnd())
			current++;
		return tokens[current-1];
	}
	bool Match(std::initializer_list<TokenType> types) {
		for (auto type : types) {
			if (Check(type)) {
				Advance();
				return true;
			}
		}
		return false;
	}
	Token Consume(TokenType type, const std::string& message, bool panic_mode = true) {
		if (Check(type))
			return Advance();
		Error(Peek().line, message);
		had_error = true;
		if (panic_mode) Synchronize();
		return Token();
	}
};
#endif
