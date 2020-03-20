#include <catch2/catch.hpp>

#include <percy/variant.hpp>

#include <string>

TEST_CASE("Test 1.", "[test]") {
  percy::variant_union<std::string, percy::valueless> u1(std::string("hello"), percy::variant_index<0>{});

  percy::variant_union<std::string, percy::valueless> u2(percy::valueless{}, percy::variant_index<1>{});
  u2.copy(u1, 0);

  u1.destroy(0);
  u2.destroy(0);
}

TEST_CASE("Test 2.", "[test]") {
  percy::variant<std::string> v1(std::string("hello"));

  percy::variant<std::string> v2(v1);
}

struct copy_only {
  std::size_t& copies_;
  std::size_t& destructions_;

  explicit copy_only(std::size_t& copies, std::size_t& destructions) : copies_(copies), destructions_(destructions) {}

  copy_only(copy_only&&) = delete;
  copy_only& operator=(copy_only&&) = delete;

  copy_only(const copy_only& other) : copies_(other.copies_), destructions_(other.destructions_) {
    copies_ += 1;
  }

  copy_only& operator=(const copy_only& other) {
    copies_ = const_cast<copy_only&>(other).copies_;
    destructions_ = other.destructions_;
    copies_ += 1;
    return *this;
  }

  ~copy_only() {
    destructions_ += 1;
  }
};

TEST_CASE("Test 3.", "[test]") {
  std::size_t copies = 0;
  std::size_t destructions = 0;

  {
    copy_only instance(copies, destructions);
    percy::variant<copy_only> v1(instance);
    percy::variant<copy_only> v2(v1);
  }

  REQUIRE(copies == 3);
  REQUIRE(destructions == 3);
}

TEST_CASE("A `variant` can be constructed with a value of the 1st type.", "[variant]") {
  constexpr percy::variant<int, char, unsigned> variant(42);

  STATIC_REQUIRE(variant.index() == 0);
  STATIC_REQUIRE(percy::holds_alternative<int>(variant));
  STATIC_REQUIRE(percy::get<0>(variant) == 42);
  STATIC_REQUIRE(percy::get<int>(variant) == 42);
}

TEST_CASE("A `variant` can be constructed with a value of the 2nd type.", "[variant]") {
  constexpr percy::variant<int, char, unsigned> variant('x');

  STATIC_REQUIRE(variant.index() == 1);
  STATIC_REQUIRE(percy::holds_alternative<char>(variant));
  STATIC_REQUIRE(percy::get<1>(variant) == 'x');
  STATIC_REQUIRE(percy::get<char>(variant) == 'x');
}

TEST_CASE("A `variant` can be constructed with a value of the 3rd type.", "[variant]") {
  constexpr percy::variant<int, char, unsigned> variant(42u);

  STATIC_REQUIRE(variant.index() == 2);
  STATIC_REQUIRE(percy::holds_alternative<unsigned>(variant));
  STATIC_REQUIRE(percy::get<2>(variant) == 42u);
  STATIC_REQUIRE(percy::get<unsigned>(variant) == 42u);
}

struct x {
  constexpr x() = delete;
};

struct y {
  constexpr y() = delete;
  constexpr explicit y(int) {}
};

TEST_CASE("A `variant` can be constructed with a value when all default constructors are deleted.", "[variant]") {
  constexpr percy::variant<x, y> variant(y(42));

  STATIC_REQUIRE(variant.index() == 1);
  STATIC_REQUIRE(percy::holds_alternative<y>(variant));
}

TEST_CASE("A `variant` can be copy constructed when all default constructors are deleted.", "[variant]") {
  constexpr percy::variant<x, y> variant(y(42));

  constexpr auto copy = variant;

  STATIC_REQUIRE(variant.index() == 1);
  STATIC_REQUIRE(percy::holds_alternative<y>(variant));

  STATIC_REQUIRE(copy.index() == 1);
  STATIC_REQUIRE(percy::holds_alternative<y>(copy));
}

TEST_CASE("A `variant` can clean up memory dynamically allocated by an element.", "[variant]") {
  percy::variant<std::string> variant(std::string("abc"));

  REQUIRE(variant.index() == 0);
}

TEST_CASE("A `variant` correctly manages dynamically allocated memory when making a copy.", "[variant]") {
  percy::variant<std::string> original(std::string("abc"));

  auto copy = original;

  REQUIRE(original.index() == 0);
  REQUIRE(copy.index() == 0);
}

TEST_CASE("Nested `variant` correctly manages dynamically allocated memory.", "[variant]") {
  percy::variant<std::string> inner(std::string("abc"));

  percy::variant<decltype(inner)> outer(inner);

  REQUIRE(inner.index() == 0);
  REQUIRE(outer.index() == 0);
}
