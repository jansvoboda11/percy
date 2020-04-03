#ifndef PERCY_EXAMPLE_AST
#define PERCY_EXAMPLE_AST

#include <percy/variant.hpp>

namespace example::ast {
struct expr;

struct literal {
  char value;
};

struct variable {
  char name;
};

struct call {
  char name;
  const expr* arg1;
  const expr* arg2;
};

struct expr {
  percy::variant<literal, variable, call> value;
};
} // namespace example::ast

#endif
