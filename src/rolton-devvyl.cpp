#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "parser.hpp"
#include "lexer.hpp"
#include "compiler.hpp"

using namespace std;

namespace {
    const string PROGRAM_NAME = "programm";
    const string EXIT_COMMAND = "EXIT";
    const vector<rbl_types::var> SYSTEM_VARIABLES = {
        {"SYSVERSION", "string", "0.2-alpha"},
        {"SYSDEVELOPER", "string", "KirillMos1"},
        {"FORKDEVELOPER", "string", "N.O.N AND DEEPSEEK v3.2"}
    };

    string trim(const string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");
        return (start == string::npos) ? "" : str.substr(start, end - start + 1);
    }

    bool shouldExit(const string& input) {
        return trim(input) == EXIT_COMMAND;
    }

    void compileAndBuild(const string& code, const string& outputName = PROGRAM_NAME) {
        ofstream outFile(outputName + ".c");
        if (!outFile.is_open()) {
            cerr << "ERROR: Cannot open file " << outputName << ".c for writing\n";
            return;
        }
        outFile << code << endl;
        outFile.close();

        string compileCommand = "gcc " + outputName + ".c -o " + outputName;
        int result = system(compileCommand.c_str());

        if (result == 0) {
            cout << "SUCCESS: Program compiled successfully as " << outputName << endl;
        } else {
            cerr << "ERROR: Compilation failed with code " << result << endl;
        }
    }

    void printWelcomeMessage() {
        cout << "=== RDL Language Interpreter ===" << endl;
        cout << "Type '" << EXIT_COMMAND << "' to finish and compile" << endl;
        cout << "System version: " << SYSTEM_VARIABLES[0].value << endl;
        cout << "================================" << endl;
    }
}

int main(int argc, char** argv) {
    string inputLine;
    vector<rbl_types::ast_type> abstractSyntaxTree;

    printWelcomeMessage();

    while (true) {
        cout << "\nEnter code: ";
        getline(cin, inputLine);

        if (shouldExit(inputLine)) {
            break;
        }

        if (trim(inputLine).empty()) {
            continue;
        }

        try {
            cout << "[LOG] Starting lexer analysis..." << endl;
            vector<lexer::token> tokens = lexer::lexering(inputLine);

            cout << "[LOG] Starting parser..." << endl;
            rbl_types::ast_type syntaxTree = parser::parse(tokens);

            cout << "[LOG] Adding to abstract syntax tree" << endl;
            abstractSyntaxTree.push_back(syntaxTree);

        } catch (const exception& e) {
            cerr << "[ERROR] Processing failed: " << e.what() << endl;
        }
    }

    if (!abstractSyntaxTree.empty()) {
        cout << "[LOG] Starting code translation..." << endl;

        try {
            string compiledCode = compiler::translating(abstractSyntaxTree, SYSTEM_VARIABLES);

            string outputName = PROGRAM_NAME;
            if (argc > 1) {
                outputName = argv[1];
            }

            compileAndBuild(compiledCode, outputName);

        } catch (const exception& e) {
            cerr << "[ERROR] Compilation failed: " << e.what() << endl;
            return 1;
        }
    } else {
        cout << "[INFO] No code to compile. Exiting." << endl;
    }

    return 0;
}
