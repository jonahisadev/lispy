#include "node.hpp"
#include "interpreter.hpp"
#include "list.hpp"
#include "parser.hpp"
#include "variable.hpp"

#include <iostream>

void Node::print_debug(std::size_t depth) {
  for (std::size_t i = 0; i < depth; i++)
    std::cout << " ";

  switch (this->type) {
  case Node::Paren:
    std::cout << "PAREN\t\t" << this->as<char>() << std::endl;
    break;
  case Node::Operator:
    std::cout << "OPER\t\t" << this->as<char>() << std::endl;
    break;
  case Node::Number:
    std::cout << "NUMBER\t\t" << this->as<int>() << std::endl;
    break;
  case Node::Bool:
    std::cout << "BOOL\t\t" << this->as<bool>() << std::endl;
    break;
  case Node::Keyword:
    std::cout << "KEYWORD\t\t" << keyword_str(this->as<::Keyword>())
              << std::endl;
    break;
  case Node::Identifier:
    std::cout << "IDENT\t\t" << this->as<std::string>() << std::endl;
    break;
  case Node::Vec:
    std::cout << "VECTOR\t\t";
    this->as<Vector>().print();
    break;
  case Node::List:
    std::cout << "LIST\t\t";
    this->as<::List *>()->print();
    break;
  case Node::Body: {
    std::cout << "BODY" << std::endl;
    auto nodes = this->as<std::vector<Node>>();
    for (auto &n : nodes) {
      n.print_debug(depth + 2);
    }
    break;
  }
  default:
    break;
  }
}

void Node::print(const Interpreter &ctx) {
  switch (this->type) {
  case Node::Undefined:
    std::cout << "undefined" << std::endl;
    break;
  case Node::Paren:
  case Node::Operator:
    break;
  case Node::Number:
    std::cout << this->as<int>() << std::endl;
    break;
  case Node::Bool:
    std::cout << (this->as<bool>() ? "true" : "false") << std::endl;
    break;
  case Node::Keyword:
    break;
  case Node::Identifier: {
    auto name = this->as<std::string>();
    auto val = ctx.get_symbol(name);
    if (!val.has_value()) {
      std::cout << "'" << name << "' is undefined" << std::endl;
      return;
    }
    switch (val->type) {
    case Variable::Integer:
      std::cout << val->as<int>() << std::endl;
      break;
    case Variable::Vec:
      val->as<Vector>().print();
      break;
    case Variable::List:
      val->as<::List *>()->print();
      break;
    default:
      break;
    }
    break;
  }
  case Node::Vec:
    this->as<Vector>().print();
    break;
  case Node::Body: {
    break;
  }
  case Node::List: {
    this->as<::List *>()->print();
    break;
  }
  case Node::Symbol: {
    auto v = this->as<Variable>();
    switch (v.type) {
    case Variable::Function: {
      auto func = v.as<Function>();
      std::cout << "#" << v.name << "/" << func.params.data.size() << std::endl;
      break;
    }
    default:
      std::cout << "#" << v.name << std::endl;
      break;
    }
    break;
  }
  }
}

void Vector::add_element(Token tok) {
  switch (tok.type) {
  case Token::Number:
    this->data.push_back({Node::Number, tok.as<int>()});
    break;
  case Token::Identifier:
    this->data.push_back({Node::Identifier, tok.as<std::string>()});
    break;
  default:
    std::cerr << "Invalid token for vector" << std::endl;
    break;
  }
}

void Vector::print() {
  std::cout << "[";
  for (auto &node : this->data) {
    std::cout << " ";
    switch (node.type) {
    case Node::Number:
      std::cout << node.as<int>();
      break;
    case Node::Identifier:
      std::cout << node.as<std::string>();
      break;
    default:
      break;
    }
  }
  std::cout << " ]" << std::endl;
}
