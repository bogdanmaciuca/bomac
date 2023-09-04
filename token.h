#ifndef TOKEN_H
#define TOKEN_H

#include "util.h"
#undef EOF
enum class TokenType {
	EOF = 0,
	EQUAL, PLUS, MINUS, STAR, SLASH,
	LEFT_PARAN, RIGHT_PARAN,
	IDENTIFIER, COLON,
	INT, PRINT,
	NUMBER
};

struct Token {
	TokenType type;
	std::string lexeme;
	float literal = 0;
	u16 line = 0;
	static std::string TypeStr(TokenType _type) {
		std::string type_str;
		switch(_type) {
			case TokenType::EQUAL: type_str = "EQUAL"; break;
			case TokenType::PLUS: type_str = "PLUS"; break;
			case TokenType::MINUS: type_str = "MINUS"; break;
			case TokenType::STAR: type_str = "STAR"; break;
			case TokenType::SLASH: type_str = "SLASH"; break;
			case TokenType::LEFT_PARAN: type_str = "LEFT_PARAN"; break;
			case TokenType::RIGHT_PARAN: type_str = "RIGHT_PARAN"; break;
			case TokenType::IDENTIFIER: type_str = "IDENTIFIER"; break;
			case TokenType::COLON: type_str = "COLON"; break;
			case TokenType::INT: type_str = "INT"; break;
			case TokenType::PRINT: type_str = "PRINT"; break;
			case TokenType::NUMBER: type_str = "NUMBER"; break;
			default: type_str = "ERROR_TYPE"; break;
		}
		return type_str;
	}
	std::string TypeStr() {
		return TypeStr(type);
	}
	std::string str() {
		return TypeStr() + " " + lexeme + " " + std::to_string(literal);
	}
	void Print() {
		std::cout << str() << "\n";
	}
};
#endif
