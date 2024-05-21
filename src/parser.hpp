#pragma once

#include <any>
#include <stdexcept>
#include <string>
#include <vector>

enum Keyword { Def, Defn, If };

std::string keyword_str(Keyword kw);

struct Token {
  enum Type {
    Paren,
    Bracket,
    Quote,
    Operator,
    Number,
    Bool,
    Keyword,
    Identifier
  } type;
  std::any value;

  template <typename T> T as() { return std::any_cast<T>(value); }

  template <typename T> T as() const { return std::any_cast<T>(value); }

  Token get_if(Type _type) {
    if (_type != type) {
      throw std::runtime_error("Token::get_if() failed");
    }
    return *this;
  }
};

std::vector<Token> parse(const std::string &source);
