#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <percy/parser.hpp>
#include <percy/static_input.hpp>

// todo: enable again when one_of is implemented

//// clang-format off
//namespace ast {
//struct paren;
//
//struct round {
//  std::vector<paren> parens;
//  explicit round(std::vector<paren> parens) : parens(std::move(parens)) {}
//  bool operator==(const round& other) const { return parens == other.parens; }
//};
//
//struct curly {
//  std::vector<paren> parens;
//  explicit curly(std::vector<paren> parens) : parens(std::move(parens)) {}
//  bool operator==(const curly& other) const { return parens == other.parens; }
//};
//
//struct paren {
//  std::variant<round, curly> concrete;
//  explicit paren(round r) : concrete(r) {}
//  explicit paren(curly c) : concrete(c) {}
//  bool operator==(const paren& other) const { return concrete == other.concrete; }
//};
//} // namespace ast
//// clang-format on
//
//namespace grammar {
//struct paren;
//
//struct round {
//  using rule = percy::sequence<percy::symbol<'('>, percy::many<paren>, percy::symbol<')'>>;
//  static auto action(percy::result<std::tuple<char, std::vector<ast::paren>, char>> parsed) {
//    auto parens = std::get<1>(parsed.get());
//    return ast::round(parens);
//  }
//};
//
//struct curly {
//  using rule = percy::sequence<percy::symbol<'{'>, percy::many<paren>, percy::symbol<'}'>>;
//  static auto action(percy::result<std::tuple<char, std::vector<ast::paren>, char>> parsed) {
//    auto parens = std::get<1>(parsed.get());
//    return ast::curly(parens);
//  }
//};
//
//struct paren {
//  using rule = percy::either<round, curly>;
//  static auto action(percy::result<std::variant<ast::round, ast::curly>> parsed) {
//    return std::visit([](auto paren) { return ast::paren(paren); }, parsed.get());
//  }
//};
//} // namespace grammar
//
//// clang-format off
//TEST_CASE("It parses nested parentheses correctly.", "[example]") {
//  using parser = percy::parser<grammar::paren>;
//  auto input = percy::static_input("{(()){}}");
//  auto result = parser::parse(input);
//
//  REQUIRE(result.is_success());
//  REQUIRE(result.begin() == 0);
//  REQUIRE(result.end() == 8);
//  REQUIRE(result.get() == ast::paren(ast::curly({
//                              ast::paren(ast::round({
//                                  ast::paren(ast::round({}))})),
//                              ast::paren(ast::curly({}))})));
//}
//// clang-format on
//
//TEST_CASE("It fails to parse unbalanced parentheses.", "[example]") {
//  using parser = percy::parser<grammar::paren>;
//  auto input = percy::static_input("({)");
//  auto result = parser::parse(input);
//
//  REQUIRE(result.is_failure());
//  REQUIRE(result.begin() == 0);
//  REQUIRE(result.end() == 2);
//}
//
//TEST_CASE("It fails to parse mismatched parentheses.", "[example]") {
//  using parser = percy::parser<grammar::paren>;
//  auto input = percy::static_input("({))");
//  auto result = parser::parse(input);
//
//  REQUIRE(result.is_failure());
//  REQUIRE(result.begin() == 0);
//  REQUIRE(result.end() == 2);
//}
