#ifndef LEXER_H
#define LEXER_H

#include "util.h"
#include "token.h"

class Lexer {
public:
	std::vector<Token> tokens;
	bool Lex(const std::string source) {
		mSource = source;
		mStart = 0;
		mCurrent = 0;
		mLine = 1;
		mHadError = false;
		tokens.clear();
		mKeywords["var"] = TokenType::VAR;
		mKeywords["print"] = TokenType::PRINT;
		mKeywords["true"] = TokenType::TRUE;
		mKeywords["false"] = TokenType::FALSE;
		mKeywords["and"] = TokenType::AND;
		mKeywords["or"] = TokenType::OR;
		mKeywords["if"] = TokenType::IF;
		mKeywords["else"] = TokenType::ELSE;
		mKeywords["while"] = TokenType::WHILE;
		mKeywords["for"] = TokenType::FOR;
		mKeywords["break"] = TokenType::BREAK;
		mKeywords["continue"] = TokenType::CONTINUE;
		mKeywords["class"] = TokenType::CLASS;
		mKeywords["fn"] = TokenType::FN;
		mKeywords["return"] = TokenType::RETURN;
		ScanTokens();
		return mHadError;
	}
private:
	std::string mSource;
	u32 mStart = 0;
	u32 mCurrent = 0;
	u32 mLine = 1;
	bool mHadError = false;
	std::unordered_map<std::string, TokenType> mKeywords;
	void ScanTokens() {
		while (!AtEnd()) {
			mStart = mCurrent;
			ScanToken();
		}
		Token eof;
		eof.line = mLine;
		eof.type = TokenType::EOF;
		tokens.push_back(eof);
	}
	void ScanToken() {
		char c = Advance();
		switch (c) {
			case ' ': case '\t': case '\r': break;
			case '\n': mLine++; break;
			case '#':
				while (Peek() != '\n' && !AtEnd()) Advance();
				break;
			case '!': AddToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
			case '=': AddToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
			case '+': AddToken(Match('+') ? TokenType::PLUS_PLUS : TokenType::PLUS); break;
			case '-':
				if (Match('-')) AddToken(TokenType::MINUS_MINUS);
				else if (Match('>')) AddToken(TokenType::MINUS_GREATER);
				else AddToken(TokenType::MINUS);
				break;
			case '*': AddToken(Match('*') ? TokenType::STAR_STAR : TokenType::STAR); break;
			case '/': AddToken(TokenType::SLASH); break;
			case '%': AddToken(TokenType::MODULO); break;
			case '<': AddToken(Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
			case '>': AddToken(Match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
			case '(': AddToken(TokenType::LEFT_PAREN); break;
			case ')': AddToken(TokenType::RIGHT_PAREN); break;
			case '[': AddToken(TokenType::LEFT_BRACKET); break;
			case ']': AddToken(TokenType::RIGHT_BRACKET); break;
			case '{': AddToken(TokenType::LEFT_BRACE); break;
			case '}': AddToken(TokenType::RIGHT_BRACE); break;
			case ';': AddToken(TokenType::SEMICOLON); break;
			case '"': String(); break;
			default:
				if (isdigit(c))
					Number();
				else if (isalpha(c) || c == '_') {
					Identifier();
				}
				else {
					Error(mLine, "Unexpected character: \'" + std::string({c}) + std::string("\'."));
					mHadError = true;
				}
				break;
		}
	}
	void Number() {
		while (isdigit(Peek())) Advance();
		if (Peek() == '.' && isdigit(PeekNext())) {
			Advance();
			while (isdigit(Peek())) Advance();
		}
		AddToken(TokenType::NUMBER, Object(std::stof(mSource.substr(mStart, mCurrent-mStart))));
	}
	void String() {
		while (Peek() != '"' && !AtEnd()) {
			if (Peek() == '\n')
				mLine++;
			Advance();
		}
		if (AtEnd()) {
			Error(mLine, "Unterminated string.");
			return;
		}
		Advance();
		
		std::string value = mSource.substr(mStart + 1, mCurrent - mStart - 2);
		AddToken(TokenType::STRING, value);
	}
	void Identifier() {
		while (isalnum(Peek()) || Peek() == '_') Advance();
		std::string text = mSource.substr(mStart, mCurrent - mStart);
		auto iter = mKeywords.find(text);
		if (iter == mKeywords.end())
			AddToken(TokenType::IDENTIFIER);
		else
			AddToken(iter->second);
	}
	void AddToken(TokenType type, Object literal = Object(0.0f)) {
		Token tok;
		tok.type = type;
		tok.lexeme = mSource.substr(mStart, mCurrent - mStart);
		tok.line = mLine;
		tok.literal = literal;
		tokens.push_back(tok);
	}
	bool AtEnd() {
		return mCurrent >= mSource.size();
	}
	char Advance() {
		return mSource[mCurrent++];
	}
	char Peek() {
		if (AtEnd()) return 0;
		return mSource[mCurrent];
	}
	char PeekNext() {
		if (mCurrent + 1 >= mSource.length()) return 0;
		return mSource[mCurrent+1];
	}
	bool Match(char c) {
		if (AtEnd() || c != mSource[mCurrent]) return false;
		Advance();
		return true;
	}
	void Error(u16 line, const std::string& message) {
		std::cout << "Error on line " << line << ": " << message << "\n";
	}
};
#endif