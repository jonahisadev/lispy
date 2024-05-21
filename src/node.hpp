#pragma once

#include <any>
#include <functional>
#include <stdexcept>
#include <vector>

class Interpreter;
struct Token;

struct Node {
  enum Type {
    Undefined,
    Paren,
    Operator,
    Number,
    Keyword,
    Identifier,
    Bool,
    Vec,
    List,
    Body,
    Symbol
  } type;
  std::any value;

  ~Node() {
    if (type == List) {
      // TODO: delete list
    }
  }

  template <typename T> T as() { return std::any_cast<T>(value); }

  template <typename T> T as() const { return std::any_cast<T>(value); }

  Node get_if(Type _type) {
    if (_type != type) {
      throw std::runtime_error("Node::get_if() failed");
    }
    return *this;
  }

  Node get_if_or(Type _type, Interpreter &ctx,
                 std::function<Node(Interpreter &, Node, Node::Type)> fn) {
    if (_type != type) {
      auto node = fn(ctx, *this, _type);
      if (node.type == Node::Undefined) {
        throw std::runtime_error("Node::get_if_or() failed");
      }
      return node;
    }
    return *this;
  }

  void print_debug(std::size_t depth = 0);
  void print(const Interpreter &ctx);
};

struct Vector {
  std::vector<Node> data;

  void add_element(Token tok);
  void print();
};
