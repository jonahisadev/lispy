#include "core.hpp"
#include "interpreter.hpp"
#include "list.hpp"
#include "node.hpp"
#include "variable.hpp"

#include <cmath>
#include <stdexcept>

namespace Core {

FN(add) {
  int sum = 0;
  for (auto &arg : args) {
    auto ret = arg.get_if_or(Node::Number, ctx, eval_id);
    sum += ret.as<int>();
  }
  return {Node::Number, sum};
}

FN(multiply) {
  int sum = 1;
  for (auto &arg : args) {
    auto ret = arg.get_if_or(Node::Number, ctx, eval_id);
    sum *= ret.as<int>();
  }
  return {Node::Number, sum};
}

FN(minus) {
  // Negate
  if (args.size() == 1) {
    auto ret = args[0].get_if_or(Node::Number, ctx, eval_id);
    return {Node::Number, -(ret.as<int>())};
  }

  // Minus
  auto sum_node = args[1].get_if_or(Node::Number, ctx, eval_id);
  auto other_node = args[0].get_if_or(Node::Number, ctx, eval_id);
  int total = sum_node.as<int>() - other_node.as<int>();

  return {Node::Number, total};
}

FN(divide) {
  auto dividend = args[0].get_if_or(Node::Number, ctx, eval_id).as<int>();
  auto divisor = args[1].get_if_or(Node::Number, ctx, eval_id).as<int>();

  auto quotient = dividend / divisor;
  return {Node::Number, quotient};
}

FN(equals) {
  auto left = args[1].get_if_or(Node::Number, ctx, eval_id).as<int>();
  auto right = args[0].get_if_or(Node::Number, ctx, eval_id).as<int>();

  return {Node::Bool, left == right};
}

FN(less_than) {
  auto left = args[1].get_if_or(Node::Number, ctx, eval_id).as<int>();
  auto right = args[0].get_if_or(Node::Number, ctx, eval_id).as<int>();

  return {Node::Bool, left < right};
}

FN(greater_than) {
  auto left = args[1].get_if_or(Node::Number, ctx, eval_id).as<int>();
  auto right = args[0].get_if_or(Node::Number, ctx, eval_id).as<int>();

  return {Node::Bool, left > right};
}

FN(sqrt) {
  auto arg = args[0].get_if_or(Node::Number, ctx, eval_id);
  return {Node::Number, (int)std::sqrt(arg.as<int>())};
}

FN(map) {
  auto id = args[0].get_if(Node::Identifier).as<std::string>();
  auto vec = args[1].get_if_or(Node::Vec, ctx, eval_id).as<Vector>();

  auto fn = ctx.get_symbol(id);
  if (!fn->is_function()) {
    throw std::runtime_error("map requires a function");
  }

  Vector nvec;
  for (auto &el : vec.data) {
    auto ret = collapse(ctx, args[0], {el});
    nvec.data.push_back(ret);
  }

  return Node{Node::Vec, nvec};
}

FN(range) {
  auto first = args[0].get_if_or(Node::Number, ctx, eval_id).as<int>();
  auto last = args[1].get_if_or(Node::Number, ctx, eval_id).as<int>();

  Vector vec;
  for (int i = first; i <= last; i++) {
    vec.data.push_back({Node::Number, i});
  }

  return Node{Node::Vec, vec};
}

FN(size) {
  auto vec = args[0].get_if_or(Node::Vec, ctx, eval_id).as<Vector>();
  return Node{Node::Number, (int)vec.data.size()};
}

FN(len) {
  auto *list = args[0].get_if_or(Node::List, ctx, eval_id).as<::List *>();
  int count = 0;
  auto *curr = list;
  while (curr != nullptr) {
    ++count;
    curr = curr->next;
  }
  return Node{Node::Number, count};
}

FN(head) {
  auto *list = args[0].get_if_or(Node::List, ctx, eval_id).as<::List *>();
  return Node{Node::Number, list->value};
}

FN(tail) {
  auto list = args[0].get_if_or(Node::List, ctx, eval_id).as<::List *>();
  return Node{Node::List, list->next};
}

FN(nth) {
  auto index = args[0].get_if_or(Node::Number, ctx, eval_id).as<int>();
  auto list = args[1].get_if_or(Node::List, ctx, eval_id).as<::List *>();
  auto *curr = list;
  for (int i = 0; i < index; i++) {
    curr = curr->next;
  }
  return Node{Node::Number, curr->value};
}

FN(rem) {
  auto dividend = args[0].get_if_or(Node::Number, ctx, eval_id).as<int>();
  auto divisor = args[1].get_if_or(Node::Number, ctx, eval_id).as<int>();

  auto remainder = dividend % divisor;
  return {Node::Number, remainder};
}

FN(filter) {
  auto id = args[0].get_if(Node::Identifier).as<std::string>();
  auto vec = args[1].get_if_or(Node::Vec, ctx, eval_id).as<Vector>();

  auto fn = ctx.get_symbol(id);
  if (!fn->is_function()) {
    throw std::runtime_error("filter requires a function");
  }

  Vector nvec;
  for (auto &el : vec.data) {
    auto ret = collapse(ctx, args[0], {el}).get_if(Node::Bool);
    if (ret.as<bool>()) {
      nvec.data.push_back(el);
    }
  }

  return Node{Node::Vec, nvec};
}

FN(reduce) {
  auto id = args[0].get_if(Node::Identifier).as<std::string>();
  auto vec = args[1].get_if_or(Node::Vec, ctx, eval_id).as<Vector>();

  auto fn = ctx.get_symbol(id);
  if (!fn->is_function()) {
    throw std::runtime_error("reduce requires a function");
  }

  int value = vec.data[0].as<int>();
  for (int i = 1; i < (int)vec.data.size(); i++) {
    auto ret = collapse(ctx, args[0], {Node{Node::Number, value}, vec.data[i]});
    value = ret.as<int>();
  }

  return Node{Node::Number, value};
}

Node eval_id(Interpreter &ctx, Node node, Node::Type expected) {
  if (node.type == Node::Identifier) {
    auto val = ctx.get_symbol(node.as<std::string>());
    if (!val.has_value()) {
      return Node{Node::Undefined};
    }
    switch (val->type) {
    case Variable::Integer:
      if (expected == Node::Number)
        return Node{Node::Number, val->as<int>()};
    case Variable::Vec:
      if (expected == Node::Vec)
        return Node{Node::Vec, val->as<Vector>()};
    case Variable::List:
      if (expected == Node::List)
        return Node{Node::List, val->as<::List *>()};
    case Variable::Bool:
      if (expected == Node::Bool)
        return Node{Node::Bool, val->as<bool>()};
    default:
      break;
    }
  }
  return Node{Node::Undefined};
}

} // namespace Core
