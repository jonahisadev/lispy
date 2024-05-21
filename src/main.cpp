#include "interpreter.hpp"
#include "parser.hpp"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "lispy v0.0.1" << std::endl;

  Interpreter interpreter;
  std::string line;
  while (true) {
    std::cout << "=> ";
    std::getline(std::cin, line);

    if (std::cin.eof()) {
      break;
    }

    auto tokens = parse(line);
    auto program = interpreter.compile(tokens);
    auto ret = interpreter.run(program);

    ret.print(interpreter);
  }

  return 0;
}
