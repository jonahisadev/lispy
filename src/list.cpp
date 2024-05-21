#include "list.hpp"

#include <iostream>

void List::print() {
  List *curr = this;
  std::cout << "(";
  while (curr != nullptr) {
    std::cout << " " << curr->value;
    curr = curr->next;
  }
  std::cout << " )" << std::endl;
}
