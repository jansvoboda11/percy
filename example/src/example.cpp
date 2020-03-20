#include <example/grammar.hpp>

#include <percy.hpp>

int main() {
  using parser = percy::parser<example::grammar::grammar>;

  constexpr auto exit_code = [](){
    auto input = percy::static_input(INPUT_CODE);
    auto result = parser::parse(input);
    delete result.get();
    return result.is_success() ? 0 : 1;
  }();

  return exit_code;
}
