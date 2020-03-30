#include "testing.hpp"

#include <catch2/catch.hpp>

#include <percy/parser.hpp>

#include <percy/static_input.hpp>

TEST_CASE("Parser `end` succeeds on input end.", "[parser][end]") {
  using parser = percy::parser<percy::end>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc").advanced_to(3));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 3);
  STATIC_REQUIRE(result.end() == 3);
}

TEST_CASE("Parser `end` fails in the middle of input.", "[parser][end]") {
  using parser = percy::parser<percy::end>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc").advanced_to(1));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 1);
  STATIC_REQUIRE(result.end() == 1);
}

TEST_CASE("Parser `symbol` succeeds on matching character.", "[parser][symbol]") {
  using parser = percy::parser<percy::symbol<'a'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == 'a');
}

TEST_CASE("Parser `symbol` fails on different character.", "[parser][symbol]") {
  using parser = percy::parser<percy::symbol<'x'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
}

TEST_CASE("Parser `symbol` fails on input end.", "[parser][symbol]") {
  using parser = percy::parser<percy::symbol<'a'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc").advanced_to(3));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 3);
  STATIC_REQUIRE(result.end() == 3);
}

TEST_CASE("Parser `range` succeeds on the beginning character.", "[parser][range]") {
  using parser = percy::parser<percy::range<'b', 'd'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("bcd"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == 'b');
}

TEST_CASE("Parser `range` succeeds on an included character.", "[parser][range]") {
  using parser = percy::parser<percy::range<'b', 'd'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("cde"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == 'c');
}

TEST_CASE("Parser `range` succeeds on the ending character.", "[parser][range]") {
  using parser = percy::parser<percy::range<'b', 'd'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("def"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == 'd');
}

TEST_CASE("Parser `range` fails on a lesser character.", "[parser][range]") {
  using parser = percy::parser<percy::range<'b', 'd'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
}

TEST_CASE("Parser `range` fails on a greater character.", "[parser][range]") {
  using parser = percy::parser<percy::range<'b', 'd'>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("efg"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
}

struct ab {
  constexpr static std::string_view string = "ab";
};

TEST_CASE("Parser `word` succeeds on matching string.", "[parser][word]") {
  using parser = percy::parser<percy::word<ab>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 2);
  STATIC_REQUIRE(result.get() == std::string_view("ab"));
}

TEST_CASE("Parser `word` fails on different string.", "[parser][word]") {
  using parser = percy::parser<percy::word<ab>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("axc"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 2);
}

TEST_CASE("Parser `word` fails on input end.", "[parser][word]") {
  using parser = percy::parser<percy::word<ab>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("a"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 2);
}

TEST_CASE("Parser `sequence` succeeds when all rules match.", "[parser][sequence]") {
  using parser = percy::parser<percy::sequence<percy::symbol<'a'>, percy::symbol<'b'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 2);
  STATIC_REQUIRE(result.get() == std::tuple<char, char>('a', 'b'));
}

TEST_CASE("Parser `sequence` fails when first rule does not match.", "[parser][sequence]") {
  using parser = percy::parser<percy::sequence<percy::symbol<'x'>, percy::symbol<'b'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
}

TEST_CASE("Parser `sequence` fails when following rule does not match.", "[parser][sequence]") {
  using parser = percy::parser<percy::sequence<percy::symbol<'a'>, percy::symbol<'x'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 2);
}

TEST_CASE("Parser `either` succeeds when first rule matches.", "[parser][either]") {
  using parser = percy::parser<percy::either<percy::symbol<'a'>, percy::symbol<'b'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == 'a');
}

TEST_CASE("Parser `either` succeeds when alternative rule matches.", "[parser][either]") {
  using parser = percy::parser<percy::either<percy::symbol<'x'>, percy::symbol<'a'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == 'a');
}

TEST_CASE("Parser `either` fails when no rule matches.", "[parser][either]") {
  using parser = percy::parser<percy::either<percy::symbol<'x'>, percy::symbol<'x'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
}

struct abc {
  constexpr static std::string_view string = "abc";
};

TEST_CASE("Parser `one_of` succeeds when first rule matches.", "[parser][one_of]") {
  using parser = percy::parser<percy::one_of<percy::word<abc>, percy::symbol<'x'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 3);
  STATIC_REQUIRE(result.get() == percy::variant<std::string_view, char>("abc"));
}

TEST_CASE("Parser `one_of` succeeds when alternative rule matches.", "[parser][one_of]") {
  using parser = percy::parser<percy::one_of<percy::word<abc>, percy::symbol<'x'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("xyz"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == percy::variant<std::string_view, char>('x'));
}

TEST_CASE("Parser `one_of` fails when no rule matches.", "[parser][one_of]") {
  using parser = percy::parser<percy::one_of<percy::symbol<'x'>, percy::word<abc>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("ijk"));

  STATIC_REQUIRE(result.is_failure());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 3);
}

TEST_CASE("Parser `many` succeeds even when rule matches zero times.", "[parser][many]") {
  using parser = percy::parser<percy::many<percy::symbol<'x'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("abc"));

  REQUIRE(result.is_success());
  REQUIRE(result.begin() == 0);
  REQUIRE(result.end() == 0);
  REQUIRE(result.get() == std::vector<char>{});
}

TEST_CASE("Parser `many` succeeds when rule matches multiple times.", "[parser][many]") {
  using parser = percy::parser<percy::many<percy::symbol<'a'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("aac"));

  REQUIRE(result.is_success());
  REQUIRE(result.begin() == 0);
  REQUIRE(result.end() == 2);
  REQUIRE(result.get() == std::vector<char>{'a', 'a'});
}

TEST_CASE("Parser `many` stops at the input end.", "[parser][many]") {
  using parser = percy::parser<percy::many<percy::symbol<'a'>>>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("aaa"));

  REQUIRE(result.is_success());
  REQUIRE(result.begin() == 0);
  REQUIRE(result.end() == 3);
  REQUIRE(result.get() == std::vector<char>{'a', 'a', 'a'});
}

struct left_curly {
  using rule = percy::sequence<percy::symbol<'{'>>;
  static constexpr auto action(percy::result<std::tuple<char>> parsed) {
    return std::get<0>(parsed.get());
  }
};

TEST_CASE("Parser of custom rule succeeds when the inner rule matches.", "[parser][custom]") {
  using parser = percy::parser<left_curly>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("{"));

  STATIC_REQUIRE(result.is_success());
  STATIC_REQUIRE(result.begin() == 0);
  STATIC_REQUIRE(result.end() == 1);
  STATIC_REQUIRE(result.get() == '{');
}

struct right_curly {
  using rule = percy::sequence<percy::symbol<'}'>>;
  static auto action(percy::result<std::tuple<char>> parsed) {
    FAIL("Called action of a custom rule after failure.");
    return std::false_type();
  }
};

TEST_CASE("Parser of custom rule fails when the inner rule fails.", "[parser][custom]") {
  using parser = percy::parser<right_curly>;

  PERCY_CONSTEXPR auto result = parser::parse(percy::static_input("x"));

  REQUIRE(result.is_failure());
  REQUIRE(result.begin() == 0);
  REQUIRE(result.end() == 1);
}
