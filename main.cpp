/*
TODO:
- better error handling (remember exact position of the character and output line to pinpoint it, panic mode recovery)
*/

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include <fstream>

std::string ReadFile(const char* filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		Error(0, "Could not open file: " + std::string(filename));
		exit(0); // TODO: proper exit codes
	}
	std::string result, line;
	while (std::getline(file, line)) {
		result += line + "\n";
	}
	return result;
}

int main(int argc, char **argv) {
	Lexer lexer;
	Parser parser;
	if (argc > 1) {
		lexer.Lex(ReadFile(argv[1]));
		parser.Parse(lexer.tokens);
		
		if (!parser.HadError()) {
			for(int i = 0; i < parser.statements.size(); i++) {
				parser.statements[i]->Evaluate();
				parser.statements[i]->Destroy();
			}
		}
	}
	else {
		while (true) {
			std::cout << ">>>";
			std::string input;
			std::getline(std::cin, input);
			lexer.Lex(input);
			parser.Parse(lexer.tokens);
			//for(auto tok : lexer.tokens) {
			//	std::cout << tok.str() << "\n";
			//}
			if (!parser.HadError()) {
				for(int i = 0; i < parser.statements.size(); i++) {
					std::cout << parser.statements[i]->Str() << "\n";
					parser.statements[i]->Evaluate();
					parser.statements[i]->Destroy();
				}
			}
		}
	}
	if (environment)
		environment->Destroy();
	return 0;
}
