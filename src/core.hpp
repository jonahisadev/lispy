#pragma once

#include "node.hpp"
#include <vector>

#define FN(x) Node x(Interpreter &ctx, std::vector<Node> args)

class Interpreter;

namespace Core {

// Operators
FN(add);
FN(multiply);
FN(minus);
FN(divide);
FN(equals);
FN(less_than);
FN(greater_than);

// Core lib
FN(sqrt);
FN(map);
FN(range);
FN(size);
FN(len);
FN(head);
FN(tail);
FN(nth);
FN(rem);
FN(filter);
FN(reduce);

// Helper funcs
Node eval_id(Interpreter &ctx, Node node, Node::Type expected);

} // namespace Core
