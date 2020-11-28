#ifndef PERCY_EXAMPLE_AST
#define PERCY_EXAMPLE_AST

#include <percy/variant.hpp>

namespace example::ast {
struct expr;

struct literal {
  char value;

  constexpr explicit literal(char v) : value(v) {}
};

struct variable {
  char name;

  constexpr explicit variable(char n) : name(n) {}
};

struct call {
  char name;
  const expr *arg1;
  const expr *arg2;

  constexpr explicit call(char n, const expr *a1, const expr *a2) : name(n), arg1(a1), arg2(a2) {}

  constexpr ~call();
};

struct expr {
  percy::variant<literal, variable, call> value;

  constexpr explicit expr(literal l) : value(std::move(l)) {}
  constexpr explicit expr(variable v) : value(std::move(v)) {}
  constexpr explicit expr(call c) : value(std::move(c)) {}
};

struct ast {
  const expr *expression;

  constexpr explicit ast(const expr *e) : expression(e) {}

  constexpr ~ast() { int x = 0; }
};

constexpr call::~call() { delete arg1; delete arg2; }
} // namespace example::ast

#endif
