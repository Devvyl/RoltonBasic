#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <cstring>

#include "parser.hpp"
#include "lexer.hpp"
#include "compiler.hpp"

using namespace std;

int main(int argc, char** argv) {
    string line;
    vector<rbl_types::ast_type> ast_full;
    vector<rbl_types::var> vars_vector = {{"SYSVERSION", "string", "1.0.1-alpha"}, {"SYSDEVELOPER", "string", "KirillMos1"}};

    do {
        cout<<"\n\nEnter code: ";
        getline(cin, line);
        if (line == "EXIT") { break; } // TODO 0xd001 Fix. if input " EXIT" it DOESNT WORK but it MUST work / Починить. Если ввод " EXIT" то это НИХРЕНА не работает, А НАДО
        cout<<"[LOG] START LEXER\n";
        vector<lexer::token> tokens = lexer::lexering(line);
        cout<<"[LOG] START PARSER\n";
        rbl_types::ast_type ast = parser::parse(tokens);
        cout<<"[LOG] ADD IN FULL AST\n";
        ast_full.push_back(ast);
    } while (true); // TODO 0xd001

    cout<<"[LOG] START TRANSLATING\n";
    string translated = compiler::translating(ast_full, vars_vector);
    std::cout << "Your name for out file: program";
    std::ofstream out;
    out.open("programm.c");
    if (out.is_open()) {
        out << translated << endl;
    }
    out.close();
    system("gcc programm.c -o programm");
}
