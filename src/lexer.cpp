#include "lexer.hpp"
#include <cctype>
#include <iostream>

namespace lexer {

    void flush_buffer(std::string& buf, int& mod, std::vector<token>& tokens) {
         if (mod == 1) {
             if (buf == "PRINT" || buf == "VARINT" || buf == "VARSTR" ||
                 buf == "TYPE" || buf == "INPUT" || buf == "SUM" || buf == "SUB") {
                 tokens.push_back({"KWORD", buf});
             } else {
                 tokens.push_back({"WORD", buf});
             }
         } else if (mod == 2) {
             tokens.push_back({"DIGIT", buf});
         }
         buf = "";
         mod = 0;
     }

    std::vector<token> lexering(std::string code) {
        std::vector<token> tokens;
        std::string buf = "";
        int mod = 0;
        token newtoken;

        for (int i = 0; i < code.size(); i++) {
            char symb = code[i];

            if (!std::isalpha(symb) && mod == 1) {
                if (buf == "PRINT" || buf == "VARINT" || buf == "VARSTR" ||
                    buf == "TYPE" || buf == "INPUT" || buf == "SUM" || buf == "SUB") {
                    newtoken = {"KWORD", buf};
                } else {
                    newtoken = {"WORD", buf};
                }
                tokens.push_back(newtoken);
                buf = "";
                mod = 0;
            }
            else if (!std::isdigit(symb) && mod == 2) {
                newtoken = {"DIGIT", buf};
                tokens.push_back(newtoken);
                buf = "";
                mod = 0;
            }

            switch (symb) {
                case '=':
                    if (i + 1 < code.size() && code[i+1] == '=') {
                        i++;
                        tokens.push_back({"EQUALS", "NONE"});
                    } else {
                        tokens.push_back({"EQUAL", "NONE"});
                    }
                    break;
                case ' ': tokens.push_back({"SPACE", "NONE"}); break;
                case '+': tokens.push_back({"PLUS", "NONE"}); break;
                case '-': tokens.push_back({"MINUS", "NONE"}); break;
                case '*': tokens.push_back({"STAR", "NONE"}); break;
                case '%': tokens.push_back({"MOD", "NONE"}); break;
                case '/': tokens.push_back({"SLASH", "NONE"}); break;
                case '(': tokens.push_back({"L_PAREN", "NONE"}); break;
                case ')': tokens.push_back({"R_PAREN", "NONE"}); break;
                case '\'':
                case '\"': tokens.push_back({"QUOTE", "NONE"}); break;
                case '\\':
                    if (i + 1 < code.size()) {
                        if (code[i+1] == 'n') { tokens.push_back({"SPECSYMBOL", "\n"}); }
                        else if (code[i+1] == 't') { tokens.push_back({"SPECSYMBOL", "\t"}); }
                        else if (code[i+1] == 'v') { tokens.push_back({"SPECSYMBOL", "\v"}); }
                        else { std::cout << "\nLEXERFAIL: Unknown symbol"; exit(10); }
                        i++;
                    }
                    break;
                case '.':
                case ',':
                case '?':
                case '!':
                case '@':
                case '#':
                case '$':
                case ';':
                case '&':
                case '_':
                    buf += symb;
                    tokens.push_back({"SYMBOL", buf});
                    buf = "";
                    break;
                case '\xD0':
                    if (i + 1 < code.size() && code[i+1] == '\x9E') {
                        buf += "№";
                        tokens.push_back({"SYMBOL", buf});
                        buf = "";
                        i++;
                        break;
                    }
                    [[fallthrough]];
                default:
                    if (std::isalpha(symb)) {
                        if (buf.empty()) {
                            buf += symb;
                            mod = 1;
                        } else {
                            if (mod == 1) {
                                buf += symb;
                            } else {
                                flush_buffer(buf, mod, tokens);
                            }
                        }
                    }
                    else if (std::isdigit(symb)) {
                        if (buf.empty()) {
                            buf += symb;
                            mod = 2;
                        } else {
                            if (mod == 2) {
                                buf += symb;
                            } else {
                                flush_buffer(buf, mod, tokens);
                            }
                        }
                    }
                    else {
                        tokens.push_back({"SYMBOL", std::string(1, symb)});
                    }
                    break;
            }
        }

        if (!buf.empty()) {
            if (mod == 1) {
                tokens.push_back({"WORD", buf});
            } else if (mod == 2) {
                tokens.push_back({"DIGIT", buf});
            }
        }

        return tokens;
    }
}
