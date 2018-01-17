#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <algorithm>

#include "opcodes.h"
#include "utils.h"

/**
 * Rules:
 * Registers are just letters ex: A
 * Address value pointers are in brackets ex: (BC)
 * Numbers must begin with a $ eg: $42
 * Parameters are seperated using a comma ex: (BC),A
 * A space must separate the statement and the parameters
 **/

void get_tokens(const std::string& line, std::vector<std::string>& tokens, int& it) {
    // Disregard any spaces if there is nothing in the string yet
    std::string token;
    while (it <= line.length()) {
        // If the line has a comment, return from the function
        if (line[it] == ';') return;
        if (is_valid_char(line[it])) {
            token += line[it];
        }
        if ((line[it] == ' ' || line[it] == ',' || it == line.length()) && token != "") {
            tokens.push_back(uppercase(token));
            token = "";
        }
        it++;
    }
}


void assemble_literals(std::vector<std::string>& tokens, std::string token, int& literal) {
    if (is_register(token) == 0) {
        literal = convert_literals(token);
        
        if (tokens.size() > 2) {
            if (is_address(tokens[2]) && tokens[0] == "LDH") tokens[2] = "(a8)";
            else if (is_address(tokens[2])) tokens[2] = "(a16)";
            else if (is_register(tokens[1]) == 8) tokens[2] = "d8";
            else if (is_register(tokens[1]) == 16) tokens[2] = "d16";
            else if (tokens[0] == "JP" || tokens[0] == "CALL") tokens[2] = "a16";
            else if (is_register(tokens[1]) == 1 || tokens[0] == "LDHL") tokens[2] = "r8";
        }
        
        else if (tokens.size() > 1) {
            if (is_address(tokens[1]) && tokens[0] == "LDH") tokens[1] = "(a8)";
            else if (is_address(tokens[1])) tokens[1] = "(a16)";
            else if (tokens[0] == "SUB" || tokens[0] == "AND" || tokens[0] == "OR" || tokens[0] == "XOR" || tokens[0] == "CP") tokens[1] = "d8";
            else if (tokens[0] == "JP" || tokens[0] == "CALL") tokens[1] = "a16";    
        }
    }
}

int parse_tokens(std::vector<std::string>& tokens, std::vector<uint8_t>& opcode_buffer, std::unordered_map<std::string, int>& constants, std::unordered_map<std::string, uint16_t>& addresses) {
    int opcode;
    bool cb_opcode = false;
    int literal;
    
    // If there are no tokens, return the function as successful
    if (tokens.size() <= 0) return 0;
    
    // Check if using the DB statement, if so, just push back the desired literals
    if (tokens[0] == "DB" || tokens[0] == ".DB") {
        for (int i = 1; i < tokens.size(); i++) {
            literal = convert_literals(tokens[i]);
            opcode_buffer.push_back(literal & 0xFF);
        }
        return 0;
    }
    
    // CB opcodes don't use literals, check if CB opcode before literals
    std::string cb_statement = assemble_statement(tokens);
    opcode = get_cb_opcode(cb_statement);
    if (opcode >= 0) {
        opcode_buffer.push_back(0xCB);
        opcode_buffer.push_back(opcode & 0xFF);
        return 0;
    }
    
    // Check literals and change tokens accordingly
    if (tokens.size() > 1) {
        for (int i = 1; i < tokens.size(); i++) {
            assemble_literals(tokens, tokens[i], literal);
        }
    }
    
    // Compile the tokens into one complete statement
    std::string statement = assemble_statement(tokens);
    
    // Print out the statement, (for debugging purposes)
    printf("Statement: %s", statement.c_str());
    
    // Check through the statements, if there are no matching statements, return an error code
    opcode = get_opcode(statement);
    if (opcode == -1) {
        printf("ERROR: Undefined statement %s\n", statement.c_str());
        return -1;
    }
    
    opcode_buffer.push_back(opcode & 0xFF);
    
    // Push back any literals used
    if (uses_literal(tokens) == 8) opcode_buffer.push_back(literal & 0xFF);
    else if (uses_literal(tokens) == 16) {
        opcode_buffer.push_back(literal & 0xFF);
        opcode_buffer.push_back((literal >> 8) & 0xFF);
    }
    printf("\n");
    return 0;
}

void read_statements(std::ifstream& source) {
    std::vector<std::string> tokens;
    std::vector<uint8_t> opcode_buffer;
    std::string line;
    std::unordered_map<std::string, uint16_t> addresses;
    std::unordered_map<std::string, int> constants;
    
    // Read each line from the source file
    while (std::getline(source, line, '\n')) {
        int it = 0;
        get_tokens(line, tokens, it);
        if (parse_tokens(tokens, opcode_buffer, constants, addresses) < 0) {
            printf("ERROR: Could not parse\n");
            return;
        }
        tokens.clear();
    }
    
    // Print out every opcode in the buffer (Will eventually output to a file)
    for (int i = 0; i < opcode_buffer.size(); i++) {
        printf("%02X ", opcode_buffer[i]);
    }
}

int main(int argc, char **argv) {
    std::string out_file_path;
    if (argc != 2) {
        printf("Usage gb++ <source> -o <bin>\n");
        return -1;
    }

    std::ifstream source;
    source.open(argv[1], std::ios::binary);

    if (!source.is_open()) {
        printf("Unable to open %s\n", argv[1]);
        return -1;
    }

    read_statements(source);

    source.close();

    /*std::ofstream out;
    out.open("out");
    if (!out.is_open()) {
        printf("Could not create out file.\n");
        return -1;
    }
    for (int i = 0; i < tokens.size(); i++) {
        printf("%s\n", tokens[i].c_str());
    }
    for (int i = 0; i < opcode_buffer.size(); i++) {
        out.write(reinterpret_cast<char*>(&opcode_buffer[i]), 1);
        printf("%02X ", opcode_buffer[i]);
    }
    out.close();
    */
    return 0;
}
