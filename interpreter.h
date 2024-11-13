#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <bitset>
#include <unordered_map>

class Interpreter
{
public:
	Interpreter(std::ostream &out_stream);
	bool read_exp(std::string fileName);
	std::vector<std::string> get_tokens();
	std::vector < std::string> get_statements();
	std::vector<std::string> tokenize(std::string expr);
	void evaluate(const std::vector<std::string> &tokens);
	
private:
	std::ostream &output_stream;
	std::unordered_map<std::string,int> variables;
	std::vector<std::string> cat_statements;
	std::vector<std::string> tokens;
	enum output_format {
		decimal,
		hexadecimal,
		binary
	};
	enum output_format format = decimal;
	std::regex variable_regex;
	std::regex math_regex;
	int position{};
	const std::string ETX = "\u0003";

	void parse_stmt(const std::vector<std::string> &tokens);
	void parse_config_stmt(const std::vector<std::string> &tokens);
	void parse_assgn_stmt(const std::vector<std::string> &tokens);
	void parse_print_stmt(const std::vector<std::string> &tokens);
	int parse_math_exp(const std::vector<std::string> &tokens);
	int parse_sum_exp(const std::vector<std::string> &tokens);
	int parse_product_exp(const std::vector<std::string> &tokens);
	int parse_primary_exp(const std::vector<std::string> &tokens);
	std::string peek(int steps);
	std::string peek();
	void consume(const std::string &token);
};

