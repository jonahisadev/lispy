#pragma once

#include "node.hpp"

#include <any>
#include <string>
#include <vector>

struct Function {
  Vector params;
  std::vector<Node> body;
};

using NativeFunction = std::function<Node(Interpreter &, std::vector<Node>)>;

struct Variable {
  enum Type { Integer, String, Bool, Function, Vec, List, NativeFn } type;
  std::string name;
  std::any value;

  template <typename T> T as() { return std::any_cast<T>(value); }

  template <typename T> T as() const { return std::any_cast<T>(value); }

  inline bool is_function() { return type == Function || type == NativeFn; }
};
