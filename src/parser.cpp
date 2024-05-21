#include "parser.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstring>
#include <vector>

bool is_paren(char c) { return c == '(' || c == ')'; }

bool is_op(char c) {
  return c == '+' || c == '*' || c == '-' || c == '/' || c == '=' || c == '<' ||
         c == '>';
}

bool is_whitespace(char c) {
  return c == '\n' || c == ' ' || c == '\t' || c == '\0';
}

bool is_bracket(char c) { return c == '[' || c == ']'; }

bool is_quote(char c) { return c == '\''; }

bool is_special(char c) {
  return is_paren(c) || is_op(c) || is_whitespace(c) || is_bracket(c) ||
         is_quote(c);
}

bool is_number(const std::string &str) {
  return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string keyword_str(Keyword kw) {
  switch (kw) {
  case Keyword::Def:
    return "DEF";
  case Keyword::Defn:
    return "DEFN";
  case Keyword::If:
    return "IF";
  }
  return "???";
}

std::vector<Token> parse(const std::string &source) {
  std::vector<Token> tokens;
  const char *cstr = source.c_str();

  std::size_t i = 0;
  char buf[1024];
  std::memset(buf, 0, 1024);
  std::size_t bufi = 0;

  while (true) {
    while (!is_special(cstr[i])) {
      buf[bufi++] = cstr[i++];
    }

    if (bufi == 0) {
      if (is_whitespace(cstr[i])) {
        if (cstr[i] == '\0') {
          break;
        }
        i++;
        continue;
      }

      if (cstr[i] == '\0') {
        break;
      }

      if (is_paren(cstr[i])) {
        tokens.push_back({Token::Paren, cstr[i]});
        i++;
        continue;
      }

      if (is_op(cstr[i])) {
        tokens.push_back({Token::Operator, cstr[i]});
        i++;
        continue;
      }

      if (is_bracket(cstr[i])) {
        tokens.push_back({Token::Bracket, cstr[i]});
        i++;
        continue;
      }

      if (is_quote(cstr[i])) {
        tokens.push_back({Token::Quote, cstr[i]});
        i++;
        continue;
      }
    }

    if (bufi != 0) {
      buf[bufi] = '\0';
    }

    std::string str = bufi == 0 ? &cstr[i] : buf;

    if (str == "def") {
      tokens.push_back({Token::Keyword, Keyword::Def});
    } else if (str == "defn") {
      tokens.push_back({Token::Keyword, Keyword::Defn});
    } else if (str == "if") {
      tokens.push_back({Token::Keyword, Keyword::If});
    } else if (str == "true") {
      tokens.push_back({Token::Bool, true});
    } else if (str == "false") {
      tokens.push_back({Token::Bool, false});
    } else if (is_number(str)) {
      int number;
      std::from_chars(str.data(), str.data() + str.size(), number);
      tokens.push_back({Token::Number, number});
    } else {
      tokens.push_back({Token::Identifier, str});
    }

    if (cstr[i] == '\0') {
      break;
    }

    bufi = 0;
  }

  return tokens;
}
