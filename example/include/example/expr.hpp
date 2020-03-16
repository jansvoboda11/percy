#ifndef PERCY_EXAMPLE_EXPR
#define PERCY_EXAMPLE_EXPR

#include <variant>

#include <cstdint>

#include <percy/parser.hpp>

namespace example {
namespace ast {
struct expr;

struct int_lit {
  constexpr explicit int_lit(std::uint8_t val) : val(val) {}
  std::uint8_t val;
};

struct var_ref {
  constexpr explicit var_ref(char name) : name(name) {}
  char name;
};

enum op_code {
  add,
  sub,
  mul,
  div,
};

struct bin_op {
  constexpr explicit bin_op(const expr& lhs, op_code op, const expr& rhs) : lhs(lhs), op(op), rhs(rhs) {}
  const expr& lhs;
  op_code op;
  const expr& rhs;
};

struct expr {
  constexpr explicit expr(std::variant<int_lit, var_ref, bin_op> value) : value(value) {}
  std::variant<int_lit, var_ref, bin_op> value;
};
} // namespace ast

namespace grammar {
struct expr;

struct int_lit {
  using rule = percy::range<'0', '9'>;

  constexpr static auto action(percy::result<char> parsed) {
    return ast::int_lit(std::uint8_t(parsed.get()) - std::uint8_t('0'));
  }
};

struct var_ref {
  using rule = percy::range<'a', 'z'>;

  constexpr static auto action(percy::result<char> parsed) {
    return ast::var_ref(parsed.get());
  }
};

struct op_code {
  using rule = percy::either<percy::symbol<'+'>, percy::symbol<'-'>, percy::symbol<'*'>, percy::symbol<'/'>>;

  constexpr static auto action(percy::result<char> parsed) {
    return ast::op_code(parsed.get());
  }
};

struct bin_op {
  using rule = percy::sequence<expr, op_code, expr>;

  constexpr static auto action(percy::result<std::tuple<ast::expr, ast::op_code, ast::expr>> parsed) {
    auto result = parsed.get();
    return ast::bin_op(std::get<0>(result), std::get<1>(result), std::get<2>(result));
  }
};

struct expr {
  using rule = percy::one_of<int_lit, var_ref, bin_op>;

  constexpr static auto action(percy::result<std::variant<ast::int_lit, ast::var_ref, ast::bin_op>> parsed) {
    return ast::expr(parsed.get());
  }
};
} // namespace grammar
} // namespace example

#endif
