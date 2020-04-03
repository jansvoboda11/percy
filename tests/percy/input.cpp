#include "testing.hpp"

#include <catch2/catch.hpp>

#include <percy/input.hpp>

#include <percy/result.hpp>

TEST_CASE("Fresh empty input.", "[inputs][input]") {
  PERCY_CONSTEXPR auto input = percy::input("");

  STATIC_REQUIRE(input.ended());
  STATIC_REQUIRE(input.loc() == 0);
}

TEST_CASE("Fresh non-empty input.", "[inputs][input]") {
  PERCY_CONSTEXPR auto input = percy::input("ab");

  STATIC_REQUIRE(!input.ended());
  STATIC_REQUIRE(input.loc() == 0);
  STATIC_REQUIRE(input.peek() == 'a');
}

TEST_CASE("Non-empty input advanced by offset.", "[inputs][input]") {
  PERCY_CONSTEXPR auto input = percy::input("ab").advanced_by(1);

  STATIC_REQUIRE(!input.ended());
  STATIC_REQUIRE(input.loc() == 1);
  STATIC_REQUIRE(input.peek() == 'b');
}

TEST_CASE("Non-empty input advanced to position.", "[inputs][input]") {
  PERCY_CONSTEXPR auto input = percy::input("abc").advanced_to(percy::input_location(2));

  STATIC_REQUIRE(!input.ended());
  STATIC_REQUIRE(input.loc() == 2);
  STATIC_REQUIRE(input.peek() == 'c');
}

TEST_CASE("Non-empty input advanced to end.", "[inputs][input]") {
  PERCY_CONSTEXPR auto input = percy::input("ab").advanced_by(2);

  STATIC_REQUIRE(input.ended());
  STATIC_REQUIRE(input.loc() == 2);
}
