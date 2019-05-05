#include "properties_reader.hxx"

//#include <charconv> // not found on Visual Studio 2017.7
#include <algorithm>
#include <charconv>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"

struct token
{
    enum class type_t : uint8_t
    {
        none,
        type_id,
        identifier,
        operation,
        float_literal,
        string_literal,
        semicolon,
        open_curly_bracket,
        close_curle_bracket,
        comma
    };

    type_t           type_t = type_t::none;
    std::string_view value;
};

std::ostream& operator<<(std::ostream& stream, const enum token::type_t t);

struct lexer_t
{
    struct token_regex
    {
        token_regex(enum token::type_t t_, const char* r_)
            : type{ t_ }
            , regex{ r_ }
        {
        }
        enum token::type_t type;
        std::regex         regex;
    };

    explicit lexer_t(std::string content_)
        : content{ std::move(content_) }
    {
        std::vector<token_regex> token_bind;

        token_bind.reserve(10);
        token_bind.emplace_back(token::type_t::type_id,
                                R"(float|std::string|glm::vec3)");
        token_bind.emplace_back(token::type_t::operation, R"(=|\+|-|\*|\/)");
        token_bind.emplace_back(token::type_t::identifier,
                                R"([a-zA-Z_][a-zA-Z0-9_]*)");
        token_bind.emplace_back(token::type_t::float_literal,
                                R"([+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)f)");
        // original from internet: /"([^"\\]|\\.)*"/
        token_bind.emplace_back(token::type_t::string_literal,
                                R"("([^"\\]|\\.)*")");
        token_bind.emplace_back(token::type_t::semicolon, R"(;)");
        token_bind.emplace_back(token::type_t::open_curly_bracket, R"(\{)");
        token_bind.emplace_back(token::type_t::close_curle_bracket, R"(\})");
        token_bind.emplace_back(token::type_t::comma, R"(\,)");
        token_bind.emplace_back(token::type_t::none, R"(#.*\n|//.*\n| |\n|\t)");

        std::string_view rest_content{ content };
        bool             cant_find_match = false;

        while (!rest_content.empty() && !cant_find_match)
        {
            std::cmatch        token_best_match;
            enum token::type_t token_best_type{ token::type_t::none };

            for (token_regex& tok_regex : token_bind)
            {
                std::cmatch token_match;
                if (std::regex_search(rest_content.data(),
                                      rest_content.data() +
                                          rest_content.length(),
                                      token_match, tok_regex.regex,
                                      std::regex_constants::match_continuous))
                {
                    auto first = token_match.cbegin();
                    if (token_best_match.empty() ||
                        first->length() > token_best_match.cbegin()->length())
                    {
                        token_best_match = token_match;
                        token_best_type  = tok_regex.type;
                    }
                }
            }

            if (!token_best_match.empty())
            {
                token tok;
                tok.type_t                 = token_best_type;
                auto&       first_match    = token_best_match[0];
                const char* first_char_ptr = first_match.first;
                size_t      length = static_cast<size_t>(first_match.length());
                tok.value          = std::string_view(first_char_ptr, length);

                rest_content = rest_content.substr(tok.value.size());

                if (tok.type_t == token::type_t::string_literal)
                {
                    // skip "" - charecters in string literal
                    tok.value = tok.value.substr(1, tok.value.size() - 2);
                }

                if (token_best_type == token::type_t::none)
                {
                    continue;
                }
                else
                {
                    tokens.push_back(tok);
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

        for (auto tok : tokens)
        {
            std::clog << tok.type_t << " = [" << tok.value << "]\n";
        }
    }

    std::vector<token> tokens;
    const std::string  content;
};

using value_t = std::variant<std::string, glm::vec3, float>;

namespace prog_struct
{

struct var_name
{
    const token* name = nullptr;
};

struct declaration
{
    const token* type = nullptr;
    var_name     name_tok;
};

struct variable
{
    std::variant<std::monostate, declaration, var_name> name;
};

struct constant
{
    const token* value = nullptr;
};

struct operation
{
    const token* op = nullptr;
};

using name_or_constant = std::variant<std::monostate, variable, constant>;

struct init_list
{
    std::forward_list<name_or_constant> items;
};

struct operand
{
    name_or_constant value;
    const token*     first_token    = nullptr;
    const token*     past_end_token = nullptr;
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
} // namespace prog_struct

struct parser_t
{
    std::vector<prog_struct::expression> commands;

    prog_struct::expression parse_expression(
        std::vector<token>::iterator& token_it);
    prog_struct::operand parse_operand(std::vector<token>::iterator& token_it);
    prog_struct::declaration parse_declaration(
        std::vector<token>::iterator& token_it);
    prog_struct::variable parse_variable_name(
        std::vector<token>::iterator& token_it);
    prog_struct::constant parse_constant(
        std::vector<token>::iterator& token_it);
    prog_struct::operation parse_operation(
        std::vector<token>::iterator& token_it);
    prog_struct::expr_or_operand parse_expr_or_operand(
        std::vector<token>::iterator& token_it);

    lexer_t& lexer;

    parser_t(lexer_t& lexer_)
        : lexer{ lexer_ }
    {
        using namespace prog_struct;

        for (auto token_iter = begin(lexer.tokens);
             token_iter != end(lexer.tokens);)
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
                    const enum token::type_t            type)
    {
        if (it == end(lexer.tokens))
        {
            std::stringstream ss;
            ss << "error: expected " << type << " but got: EOF";
            throw std::runtime_error(ss.str());
        }
        if (it->type_t != type)
        {
            std::stringstream ss;
            ss << "error: expected " << type << " but got: " << it->value;
            ss << print_position_of_token(*it);
            throw std::runtime_error(ss.str());
        }
        return &(*it);
    }

    token* expected_one_of(
        const std::vector<token>::iterator&              it,
        const std::initializer_list<enum token::type_t>& types)
    {
        if (it == end(lexer.tokens))
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
        auto type_it = std::find(begin(types), end(types), it->type_t);
        if (type_it == end(types))
        {
            std::stringstream ss;
            ss << "error: expected ";
            for (auto type : types)
            {
                ss << type << ", ";
            }
            ss << "but got: " << it->type_t;
            ss << print_position_of_token(*it);
            throw std::runtime_error(ss.str());
        }
        return &(*it);
    }
};

prog_struct::expression parser_t::parse_expression(
    std::vector<token>::iterator& token_it)
{
    using namespace prog_struct;

    operand         lvalue = parse_operand(token_it);
    operation       op     = parse_operation(token_it);
    expr_or_operand rvalue = parse_expr_or_operand(token_it);

    if (std::holds_alternative<operand>(rvalue.value))
    {
        expected(token_it, token::type_t::semicolon);
        ++token_it;
    }

    return { lvalue, op, rvalue };
}

prog_struct::operand parser_t::parse_operand(
    std::vector<token>::iterator& token_it)
{
    using namespace prog_struct;

    operand result;

    const auto value_types = { token::type_t::float_literal,
                               token::type_t::string_literal };

    // operand can be: declaration, var_name, constant
    const token& first_token = *token_it;
    if (first_token.type_t == token::type_t::type_id)
    {
        declaration decl = parse_declaration(token_it);
        variable    var{ decl };
        result.value          = var;
        result.first_token    = &first_token;
        result.past_end_token = &(*token_it);
    }
    else if (first_token.type_t == token::type_t::identifier)
    {
        variable var          = parse_variable_name(token_it);
        result.value          = var;
        result.first_token    = &first_token;
        result.past_end_token = &(*token_it);
    }
    else if (first_token.type_t == token::type_t::open_curly_bracket)
    {
        // TODO implement it
        std::clog << "implement me";
    }
    else if (std::any_of(
                 begin(value_types), end(value_types),
                 [&first_token](auto& v) { return v == first_token.type_t; }))
    {
        constant const_value  = parse_constant(token_it);
        result.value          = const_value;
        result.first_token    = &first_token;
        result.past_end_token = &(*token_it);
    }

    return result;
}

prog_struct::declaration parser_t::parse_declaration(
    std::vector<token>::iterator& token_it)
{
    using namespace prog_struct;

    expected(token_it, token::type_t::type_id);

    const token& first_token = *token_it;
    ++token_it;

    expected(token_it, token::type_t::identifier);
    const token& second_token = *token_it;

    declaration decl;
    decl.type          = &first_token;
    decl.name_tok.name = &second_token;

    ++token_it;

    return decl;
}

prog_struct::variable parser_t::parse_variable_name(
    std::vector<token>::iterator& token_it)
{
    using namespace prog_struct;

    expected(token_it, token::type_t::identifier);

    var_name var_name;
    var_name.name = &(*token_it);

    variable var;
    var.name = var_name;

    ++token_it;

    return var;
}

prog_struct::constant parser_t::parse_constant(
    std::vector<token>::iterator& token_it)
{
    using namespace prog_struct;

    expected_one_of(token_it, { token::type_t::float_literal,
                                token::type_t::string_literal });

    const token* t = &(*token_it);

    ++token_it;
    return { t };
}

prog_struct::operation parser_t::parse_operation(
    std::vector<token>::iterator& token_it)
{
    using namespace prog_struct;

    expected(token_it, token::type_t::operation);
    const token* t = &(*token_it);

    ++token_it;
    return { t };
}

prog_struct::expr_or_operand parser_t::parse_expr_or_operand(
    std::vector<token>::iterator& token_it)
{
    using namespace prog_struct;

    std::vector<token>::iterator token_it_copy = token_it;

    const token* first_token = &(*token_it);

    operand first_operand = parse_operand(token_it);

    if (token_it->type_t == token::type_t::semicolon)
    {
        const token* past_end_token = &(*token_it);
        return { first_operand, first_token, past_end_token };
    }
    else
    {
        expression   expr           = parse_expression(token_it_copy);
        const token* past_end_token = &(*token_it_copy);
        // restore iterator to current state
        token_it = token_it_copy;
        // FIXME memory leak
        return { new expression(expr), first_token, past_end_token };
    }
}

struct interpretator_t
{
    parser_t& parser;
    interpretator_t(parser_t& parser_)
        : parser{ parser_ }
    {
    }

    void execute(const prog_struct::expression&            command,
                 std::unordered_map<std::string, value_t>& key_values)
    {
        using namespace prog_struct;

        if (std::holds_alternative<variable>(command.left_operand.value))
        {
            variable var = std::get<variable>(command.left_operand.value);

            declaration decl = std::get<declaration>(var.name);

            std::string key(decl.name_tok.name->value);

            if (std::holds_alternative<expression*>(
                    command.right_operand.value))
            {
                expression* expr =
                    std::get<expression*>(command.right_operand.value);
                value_t value   = calculate_expression(expr, key_values);
                key_values[key] = value;
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
        // interpret program and fill key_values map
        for (const auto& command : parser.commands)
        {
            execute(command, key_values);
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
        else if (std::holds_alternative<glm::vec3>(left))
        {
            glm::vec3 v0 = std::get<glm::vec3>(left);
            glm::vec3 v1 = std::get<glm::vec3>(right);
            if (operator_literal == "+")
            {
                return { v0 + v1 };
            }
            else if (operator_literal == "-")
            {
                return { v0 - v1 };
            }
            else
            {
                throw std::runtime_error("can't do operator: " +
                                         std::string(operator_literal));
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
        throw std::runtime_error("value_t !(string, glm::vec3, float)");
    }
    value_t calculate_expression(
        prog_struct::expression*                        expr,
        const std::unordered_map<std::string, value_t>& key_values)
    {
        value_t first_value = get_value(expr->left_operand, key_values);
        std::string_view op = expr->op.op->value;
        if (std::holds_alternative<prog_struct::expression*>(
                expr->right_operand.value))
        {
            auto next_exp =
                std::get<prog_struct::expression*>(expr->right_operand.value);

            value_t next = calculate_expression(next_exp, key_values);
            return apply(op, first_value, next);
        }
        else
        {
            prog_struct::operand operand =
                std::get<prog_struct::operand>(expr->right_operand.value);
            value_t next = get_value(operand, key_values);
            return apply(op, first_value, next);
        }
    }
    value_t get_value(
        const prog_struct::operand&                     op,
        const std::unordered_map<std::string, value_t>& key_values)
    {
        using namespace prog_struct;
        // it can be
        // 1. constant {int, float, string}
        // 2. variable {int, float, string}

        if (std::holds_alternative<variable>(op.value))
        {
            variable var = std::get<variable>(op.value);
            if (std::holds_alternative<var_name>(var.name))
            {
                var_name lv = std::get<var_name>(var.name);
                // TODO get value_t by lvalue name
                std::string var_name(lv.name->value);
                auto        it = key_values.find(var_name);
                if (it == std::end(key_values))
                {
                    throw std::runtime_error(
                        "can't find lvalue with name: " + var_name +
                        parser.print_position_of_token(*lv.name));
                }
                return it->second;
            }
            else
            {
                throw std::runtime_error("expected lvalue not declaration");
            }
        }
        else if (std::holds_alternative<constant>(op.value))
        {
            constant val = std::get<constant>(op.value);
            if (val.value->type_t == token::type_t::string_literal)
            {
                return { std::string(val.value->value) };
            }
            else if (val.value->type_t == token::type_t::float_literal)
            {
                float result = std::stof(std::string(val.value->value));
                return { result };
            }
            throw std::runtime_error("constant not (int, float, string)");
        }
        throw std::runtime_error("expected operant be constant or variable");
    }
};

class properties_reader::impl
{
public:
    impl(const fs::path& path_)
        : path{ path_ }
        , last_update_time{ fs::last_write_time(path) }
    {
        load_and_parse();
    }

    std::unordered_map<std::string, value_t>& get_map() { return key_values; }

    void update_changes()
    {
        fs::file_time_type new_time = fs::last_write_time(path);
        if (new_time != last_update_time)
        {
            last_update_time = new_time;
            load_and_parse();
        }
    }

private:
    void load_and_parse()
    {
        std::ifstream file;
        file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

        file.open(path);

        std::string content{ std::istreambuf_iterator<char>(file),
                             std::istreambuf_iterator<char>() };

        lexer_t         lexer(content);
        parser_t        parser(lexer);
        interpretator_t interpretator(parser);

        std::unordered_map<std::string, value_t> generated_key_values;
        interpretator.run(generated_key_values);

        std::swap(generated_key_values, key_values);
    }

    std::unordered_map<std::string, value_t> key_values;
    fs::path                                 path;
    fs::file_time_type                       last_update_time;
};

properties_reader::properties_reader(const fs::path& path)
    : ptr(new impl(path))
{
}

void properties_reader::update_changes()
{
    ptr->update_changes();
}

const std::string& properties_reader::get_string(std::string_view key) const
{
    return std::get<std::string>(ptr->get_map()[std::string(key)]);
}

float properties_reader::get_float(std::string_view key) const
{
    return std::get<float>(ptr->get_map()[std::string(key)]);
}

const glm::vec3& properties_reader::get_vec3(std::string_view name) const
    noexcept(false)
{
    return std::get<glm::vec3>(ptr->get_map()[std::string(name)]);
}

properties_reader::~properties_reader() {}

std::ostream& operator<<(std::ostream& stream, const enum token::type_t t)
{
    const auto index_of_type = static_cast<size_t>(t);

    if (index_of_type > static_cast<size_t>(token::type_t::comma))
    {
        throw std::runtime_error("invalid t, forgot to add new value?");
    }

    // clang-format off
    const char* names[]{ "none",
                         "type_id",
                         "identifier",
                         "operation",
                         "float_literal",
                         "string_literal",
                         "semicolon",
                         "open_braket",
                         "close_braket",
                         "comma" };
    // clang-format on

    const char* type_name = names[index_of_type];
    stream << type_name;

    return stream;
}
