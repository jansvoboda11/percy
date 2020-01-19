#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <percy/parser.hpp>
#include <percy/static_input.hpp>

#include <variant>
#include <vector>

namespace ast {
struct paren;

struct round {
  std::vector<paren> parens;
  explicit round(std::vector<paren> parens) : parens(parens) {}
};

struct curly {
  std::vector<paren> parens;
  explicit curly(std::vector<paren> parens) : parens(parens) {}
};

struct paren {
  std::variant<round, curly> concrete;
  explicit paren(round r) : concrete(r) {}
  explicit paren(curly c) : concrete(c) {}
};
}

namespace grammar {
struct paren;

struct round {
  using rule = percy::sequence<percy::symbol<'('>, percy::repeat<paren>, percy::symbol<')'>>;
  static /*constexpr*/ auto action(percy::result<std::tuple<char, std::vector<ast::paren>, char>> parsed) {
    auto parens = std::get<1>(parsed.get());
    return ast::round(parens);
  };
};

struct curly {
  using rule = percy::sequence<percy::symbol<'{'>, percy::repeat<paren>, percy::symbol<'}'>>;
  static /*constexpr*/ auto action(percy::result<std::tuple<char, std::vector<ast::paren>, char>> parsed) {
    auto parens = std::get<1>(parsed.get());
    return ast::curly(parens);
  };
};

struct paren {
  using rule = percy::one_of<round, curly>;
  static /*constexpr*/ auto action(percy::result<std::variant<ast::round, ast::curly>> parsed) {
    auto value = parsed.get();
    return std::holds_alternative<ast::round>(value) ? ast::paren(std::get<ast::round>(value))
                                                     : ast::paren(std::get<ast::curly>(value));
  };
};
}

//namespace percy {
//template <>
//struct parser<grammar::paren> {
//  template <typename Input>
//  static constexpr auto parse(Input input) {
//    return double(0.0);
//  }
//};
//}

TEST_CASE("It parses nested parentheses correctly.", "[example]") {
  auto input = percy::static_input("(({}))");

  using x = percy::parser<grammar::paren>;

//  using y = decltype(percy::parser<grammar::paren>::parse<percy::static_input>);

  using z = decltype(percy::parser<percy::repeat<grammar::paren>>::parse<percy::static_input>);

  static_assert(std::is_same_v<percy::action_return_t<grammar::paren>, ast::paren>);
//  static_assert(std::is_same_v<, ast::paren>);

//  percy::parser<grammar::paren>::parse(percy::static_input("."));

//  std::invoke_result_t<decltype(percy::parser<grammar::paren>::template parse<percy::static_input>), percy::static_input>;

//  percy::parser_result_t<percy::static_input, grammar::paren>;

//  percy::parser<grammar::paren>::parse(input);
}
