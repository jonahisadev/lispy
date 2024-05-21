#pragma once

struct List {
  // TODO: we may want this to have ids as well
  int value;
  List *next = nullptr;

  void print();
};
