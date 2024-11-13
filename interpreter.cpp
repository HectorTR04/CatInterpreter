#include "interpreter.h"

Interpreter::Interpreter(std::ostream &out_stream) 
	: output_stream(out_stream)
{
	variable_regex = "([a-zA-Z][a-zA-Z0-9]*)";
	math_regex = "(-?[0-9]+|\\()";
}

bool Interpreter::read_exp(std::string fileName)
{
	std::string line;
	std::ifstream my_file(fileName);		
	if (!my_file.is_open())
	{
		std::cout << "Unable to open file" << std::endl;
		return false;
	}
	while (getline(my_file, line))
	{
		cat_statements.push_back(line);
	}
	my_file.close();
	return true;	
}

std::vector<std::string> Interpreter::get_tokens()
{
	return tokens;
}

std::vector<std::string> Interpreter::get_statements()
{
	return cat_statements;
}

std::vector<std::string> Interpreter::tokenize(std::string expr)
{
	std::regex pattern("([a-zA-Z][a-zA-Z0-9]*)|(-?[0-9]+)|([=+*/()-])");

	auto begin = std::sregex_iterator(expr.begin(), expr.end(), pattern);
	auto end = std::sregex_iterator();

	for (std::sregex_iterator i = begin; i != end; i++) {
		std::smatch match = *i;
		tokens.push_back(match.str());
	}
	return tokens;
}

void Interpreter::evaluate(const std::vector<std::string> &tokens)
{
	parse_stmt(tokens);
}

void Interpreter::parse_stmt(const std::vector<std::string> &tokens)
{
	std::regex variable("([a-zA-Z][a-zA-Z0-9]*)");
	std::regex config("config");
	std::regex print("print");
	std::string next_token = peek();
	if (std::regex_match(next_token, config)) {
		consume(next_token);
		parse_config_stmt(tokens);
	}
	else if (std::regex_match(next_token, print)) {
		consume(next_token);
		parse_print_stmt(tokens);
	}
	else if (std::regex_match(next_token, variable_regex)) {
		variables[next_token] = 0;
		parse_assgn_stmt(tokens);
	}
	else if (next_token == ETX) {
		return;
	}
	else {
		std::cerr << "Invalid C@ syntax" << std::endl;
		throw std::runtime_error("Invalid C@ syntax.");
	}
}

void Interpreter::parse_config_stmt(const std::vector<std::string> &tokens)
{
	std::regex dec("dec"), hex("hex"), bin("bin");
	std::string next_token = peek();

	if (std::regex_match(next_token, dec)) {
		consume(next_token);
		format = decimal;
	}
	else if ((std::regex_match(next_token, hex))) {
		consume(next_token);
		format = hexadecimal;
	}
	else if ((std::regex_match(next_token, bin))) {
		consume(next_token);
		format = binary;
	}
	else {
		std::cerr << "Invalid config statement, token followed by config: " + next_token << std::endl;
		throw std::runtime_error("Invalid config statement, token followed by config: " + next_token);
	}
}

void Interpreter::parse_assgn_stmt(const std::vector<std::string> &tokens)
{
	std::regex equal("\\=");
	std::string variable_token = peek();
	consume(variable_token);
	std::string next_token = peek();
	if (next_token == ETX) {
		return;
	}
	if (!std::regex_match(next_token, equal)) {
		std::cerr << "Invalid assignment operation, token followed by variable_name" + next_token << std::endl;
		throw std::runtime_error("Invalid assignment operation, token followed by variable_name" + next_token);
	}
	consume(next_token);

	int result = parse_math_exp(tokens);

	variables[variable_token] = result;

}

void Interpreter::parse_print_stmt(const std::vector<std::string> &tokens) {
	int result = 0;
	std::string next_token = peek();
	if (std::regex_match(next_token, variable_regex)) {
		result = variables[next_token];
		consume(next_token);
	}
	else if (std::regex_match(next_token, math_regex)) {
		result = parse_math_exp(tokens);
	}
	switch (format) {
		case decimal:
			output_stream << std::dec << result << std::endl;
			break;
		case hexadecimal:
			output_stream << std::showbase << std::hex << result << std::endl;
			break;
		case binary:
			output_stream << std::bitset<32>(result) << std::endl;
			break;
	}
}

int Interpreter::parse_math_exp(const std::vector<std::string> &tokens)
{
	return parse_sum_exp(tokens);
}

int Interpreter::parse_sum_exp(const std::vector<std::string> &tokens)
{
	std::regex plus("\\+"), minus("\\-");
	int result = parse_product_exp(tokens);
	std::string next_token = peek();
	while (true) {
		if (std::regex_match(next_token, plus)) {
			consume(next_token);
			result = result + parse_product_exp(tokens);
		}
		else if (std::regex_match(next_token, minus)) {
			consume(next_token);
			result = result - parse_product_exp(tokens);
		}
		else {
			break;
		}
		next_token = peek();
	}
	return result;
}

int Interpreter::parse_product_exp(const std::vector<std::string> &tokens)
{
	std::regex multiplication("\\*"), division("\\/");
	int result = parse_primary_exp(tokens);
	std::string next_token = peek();
	while (true) {
		if (std::regex_match(next_token, multiplication)) {
			consume(next_token);
			result = result * parse_primary_exp(tokens);
		}
		else if (std::regex_match(next_token, division)) {
			consume(next_token);
			result = result / parse_primary_exp(tokens);
		}
		else {
			break;
		}
		next_token = peek();
	}
	return result;
}

int Interpreter::parse_primary_exp(const std::vector<std::string> &tokens)
{
	std::regex integer("-?[0-9]+"), left_parentheses("\\("), right_parentheses("\\)");
	int value = 0;
	std::string next_token = peek();
	if (std::regex_match(next_token, integer)) {
		value = std::stoi(next_token);
		consume(next_token);
	}
	else if (std::regex_match(next_token, left_parentheses)) {
		consume(next_token);
		value = parse_math_exp(tokens);
		if (std::regex_match(peek(), right_parentheses)) {
			consume(peek());
		}
		else {
			std::cerr << "Mismatched parentheses" << std::endl;
			throw std::runtime_error("Mismatched parentheses");
		}
	}
	else if (std::regex_match(next_token, variable_regex)) {
		value = variables[next_token];
		consume(next_token);
	}
	else {
		std::cerr << "Expected int or ( ), found " + next_token << std::endl;
		throw std::runtime_error("Expected int or ( ), found " + next_token);
	}
	return value;
}

std::string Interpreter::peek(int steps)
{
	if (position + steps >= tokens.size()) return ETX;
	return tokens[position + steps];
}

std::string Interpreter::peek()
{
	return peek(0);
}

void Interpreter::consume(const std::string &token)
{
	std::string next_token = peek();
	if (next_token == ETX) {
		std::cerr << "Consumed past last token" << std::endl;
		throw std::runtime_error("Consumed past last token\n");
	}
	if (next_token != token) {
		std::cerr << "Could not consume token '" + token << std::endl;
		throw std::runtime_error("Could not consume token '" + token + "', found '" + next_token);
	}
	++position;
}
