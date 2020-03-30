#ifndef PERCY_EXAMPLE_AST
#define PERCY_EXAMPLE_AST

#include <percy/variant.hpp>

namespace example::ast {
struct expr;

struct literal {
  constexpr explicit literal(char value) : value_(value) {}
  char value_;
};

struct variable {
  constexpr explicit variable(char name) : name_(name) {}
  char name_;
};

struct call {
  constexpr explicit call(char name, const expr* arg1, const expr* arg2) : name_(name), arg1_(arg1), arg2_(arg2) {}
  char name_;
  const expr* arg1_;
  const expr* arg2_;
};

struct expr {
  constexpr explicit expr(literal l) : value_(l) {}
  constexpr explicit expr(variable v) : value_(v) {}
  constexpr explicit expr(call c) : value_(c) {}
  percy::variant<literal, variable, call> value_;
};
} // namespace example::ast

#endif
