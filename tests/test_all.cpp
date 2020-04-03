#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <percy.hpp>

namespace ast {
struct paren;

struct round {
  std::vector<paren> parens;
  explicit round(std::vector<paren> parens) : parens(std::move(parens)) {}
  bool operator==(const round &other) const { return parens == other.parens; }
};

struct curly {
  std::vector<paren> parens;
  explicit curly(std::vector<paren> parens) : parens(std::move(parens)) {}
  bool operator==(const curly &other) const { return parens == other.parens; }
};

struct paren {
  percy::variant<round, curly> concrete;
  explicit paren(round r) : concrete(r) {}
  explicit paren(curly c) : concrete(c) {}
  bool operator==(const paren &other) const { return concrete == other.concrete; }
};
} // namespace ast

namespace grammar {
struct paren;

struct round {
  using rule = percy::sequence<percy::symbol<'('>, percy::many<paren>, percy::symbol<')'>>;
  static auto action(char l_round, std::vector<ast::paren> parens, char r_round) {
    return ast::round(parens);
  }
};

struct curly {
  using rule = percy::sequence<percy::symbol<'{'>, percy::many<paren>, percy::symbol<'}'>>;
  static auto action(char l_curly, std::vector<ast::paren> parens, char r_curly) {
    return ast::curly(parens);
  }
};

struct paren {
  using rule = percy::one_of<round, curly>;
  using result = ast::paren;

  static result action(ast::round round) { return ast::paren(round); }
  static result action(ast::curly curly) { return ast::paren(curly); }
};
} // namespace grammar

TEST_CASE("It parses nested parentheses correctly.", "[example]") {
  using parser = percy::parser<grammar::paren>;
  auto input = percy::input("{(()){}}");
  auto result = parser::parse(input);

  REQUIRE(result.is_success());
  REQUIRE(result->begin() == 0);
  REQUIRE(result->end() == 8);
  // clang-format off
  REQUIRE(result->get() == ast::paren(ast::curly({
                               ast::paren(ast::round({
                                   ast::paren(ast::round({}))})),
                               ast::paren(ast::curly({}))})));
  // clang-format on
}

TEST_CASE("It fails to parse unbalanced parentheses.", "[example]") {
  using parser = percy::parser<grammar::paren>;
  auto input = percy::input("({}");
  auto result = parser::parse(input);

  REQUIRE(result.is_failure());
  REQUIRE(result.failure().loc() == 0);
}

TEST_CASE("It fails to parse mismatched parentheses.", "[example]") {
  using parser = percy::parser<grammar::paren>;
  auto input = percy::input("({))");
  auto result = parser::parse(input);

  REQUIRE(result.is_failure());
  REQUIRE(result.failure().loc() == 0);
}
