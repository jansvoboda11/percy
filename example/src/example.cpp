#include <example/grammar.hpp>

#include <percy.hpp>

int main() {
  using parser = percy::parser<example::grammar::grammar>;
  auto input = percy::static_input(INPUT_CODE);
  auto result = parser::parse(input);
  return result.is_success() ? 0 : 1;
}
