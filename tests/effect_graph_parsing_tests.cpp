#include "testing.hpp"
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace rgbctl::effects::parsing
{

struct InvalidEscapeSequenceError : std::runtime_error
{
    explicit InvalidEscapeSequenceError(char const* p) noexcept
        : runtime_error { "Invalid escape sequence" }
        , at { p }
    { }

    char const* at;
};

enum class TokenType
{
    colon,
    comma,
    semicolon,
    equals,
    string,
    byte,
    end,
    backslash,
    invalid_escape,
    whitespace
};

struct Token
{
    TokenType type;
    std::string_view value;
};

struct ParseError : std::runtime_error
{
    explicit ParseError(Token t) noexcept
        : runtime_error { "Unexpected token" }
        , token { t }
    { }

    Token token;
};

auto peek(char const* first,
          char const* last,
          std::size_t n = 1,
          bool escaped = false) noexcept -> std::pair<TokenType, std::size_t>
{
    if (first == last) {
        return { TokenType::end, 0 };
    }

    if (escaped) {
        switch (*first) {
        case ':':
        case ',':
        case ';':
        case '=':
        case '\\':
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return { TokenType::byte, n };
        default:
            return { TokenType::invalid_escape, n };
        }
    }

    switch (*first) {
    case ':':
        return { TokenType::colon, n };
    case ',':
        return { TokenType::comma, n };
    case ';':
        return { TokenType::semicolon, n };
    case '=':
        return { TokenType::equals, n };
    case '\\':
        return peek(++first, last, n + 1, true);
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        return { TokenType::whitespace, n };
    default:
        return { TokenType::byte, n };
    }
}

auto consume(char const*& first, char const* last) noexcept -> TokenType
{
    auto const [token_type, n] = peek(first, last);
    std::advance(first, n);
    return token_type;
}

auto group(char const* first, char const* const last, TokenType input)
    -> std::size_t
{
    std::size_t len = 0;
    auto [type, n] = peek(first, last);
    for (; type == input; std::tie(type, n) = peek(first, last)) {
        std::advance(first, n);
        len += n;
    }

    return len;
}

template <typename Alloc = std::allocator<Token>>
auto tokenize(char const* data, std::size_t len, Alloc alloc = Alloc {})
    -> std::vector<
        Token,
        typename std::allocator_traits<Alloc>::template rebind_alloc<Token>>
{
    std::vector<
        Token,
        typename std::allocator_traits<Alloc>::template rebind_alloc<Token>>
        tokens { alloc };

    char const* end = data + len;

    while (data != end) {
        auto saved_pos = data;

        auto current = consume(data, end);

        if (current == TokenType::invalid_escape)
            throw InvalidEscapeSequenceError(data);

        if (current == TokenType::byte) {
            auto n = group(data, end, TokenType::byte);
            std::advance(data, n);
            tokens.push_back(
                { TokenType::string,
                  { saved_pos, static_cast<std::size_t>(data - saved_pos) } });
        }
        else if (current == TokenType::whitespace) {
            auto n = group(data, end, TokenType::whitespace);
            std::advance(data, n);
            tokens.push_back(
                { TokenType::whitespace,
                  { saved_pos, static_cast<std::size_t>(data - saved_pos) } });
        }
        else {
            tokens.push_back({ current, { saved_pos, 1 } });
        }
    }

    return tokens;
}

template <typename Iter>
auto expect(std::initializer_list<TokenType> token_types,
            Iter first,
            Iter last) noexcept -> bool
{
    if (static_cast<decltype(token_types.size())>(std::distance(first, last))
        < token_types.size())
        return false;

    auto i = std::begin(token_types);
    for (; i != std::end(token_types); ++i, (void)++first)
        if (*i != first->type)
            return false;

    return true;
}

enum class ComponentSource
{
    builtin,
    user_defined
};

struct ComponentDefinition
{
    struct Parameter
    {
        std::string name;
        std::string value;
    };

    std::string name;
    ComponentSource source;
    std::vector<Parameter> parameters;
};

template <typename T, typename P>
auto unescape(P const& value) -> T
{
    T result {};

    using std::begin;
    using std::end;

    auto pos = begin(value);
    while (pos != end(value)) {
        if (*pos == '\\') {
            ++pos;
        }

        if (pos != end(value))
            result.push_back(*pos++);
    }

    return result;
}

template <typename It>
auto parse_component_definition(It first, It last, ComponentDefinition& def)
    -> bool
{
    if (!expect({ TokenType::string }, first, last))
        return false;

    Token a, b;

    a = *first++;

    if (expect({ TokenType::colon, TokenType::string }, first, last)) {
        b = *(++first);
        ++first;

        def.name = b.value;
        if (a.value == "builtin") {
            def.source = ComponentSource::builtin;
        }
        else if (a.value == "user") {
            def.source = ComponentSource::user_defined;
        }
    }
    else {
        def.name = a.value;
        def.source = ComponentSource::builtin;
    }

    if (expect({ TokenType::semicolon }, first, last)) {
        do {
            ++first;
            if (!expect({ TokenType::string }, first, last))
                break;

            ComponentDefinition::Parameter p;
            p.name = (*first++).value;
            if (expect({ TokenType::equals, TokenType::string }, first, last)) {
                Token arg_value = *(++first);
                ++first;
                p.value = unescape<std::string>(arg_value.value);
            }

            def.parameters.push_back(std::move(p));
        }
        while (expect({ TokenType::semicolon }, first, last));
    }

    return true;
}

} // namespace rgbctl::effects::parsing

using rgbctl::effects::parsing::ComponentDefinition;
using rgbctl::effects::parsing::parse_component_definition;
using rgbctl::effects::parsing::Token;
using rgbctl::effects::parsing::tokenize;
using rgbctl::effects::parsing::TokenType;

auto operator<<(std::ostream& os, Token const& token) -> std::ostream&
{
    switch (token.type) {
    case TokenType::colon:
        return os << "COLON";
    case TokenType::comma:
        return os << "COMMA";
    case TokenType::semicolon:
        return os << "SEMICOLON";
    case TokenType::equals:
        return os << "EQUALS";
    case TokenType::string:
        return os << "STRING (" << token.value << ")";
    case TokenType::byte:
        return os << "BYTE (" << token.value << ")";
    case TokenType::backslash:
        return os << "BACKSLASH";
    case TokenType::invalid_escape:
        return os << "INVALID_ESCAPE";
    case TokenType::whitespace:
        return os << "WS";
    case TokenType::end:
        return os << "END";
    }

    return os;
}

auto should_parse_component()
{
    auto constexpr kDefintition
        = "builtin:colour-array;width=6;values=1\\,\\ 2\\,\\ 3\\,\\ 4\\,\\ "
          "5\\,\\ 6";

    auto tokens = tokenize(kDefintition, std::strlen(kDefintition));
    for (auto const& t : tokens)
        std::cerr << t << '\n';

    std::cerr << tokens.size() << '\n';
    EXPECT(tokens.size() == 11);

    EXPECT(expect(
        {
            TokenType::string,
            TokenType::colon,
            TokenType::string,
            TokenType::semicolon,
            TokenType::string,
            TokenType::equals,
            TokenType::string,
            TokenType::semicolon,
            TokenType::string,
            TokenType::equals,
            TokenType::string,
        },
        begin(tokens),
        end(tokens)));

    ComponentDefinition def;
    EXPECT(parse_component_definition(begin(tokens), end(tokens), def));

    std::cout << def.name << '\n';
    for (auto const& p : def.parameters)
        std::cout << p.name << '=' << p.value << '\n';
}

auto main() -> int
{
    return rgbctl::testing::run({ TEST(should_parse_component) });
}
