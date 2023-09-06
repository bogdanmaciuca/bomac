#ifndef TOKEN_H
#define TOKEN_H

#include "util.h"
#undef EOF

enum class TokenType {
	EOF = 0,
	BANG, EQUAL, PLUS, MINUS, STAR, SLASH, STAR_STAR,

	EQUAL_EQUAL, BANG_EQUAL, LESS,
	GREATER, LESS_EQUAL, GREATER_EQUAL,
	MINUS_GREATER,

	LEFT_PAREN, RIGHT_PAREN,
	LEFT_BRACKET, RIGHT_BRACKET,
	LEFT_BRACE, RIGHT_BRACE,

	IDENTIFIER, SEMICOLON, IF, WHILE, FOR,
	VAR, PRINT, TRUE, FALSE, AND, OR,
	CLASS, FN, RETURN, NUMBER, STRING
};

struct Token {
	TokenType type;
	std::string lexeme;
	Object literal;
	u16 line = 0;
	static std::string TypeStr(TokenType _type) {
		std::string type_str;
		switch(_type) {
			case TokenType::EOF: type_str = "EOF"; break;
			case TokenType::BANG: type_str = "BANG"; break;
			case TokenType::EQUAL: type_str = "EQUAL"; break;
			case TokenType::PLUS: type_str = "PLUS"; break;
			case TokenType::MINUS: type_str = "MINUS"; break;
			case TokenType::STAR: type_str = "STAR"; break;
			case TokenType::SLASH: type_str = "SLASH"; break;
			case TokenType::STAR_STAR: type_str = "STAR_STAR"; break;
			case TokenType::EQUAL_EQUAL: type_str = "EQUAL_EQUAL"; break;
			case TokenType::BANG_EQUAL: type_str = "BANG_EQUAL"; break;
			case TokenType::LESS: type_str = "LESS"; break;
			case TokenType::GREATER: type_str = "GREATER"; break;
			case TokenType::LESS_EQUAL: type_str = "LESS_EQUAL"; break;
			case TokenType::GREATER_EQUAL: type_str = "GREATER_EQUAL"; break;
			case TokenType::LEFT_PAREN: type_str = "LEFT_PAREN"; break;
			case TokenType::RIGHT_PAREN: type_str = "RIGHT_PAREN"; break;
			case TokenType::LEFT_BRACKET: type_str = "LEFT_BRACKET"; break;
			case TokenType::RIGHT_BRACKET: type_str = "RIGHT_BRACKET"; break;
			case TokenType::LEFT_BRACE: type_str = "LEFT_BRACE"; break;
			case TokenType::RIGHT_BRACE: type_str = "RIGHT_BRACE"; break;
			case TokenType::IDENTIFIER: type_str = "IDENTIFIER"; break;
			case TokenType::SEMICOLON: type_str = "COLON"; break;
			case TokenType::VAR: type_str = "VAR"; break;
			case TokenType::IF: type_str = "IF"; break;
			case TokenType::WHILE: type_str = "WHILE"; break;
			case TokenType::FOR: type_str = "FOR"; break;
			case TokenType::PRINT: type_str = "PRINT"; break;
			case TokenType::NUMBER: type_str = "NUMBER"; break;
			case TokenType::STRING: type_str = "STRING"; break;
			case TokenType::CLASS: type_str = "CLASS"; break;
			case TokenType::FN: type_str = "FN"; break;
			case TokenType::RETURN: type_str = "RETURN"; break;
			case TokenType::AND: type_str = "AND"; break;
			case TokenType::OR: type_str = "OR"; break;
			case TokenType::TRUE: type_str = "TRUE"; break;
			case TokenType::FALSE: type_str = "FALSE"; break;
			default: type_str = "ERROR_TYPE"; break;
		}
		return type_str;
	}
	std::string TypeStr() {
		return TypeStr(type);
	}
	std::string str() {
		return TypeStr() + " " + lexeme + " " + ObjToStr(literal);
	}
	void Print() {
		std::cout << str() << "\n";
	}
};
#endif
