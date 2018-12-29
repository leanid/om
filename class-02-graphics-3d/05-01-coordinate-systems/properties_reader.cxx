#include "properties_reader.hxx"

//#include <charconv> // not found on Visual Studio 2017.7
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"

enum class token_type : uint8_t
{
    none,
    type_name,
    prop_name,
    operator_name,
    int_value,
    float_value,
    string_value,
    end_of_expr
};

struct token
{
    token_type       type = token_type::none;
    std::string_view value;
};

struct properties_lexer
{

    std::vector<token> token_list;
    std::string        content;

    struct token_regex
    {
        token_regex(token_type t_, const char* r_)
            : type{ t_ }
            , regex{ r_ }
        {
        }
        token_type type;
        std::regex regex;
    };

    std::vector<token_regex> token_bind;

    properties_lexer(std::string content_)
        : content{ std::move(content_) }
    {
        token_bind.reserve(10);
        token_bind.emplace_back(token_type::type_name,
                                R"(float|std::string|int)");
        token_bind.emplace_back(token_type::operator_name, R"(=|\+|-|\*|\/)");
        token_bind.emplace_back(token_type::prop_name,
                                R"([a-zA-Z_][a-zA-Z0-9_]*)");
        token_bind.emplace_back(token_type::int_value, R"([1-9]\d*)");
        token_bind.emplace_back(token_type::float_value,
                                R"([+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)f)");
        // original from internet: /"([^"\\]|\\.)*"/
        token_bind.emplace_back(token_type::string_value, R"("([^"\\]|\\.)*")");
        token_bind.emplace_back(token_type::end_of_expr, R"(;)");
        token_bind.emplace_back(token_type::none, R"(#.*\n|//.*\n| |\n|\t)");

        std::string_view rest_content{ content };
        bool             cant_find_match = false;

        while (!rest_content.empty() && !cant_find_match)
        {
            std::cmatch  token_best_match;
            token_regex* best_token_regex = nullptr;
            for (token_regex& tok_regex : token_bind)
            {
                std::cmatch token_match;
                if (std::regex_search(rest_content.data(),
                                      rest_content.data() +
                                          rest_content.length(),
                                      token_match, tok_regex.regex,
                                      std::regex_constants::match_continuous))
                {
                    auto& first = *token_match.cbegin();
                    if (token_best_match.empty() ||
                        first.length() > token_best_match.cbegin()->length())
                    {
                        token_best_match = token_match;
                        best_token_regex = &tok_regex;
                    }
                }
            }

            if (!token_best_match.empty())
            {
                token tok;
                tok.type                   = best_token_regex->type;
                auto&       first_match    = token_best_match[0];
                const char* first_char_ptr = first_match.first;
                size_t      length = static_cast<size_t>(first_match.length());
                tok.value          = std::string_view(first_char_ptr, length);

                rest_content = rest_content.substr(tok.value.size());

                if (best_token_regex->type == token_type::none)
                {
                    continue;
                }
                else
                {
                    token_list.push_back(tok);
                }
            }
            else
            {
                cant_find_match = true;
            }
        } // while

        if (!rest_content.empty())
        {
            throw std::runtime_error("can't find regex startfing from: " +
                                     std::string(rest_content));
        }
    }
};

std::ostream& operator<<(std::ostream& stream, const token_type t)
{
    switch (t)
    {
        case token_type::operator_name:
            stream << "operator";
            break;
        case token_type::float_value:
            stream << "float";
            break;
        case token_type::int_value:
            stream << "int";
            break;
        case token_type::string_value:
            stream << "string";
            break;
        case token_type::prop_name:
            stream << "variable_name";
            break;
        case token_type::type_name:
            stream << "type_name";
            break;
        case token_type::none:
            stream << "ws";
            break;
        case token_type::end_of_expr:
            stream << "end_of_expr";
            break;
    }
    return stream;
}

using value_t = std::variant<std::string, std::int32_t, float>;

namespace program_structure
{

struct lvalue
{
    const token* name = nullptr;
};

struct declaration
{
    const token* type = nullptr;
    lvalue       var;
};

struct variable
{
    std::variant<std::monostate, declaration, lvalue> name;
};

struct constant
{
    const token* value = nullptr;
};

struct operation
{
    const token* op = nullptr;
};

struct operand
{
    std::variant<std::monostate, variable, constant> value;
    const token*                                     first_token    = nullptr;
    const token*                                     past_end_token = nullptr;
};

struct expression;

struct expr_or_operand
{
    std::variant<std::monostate, expression*, operand> value;
    const token*                                       first_token    = nullptr;
    const token*                                       past_end_token = nullptr;
};

struct expression
{
    operand         left_operand;
    operation       op;
    expr_or_operand right_operand;
};
} // end namespace program_structure

struct properties_parser
{
    std::vector<program_structure::expression> commands;

    program_structure::expression parse_expression(
        std::vector<token>::iterator& token_it);
    program_structure::operand parse_operand(
        std::vector<token>::iterator& token_it);
    program_structure::declaration parse_declaration(
        std::vector<token>::iterator& token_it);
    program_structure::variable parse_variable_name(
        std::vector<token>::iterator& token_it);
    program_structure::constant parse_constant(
        std::vector<token>::iterator& token_it);
    program_structure::operation parse_operation(
        std::vector<token>::iterator& token_it);
    program_structure::expr_or_operand parse_expr_or_operand(
        std::vector<token>::iterator& token_it);

    properties_lexer& lexer;

    properties_parser(properties_lexer& lexer_)
        : lexer{ lexer_ }
    {
        using namespace program_structure;

        std::clog << "--------start tokens" << std::endl;
        for (auto& token : lexer.token_list)
        {
            std::clog << token.type << ":{" << token.value << "}" << std::endl;
        }
        std::clog << "--------end tokens" << std::endl;

        for (auto token_iter = begin(lexer.token_list);
             token_iter != end(lexer.token_list);)
        {
            expression expr = parse_expression(token_iter);
            commands.push_back(expr);
        }
    }

    std::string print_position_of_token(const token& token)
    {
        std::string_view from_start_to_token(
            lexer.content.data(),
            static_cast<size_t>(token.value.data() - lexer.content.data()) +
                token.value.length());
        std::stringstream ss;
        ss << '\n' << from_start_to_token << '\n';
        size_t last_line_length = 0;
        for (auto back_char = &from_start_to_token.back();
             *back_char != '\0' && *back_char != '\n'; --back_char)
        {
            ++last_line_length;
        }
        last_line_length -= token.value.length();
        ss << std::string(last_line_length, ' ') << '^';
        return ss.str();
    }

    token* expected(const std::vector<token>::iterator& it,
                    const token_type                    type)
    {
        if (it == end(lexer.token_list))
        {
            std::stringstream ss;
            ss << "error: expected " << type << " but got: EOF";
            throw std::runtime_error(ss.str());
        }
        if (it->type != type)
        {
            std::stringstream ss;
            ss << "error: expected " << type << " but got: " << it->value;
            ss << print_position_of_token(*it);
            throw std::runtime_error(ss.str());
        }
        return &(*it);
    }

    token* expected_one_of(const std::vector<token>::iterator&      it,
                           const std::initializer_list<token_type>& types)
    {
        if (it == end(lexer.token_list))
        {
            std::stringstream ss;
            ss << "error: expected one of: ";
            for (auto type : types)
            {
                ss << type << ", ";
            }
            ss << " but got: EOF";
            throw std::runtime_error(ss.str());
        }
        auto type_it = std::find(begin(types), end(types), it->type);
        if (type_it == end(types))
        {
            std::stringstream ss;
            ss << "error: expected ";
            for (auto type : types)
            {
                ss << type << ", ";
            }
            ss << "but got: " << it->type;
            ss << print_position_of_token(*it);
            throw std::runtime_error(ss.str());
        }
        return &(*it);
    }
};

program_structure::expression properties_parser::parse_expression(
    std::vector<token>::iterator& token_it)
{
    using namespace program_structure;

    operand         lvalue = parse_operand(token_it);
    operation       op     = parse_operation(token_it);
    expr_or_operand rvalue = parse_expr_or_operand(token_it);

    if (std::holds_alternative<operand>(rvalue.value))
    {
        expected(token_it, token_type::end_of_expr);
        ++token_it;
    }

    return { lvalue, op, rvalue };
}

program_structure::operand properties_parser::parse_operand(
    std::vector<token>::iterator& token_it)
{
    using namespace program_structure;

    operand result;

    const auto value_types = { token_type::int_value, token_type::float_value,
                               token_type::string_value };

    // operand can be: declaration, var_name, constant
    const token& first_token = *token_it;
    if (first_token.type == token_type::type_name)
    {
        declaration decl = parse_declaration(token_it);
        variable    var{ decl };
        result.value          = var;
        result.first_token    = &first_token;
        result.past_end_token = &(*token_it);
    }
    else if (first_token.type == token_type::prop_name)
    {
        variable var          = parse_variable_name(token_it);
        result.value          = var;
        result.first_token    = &first_token;
        result.past_end_token = &(*token_it);
    }
    else if (std::any_of(
                 begin(value_types), end(value_types),
                 [&first_token](auto& v) { return v == first_token.type; }))
    {
        constant const_value  = parse_constant(token_it);
        result.value          = const_value;
        result.first_token    = &first_token;
        result.past_end_token = &(*token_it);
    }

    return result;
}

program_structure::declaration properties_parser::parse_declaration(
    std::vector<token>::iterator& token_it)
{
    using namespace program_structure;

    expected(token_it, token_type::type_name);

    const token& first_token = *token_it;
    ++token_it;

    expected(token_it, token_type::prop_name);
    const token& second_token = *token_it;

    declaration decl;
    decl.type     = &first_token;
    decl.var.name = &second_token;

    ++token_it;

    return decl;
}

program_structure::variable properties_parser::parse_variable_name(
    std::vector<token>::iterator& token_it)
{
    using namespace program_structure;

    expected(token_it, token_type::prop_name);

    declaration decl;
    decl.var.name = &(*token_it);
    variable var;
    var.name = decl;

    ++token_it;

    return var;
}

program_structure::constant properties_parser::parse_constant(
    std::vector<token>::iterator& token_it)
{
    using namespace program_structure;

    expected_one_of(token_it, { token_type::int_value, token_type::float_value,
                                token_type::string_value });

    const token* t = &(*token_it);

    ++token_it;
    return { t };
}

program_structure::operation properties_parser::parse_operation(
    std::vector<token>::iterator& token_it)
{
    using namespace program_structure;

    expected(token_it, token_type::operator_name);
    const token* t = &(*token_it);

    ++token_it;
    return { t };
}

program_structure::expr_or_operand properties_parser::parse_expr_or_operand(
    std::vector<token>::iterator& token_it)
{
    using namespace program_structure;

    const token* first_token = &(*token_it);

    operand first_operand = parse_operand(token_it);

    if (token_it->type == token_type::end_of_expr)
    {
        const token* past_end_token = &(*token_it);
        return { first_operand, first_token, past_end_token };
    }
    else
    {
        expression expr = parse_expression(token_it);
        return { new expression(expr) };
    }
}

struct properties_interpretator
{
    properties_parser& parser;
    properties_interpretator(properties_parser& parser_)
        : parser{ parser_ }
    {
    }

    void execute(const program_structure::expression&      command,
                 std::unordered_map<std::string, value_t>& key_values)
    {
        using namespace program_structure;

        if (std::holds_alternative<variable>(command.left_operand.value))
        {
            variable var = std::get<variable>(command.left_operand.value);

            declaration decl = std::get<declaration>(var.name);

            std::string key(decl.var.name->value);

            if (std::holds_alternative<expression*>(
                    command.right_operand.value))
            {
                // TODO calculate expression
                std::clog << "do expression!" << std::endl;
                expression* expr =
                    std::get<expression*>(command.right_operand.value);
                value_t value = calculate_expression(expr, key_values);
            }
            else
            {
                operand right_value =
                    std::get<operand>(command.right_operand.value);

                if (command.op.op->value != "=")
                {
                    throw std::runtime_error(
                        "expected operator =\n" +
                        parser.print_position_of_token(*command.op.op));
                }

                if (decl.type->value == "std::string")
                {
                    if (!std::holds_alternative<constant>(right_value.value))
                    {
                        throw std::runtime_error(
                            "expected std::string constant: " +
                            parser.print_position_of_token(
                                *right_value.first_token));
                    }

                    std::string value(
                        std::get<constant>(right_value.value).value->value);

                    key_values[key] = value;
                }
                else if (decl.type->value == "int")
                {
                    // TODO
                    throw std::runtime_error("implement me");
                }
                else if (decl.type->value == "float")
                {
                    if (!std::holds_alternative<constant>(right_value.value))
                    {
                        throw std::runtime_error("expected float constant: " +
                                                 parser.print_position_of_token(
                                                     *right_value.first_token));
                    }

                    std::string value_str(
                        std::get<constant>(right_value.value).value->value);
                    float value     = stof(value_str);
                    key_values[key] = value;
                }
            }
        }
        else
        {
            // TODO
        }
    }

    void run(std::unordered_map<std::string, value_t>& key_values)
    {
        // TODO interpret program and fill key_values map

        for (const auto& command : parser.commands)
        {
            execute(command, key_values);
        }

        // debug
        for (auto it : key_values)
        {
            std::clog << "key: " << it.first << " value: ";
            if (std::holds_alternative<std::string>(it.second))
            {
                std::clog << std::get<std::string>(it.second);
            }
            else if (std::holds_alternative<std::int32_t>(it.second))
            {
                std::clog << std::get<std::int32_t>(it.second);
            }
            else if (std::holds_alternative<float>(it.second))
            {
                std::clog << std::get<float>(it.second);
            }
            std::clog << std::endl;
        }
    }

private:
    value_t apply(std::string_view operator_literal, const value_t& left,
                  const value_t& right)
    {
        if (std::holds_alternative<std::string>(left))
        {
            const std::string& str0 = std::get<std::string>(left);
            const std::string& str1 = std::get<std::string>(right);
            if (operator_literal == "+")
            {
                return { str0 + str1 };
            }
            throw std::runtime_error("expected + operator for strings");
        }
        else if (std::holds_alternative<int32_t>(left))
        {
            int32_t i0 = std::get<int32_t>(left);
            int32_t i1 = std::get<int32_t>(right);
            if (operator_literal == "+")
            {
                return { i0 + i1 };
            }
            else if (operator_literal == "-")
            {
                return { i0 - i1 };
            }
            else if (operator_literal == "*")
            {
                return { i0 * i1 };
            }
            else if (operator_literal == "/")
            {
                return { i0 / i1 };
            }
        }
        else if (std::holds_alternative<float>(left))
        {
            float f0 = std::get<float>(left);
            float f1 = std::get<float>(right);
            if (operator_literal == "+")
            {
                return { f0 + f1 };
            }
            else if (operator_literal == "-")
            {
                return { f0 - f1 };
            }
            else if (operator_literal == "*")
            {
                return { f0 * f1 };
            }
            else if (operator_literal == "/")
            {
                return { f0 / f1 };
            }
        }
        throw std::runtime_error("value_t !(string, int32_t, float)");
    }
    value_t calculate_expression(
        program_structure::expression*                  expr,
        const std::unordered_map<std::string, value_t>& key_values)
    {
        value_t first_value = get_value(expr->left_operand, key_values);
        std::string_view op = expr->op.op->value;
        if (std::holds_alternative<program_structure::expression*>(
                expr->right_operand.value))
        {
            auto next_exp = std::get<program_structure::expression*>(
                expr->right_operand.value);

            value_t next = calculate_expression(next_exp, key_values);
            return apply(op, first_value, next);
        }
        else
        {
            program_structure::operand operand =
                std::get<program_structure::operand>(expr->right_operand.value);
            value_t next = get_value(operand, key_values);
            return apply(op, first_value, next);
        }
    }
    value_t get_value(
        const program_structure::operand&               op,
        const std::unordered_map<std::string, value_t>& key_values)
    {
        using namespace program_structure;
        // it can be
        // 1. constant {int, float, string}
        // 2. variable {int, float, string}

        if (std::holds_alternative<variable>(op.value))
        {
            variable var = std::get<variable>(op.value);
            if (std::holds_alternative<lvalue>(var.name))
            {
                lvalue lv = std::get<lvalue>(var.name);
                // TODO get value_t by lvalue name
                std::string var_name(lv.name->value);
                auto        it = key_values.find(var_name);
                if (it == std::end(key_values))
                {
                    throw std::runtime_error(
                        "can't find lvalue with name: " + var_name +
                        parser.print_position_of_token(*lv.name));
                }
            }
            else
            {
                throw std::runtime_error("expected lvalue not declaration");
            }
        }
    }
};

class properties_reader::impl
{
public:
    impl(const fs::path& path_)
        : path{ path_ }
    {
        // TODO parse file
        std::ifstream file;
        file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

        file.open(path);

        std::stringstream ss;
        ss << file.rdbuf();

        const std::string content{ ss.str() };

        properties_lexer         lexer(content);
        properties_parser        parser(lexer);
        properties_interpretator generator(parser);

        generator.run(key_values);
    }

    std::unordered_map<std::string, value_t>& get_map() { return key_values; }

private:
    fs::path                                 path;
    std::unordered_map<std::string, value_t> key_values;
};

properties_reader::properties_reader(const fs::path& path)
    : ptr(new impl(path))
{
}

void properties_reader::update_changes() {}

std::string_view properties_reader::get_string(std::string_view key) const
{
    return std::get<std::string>(ptr->get_map()[std::string(key)]);
}

int32_t properties_reader::get_int(std::string_view key) const
{
    return std::get<int32_t>(ptr->get_map()[std::string(key)]);
}

float properties_reader::get_float(std::string_view key) const
{
    return std::get<float>(ptr->get_map()[std::string(key)]);
}

properties_reader::~properties_reader() {}
