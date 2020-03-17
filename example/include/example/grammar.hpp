#ifndef PERCY_EXAMPLE_GRAMMAR
#define PERCY_EXAMPLE_GRAMMAR

#include "ast.hpp"

#include <percy.hpp>

namespace example::grammar {
struct expr;

struct literal {
  using rule = percy::range<'0', '9'>;

  constexpr static auto action(percy::result<char> parsed) {
    return ast::literal(parsed.get() - '0');
  }
};

struct variable {
  using rule = percy::range<'a', 'z'>;

  constexpr static auto action(percy::result<char> parsed) {
    return ast::variable(parsed.get());
  }
};

struct call {
  using rule =
      percy::sequence<percy::range<'a', 'z'>, percy::symbol<'('>, expr, percy::symbol<','>, expr, percy::symbol<')'>>;

  constexpr static auto action(percy::result<std::tuple<char, char, ast::expr*, char, ast::expr*, char>> parsed) {
    auto [name, l_paren, arg1, comma, arg2, r_paren] = parsed.get();
    return ast::call(name, arg1, arg2);
  }
};

struct expr {
  using rule = percy::one_of<call, literal, variable>;

  constexpr static auto action(percy::result<std::variant<ast::call, ast::literal, ast::variable>> parsed) {
    return std::visit([](auto item) { return new ast::expr(item); }, parsed.get());
  }
};

struct grammar {
  using rule = percy::sequence<expr, percy::end>;

  constexpr static auto action(percy::result<std::tuple<ast::expr*, std::true_type>> parsed) {
    auto [expr, eof] = parsed.get();
    return expr;
  }
};
} // namespace example::grammar

#endif
