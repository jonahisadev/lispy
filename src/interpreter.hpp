#pragma once

#include "core.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "stack.hpp"
#include "variable.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

#define NFN(x)                                                                 \
  add_symbol(#x, Variable{Variable::NativeFn, #x, (NativeFunction)Core::x})

class Interpreter {
private:
  Stack<std::unordered_map<std::string, Variable>> m_symbols;

public:
  Interpreter() {
    std::cout << "Loading interpreter..." << std::endl;
    new_scope();

    // TODO: there might be an easier way to handle this
    NFN(sqrt);
    NFN(map);
    NFN(range);
    NFN(size);
    NFN(len);
    NFN(head);
    NFN(tail);
    NFN(nth);
    NFN(rem);
    NFN(filter);
    NFN(reduce);
  }
  ~Interpreter() {}

  std::vector<Node> compile(std::vector<Token> tokens, std::size_t depth = 0,
                            std::size_t *parsed = nullptr);

  Node run(std::vector<Node> program);

  std::optional<Variable> get_symbol(const std::string &name) const;
  void add_symbol(const std::string &name, Variable v);
  void new_scope();
  void drop_scope();
};

Node collapse(Interpreter &ctx, Node action, std::vector<Node> args);
