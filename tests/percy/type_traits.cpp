#include <catch2/catch.hpp>

#include <percy/type_traits.hpp>

TEST_CASE("Predicate `are_same` holds when all types are identical.", "[type_traits][are_same]") {
  STATIC_REQUIRE(percy::are_same_v<bool> == true);
  STATIC_REQUIRE(percy::are_same_v<bool, bool> == true);
  STATIC_REQUIRE(percy::are_same_v<bool, bool, bool> == true);
  STATIC_REQUIRE(percy::are_same_v<bool, bool, bool, bool> == true);
}

TEST_CASE("Predicate `are_same` does not hold when at least one type is different.", "[type_traits][are_same]") {
  STATIC_REQUIRE(percy::are_same_v<bool, int> == false);

  STATIC_REQUIRE(percy::are_same_v<bool, bool, int> == false);
  STATIC_REQUIRE(percy::are_same_v<bool, int, bool> == false);

  STATIC_REQUIRE(percy::are_same_v<bool, bool, bool, int> == false);
  STATIC_REQUIRE(percy::are_same_v<bool, bool, int, bool> == false);
  STATIC_REQUIRE(percy::are_same_v<bool, int, bool, bool> == false);
}

TEST_CASE("Predicate `are_unique` holds when no types are duplicated.", "[type_traits][are_unique]") {
  STATIC_REQUIRE(percy::are_unique_v<bool> == true);
  STATIC_REQUIRE(percy::are_unique_v<bool, char> == true);
  STATIC_REQUIRE(percy::are_unique_v<bool, char, int> == true);
  STATIC_REQUIRE(percy::are_unique_v<bool, char, int, float> == true);
}

TEST_CASE("Predicate `are_unique` does not hold when at least one type is duplicated.", "[type_traits][are_unique]") {
  STATIC_REQUIRE(percy::are_unique_v<bool, bool, int> == false);
  STATIC_REQUIRE(percy::are_unique_v<bool, int, bool> == false);

  STATIC_REQUIRE(percy::are_unique_v<bool, bool, int, float> == false);
  STATIC_REQUIRE(percy::are_unique_v<bool, int, bool, float> == false);
  STATIC_REQUIRE(percy::are_unique_v<bool, int, float, bool> == false);

  STATIC_REQUIRE(percy::are_unique_v<bool, bool> == false);
  STATIC_REQUIRE(percy::are_unique_v<bool, bool, bool> == false);
  STATIC_REQUIRE(percy::are_unique_v<bool, bool, bool, bool> == false);
}
