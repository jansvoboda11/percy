#ifndef PERCY_EXAMPLE_AST
#define PERCY_EXAMPLE_AST

#include <variant>

namespace example::ast {
struct expr;

struct literal {
  constexpr explicit literal(std::uint8_t val) : val(val) {}
  std::uint8_t val;
};

struct variable {
  constexpr explicit variable(char name) : name(name) {}
  char name;
};

struct call {
  constexpr explicit call(char name, const expr* arg1, const expr* arg2) : name_(name), arg1_(arg1), arg2_(arg2) {}
  char name_;
  const expr* arg1_;
  const expr* arg2_;
};

struct expr {
  constexpr explicit expr(literal value) : value(value) {}
  constexpr explicit expr(variable value) : value(value) {}
  constexpr explicit expr(call value) : value(value) {}
  std::variant<literal, variable, call> value;
};
} // namespace example::ast

#endif
