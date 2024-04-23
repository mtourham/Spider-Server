#ifndef REQUESTLEXER_HPP
#define REQUESTLEXER_HPP
#include <iostream>
#include <string>
#include <vector>

enum type 
{
    TYPE_TEXT,
    TOKEN_END_OF_LINE,
    TOKEN_CR,
    TOKEN_END_OF_SSECTION,
    TOKEN_TWO_POINT,
    TYPE_EOF
};

struct Token 
{
    std::string value;
    int type;
};

class requestLexer 
{
    private:
        const std::string& src;
        const unsigned int size;
        unsigned int index;
        bool is_key;
        Token _Token;

    public:
    requestLexer(const std::string& val)
        : src(val), size(val.length()), index(0), is_key(false)
    {
        _Token.type = TYPE_EOF;
    }

    const std::string& get_src() const
    {
        return src;
    }

    int get_index(){return index;}

    Token get_next_Token(int is_line) {
        _Token.value.clear();

        if (index >= size) {
            _Token.type = TYPE_EOF;
            return _Token;
        }

        while (index < size) {
            if (is_double_newline() || is_carriage_return_pair()) {
                _Token.type = TOKEN_END_OF_SSECTION;
                skip(2);
                break;
            }

            if (is_newline()) {
                _Token.type = TOKEN_END_OF_LINE;
                _Token.value += src[index];
                is_key = false;
                ++index;
                break;
            }

            if (is_carriage_return()) {
                _Token.type = TOKEN_CR;
                _Token.value += src[index];
                is_key = false;
                ++index;
                break;
            }

            if (is_whitespace() && !is_key) {
                skip_whitespace();
                break;
            }

            if (is_colon(is_line) && !is_key && is_line == 1) {
                _Token.type = TOKEN_TWO_POINT;
                _Token.value += src[index];
                is_key = true;
                ++index;
                break;
            }

            _Token.value += src[index];
            _Token.type = TYPE_TEXT;

            ++index;

            if (is_break_condition(is_line))
                break;
        }

        return _Token;
    }

    // Helper functions
    bool is_double_newline() {
        return (src[index] == '\n' && (index + 1 < size && src[index + 1] == '\n')) ||
            (src[index] == 13 && (index + 1 < size && src[index + 1] == 13));
    }

    bool is_carriage_return_pair() {
        return (src[index] == 13 && (index + 1 < size && src[index + 1] == 10)) &&
            (index + 2 < size && src[index + 2] == 13) &&
            (index + 3 < size && src[index + 3] == 10);
    }

    bool is_newline() {
        return src[index] == '\n';
    }

    bool is_carriage_return() {
        return src[index] == 13;
    }

    bool is_whitespace() {
        return (src[index] == '\t' || src[index] == 32) && !is_key;
    }

    void skip_whitespace() {
        while (src[index] == '\t' || src[index] == 32)
            ++index;
    }

    bool is_colon(int is_line) {
        return src[index] == ':' && !is_key && is_line == 1;
    }

    void skip(int count) {
        index += count;
    }

    bool is_break_condition(int is_line) {
        return (src[index] == 13 || src[index] == '\n' ||
                (src[index] == ':' && !is_key && is_line == 1) ||
                (src[index] == '\n' && (index + 1 < size && src[index + 1] == '\n')) ||
                (src[index] == 13 && (index + 1 < size && src[index + 1] == 13)));
    }

};

#endif