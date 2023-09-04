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
		mKeywords["int"] = TokenType::INT;
		mKeywords["print"] = TokenType::PRINT;
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
			case '=': AddToken(TokenType::EQUAL); break;
			case '+': AddToken(TokenType::PLUS); break;
			case '-': AddToken(TokenType::MINUS); break;
			case '*': AddToken(TokenType::STAR); break;
			case '/': AddToken(TokenType::SLASH); break;
			case '(': AddToken(TokenType::LEFT_PARAN); break;
			case ')': AddToken(TokenType::RIGHT_PARAN); break;
			case ';': AddToken(TokenType::COLON); break;
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
		AddToken(TokenType::NUMBER, std::stof(mSource.substr(mStart, mCurrent-mStart)));
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
	void AddToken(TokenType type, float literal = 0) {
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
};
#endif