#ifndef UTILS_H
#define UTILS_H

#include <unordered_map>

// Checks literals
bool is_valid_char(char c) {
    return (isalnum(c) || c == '%' || c == '$' || c == '-' || c == '(' || c == ')');
}

// Checks if the literal used is an address (stored within brackets)
bool is_address(const std::string token) {
    if (token[0] == '(' && token[token.length() - 1] == ')') return true;
    return false;
}

// Checks if register and returns register information
int is_register(const std::string reg) {
    try {
        return registers.at(reg);
    } catch(std::out_of_range) {
        return 0;
    }
}

// Returns a string with all characters uppercase
std::string uppercase(std::string input) {
    for (int i = 0; i < input.length(); i++) {
        input[i] = toupper(input[i]);
    }
    return input;
}

// Removes any non-numeric characters
std::string remove_identifier(std::string input) {
    std::string output;
    for (int i = 0; i < input.length(); i++) {
        if (isalnum(input[i]) || input[i] == '-' || input[i] == '_') output += input[i];
    }
    return output;
}

int convert_literals(std::string input) {
    int output;
    try {
        if (input[0] == '$' || (is_address(input) && input[1] == '$')) output = std::stoi(remove_identifier(input), nullptr, 16);
        else if (input[0] == '%' || (is_address(input) && input[1] == '%')) output = std::stoi(remove_identifier(input), nullptr, 2);
        else output = std::stoi(remove_identifier(input));
    } catch(...) {
        printf("ERROR: Invalid literal declaration %s\n", input.c_str());
        exit(-1);
    }
    return output;
}

int uses_literal(const std::vector<std::string>& tokens) {
    if (tokens.size() > 2) {
        if (tokens[2] == "d8" || tokens[2] == "r8" || tokens[2] == "(a8)") return 8;
        else if (tokens[2] == "d16" || tokens[2] == "(a16)" || tokens[2] == "a16") return 16;
    }
    else if (tokens.size() > 1) {
        if (tokens[1] == "(a8)") return 8;
        else if (tokens[1] == "(a16)" || tokens[1] == "a16") return 16;
    }
    return -1;
}

int get_opcode(std::string statement) {
    try {
        int opcode = opcodes.at(statement);
        return opcode;
    } catch(...) {
        return -1;
    }
}

int get_cb_opcode(std::string statement) {
    try {
        int opcode = cb_opcodes.at(statement);
        return opcode;
    } catch(...) {
        return -1;
    }
}

std::string assemble_statement(const std::vector<std::string>& tokens) {
    std::string statement;
    for (int i = 0; i < tokens.size(); i++) {
        if (i == 1) statement += " ";
        if (i == 2) statement += ",";
        statement += tokens[i];
    }
    return statement;
}

#endif // UTILS_H