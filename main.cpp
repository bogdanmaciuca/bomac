/*
TODO:
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
		//for(int i = 0; i < parser.nodes.size(); i++) {
		//	std::cout << i << ": " << parser.nodes[i]->str() << "\n";
		//}
		if (!parser.HadError()) {
			for(int i = 0; i < parser.nodes.size(); i++) {
				parser.nodes[i]->evaluate();
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
			//for(int i = 0; i < parser.nodes.size(); i++) {
			//	std::cout << parser.nodes[i]->str() << "\n";
			//}
			if (!parser.HadError()) {
				for(int i = 0; i < parser.nodes.size(); i++) {
					parser.nodes[i]->evaluate();
				}
			}
		}
	}
	return 0;
}
