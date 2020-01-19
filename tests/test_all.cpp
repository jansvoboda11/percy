#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <percy/parser.hpp>
#include <percy/static_input.hpp>

#include <utility>
#include <variant>
#include <vector>

namespace ast {
struct paren;
struct curly;

struct round {
  std::vector<paren> parens;
  explicit round(std::vector<paren> parens) : parens(std::move(parens)) {}
  bool operator==(const round& other) const { return parens == other.parens; }
  bool operator==(const curly& other) const { return false; }
};

struct curly {
  std::vector<paren> parens;
  explicit curly(std::vector<paren> parens) : parens(std::move(parens)) {}
  bool operator==(const curly& other) const { return parens == other.parens; }
  bool operator==(const round& other) const { return false; }
};

struct paren {
  std::variant<round, curly> concrete;
  explicit paren(round r) : concrete(r) {}
  explicit paren(curly c) : concrete(c) {}
  bool operator==(const paren& other) const { return concrete == other.concrete; }
};
}

namespace grammar {
struct paren;

struct round {
  using rule = percy::sequence<percy::symbol<'('>, percy::repeat<paren>, percy::symbol<')'>>;
  static auto action(percy::result<std::tuple<char, std::vector<ast::paren>, char>> parsed) {
    auto parens = std::get<1>(parsed.get());
    return ast::round(parens);
  };
};

struct curly {
  using rule = percy::sequence<percy::symbol<'{'>, percy::repeat<paren>, percy::symbol<'}'>>;
  static auto action(percy::result<std::tuple<char, std::vector<ast::paren>, char>> parsed) {
    auto parens = std::get<1>(parsed.get());
    return ast::curly(parens);
  };
};

struct paren {
  using rule = percy::one_of<round, curly>;
  static auto action(percy::result<std::variant<ast::round, ast::curly>> parsed) {
    auto value = parsed.get();
    return std::holds_alternative<ast::round>(value) ? ast::paren(std::get<ast::round>(value))
                                                     : ast::paren(std::get<ast::curly>(value));
  };
};
}

TEST_CASE("It parses nested parentheses correctly.", "[example]") {
  using parser = percy::parser<grammar::paren>;

  auto input = percy::static_input("{(()){}}");

  auto result = parser::parse(input);

  REQUIRE(result.is_success());
  REQUIRE(result.begin() == 0);
  REQUIRE(result.end() == 8);
  REQUIRE(result.get() == ast::paren(ast::curly({
                              ast::paren(ast::round({
                                  ast::paren(ast::round({}))})),
                              ast::paren(ast::curly({}))})));
}

TEST_CASE("It fails to parse unbalanced parentheses.", "[example]") {
  using parser = percy::parser<grammar::paren>;

  auto input = percy::static_input("({)");

  auto result = parser::parse(input);

  REQUIRE(result.is_failure());
  REQUIRE(result.begin() == 0);
  // todo: decide what this should be
//  REQUIRE(result.end() == 0);
}

TEST_CASE("It fails to parse mismatched parentheses.", "[example]") {
  using parser = percy::parser<grammar::paren>;

  auto input = percy::static_input("({))");

  auto result = parser::parse(input);

  REQUIRE(result.is_failure());
  REQUIRE(result.begin() == 0);
  // todo: decide what this should be
//  REQUIRE(result.end() == 0);
}
