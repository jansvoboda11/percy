#include <catch2/catch.hpp>

#include <percy/type_traits.hpp>

TEST_CASE("Predicate `are_same` holds when all types are identical.", "[type_traits][are_same]") {
  STATIC_REQUIRE(percy::are_same_v<bool>);
  STATIC_REQUIRE(percy::are_same_v<bool, bool>);
  STATIC_REQUIRE(percy::are_same_v<bool, bool, bool>);
  STATIC_REQUIRE(percy::are_same_v<bool, bool, bool, bool>);
}

TEST_CASE("Predicate `are_same` does not hold when at least one type is different.", "[type_traits][are_same]") {
  STATIC_REQUIRE_FALSE(percy::are_same_v<bool, int>);

  STATIC_REQUIRE_FALSE(percy::are_same_v<bool, bool, int>);
  STATIC_REQUIRE_FALSE(percy::are_same_v<bool, int, bool>);

  STATIC_REQUIRE_FALSE(percy::are_same_v<bool, bool, bool, int>);
  STATIC_REQUIRE_FALSE(percy::are_same_v<bool, bool, int, bool>);
  STATIC_REQUIRE_FALSE(percy::are_same_v<bool, int, bool, bool>);
}

TEST_CASE("Predicate `are_unique` holds when no types are duplicated.", "[type_traits][are_unique]") {
  STATIC_REQUIRE(percy::are_unique_v<bool>);
  STATIC_REQUIRE(percy::are_unique_v<bool, char>);
  STATIC_REQUIRE(percy::are_unique_v<bool, char, int>);
  STATIC_REQUIRE(percy::are_unique_v<bool, char, int, float>);
}

TEST_CASE("Predicate `are_unique` does not hold when at least one type is duplicated.", "[type_traits][are_unique]") {
  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, bool, int>);
  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, int, bool>);

  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, bool, int, float>);
  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, int, bool, float>);
  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, int, float, bool>);

  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, bool>);
  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, bool, bool>);
  STATIC_REQUIRE_FALSE(percy::are_unique_v<bool, bool, bool, bool>);
}

TEST_CASE("Trait `index_of` returns correct indices.", "[type_traits][index_of]") {
  STATIC_REQUIRE(percy::index_of_v<bool, bool> == 0);
  STATIC_REQUIRE(percy::index_of_v<bool, bool, char> == 0);
  STATIC_REQUIRE(percy::index_of_v<bool, bool, char, char> == 0);

  STATIC_REQUIRE(percy::index_of_v<bool, char, bool> == 1);
  STATIC_REQUIRE(percy::index_of_v<bool, char, char, bool> == 2);
  STATIC_REQUIRE(percy::index_of_v<bool, char, char, char, bool> == 3);

  STATIC_REQUIRE(percy::index_of_v<bool, char, bool, char> == 1);
  STATIC_REQUIRE(percy::index_of_v<bool, char, char, bool, char> == 2);
}
