#include <example/expr.hpp>

#include <percy/parser.hpp>
#include <percy/static_input.hpp>

int main() {
  using parser = percy::parser<example::grammar::expr>;
  constexpr auto input = percy::static_input(INPUT_CODE);
  constexpr auto result = parser::parse(input);

  return 42;
}
