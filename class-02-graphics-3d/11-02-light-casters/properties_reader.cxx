// mini grammatics for parser
//
// float     z_near           =       3.f;
// glm::vec3 move_camera      =       { 0.f, 0.f, -2.f };
//
// <type>   <identifier>  <operation> <expression>;
// type:   <float, std::string, glm::vec3>
// identifier: <a-zA-Z_0-9>
// operation: <+, -, =, /, *>
// expression: <float_literal,
//              string_literal,
//              bool_literal,
//              identifier,
//              *_literal operation expression,
//              identifier operation expression,
//              '{' expression ',' expression ',' expression '}'>

#include "properties_reader.hxx"

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

constexpr bool print_debug_info{ false };

struct token
{
    enum class type : uint8_t
    {
        none,
        type_id,
        identifier,
        operation,
        float_literal,
        string_literal,
        bool_literal,
        semicolon,
        open_curly_bracket,
        close_curle_bracket,
        comma
    };

    type             type = type::none;
    std::string_view value;
};

using value_t = std::variant<std::string, glm::vec3, float, bool>;

std::ostream& operator<<(std::ostream& stream, const enum token::type t);
std::ostream& operator<<(std::ostream& stream, const value_t& t);

struct lexer_t
{
    struct token_regex
    {
        token_regex(enum token::type t_, const char* r_)
            : type{ t_ }
            , regex{ r_ }
        {
        }
        enum token::type type;
        std::regex       regex;
    };

    explicit lexer_t(std::string content_)
        : content{ std::move(content_) }
    {
        try
        {
            generate_token_stream();
        }
        catch (...)
        {
            std::cerr << "error: lexer failed:" << std::endl;
            throw;
        }
    }

    std::vector<token> tokens;
    const std::string  content;

private:
    void generate_token_stream()
    {
        std::vector<token_regex> token_bind;

        token_bind.reserve(10);
        token_bind.emplace_back(token::type::type_id,
                                R"(float|std::string|glm::vec3|bool)");
        token_bind.emplace_back(token::type::operation, R"(=|\+|-|\*|\/)");
        token_bind.emplace_back(token::type::float_literal,
                                R"([+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)f)");
        // original from internet: /"([^"\\]|\\.)*"/
        token_bind.emplace_back(token::type::string_literal,
                                R"("([^"\\]|\\.)*")");
        token_bind.emplace_back(token::type::bool_literal, R"(true|false)");
        token_bind.emplace_back(token::type::identifier,
                                R"([a-zA-Z_][a-zA-Z0-9_]*)");
        token_bind.emplace_back(token::type::semicolon, R"(;)");
        token_bind.emplace_back(token::type::open_curly_bracket, R"(\{)");
        token_bind.emplace_back(token::type::close_curle_bracket, R"(\})");
        token_bind.emplace_back(token::type::comma, R"(\,)");
        token_bind.emplace_back(token::type::none, R"(#.*\n|//.*\n| |\n|\t)");

        std::string_view rest_content{ content };
        bool             cant_find_match = false;

        while (!rest_content.empty() && !cant_find_match)
        {
            std::cmatch      token_best_match;
            enum token::type token_best_type{ token::type::none };

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
                tok.type                   = token_best_type;
                auto&       first_match    = token_best_match[0];
                const char* first_char_ptr = first_match.first;
                size_t      length = static_cast<size_t>(first_match.length());
                tok.value          = std::string_view(first_char_ptr, length);

                rest_content = rest_content.substr(tok.value.size());

                if (tok.type == token::type::string_literal)
                {
                    // skip "" - charecters in string literal
                    tok.value = tok.value.substr(1, tok.value.size() - 2);
                }

                if (token_best_type == token::type::none)
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

        if constexpr (print_debug_info)
        {
            for (auto tok : tokens)
            {
                std::clog << tok.type << " = [" << tok.value << "]\n";
            }
        }
    }
};

struct parser_t
{
    struct expression_t
    {
        virtual ~expression_t() = default;
        virtual value_t evaluate(
            std::unordered_map<std::string, value_t>& key_values) const = 0;
    };

    struct definition_ast
    {
        token*                        type_id    = nullptr;
        token*                        identifier = nullptr;
        token*                        operation  = nullptr;
        std::shared_ptr<expression_t> expression;
        token*                        semicolon = nullptr;
    };

    struct literal_expr : expression_t
    {
        token* literal = nullptr;

        value_t evaluate(
            std::unordered_map<std::string, value_t>&) const override
        {
            if (literal->type == token::type::string_literal)
            {
                return std::string{ literal->value };
            }
            else if (literal->type == token::type::float_literal)
            {
                const std::string value{ literal->value };
                return std::stof(value);
            }
            else if (literal->type == token::type::bool_literal)
            {
                return literal->value == "true";
            }
            else
            {
                throw std::runtime_error("error: unsuported literal: " +
                                         std::string(literal->value));
            }
        }
    };

    struct identifier_expr : expression_t
    {
        token* identifier = nullptr;

        value_t evaluate(
            std::unordered_map<std::string, value_t>& key_values) const override
        {
            const std::string value{ identifier->value };
            return key_values.at(value);
        }
    };

    struct gml_vec3_expr : expression_t
    {
        token*                        open_curl = nullptr;
        std::shared_ptr<expression_t> x_component;
        token*                        first_comma = nullptr;
        std::shared_ptr<expression_t> y_component;
        token*                        second_comma = nullptr;
        std::shared_ptr<expression_t> z_component;
        token*                        close_curl = nullptr;

        value_t evaluate(
            std::unordered_map<std::string, value_t>& key_values) const override
        {
            float x = std::get<float>(x_component->evaluate(key_values));
            float y = std::get<float>(y_component->evaluate(key_values));
            float z = std::get<float>(z_component->evaluate(key_values));
            return { glm::vec3(x, y, z) };
        }
    };

    struct sub_expr : expression_t
    {
        std::shared_ptr<expression_t> identifier_or_literal;
        token*                        operation = nullptr;
        std::shared_ptr<expression_t> other_expr;

        value_t evaluate(
            std::unordered_map<std::string, value_t>& key_values) const override
        {
            value_t left  = identifier_or_literal->evaluate(key_values);
            value_t right = other_expr->evaluate(key_values);
            return parser_t::apply(operation->value, left, right);
        }
    };

    std::vector<definition_ast> commands;

    lexer_t& lexer;

    void generate_ast()
    {
        for (auto token_iter = begin(lexer.tokens);
             token_iter != end(lexer.tokens);)
        {
            definition_ast key_val;
            key_val.type_id = expected(token_iter, token::type::type_id);
            ++token_iter;
            key_val.identifier = expected(token_iter, token::type::identifier);
            ++token_iter;
            key_val.operation = expected(token_iter, token::type::operation);
            ++token_iter;
            key_val.expression = parse_expression(token_iter);
            key_val.semicolon  = expected(token_iter, token::type::semicolon);
            ++token_iter;
            commands.push_back(key_val);
        }
    }

    explicit parser_t(lexer_t& lexer_)
        : lexer{ lexer_ }
    {
        try
        {
            generate_ast();
        }
        catch (...)
        {
            std::cerr << "error: parser failed:" << std::endl;
            throw;
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
                    const enum token::type              type)
    {
        if (it == end(lexer.tokens))
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

    token* expected_one_of(const std::vector<token>::iterator&            it,
                           const std::initializer_list<enum token::type>& types)
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
    std::shared_ptr<expression_t> parse_expression(
        std::vector<token>::iterator& token_it)
    {
        if (token_it->type == token::type::open_curly_bracket)
        {
            // parse {expression, expression, expression};
            auto expr = std::make_shared<parser_t::gml_vec3_expr>();
            expr->open_curl =
                expected(token_it, token::type::open_curly_bracket);
            ++token_it;
            expr->x_component = parse_expression(token_it);
            expr->first_comma = expected(token_it, token::type::comma);
            ++token_it;
            expr->y_component  = parse_expression(token_it);
            expr->second_comma = expected(token_it, token::type::comma);
            ++token_it;
            expr->z_component = parse_expression(token_it);
            expr->close_curl =
                expected(token_it, token::type::close_curle_bracket);
            ++token_it;
            return expr;
        }
        else if (token_it->type == token::type::float_literal ||
                 token_it->type == token::type::string_literal ||
                 token_it->type == token::type::bool_literal)
        {
            token* literal = &*token_it;
            ++token_it;
            if (token_it->type == token::type::semicolon ||
                token_it->type == token::type::close_curle_bracket ||
                token_it->type == token::type::comma)
            {
                auto expr     = std::make_shared<parser_t::literal_expr>();
                expr->literal = literal;
                return expr;
            }
            else
            {
                token* operation  = expected(token_it, token::type::operation);
                auto   first_expr = std::make_shared<literal_expr>();
                first_expr->literal         = literal;
                auto expr                   = std::make_shared<sub_expr>();
                expr->identifier_or_literal = first_expr;
                expr->operation             = operation;
                ++token_it;
                expr->other_expr = parse_expression(token_it);
                return expr;
            }
        }
        else if (token_it->type == token::type::identifier)
        {
            token* identifier = &*token_it;
            ++token_it;
            if (token_it->type == token::type::semicolon ||
                token_it->type == token::type::close_curle_bracket ||
                token_it->type == token::type::comma)
            {
                auto expr = std::make_shared<parser_t::identifier_expr>();
                expr->identifier = identifier;
                return expr;
            }
            else
            {
                token* operation  = expected(token_it, token::type::operation);
                auto   first_expr = std::make_shared<identifier_expr>();
                first_expr->identifier      = identifier;
                auto expr                   = std::make_shared<sub_expr>();
                expr->identifier_or_literal = first_expr;
                expr->operation             = operation;
                ++token_it;
                expr->other_expr = parse_expression(token_it);
                return expr;
            }
        }

        throw std::runtime_error("error: parse failed:");
    }

    static value_t apply(std::string_view operator_literal, const value_t& left,
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
};

struct interpretator_t
{
    parser_t& parser;
    interpretator_t(parser_t& parser_)
        : parser{ parser_ }
    {
    }

    void run(std::unordered_map<std::string, value_t>& key_values)
    {
        // interpret program and fill key_values map
        for ([[maybe_unused]] const auto& command : parser.commands)
        {
            execute(command, key_values);
        }

        if constexpr (print_debug_info)
        {
            // dump values
            for (auto [key, value] : key_values)
            {
                std::cout << key << "=[" << value << "]" << std::endl;
            }
        }
    }

private:
    void execute(const parser_t::definition_ast&           command,
                 std::unordered_map<std::string, value_t>& key_values)
    {
        value_t     value = command.expression->evaluate(key_values);
        std::string identifier{ command.identifier->value };
        key_values[identifier] = value;
    }
};

class properties_reader::impl
{
public:
    impl(const fs::path& path_)
        : path{ path_ }
        , last_update_time{ fs::last_write_time(path) }
    {
        build_properties_map();
    }

    std::unordered_map<std::string, value_t>& get_map() { return key_values; }

    void update_changes()
    {
        fs::file_time_type new_time = fs::last_write_time(path);
        if (new_time != last_update_time)
        {
            last_update_time = new_time;
            build_properties_map();
        }
    }

private:
    std::string load_file()
    {
        std::ifstream file;
        file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

        file.open(path);

        return { std::istreambuf_iterator<char>(file),
                 std::istreambuf_iterator<char>() };
    }

    void build_properties_map()
    {
        std::string content{ load_file() };

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

bool properties_reader::get_bool(std::string_view name) const
{
    return std::get<bool>(ptr->get_map()[std::string{ name }]);
}

properties_reader::~properties_reader() {}

std::ostream& operator<<(std::ostream& stream, const enum token::type t)
{
    const auto index_of_type = static_cast<size_t>(t);

    if (index_of_type > static_cast<size_t>(token::type::comma))
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
                         "bool_literal",
                         "semicolon",
                         "open_braket",
                         "close_braket",
                         "comma" };
    // clang-format on

    const char* type_name = names[index_of_type];
    stream << type_name;

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const value_t& t)
{
    if (std::holds_alternative<float>(t))
    {
        stream << std::get<float>(t);
    }
    else if (std::holds_alternative<std::string>(t))
    {
        stream << std::get<std::string>(t);
    }
    else if (std::holds_alternative<glm::vec3>(t))
    {
        glm::vec3 v = std::get<glm::vec3>(t);
        stream << v.x << ',' << v.y << ',' << v.z;
    }
    else if (std::holds_alternative<bool>(t))
    {
        stream << std::get<bool>(t);
    }
    else
    {
        throw std::runtime_error("unknown type");
    }
    return stream;
}
#pragma clang diagnostic pop
