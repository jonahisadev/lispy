#include "interpreter.hpp"
#include "core.hpp"
#include "list.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "variable.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

void print_nodes(std::vector<Node> nodes) {
  for (auto &n : nodes) {
    n.print_debug(0);
  }
}

std::vector<Node> Interpreter::compile(std::vector<Token> tokens,
                                       std::size_t depth, std::size_t *parsed) {
  std::vector<Node> nodes;
  std::size_t next_expr_is_body = 0;
  std::size_t paren_count = 1;

  for (std::size_t i = 0; i < tokens.size(); i++) {
    auto &t = tokens[i];

    switch (t.type) {
    case Token::Paren: {
      char c = t.as<char>();

      if (next_expr_is_body == 0 && depth > 0 && c == '(') {
        paren_count++;
        nodes.push_back({Node::Paren, c});
        break;
      }

      if (next_expr_is_body > 0 && c == '(') {
        std::size_t len;
        auto body =
            compile({tokens.begin() + i, tokens.end()}, depth + 1, &len);
        nodes.push_back({Node::Body, body});
        i += len;
        --next_expr_is_body;
        break;
      }

      nodes.push_back({Node::Paren, c});

      if (depth > 0 && c == ')') {
        paren_count--;
        if (paren_count == 1) {
          *parsed = i;
          return nodes;
        }
      }

      break;
    }
    case Token::Bracket: {
      if (t.as<char>() == ']') {
        std::cerr << "No matching bracket" << std::endl;
        return {};
      }
      Vector vec;
      t = tokens[++i];
      while (t.type != Token::Bracket) {
        vec.add_element(t);
        t = tokens[++i];
      }
      nodes.push_back({Node::Vec, vec});
      break;
    }
    case Token::Quote:
      if (t.as<char>() == '\'') {
        auto next = tokens[i + 1];
        if (next.type == Token::Paren && next.as<char>() == '(') {
          ++i;
          next = tokens[++i];

          List *list = new List();
          List *curr = list;

          while (true) {
            next = next.get_if(Token::Number);
            curr->value = next.as<int>();
            curr->next = new List();
            next = tokens[++i];
            if (next.type != Token::Paren) {
              curr = curr->next;
            } else {
              break;
            }
          }

          delete curr->next;
          curr->next = nullptr;
          nodes.push_back({Node::List, list});
        }
      }
      break;
    case Token::Operator:
      nodes.push_back({Node::Operator, t.as<char>()});
      break;
    case Token::Number:
      nodes.push_back({Node::Number, t.as<int>()});
      break;
    case Token::Bool:
      nodes.push_back({Node::Bool, t.as<bool>()});
      break;
    case Token::Keyword: {
      auto type = t.as<Keyword>();
      nodes.push_back({Node::Keyword, type});
      switch (type) {
      case Keyword::Defn:
        next_expr_is_body = 1;
        break;
      case Keyword::If:
        next_expr_is_body = 3;
      default:
        break;
      }
      break;
    }
    case Token::Identifier:
      nodes.push_back({Node::Identifier, t.as<std::string>()});
      break;
    }
  }

  if (depth == 0) {
    // print_nodes(nodes);
  }

  return nodes;
}

Node collapse(Interpreter &ctx, Node action, std::vector<Node> args) {
  switch (action.type) {
  case Node::Operator:
    switch (action.as<char>()) {
    case '+':
      return Core::add(ctx, args);
      break;
    case '*':
      return Core::multiply(ctx, args);
    case '-':
      return Core::minus(ctx, args);
    case '/':
      return Core::divide(ctx, args);
    case '=':
      return Core::equals(ctx, args);
    case '<':
      return Core::less_than(ctx, args);
    case '>':
      return Core::greater_than(ctx, args);
    default:
      break;
    }
    break;
  case Node::Keyword:
    switch (action.as<Keyword>()) {
    case Keyword::Def: {
      Variable v;
      auto name = args[1].get_if(Node::Identifier).as<std::string>();
      switch (args[0].type) {
      case Node::Number:
        v = Variable{Variable::Integer, name, args[0].as<int>()};
        break;
      case Node::Vec:
        v = Variable{Variable::Vec, name, args[0].as<Vector>()};
        break;
      case Node::List:
        v = Variable{Variable::List, name, args[0].as<List *>()};
        break;
      case Node::Bool:
        v = Variable{Variable::Bool, name, args[0].as<bool>()};
        break;
      default:
        break;
      }
      ctx.add_symbol(name, v);
      return {Node::Symbol, v};
    }
    case Keyword::Defn: {
      auto body = args[0].get_if(Node::Body).as<std::vector<Node>>();
      auto params = args[1].get_if(Node::Vec).as<Vector>();
      auto name = args[2].get_if(Node::Identifier).as<std::string>();
      auto func = Function{params, body};
      auto v = Variable{Variable::Function, name, func};
      ctx.add_symbol(name, v);
      return {Node::Symbol, v};
    }
    case Keyword::If: {
      auto eval = args[2].get_if(Node::Body).as<std::vector<Node>>();
      auto truthy = args[1].get_if(Node::Body).as<std::vector<Node>>();
      auto falsey = args[0].get_if(Node::Body).as<std::vector<Node>>();

      ctx.new_scope();
      Node ret = ctx.run(eval).get_if(Node::Bool);
      ctx.drop_scope();

      bool result = ret.as<bool>();
      Node final_result;
      ctx.new_scope();
      if (result) {
        final_result = ctx.run(truthy);
      } else {
        final_result = ctx.run(falsey);
      }
      ctx.drop_scope();

      return final_result;
    }
    default:
      break;
    }
    break;
  case Node::Identifier: {
    auto sym = ctx.get_symbol(action.as<std::string>());
    if (!sym.has_value()) {
      throw std::runtime_error("No such symbol exists");
    }

    if (!sym->is_function()) {
      return {Node::Number, sym->as<int>()};
    }

    // LISP function
    if (sym->type == Variable::Function) {
      auto func = sym->as<Function>();

      ctx.new_scope();
      std::reverse(args.begin(), args.end());
      for (std::size_t i = 0; i < func.params.data.size(); i++) {
        auto p_name = func.params.data[i].as<std::string>();
        auto value =
            args[i].get_if_or(Node::Number, ctx, Core::eval_id).as<int>();
        ctx.add_symbol(p_name, {Variable::Integer, p_name, value});
      }
      Node ret = ctx.run(func.body);
      ctx.drop_scope();

      return ret;
    }

    // Native function
    else if (sym->type == Variable::NativeFn) {
      auto func = sym->as<NativeFunction>();
      ctx.new_scope();
      std::reverse(args.begin(), args.end());
      auto ret = func(ctx, args);
      ctx.drop_scope();
      return ret;
    }
  }
  case Node::Bool:
  case Node::Number:
    return action;
  default:
    break;
  }

  return {};
}

void eval(Interpreter &ctx, Stack<Node> &stack) {
  std::vector<Node> args;

  while (!stack.is_empty()) {
    auto node = stack.pop();
    if (stack.peek().type == Node::Paren && stack.peek().as<char>() == '(') {
      stack.pop();
      stack.push(collapse(ctx, node, args));
      return;
    }
    args.push_back(node);
  }
}

Node Interpreter::run(std::vector<Node> program) {
  Stack<Node> stack;

  for (auto &p : program) {
    switch (p.type) {
    case Node::Paren: {
      char c = p.as<char>();
      if (c == ')') {
        eval(*this, stack);
      } else {
        stack.push(p);
      }
      break;
    }
    default:
      stack.push(p);
      break;
    }
  }

  return stack.pop();
}

std::optional<Variable> Interpreter::get_symbol(const std::string &name) const {
  auto scope = m_symbols;
  while (!scope.is_empty()) {
    auto vars = scope.pop();
    auto it = vars.find(name);
    if (it != vars.end()) {
      return it->second;
    }
  }
  return std::optional<Variable>();
}

void Interpreter::add_symbol(const std::string &name, Variable v) {
  m_symbols.peek().insert(std::make_pair(name, v));
}

void Interpreter::new_scope() { m_symbols.push({}); }

void Interpreter::drop_scope() { m_symbols.pop(); }
