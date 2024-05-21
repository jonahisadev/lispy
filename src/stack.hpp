#pragma once

#include <stdexcept>
#include <vector>

template <typename T> class Stack {
private:
  std::vector<T> m_data;

public:
  Stack();
  ~Stack();

  void push(const T &data);
  T pop();
  const T &peek() const;
  T &peek();
  std::size_t size();

  bool is_empty() const { return size() == 0; }
  bool is_empty() { return size() == 0; }
};

template <typename T> Stack<T>::Stack() {}

template <typename T> Stack<T>::~Stack() {}

template <typename T> void Stack<T>::push(const T &data) {
  m_data.push_back(data);
}

template <typename T> T Stack<T>::pop() {
  if (this->size() == 0) {
    throw std::runtime_error("Popping empty stack");
  }
  T val = m_data[m_data.size() - 1];
  m_data.pop_back();
  return val;
}

template <typename T> const T &Stack<T>::peek() const { return peek(); }

template <typename T> T &Stack<T>::peek() {
  if (this->size() == 0) {
    throw std::runtime_error("Peeking empty stack");
  }
  return m_data[m_data.size() - 1];
}

template <typename T> std::size_t Stack<T>::size() { return m_data.size(); }
