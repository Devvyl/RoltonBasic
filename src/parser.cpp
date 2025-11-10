#include "parser.hpp"
#include <iostream>

namespace parser {
    rbl_types::ast_type parse(std::vector<lexer::token> tokens) {
        std::cout << "[PARSER LOG] INIT VARS\n";
        rbl_types::ast_type ast_vct;
        std::vector<lexer::token> cls_tks;

        std::cout << "[PARSER LOG] LIST ALL TOKENS\n";
        for (int i = 0; i != tokens.size(); i++) {
            std::cout << "TOKEN TYPE: " << tokens[i].type << "\nTOKEN VALUE: " << tokens[i].value << "\n\n";
        }

        if (tokens.size() == 0) {
            std::cout << "[PARSER LOG] TOKENS NOT FOUND\n";
            ast_vct = {"", {""}};
            return ast_vct;
        }

        std::cout << "[PARSER LOG] CLEARING TOKENS (REMOVING SPACES)\n";
        for (int i = 0; i != tokens.size(); i++) {
            if (tokens[i].type != "SPACE") {
                cls_tks.push_back(tokens[i]);
            }
        }

        std::cout << "[PARSER LOG] MAIN PARSING\n";
        if (cls_tks[0].type == "KWORD") {
            std::cout << "[PARSER LOG] ADDING COMMAND IN AST: " << cls_tks[0].value << "\n";
            ast_vct.command = cls_tks[0].value;

            std::vector<std::string> args;
            for (int i = 1; i < cls_tks.size(); i++) {
                std::cout << "[PARSER LOG] PROCESSING TOKEN " << i << ": " << cls_tks[i].type << " = " << cls_tks[i].value << "\n";

                if (cls_tks[i].type == "STRING") {
                    args.push_back(cls_tks[i].value);
                }

                else if (cls_tks[i].type == "WORD" || cls_tks[i].type == "DIGIT") {
                    args.push_back(cls_tks[i].value);
                }

                else if (cls_tks[i].type == "PLUS" || cls_tks[i].type == "MINUS" ||
                         cls_tks[i].type == "STAR" || cls_tks[i].type == "SLASH" ||
                         cls_tks[i].type == "MOD" || cls_tks[i].type == "EQUAL" ||
                         cls_tks[i].type == "EQUALS" || cls_tks[i].type == "L_PAREN" ||
                         cls_tks[i].type == "R_PAREN" || cls_tks[i].type == "SYMBOL") {
                    args.push_back(cls_tks[i].value);
                }

                else if (cls_tks[i].type == "QUOTE") {
                    std::cout << "[PARSER LOG] WARNING: Found QUOTE token, but strings should be already processed\n";
                }
                else {
                    std::cout << "[PARSER LOG] WARNING: Unknown token type: " << cls_tks[i].type << "\n";
                    args.push_back(cls_tks[i].value);
                }
            }

            ast_vct.args = args;
        }
        else {
            std::cout << "[PARSER LOG] ERROR: First token must be KWORD, but got: " << cls_tks[0].type << "\n";
            exit(21);
        }

        std::cout << "[PARSER LOG] PARSING COMPLETED\n";
        std::cout << "[PARSER LOG] COMMAND: " << ast_vct.command << "\n";
        std::cout << "[PARSER LOG] ARGUMENTS COUNT: " << ast_vct.args.size() << "\n";
        for (int i = 0; i < ast_vct.args.size(); i++) {
            std::cout << "[PARSER LOG] ARG " << i << ": " << ast_vct.args[i] << "\n";
        }

        return ast_vct;
    }
}
