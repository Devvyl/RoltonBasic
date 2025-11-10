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
        bool in_string = false;
        char string_quote = '\0';
        bool escape_next = false;

        for (int i = 0; i < code.size(); i++) {
            char symb = code[i];


            if (in_string) {
                if (escape_next) {
                    switch (symb) {
                        case 'n': buf += '\n'; break;
                        case 't': buf += '\t'; break;
                        case 'v': buf += '\v'; break;
                        case '\'': buf += '\''; break;
                        case '\"': buf += '\"'; break;
                        case '\\': buf += '\\'; break;
                        default:
                            std::cout << "\nLEXERFAIL: Unknown escape sequence \\" << symb;
                            exit(10);
                    }
                    escape_next = false;
                } else if (symb == '\\') {
                    escape_next = true;
                } else if (symb == string_quote) {
                    tokens.push_back({"STRING", buf});
                    buf = "";
                    in_string = false;
                    string_quote = '\0';
                } else {
                    buf += symb;
                }
                continue;
            }

            if (!std::isalpha(symb) && mod == 1) {
                flush_buffer(buf, mod, tokens);
            }
            else if (!std::isdigit(symb) && mod == 2) {
                flush_buffer(buf, mod, tokens);
            }

            switch (symb) {
                case '=':
                    if (i + 1 < code.size() && code[i+1] == '=') {
                        i++;
                        tokens.push_back({"EQUALS", "=="});
                    } else {
                        tokens.push_back({"EQUAL", "="});
                    }
                    break;
                case ' ':
                    tokens.push_back({"SPACE", " "});
                    break;
                case '+': tokens.push_back({"PLUS", "+"}); break;
                case '-': tokens.push_back({"MINUS", "-"}); break;
                case '*': tokens.push_back({"STAR", "*"}); break;
                case '%': tokens.push_back({"MOD", "%"}); break;
                case '/': tokens.push_back({"SLASH", "/"}); break;
                case '(': tokens.push_back({"L_PAREN", "("}); break;
                case ')': tokens.push_back({"R_PAREN", ")"}); break;
                case '\'':
                case '\"':
                    in_string = true;
                    string_quote = symb;
                    buf = "";
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
                        tokens.push_back({"SYMBOL", "№"});
                        i++;
                        break;
                    }
                    [[fallthrough]];
                default:
                    if (std::isalpha(symb)) {
                        if (mod != 1) {
                            flush_buffer(buf, mod, tokens);
                        }
                        buf += symb;
                        mod = 1;
                    }
                    else if (std::isdigit(symb)) {
                        if (mod != 2) {
                            flush_buffer(buf, mod, tokens);
                        }
                        buf += symb;
                        mod = 2;
                    }
                    else {
                        tokens.push_back({"SYMBOL", std::string(1, symb)});
                    }
                    break;
            }
        }

        if (in_string) {
            std::cout << "\nLEXERFAIL: Unclosed string literal";
            exit(11);
        }

        if (!buf.empty()) {
            flush_buffer(buf, mod, tokens);
        }

        return tokens;
    }
}
