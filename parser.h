#ifndef PARSER_H
#define PARSER_H

#include "util.h"
#include "AST.h"
#include <initializer_list>
#include <stdexcept>

class Parser {
private:
	std::vector<Token> tokens;
	u32 current = 0;
	bool had_error = false;
	u8 loop_count = 0; // To prevent break and continue statements from appearing outside a loop
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
			try {
				statements.push_back(Declaration());
			} catch (std::exception e) {
				Synchronize();
			}
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
			}
			Advance();
		}
	}
	Expr* Expression() {
		return Assignment();
	}
	Expr* Assignment() {
		Expr *expr = IfExpression();

		if (Match({TokenType::EQUAL})) {
			Token equals = Prev();
			Expr *value = IfExpression();

			if (expr->Type() == NodeType::VAR_EXPR) {
				Token identifier = ((VarExpr*)expr)->identifier;
				return new AssignExpr(identifier, value);
			}

			Error(equals.line, "Invalid l-value.");
			throw std::runtime_error("Parser error");
			had_error = true;
		}
		return expr;
	}
	Expr* IfExpression() {
		if (Match({TokenType::IF})) {
			Consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
			Expr* condition = Expression();
			Consume(TokenType::RIGHT_PAREN, "Expected ')' after condition.");

			Expr* then_branch = Expression();
			Consume(TokenType::ELSE, "Expected 'else' after first branch of the 'if' expression.");
			Expr* else_branch = Expression();
			
			return new IfExpr(condition, then_branch, else_branch);
		}
		return Or();
	}
	Expr* Or() {
		Expr* expr = And();
		while (Match({TokenType::OR})) {
			Token op = Prev();
			Expr* right = And();
			expr = new LogicExpr(op, expr, right);
		}
		return expr;
	}
	Expr* And() {
		Expr* expr = Equality();
		while (Match({TokenType::AND})) {
			Token op = Prev();
			Expr* right = Equality();
			expr = new LogicExpr(op, expr, right);
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
		while (Match({TokenType::STAR, TokenType::SLASH, TokenType::MODULO})) {
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
		return Prefix();
	}
	Expr* Prefix() {
		if (Match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS})) {
			Token op = Prev();
			Expr* expr = Primary();
			return new UnaryExpr(op, expr, false);
		}

		return Postfix();
	}
	Expr* Postfix() {
		Expr* expr = Primary();
		if (Match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS}))
			return new UnaryExpr(Prev(), expr, true);
		return expr;
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
		Advance();
		Error(Prev().line, "Unexpected token: '" + Prev().lexeme + "'.");
		had_error = true;
		throw std::runtime_error("Parser error");
		return new LiteralExpr(Object(0.0f)); // Placeholder expression so parser doesn't crash
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
		else if (Match({TokenType::LEFT_BRACE}))
			return new BlockStmt(Block());
		else if (Match({TokenType::IF}))
			return If();
		else if (Match({TokenType::WHILE}))
			return While();
		else if (Match({TokenType::FOR}))
			return For();
		else if (Match({TokenType::BREAK}))
			return Break();
		else if (Match({TokenType::CONTINUE}))
			return Continue();

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
	Stmt* If() {
		Consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
		Expr* condition = Expression();
		Consume(TokenType::RIGHT_PAREN, "Expected ')' after condition.");

		Stmt* then_branch = Statement();
		Stmt* else_branch = 0;
		if (Match({TokenType::ELSE}))
			else_branch = Statement();
		
		return new IfStmt(condition, then_branch, else_branch);
	}
	Stmt* While() {
		loop_count++;
		Consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");
		Expr* expr = Expression();
		Consume(TokenType::RIGHT_PAREN, "Expected ')' after condition.");
		Stmt* stmt = Statement();
		loop_count--;
		return new WhileStmt(expr, stmt);
	}
	Stmt* For() {
		loop_count++;
		Consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'.");
		
		Stmt* initializer;
		if (Match({TokenType::SEMICOLON}))
			initializer = 0;
		else if (Match({TokenType::VAR}))
			initializer = VarDecl();
		else
			initializer = ExpressionStmt();
		
		Expr* condition = 0;
		if (!Check(TokenType::SEMICOLON))
			condition = Expression();
		Consume(TokenType::SEMICOLON, "Expected ';' after loop condition.");

		Expr* increment = 0;
		if (!Check({TokenType::RIGHT_PAREN}))
			increment = Expression();
		Consume(TokenType::RIGHT_PAREN, "Expected ')' after for clauses.");

		Stmt* body = Statement();

		loop_count--;
		return new ForStmt(initializer, condition, increment, body);
	}
	Stmt* Break() {
		if (loop_count == 0)
			Error(Prev().line, "'break' statements must be inside a loop.");
		Consume(TokenType::SEMICOLON, "Expected ';' after 'break' statement.");
		return new BreakStmt;
	}
	Stmt* Continue() {
		if (loop_count == 0)
			Error(Prev().line, "'continue' statements must be inside a loop.");
		Consume(TokenType::SEMICOLON, "Expected ';' after 'continue' statement.");
		return new ContinueStmt;
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
	void Error(u16 line, const std::string &message) {
		std::cout << "Error on line " << line << ": " << message << "\n";
		had_error = true;
		throw std::runtime_error(message);
	}
	Token Consume(TokenType type, const std::string& message) {
		if (Check(type))
			return Advance();
		Error(Peek().line, message);
		return Token();
	}
};
#endif
