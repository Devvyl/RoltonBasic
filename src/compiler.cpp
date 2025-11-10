#include "compiler.hpp"
#include <iostream>
#include <vector>
#include <sstream>

namespace compiler {
    bool in_vector(std::vector<std::string> vct, std::string find) {
        for (int i = 0; i != vct.size(); i++) {
            if (find == vct[i]) return true;
        }
        return false;
    }

    rbl_types::var* find_var(std::vector<rbl_types::var>& vars, const std::string& name) {
        for (int i = 0; i < vars.size(); i++) {
            if (vars[i].name == name) {
                return &vars[i];
            }
        }
        return nullptr;
    }

    std::string escape_string(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\n': result += "\\n"; break;
                case '\t': result += "\\t"; break;
                case '\v': result += "\\v"; break;
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                default: result += c; break;
            }
        }
        return result;
    }

    std::string translating(std::vector<rbl_types::ast_type> ast, std::vector<rbl_types::var> vars) {
        std::vector<std::string> includes;
        std::string out = "int main() {\n";
        std::string ret = "";

        for (int i = 0; i != ast.size(); i++) {
            auto astt = ast[i];
            std::cout << "[COMPILER LOG] Processing command: " << astt.command << "\n";

            if (astt.command == "PRINT") {
                if (!in_vector(includes, "<stdio.h>")) {
                    includes.push_back("<stdio.h>");
                }

                out += "    printf(\"";

                if (astt.args.empty()) {
                    std::cerr << "COMPILER ERROR: PRINT command requires arguments\n";
                    exit(24);
                }

                std::string format_str;
                std::string args_str;
                bool first_arg = true;

                for (int j = 0; j < astt.args.size(); j++) {
                    const std::string& arg = astt.args[j];

                    rbl_types::var* var_ptr = find_var(vars, arg);

                    if (var_ptr != nullptr) {
                        if (var_ptr->type == "integer") {
                            format_str += "%d";
                            if (!first_arg) args_str += ", ";
                            args_str += var_ptr->name;
                        }
                        else if (var_ptr->type == "string") {
                            format_str += "%s";
                            if (!first_arg) args_str += ", ";
                            args_str += var_ptr->name;
                        }
                    }
                    else {
                        if (!arg.empty() && arg.front() != '"' && std::isdigit(arg[0])) {
                            format_str += "%d";
                            if (!first_arg) args_str += ", ";
                            args_str += arg;
                        }
                        else {
                            std::string clean_arg = arg;
                            format_str += "%s";
                            if (!first_arg) args_str += ", ";
                            args_str += "\"" + escape_string(clean_arg) + "\"";
                        }
                    }
                    first_arg = false;
                }

                out += format_str + "\\n\"";
                if (!args_str.empty()) {
                    out += ", " + args_str;
                }
                out += ");\n";
            }
            else if (astt.command == "VARINT") {
                if (astt.args.size() < 2) {
                    std::cerr << "COMPILER ERROR: VARINT requires 2 arguments (name, value)\n";
                    exit(25);
                }

                out += "    int " + astt.args[0] + " = " + astt.args[1] + ";\n";
                vars.push_back({astt.args[0], "integer", astt.args[1]});
            }
            else if (astt.command == "VARSTR") {
                if (astt.args.size() < 2) {
                    std::cerr << "COMPILER ERROR: VARSTR requires 2 arguments (name, value)\n";
                    exit(26);
                }

                std::string value = astt.args[1];

                std::string escaped_value = escape_string(value);

                out += "    char " + astt.args[0] + "[] = \"" + escaped_value + "\";\n";
                vars.push_back({astt.args[0], "string", value});
            }
            else if (astt.command == "TYPE") {
                out += "    // TYPE function - type checking at runtime\n";
                if (astt.args.size() > 0) {
                    rbl_types::var* var_ptr = find_var(vars, astt.args[0]);
                    if (var_ptr != nullptr) {
                        out += "    printf(\"Variable " + astt.args[0] + " type: ";
                        if (var_ptr->type == "integer") {
                            out += "integer\\n\");\n";
                        } else if (var_ptr->type == "string") {
                            out += "string\\n\");\n";
                        }
                    } else {
                        out += "    printf(\"Variable " + astt.args[0] + " not found\\n\");\n";
                    }
                }
                if (!in_vector(includes, "<stdio.h>")) {
                    includes.push_back("<stdio.h>");
                }
            }
            else if (astt.command == "INPUT") {
                if (!in_vector(includes, "<stdio.h>")) {
                    includes.push_back("<stdio.h>");
                }

                if (astt.args.size() > 0) {
                    out += "    scanf(\"";
                    rbl_types::var* var_ptr = find_var(vars, astt.args[0]);
                    if (var_ptr != nullptr) {
                        if (var_ptr->type == "integer") {
                            out += "%d\", &" + astt.args[0] + ");\n";
                        } else if (var_ptr->type == "string") {
                            out += "%s\", " + astt.args[0] + ");\n";
                        }
                    } else {
                        std::cerr << "COMPILER ERROR: Variable " << astt.args[0] << " not found for INPUT\n";
                        exit(27);
                    }
                }
            }
            else if (astt.command == "SUM" || astt.command == "SUB") {
                out += "    // " + astt.command + " operation - basic implementation\n";
                if (astt.args.size() >= 3) {
                    std::string op = (astt.command == "SUM") ? "+" : "-";
                    out += "    " + astt.args[0] + " = " + astt.args[1] + " " + op + " " + astt.args[2] + ";\n";

                    for (int j = 0; j < vars.size(); j++) {
                        if (vars[j].name == astt.args[0]) {
                            vars[j].value = "computed";
                            break;
                        }
                    }
                }
            }
            else {
                out += "    // Unknown command: " + astt.command + "\n";
            }
        }

        out += "    return 0;\n";
        out += "}\n";

        for (int i = 0; i != includes.size(); i++) {
            ret += "#include " + includes[i] + "\n";
        }
        ret += "\n" + out;

        std::cout << "[COMPILER LOG] Generated C code:\n" << ret << "\n";
        return ret;
    }
}
