#ifndef PERCY_EXAMPLE_GRAMMAR
#define PERCY_EXAMPLE_GRAMMAR

#include "example/ast.hpp"

#include <percy.hpp>

namespace example::grammar {
using percy::end;
using percy::eof;
using percy::one_of;
using percy::range;
using percy::result;
using percy::sequence;
using percy::symbol;

struct expr;

struct literal {
  using rule = range<'0', '9'>;

  constexpr static auto action(result<char> parsed) {
    return ast::literal(parsed.get() - '0');
  }
};

struct variable {
  using rule = range<'a', 'z'>;

  constexpr static auto action(result<char> parsed) {
    return ast::variable(parsed.get());
  }
};

struct call {
  using rule = sequence<range<'a', 'z'>, symbol<'('>, expr, symbol<','>, expr, symbol<')'>>;

  constexpr static auto action(char name, char l_round, ast::expr* arg1, char comma, ast::expr* arg2, char r_round) {
    return ast::call(name, arg1, arg2);
  }
};

struct expr {
  using rule = one_of<call, literal, variable>;
  using result = ast::expr*;

  constexpr static auto action(ast::call call) {
    return new ast::expr(call);
  }

  constexpr static auto action(ast::literal literal) {
    return new ast::expr(literal);
  }

  constexpr static auto action(ast::variable variable) {
    return new ast::expr(variable);
  }
};

struct top {
  using rule = sequence<expr, end>;

  constexpr static auto action(ast::expr* expr, eof eof) {
    return expr;
  }
};
} // namespace example::grammar

#endif
