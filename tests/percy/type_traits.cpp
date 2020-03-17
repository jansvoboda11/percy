#include <catch2/catch.hpp>

#include <percy/type_traits.hpp>

TEST_CASE("Single type is distinct.", "[type_traits][are_distinct]") {
  STATIC_REQUIRE(percy::are_distinct_v<bool> == true);
}

TEST_CASE("Two or more different types are distinct.", "[type_traits][are_distinct]") {
  STATIC_REQUIRE(percy::are_distinct_v<bool, char> == true);
  STATIC_REQUIRE(percy::are_distinct_v<bool, char, int> == true);
  STATIC_REQUIRE(percy::are_distinct_v<bool, char, int, float> == true);
}

TEST_CASE("Two or more identical types are not distinct.", "[type_traits][are_distinct]") {
  STATIC_REQUIRE(percy::are_distinct_v<bool, bool> == false);
  STATIC_REQUIRE(percy::are_distinct_v<bool, bool, bool> == false);
  STATIC_REQUIRE(percy::are_distinct_v<bool, bool, bool, bool> == false);
}

TEST_CASE("Types with two duplicates are not distinct.", "[type_traits][are_distinct]") {
  STATIC_REQUIRE(percy::are_distinct_v<bool, bool, int> == false);
  STATIC_REQUIRE(percy::are_distinct_v<bool, int, bool> == false);

  STATIC_REQUIRE(percy::are_distinct_v<bool, bool, int, float> == false);
  STATIC_REQUIRE(percy::are_distinct_v<bool, int, bool, float> == false);
  STATIC_REQUIRE(percy::are_distinct_v<bool, int, float, bool> == false);
}
