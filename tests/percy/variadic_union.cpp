#include <catch2/catch.hpp>

#include <percy/variadic_union.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// GETTER TESTS /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// todo: all tests in this section should also call `percy::destroy`

TEST_CASE("Get the element of a single-element variadic union.", "[variadic_union]") {
  constexpr percy::variadic_union<int> uni(42);

  STATIC_REQUIRE(percy::get<int>(uni) == 42);
}

TEST_CASE("Get each element of a variadic union.", "[variadic_union]") {
  constexpr percy::variadic_union<bool, int, char, unsigned> union_0(false);
  constexpr percy::variadic_union<bool, int, char, unsigned> union_1(42);
  constexpr percy::variadic_union<bool, int, char, unsigned> union_2('x');
  constexpr percy::variadic_union<bool, int, char, unsigned> union_3(42u);

  STATIC_REQUIRE(percy::get<bool>(union_0) == false);
  STATIC_REQUIRE(percy::get<int>(union_1) == 42);
  STATIC_REQUIRE(percy::get<char>(union_2) == 'x');
  STATIC_REQUIRE(percy::get<unsigned>(union_3) == 42u);
}

TEST_CASE("Get an element of a moved variadic union.", "[variadic_union]") {
  constexpr percy::variadic_union<bool, int, char, unsigned> original(42);
  constexpr percy::variadic_union<bool, int, char, unsigned> moved(std::move(original), 1);

  STATIC_REQUIRE(percy::get<int>(moved) == 42);
}

TEST_CASE("Get an element of a copied variadic union.", "[variadic_union]") {
  constexpr percy::variadic_union<bool, int, char, unsigned> original(42);
  constexpr percy::variadic_union<bool, int, char, unsigned> copy(original, 1);

  STATIC_REQUIRE(percy::get<int>(original) == 42);
  STATIC_REQUIRE(percy::get<int>(copy) == 42);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// SETTER TESTS /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// todo: all tests in this section should also call `percy::destroy`

TEST_CASE("Set the element of a single-element variadic union.", "[variadic_union]") {
  percy::variadic_union<int> union_0(0);

  percy::set(union_0, 42);

  REQUIRE(percy::get<int>(union_0) == 42);
}

TEST_CASE("Set each element of a variadic union.", "[variadic_union]") {
  percy::variadic_union<bool, int, char, unsigned> union_0(true);
  percy::variadic_union<bool, int, char, unsigned> union_1(true);
  percy::variadic_union<bool, int, char, unsigned> union_2(true);
  percy::variadic_union<bool, int, char, unsigned> union_3(true);

  percy::set(union_0, false);
  percy::set(union_1, 42);
  percy::set(union_2, 'x');
  percy::set(union_3, 42u);

  REQUIRE(percy::get<bool>(union_0) == false);
  REQUIRE(percy::get<int>(union_1) == 42);
  REQUIRE(percy::get<char>(union_2) == 'x');
  REQUIRE(percy::get<unsigned>(union_3) == 42u);
}

// todo: also test the reference setters

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// LIFETIME TESTS HELPER /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct lifetime {
  std::size_t* move_;
  std::size_t* copy_;
  std::size_t* destroy_;

  constexpr explicit lifetime(std::size_t* move, std::size_t* copy, std::size_t* destroy)
      : move_(move), copy_(copy), destroy_(destroy) {}

  constexpr lifetime(lifetime&& other) noexcept : move_(other.move_), copy_(other.copy_), destroy_(other.destroy_) {
    *move_ += 1;
  }

  constexpr lifetime& operator=(lifetime&& other) noexcept {
    if (this != &other) {
      move_ = other.move_;
      copy_ = other.copy_;
      destroy_ = other.destroy_;
    }

    *move_ += 1;
    return *this;
  }

  constexpr lifetime(const lifetime& other) : move_(other.move_), copy_(other.copy_), destroy_(other.destroy_) {
    *copy_ += 1;
  }

  constexpr lifetime& operator=(const lifetime& other) {
    if (this != &other) {
      move_ = other.move_;
      copy_ = other.copy_;
      destroy_ = other.destroy_;
    }

    *copy_ += 1;
    return *this;
  }

  constexpr ~lifetime() {
    *destroy_ += 1;
  }
};

TEST_CASE("Lifetimes of `variadic_union` elements are managed correctly.", "[variadic_union]") {
  std::size_t move_count = 0;
  std::size_t copy_count = 0;
  std::size_t destroy_count = 0;

  SECTION("The only element.") {
    {
      percy::variadic_union<lifetime> uni(lifetime(&move_count, &copy_count, &destroy_count));
      percy::destroy(uni, 0);
    }

    // 1. move: local variable -> constructor -> value
    // 2. destroy: local variable
    // 2. destroy: value
    REQUIRE(move_count == 1);
    REQUIRE(copy_count == 0);
    REQUIRE(destroy_count == 2);
  }

  SECTION("The first of more elements.") {
    {
      percy::variadic_union<lifetime, int> uni(lifetime(&move_count, &copy_count, &destroy_count));
      percy::destroy(uni, 0);
    }

    // 1. move: local variable -> outer constructor -> outer value
    // 2. destroy: local variable
    // 3. destroy: outer value
    REQUIRE(move_count == 1);
    REQUIRE(copy_count == 0);
    REQUIRE(destroy_count == 2);
  }

  SECTION("The second of more elements.", "[variadic_union]") {
    {
      percy::variadic_union<int, lifetime> uni(lifetime(&move_count, &copy_count, &destroy_count));
      percy::destroy(uni, 1);
    }

    // 1. move: local variable -> outer constructor -> inner constructor -> inner value
    // 2. destroy: local variable
    // 3. destroy: inner value
    REQUIRE(move_count == 1);
    REQUIRE(copy_count == 0);
    REQUIRE(destroy_count == 2);
  }
}

TEST_CASE("Lifetimes of `variadic_union` elements are managed correctly during move operations.", "[variadic_union]") {
  std::size_t move_count = 0;
  std::size_t copy_count = 0;
  std::size_t destroy_count = 0;

  SECTION("The only element.") {
    {
      percy::variadic_union<lifetime> original(lifetime(&move_count, &copy_count, &destroy_count));
      percy::variadic_union<lifetime> moved(std::move(original), 0);
      percy::destroy(moved, 0);
    }

    // 1. move: local variable -> `original` constructor -> `original` value
    // 2. move: `original` value -> `moved` value
    // 3. destroy: local variable
    // 4. destroy: `moved` value
    REQUIRE(move_count == 2);
    REQUIRE(copy_count == 0);
    REQUIRE(destroy_count == 2);
  }

  SECTION("The first of many elements.") {
    {
      percy::variadic_union<lifetime, int> original(lifetime(&move_count, &copy_count, &destroy_count));
      percy::variadic_union<lifetime, int> moved(std::move(original), 0);
      percy::destroy(moved, 0);
    }

    // 1. move: local variable -> outer `original` constructor -> outer `original` value
    // 2. move: outer `original` value -> outer `moved` value
    // 3. destroy: local variable
    // 4. destroy: outer `moved` value
    REQUIRE(move_count == 2);
    REQUIRE(copy_count == 0);
    REQUIRE(destroy_count == 2);
  }

  SECTION("The second of many elements.") {
    {
      percy::variadic_union<int, lifetime> original(lifetime(&move_count, &copy_count, &destroy_count));
      percy::variadic_union<int, lifetime> moved(std::move(original), 1);
      percy::destroy(moved, 1);
    }

    // 1. move: local variable -> outer `original` constructor -> inner `original` constructor -> inner `original` value
    // 2. move: inner `original` value -> inner `moved` value
    // 3. destroy: local variable
    // 4. destroy: inner `moved` value
    REQUIRE(move_count == 2);
    REQUIRE(copy_count == 0);
    REQUIRE(destroy_count == 2);
  }
}

TEST_CASE("Lifetimes of `variadic_union` elements are managed correctly during copy operations.", "[variadic_union]") {
  std::size_t move_count = 0;
  std::size_t copy_count = 0;
  std::size_t destroy_count = 0;

  SECTION("The only element.") {
    {
      percy::variadic_union<lifetime> original(lifetime(&move_count, &copy_count, &destroy_count));
      percy::variadic_union<lifetime> copy(original, 0);
      percy::destroy(copy, 0);
      percy::destroy(original, 0);
    }

    // 1. move: local variable -> `original` constructor -> `original` value
    // 2. copy: `original` value -> `copy` value
    // 3. destroy: local variable
    // 4. destroy: `copy` value
    // 5. destroy: `original` value
    REQUIRE(move_count == 1);
    REQUIRE(copy_count == 1);
    REQUIRE(destroy_count == 3);
  }

  SECTION("The first of more elements.") {
    {
      percy::variadic_union<lifetime, int> original(lifetime(&move_count, &copy_count, &destroy_count));
      percy::variadic_union<lifetime, int> copy(original, 0);
      percy::destroy(copy, 0);
      percy::destroy(original, 0);
    }

    // 1. move: local variable -> outer `original` constructor -> outer `original` value
    // 2. copy: outer `original` value -> outer `copy` value
    // 3. destroy: local variable
    // 4. destroy: outer `copy` value
    // 5. destroy: outer `original` value
    REQUIRE(move_count == 1);
    REQUIRE(copy_count == 1);
    REQUIRE(destroy_count == 3);
  }

  SECTION("The second of more elements.") {
    {
      percy::variadic_union<int, lifetime> original(lifetime(&move_count, &copy_count, &destroy_count));
      percy::variadic_union<int, lifetime> copy(original, 1);
      percy::destroy(copy, 1);
      percy::destroy(original, 1);
    }

    // 1. move: local variable -> outer `original` constructor -> inner `original` constructor -> inner `original` value
    // 2. copy: inner `original` value -> inner `copy` value
    // 3. destroy: local variable
    // 4. destroy: inner `copy` value
    // 5. destroy: inner `original` value
    REQUIRE(move_count == 1);
    REQUIRE(copy_count == 1);
    REQUIRE(destroy_count == 3);
  }
}

TEST_CASE("Lifetimes of `variadic_union` elements are managed correctly during move set calls.", "[variadic_union]") {
  std::size_t move_count_1 = 0;
  std::size_t copy_count_1 = 0;
  std::size_t destroy_count_1 = 0;

  std::size_t move_count_2 = 0;
  std::size_t copy_count_2 = 0;
  std::size_t destroy_count_2 = 0;

  SECTION("The only element.") {
    {
      percy::variadic_union<lifetime> uni(lifetime(&move_count_1, &copy_count_1, &destroy_count_1));
      percy::set(uni, lifetime(&move_count_2, &copy_count_2, &destroy_count_2));
      percy::destroy(uni, 0);
    }

    // 1. move: local variable -> constructor -> value
    // 2. destroy: local variable
    REQUIRE(move_count_1 == 1);
    REQUIRE(copy_count_1 == 0);
    REQUIRE(destroy_count_1 == 1); // the value is not destroyed automatically!

    // 3. move: local variable -> set -> value
    // 4. destroy: local variable
    // 5. destroy: value
    REQUIRE(move_count_2 == 1);
    REQUIRE(copy_count_2 == 0);
    REQUIRE(destroy_count_2 == 2);
  }

  SECTION("The first of more elements.") {
    {
      percy::variadic_union<lifetime, int> uni(lifetime(&move_count_1, &copy_count_1, &destroy_count_1));
      percy::set(uni, lifetime(&move_count_2, &copy_count_2, &destroy_count_2));
      percy::destroy(uni, 0);
    }

    // 1. move: local variable -> constructor -> value
    // 2. destroy: local variable
    REQUIRE(move_count_1 == 1);
    REQUIRE(copy_count_1 == 0);
    REQUIRE(destroy_count_1 == 1); // the value is not destroyed automatically!

    // 3. move: local variable -> set -> value
    // 4. destroy: local variable
    // 5. destroy: value
    REQUIRE(move_count_2 == 1);
    REQUIRE(copy_count_2 == 0);
    REQUIRE(destroy_count_2 == 2);
  }

  SECTION("The second of more elements.") {
    {
      percy::variadic_union<int, lifetime> uni(lifetime(&move_count_1, &copy_count_1, &destroy_count_1));
      percy::set(uni, lifetime(&move_count_2, &copy_count_2, &destroy_count_2));
      percy::destroy(uni, 1);
    }

    // 1. move: local variable -> outer constructor -> inner constructor -> inner value
    // 2. destroy: local variable
    REQUIRE(move_count_1 == 1);
    REQUIRE(copy_count_1 == 0);
    REQUIRE(destroy_count_1 == 1); // the value is not destroyed automatically!

    // 3. move: local variable -> set -> value
    // 4. destroy: local variable
    // 5. destroy: value
    REQUIRE(move_count_2 == 1);
    REQUIRE(copy_count_2 == 0);
    REQUIRE(destroy_count_2 == 2);
  }
}

TEST_CASE("Lifetimes of `variadic_union` elements are managed correctly during copy set calls.", "[variadic_union]") {
  std::size_t move_count_1 = 0;
  std::size_t copy_count_1 = 0;
  std::size_t destroy_count_1 = 0;

  std::size_t move_count_2 = 0;
  std::size_t copy_count_2 = 0;
  std::size_t destroy_count_2 = 0;

  SECTION("The only element.") {
    {
      lifetime replacement(&move_count_2, &copy_count_2, &destroy_count_2);

      percy::variadic_union<lifetime> uni(lifetime(&move_count_1, &copy_count_1, &destroy_count_1));
      percy::set(uni, replacement);
      percy::destroy(uni, 0);
    }

    // 1. move: local variable -> constructor -> value
    // 2. destroy: local variable
    REQUIRE(move_count_1 == 1);
    REQUIRE(copy_count_1 == 0);
    REQUIRE(destroy_count_1 == 1); // the value is not destroyed automatically!

    // 3. copy: local variable -> set -> value
    // 4. destroy: local variable
    // 5. destroy: value
    REQUIRE(move_count_2 == 0);
    REQUIRE(copy_count_2 == 1);
    REQUIRE(destroy_count_2 == 2);
  }

  SECTION("The first of more elements.") {
    {
      lifetime replacement(&move_count_2, &copy_count_2, &destroy_count_2);

      percy::variadic_union<lifetime, int> uni(lifetime(&move_count_1, &copy_count_1, &destroy_count_1));
      percy::set(uni, replacement);
      percy::destroy(uni, 0);
    }

    // 1. move: local variable -> constructor -> value
    // 2. destroy: local variable
    REQUIRE(move_count_1 == 1);
    REQUIRE(copy_count_1 == 0);
    REQUIRE(destroy_count_1 == 1); // the value is not destroyed automatically!

    // 3. copy: local variable -> set -> value
    // 4. destroy: local variable
    // 5. destroy: value
    REQUIRE(move_count_2 == 0);
    REQUIRE(copy_count_2 == 1);
    REQUIRE(destroy_count_2 == 2);
  }

  SECTION("The second of more elements.") {
    {
      lifetime replacement(&move_count_2, &copy_count_2, &destroy_count_2);

      percy::variadic_union<int, lifetime> uni(lifetime(&move_count_1, &copy_count_1, &destroy_count_1));
      percy::set(uni, replacement);
      percy::destroy(uni, 1);
    }

    // 1. move: local variable -> outer constructor -> inner constructor -> inner value
    // 2. destroy: local variable
    REQUIRE(move_count_1 == 1);
    REQUIRE(copy_count_1 == 0);
    REQUIRE(destroy_count_1 == 1); // the value is not destroyed automatically!

    // 3. copy: local variable -> set -> value
    // 4. destroy: local variable
    // 5. destroy: value
    REQUIRE(move_count_2 == 0);
    REQUIRE(copy_count_2 == 1);
    REQUIRE(destroy_count_2 == 2);
  }
}
