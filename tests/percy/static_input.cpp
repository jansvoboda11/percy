#include "testing.hpp"

#include <catch2/catch.hpp>

#include <percy/static_input.hpp>

#include <percy/result.hpp>

TEST_CASE("Fresh empty input.", "[inputs][static_input]") {
  PERCY_CONSTEXPR auto input = percy::static_input("");

  STATIC_REQUIRE(input.ended());
  STATIC_REQUIRE(input.position() == 0);
}

TEST_CASE("Fresh non-empty input.", "[inputs][static_input]") {
  PERCY_CONSTEXPR auto input = percy::static_input("ab");

  STATIC_REQUIRE(!input.ended());
  STATIC_REQUIRE(input.position() == 0);
  STATIC_REQUIRE(input.peek() == 'a');
}

TEST_CASE("Non-empty input advanced by offset.", "[inputs][static_input]") {
  PERCY_CONSTEXPR auto input = percy::static_input("ab").advanced_by(1);

  STATIC_REQUIRE(!input.ended());
  STATIC_REQUIRE(input.position() == 1);
  STATIC_REQUIRE(input.peek() == 'b');
}

TEST_CASE("Non-empty input advanced to position.", "[inputs][static_input]") {
  PERCY_CONSTEXPR auto input = percy::static_input("abc").advanced_to(2);

  STATIC_REQUIRE(!input.ended());
  STATIC_REQUIRE(input.position() == 2);
  STATIC_REQUIRE(input.peek() == 'c');
}

TEST_CASE("Non-empty input advanced to end.", "[inputs][static_input]") {
  PERCY_CONSTEXPR auto input = percy::static_input("ab").advanced_by(2);

  STATIC_REQUIRE(input.ended());
  STATIC_REQUIRE(input.position() == 2);
}

TEST_CASE("Non-empty input advanced after result.", "[inputs][static_input]") {
  PERCY_CONSTEXPR auto result = percy::success('b', {1, 2});
  PERCY_CONSTEXPR auto input = percy::static_input("abcd").advanced_after(result);

  STATIC_REQUIRE(!input.ended());
  STATIC_REQUIRE(input.position() == 2);
  STATIC_REQUIRE(input.peek() == 'c');
}
