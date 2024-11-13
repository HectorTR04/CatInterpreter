#include "interpreter.h"
#include <bitset>


int main() {
	Interpreter interpreter = Interpreter(std::cout);
	interpreter.read_exp("Expressions.txt");
	for (std::string stmt : interpreter.get_statements()) {
		interpreter.evaluate(interpreter.tokenize(stmt));
	}
	return 0;
}