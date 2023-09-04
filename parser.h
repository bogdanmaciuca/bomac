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
	std::vector<Node*> nodes;
	// TODO: eliminate copying of the vector | properly free the memory
	void Parse(std::vector<Token> toks) {
		tokens = toks;
		current = 0;
		had_error = false;
		nodes.clear();
		while(!AtEnd()) {
			ParseExpr();
		}
	}
private:
	Node* Print() {
		PrintNode *print = new PrintNode;
		print->expr = Expr();
		Consume(TokenType::COLON, "Expected ';' after statement");
		return (Node*)print;
	}
	Node* Expr() {
		return Term();
	}
	Node* Term() {
		Node* left = Factor();
		BinaryNode* expr = (BinaryNode*)left;
		while (Match({TokenType::MINUS, TokenType::PLUS})) {
			Token op = Prev();
			Node* right = Factor();
			expr = new BinaryNode();
			expr->left = left;
			expr->op = op.type;
			expr->right = right;
			left = expr;
		}
		return expr;
	}
	Node* Factor() {
		Node* left = Unary();
		BinaryNode* expr = (BinaryNode*)left;
		while (Match({TokenType::STAR, TokenType::SLASH})) {
			Token op = Prev();
			Node* right = Unary();
			expr = new BinaryNode();
			expr->left = left;
			expr->op = op.type;
			expr->right = right;
			left = expr;
		}
		return expr;
	}
	Node* Unary() {
		if (Match({TokenType::MINUS})) {
			Token op = Prev();
			Node* right = Unary();
			UnaryNode* unary = new UnaryNode;
			unary->op = op.type;
			unary->right = right;
			return unary;
    	}
		return Primary();
	}
	Node* Primary() {
		if (Match({TokenType::NUMBER})) {
			LiteralNode* literal = new LiteralNode;
			literal->value = Prev().literal;
			return literal;
		}
		if (Match({TokenType::IDENTIFIER})) {
			VarNode* var = new VarNode;
			var->identifier = Prev();
			return var;
		}
		if (Match({TokenType::LEFT_PARAN})) {
			Node* expr = Expr();
			Consume(TokenType::RIGHT_PARAN, "Expected ')' after expression");
			GroupingNode* grouping = new GroupingNode;
			grouping->expr = expr;
			return grouping;
		}
		Advance();
		Error(Prev().line, "Unexpected token: '" + Prev().lexeme + "'");
		had_error = true;
		return 0;
	}
	Node* Assign() {
		AssignNode* assign = new AssignNode;
		assign->identifier = Prev().lexeme;
		Consume(TokenType::EQUAL, "Expected an assignment");
		assign->expr = Expr();
		Consume(TokenType::COLON, "Expected ';' after statement");
		return assign;
	}
	void ParseExpr() {
		Token tok = Advance();
		switch(tok.type) {
		case TokenType::PRINT:
			nodes.push_back((Node*)Print());
			break;
		case TokenType::IDENTIFIER:
			nodes.push_back((Node*)Assign());
			break;
		default:
			break;
		}
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
	Token Consume(TokenType type, const std::string& message) {
		if (Check(type))
			return Advance();
		Error(Peek().line, message);
		had_error = true;
		return Token();
	}
};
#endif
