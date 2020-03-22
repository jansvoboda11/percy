#include <catch2/catch.hpp>

#include <percy/variadic_union.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// GETTER/SETTER TESTS //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("The set & get operations of `variadic_union` have expected semantics", "[variadic_union]") {
  // todo: figure out how to make the following call `percy::destroy`

  SECTION("Get the element of a single-element union.") {
    constexpr percy::variadic_union<int> union_0(42);

    STATIC_REQUIRE(percy::get<int>(union_0) == 42);
  }

  SECTION("Get any element of an union.") {
    constexpr percy::variadic_union<bool, int, char, unsigned> union_0(false);
    constexpr percy::variadic_union<bool, int, char, unsigned> union_1(42);
    constexpr percy::variadic_union<bool, int, char, unsigned> union_2('x');
    constexpr percy::variadic_union<bool, int, char, unsigned> union_3(42u);

    STATIC_REQUIRE(percy::get<bool>(union_0) == false);
    STATIC_REQUIRE(percy::get<int>(union_1) == 42);
    STATIC_REQUIRE(percy::get<char>(union_2) == 'x');
    STATIC_REQUIRE(percy::get<unsigned>(union_3) == 42u);
  }

  SECTION("Get the element of a moved single-element union.") {
    constexpr percy::variadic_union<bool, int, char, unsigned> original(42);
    constexpr percy::variadic_union<bool, int, char, unsigned> moved(std::move(original), 1);

    STATIC_REQUIRE(percy::get<int>(moved) == 42);
  }

  SECTION("Get the element of a copied single-element union.") {
    constexpr percy::variadic_union<bool, int, char, unsigned> original(42);
    constexpr percy::variadic_union<bool, int, char, unsigned> copy(original, 1);

    STATIC_REQUIRE(percy::get<int>(original) == 42);
    STATIC_REQUIRE(percy::get<int>(copy) == 42);
  }

  // todo: figure out how to make the following `constexpr`

  SECTION("Set the element of a single-element union via move.") {
    percy::variadic_union<int> union_0(0);

    percy::set(union_0, 42);

    REQUIRE(percy::get<int>(union_0) == 42);
  }

  SECTION("Set any element of an union via move.") {
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

  SECTION("Set the element of a single-element union via copy.") {
    percy::variadic_union<int> union_0(0);

    int replacement_0 = 42;
    percy::set(union_0, replacement_0);

    REQUIRE(percy::get<int>(union_0) == 42);
  }

  SECTION("Set any element of an union via copy.") {
    percy::variadic_union<bool, int, char, unsigned> union_0(true);
    percy::variadic_union<bool, int, char, unsigned> union_1(true);
    percy::variadic_union<bool, int, char, unsigned> union_2(true);
    percy::variadic_union<bool, int, char, unsigned> union_3(true);

    bool replacement_0 = false;
    int replacement_1 = 42;
    char replacement_2 = 'x';
    unsigned replacement_3 = 42u;

    percy::set(union_0, replacement_0);
    percy::set(union_1, replacement_1);
    percy::set(union_2, replacement_2);
    percy::set(union_3, replacement_3);

    REQUIRE(percy::get<bool>(union_0) == false);
    REQUIRE(percy::get<int>(union_1) == 42);
    REQUIRE(percy::get<char>(union_2) == 'x');
    REQUIRE(percy::get<unsigned>(union_3) == 42u);
  }
}

// todo: The following is a concept of a testing DSL that enables `set` & `destroy` operations in constexpr.
//       Decide if this is worth expending upon.

template <typename... Ts>
using union_of = percy::variadic_union<Ts...>;

template <typename T>
struct set_t {
  T value_;
  constexpr explicit set_t(T value) : value_(value) {}
};

template <typename T>
constexpr set_t<T> set(T value) {
  return set_t(value);
}

template <typename T>
struct get {};

struct destroy_t {
  std::size_t index_;
  constexpr explicit destroy_t(std::size_t index) : index_(index) {}
};

constexpr destroy_t destroy(std::size_t index) {
  return destroy_t(index);
}

template <typename... Ts, typename Set, typename Get>
constexpr auto test(percy::variadic_union<Ts...> tested_union, set_t<Set> set, get<Get>, destroy_t destroy) {
  percy::set(tested_union, std::move(set.value_));
  auto result = percy::get<Get>(tested_union);
  percy::destroy(tested_union, destroy.index_);
  return result;
}

TEST_CASE("Experimental: set and get each element of a variadic union.", "[variadic_union]") {
  constexpr auto value_0 = test(union_of<bool, int, char, unsigned>(true), set(false), get<bool>(), destroy(0));
  constexpr auto value_1 = test(union_of<bool, int, char, unsigned>(true), set(42), get<int>(), destroy(1));
  constexpr auto value_2 = test(union_of<bool, int, char, unsigned>(true), set('x'), get<char>(), destroy(2));
  constexpr auto value_3 = test(union_of<bool, int, char, unsigned>(true), set(42u), get<unsigned>(), destroy(3));

  STATIC_REQUIRE(value_0 == false);
  STATIC_REQUIRE(value_1 == 42);
  STATIC_REQUIRE(value_2 == 'x');
  STATIC_REQUIRE(value_3 == 42u);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// LIFETIME TESTS HELPERS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct lifetime_summary {
  std::size_t move_count;
  std::size_t copy_count;
  std::size_t destroy_count;
  constexpr lifetime_summary() : move_count(0), copy_count(0), destroy_count(0) {}
};

struct lifetime {
  lifetime_summary* summary_;

  constexpr explicit lifetime(lifetime_summary* summary) : summary_(summary) {}

  constexpr lifetime(lifetime&& other) noexcept : summary_(other.summary_) {
    summary_->move_count += 1;
  }

  constexpr lifetime& operator=(lifetime&& other) noexcept {
    if (this != &other) {
      summary_ = other.summary_;
    }

    summary_->move_count += 1;
    return *this;
  }

  constexpr lifetime(const lifetime& other) : summary_(other.summary_) {
    summary_->copy_count += 1;
  }

  constexpr lifetime& operator=(const lifetime& other) {
    if (this != &other) {
      summary_ = other.summary_;
    }

    summary_->copy_count += 1;
    return *this;
  }

  constexpr ~lifetime() {
    summary_->destroy_count += 1;
  }
};

template <typename Function>
constexpr auto analyze_lifetime(Function&& function) {
  lifetime_summary summary;

  function(summary);

  return summary;
}

template <typename Function>
constexpr auto analyze_lifetimes(Function&& function) {
  lifetime_summary summary_1, summary_2;

  function(summary_1, summary_2);

  return std::make_pair(summary_1, summary_2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// LIFETIME TESTS ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Lifetimes of `variadic_union` elements are managed correctly.", "[variadic_union]") {
  SECTION("Element move constructor.") {
    SECTION("With the only element.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime> union_0(std::move(element));
        percy::destroy(union_0, 0);
      });

      // 1. move: local variable -> union constructor -> union value
      // 2. destroy: local variable
      // 2. destroy: union value
      STATIC_REQUIRE(summary.move_count == 1);
      STATIC_REQUIRE(summary.copy_count == 0);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }

    SECTION("With the first of more elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime, int> union_0(std::move(element));
        percy::destroy(union_0, 0);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 1);
      STATIC_REQUIRE(summary.copy_count == 0);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }

    SECTION("With the second of more elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<int, lifetime> union_0(std::move(element));
        percy::destroy(union_0, 1);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 1);
      STATIC_REQUIRE(summary.copy_count == 0);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }
  }

  SECTION("Element copy constructor.") {
    SECTION("With the only element.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime> union_0(element);
        percy::destroy(union_0, 0);
      });

      // 1. copy: local variable -> union constructor -> union value
      // 2. destroy: local variable
      // 2. destroy: union value
      STATIC_REQUIRE(summary.move_count == 0);
      STATIC_REQUIRE(summary.copy_count == 1);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }

    SECTION("With the first of more elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime, int> union_0(element);
        percy::destroy(union_0, 0);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 0);
      STATIC_REQUIRE(summary.copy_count == 1);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }

    SECTION("With the second of more elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<int, lifetime> union_0(element);
        percy::destroy(union_0, 1);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 0);
      STATIC_REQUIRE(summary.copy_count == 1);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }
  }

  SECTION("Move constructor.") {
    SECTION("With the only element.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime> original(std::move(element));
        percy::variadic_union<lifetime> moved(std::move(original), 0);
        percy::destroy(moved, 0);
      });

      // 1. move: local variable -> union constructor -> `original` union value
      // 2. move: `original` union value -> move -> `moved` union value
      // 3. destroy: local variable
      // 4. destroy: `moved` union value
      STATIC_REQUIRE(summary.move_count == 2);
      STATIC_REQUIRE(summary.copy_count == 0);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }

    SECTION("With the first of many elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime, int> original(std::move(element));
        percy::variadic_union<lifetime, int> moved(std::move(original), 0);
        percy::destroy(moved, 0);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 2);
      STATIC_REQUIRE(summary.copy_count == 0);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }

    SECTION("With the second of many elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<int, lifetime> original(std::move(element));
        percy::variadic_union<int, lifetime> moved(std::move(original), 1);
        percy::destroy(moved, 1);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 2);
      STATIC_REQUIRE(summary.copy_count == 0);
      STATIC_REQUIRE(summary.destroy_count == 2);
    }
  }

  SECTION("Copy constructor.") {
    SECTION("With the only element.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime> original(std::move(element));
        percy::variadic_union<lifetime> copy(original, 0);
        percy::destroy(copy, 0);
        percy::destroy(original, 0);
      });

      // 1. move: local variable -> union constructor -> `original` union value
      // 2. copy: `original` union value -> copy -> `copy` union value
      // 3. destroy: local variable
      // 4. destroy: `copy` union value
      // 5. destroy: `original` union value
      REQUIRE(summary.move_count == 1);
      REQUIRE(summary.copy_count == 1);
      REQUIRE(summary.destroy_count == 3);
    }

    SECTION("With the first of more elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<lifetime, int> original(std::move(element));
        percy::variadic_union<lifetime, int> copy(original, 0);
        percy::destroy(copy, 0);
        percy::destroy(original, 0);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 1);
      STATIC_REQUIRE(summary.copy_count == 1);
      STATIC_REQUIRE(summary.destroy_count == 3);
    }

    SECTION("With the second of more elements.") {
      constexpr auto summary = analyze_lifetime([](lifetime_summary& summary) {
        lifetime element(&summary);

        percy::variadic_union<int, lifetime> original(std::move(element));
        percy::variadic_union<int, lifetime> copy(original, 1);
        percy::destroy(copy, 1);
        percy::destroy(original, 1);
      });

      // same logic as above
      STATIC_REQUIRE(summary.move_count == 1);
      STATIC_REQUIRE(summary.copy_count == 1);
      STATIC_REQUIRE(summary.destroy_count == 3);
    }
  }

  SECTION("Move set.") {
    SECTION("With the only element.") {
      constexpr auto summaries = analyze_lifetimes([](lifetime_summary& summary_1, lifetime_summary& summary_2) {
        lifetime element(&summary_1);
        lifetime replacement(&summary_2);

        percy::variadic_union<lifetime> union_0(std::move(element));
        percy::set(union_0, std::move(replacement));
        percy::destroy(union_0, 0);
      });

      // 1. move: local variable -> union constructor -> union value
      // 2. destroy: local variable
      STATIC_REQUIRE(summaries.first.move_count == 1);
      STATIC_REQUIRE(summaries.first.copy_count == 0);
      STATIC_REQUIRE(summaries.first.destroy_count == 1); // the value is not destroyed automatically!

      // 3. move: local variable -> set -> union value
      // 4. destroy: local variable
      // 5. destroy: union value
      STATIC_REQUIRE(summaries.second.move_count == 1);
      STATIC_REQUIRE(summaries.second.copy_count == 0);
      STATIC_REQUIRE(summaries.second.destroy_count == 2);
    }

    SECTION("With the first of more elements.") {
      constexpr auto summaries = analyze_lifetimes([](lifetime_summary& summary_1, lifetime_summary& summary_2) {
        lifetime element(&summary_1);
        lifetime replacement(&summary_2);

        percy::variadic_union<lifetime, int> union_0(std::move(element));
        percy::set(union_0, std::move(replacement));
        percy::destroy(union_0, 0);
      });

      // same logic as above
      STATIC_REQUIRE(summaries.first.move_count == 1);
      STATIC_REQUIRE(summaries.first.copy_count == 0);
      STATIC_REQUIRE(summaries.first.destroy_count == 1);

      // same logic as above
      STATIC_REQUIRE(summaries.second.move_count == 1);
      STATIC_REQUIRE(summaries.second.copy_count == 0);
      STATIC_REQUIRE(summaries.second.destroy_count == 2);
    }

    SECTION("With the second of more elements.") {
      constexpr auto summaries = analyze_lifetimes([](lifetime_summary& summary_1, lifetime_summary& summary_2) {
        lifetime element(&summary_1);
        lifetime replacement(&summary_2);

        percy::variadic_union<int, lifetime> union_0(std::move(element));
        percy::set(union_0, std::move(replacement));
        percy::destroy(union_0, 1);
      });

      // same logic as above
      STATIC_REQUIRE(summaries.first.move_count == 1);
      STATIC_REQUIRE(summaries.first.copy_count == 0);
      STATIC_REQUIRE(summaries.first.destroy_count == 1);

      // same logic as above
      STATIC_REQUIRE(summaries.second.move_count == 1);
      STATIC_REQUIRE(summaries.second.copy_count == 0);
      STATIC_REQUIRE(summaries.second.destroy_count == 2);
    }
  }

  SECTION("Copy set.") {
    SECTION("With the only element.") {
      constexpr auto summaries = analyze_lifetimes([](lifetime_summary& summary_1, lifetime_summary& summary_2) {
        lifetime element(&summary_1);
        lifetime replacement(&summary_2);

        percy::variadic_union<lifetime> union_0(std::move(element));
        percy::set(union_0, replacement);
        percy::destroy(union_0, 0);
      });

      // 1. move: local variable -> union constructor -> union value
      // 2. destroy: local variable
      STATIC_REQUIRE(summaries.first.move_count == 1);
      STATIC_REQUIRE(summaries.first.copy_count == 0);
      STATIC_REQUIRE(summaries.first.destroy_count == 1); // the value is not destroyed automatically!

      // 3. copy: local variable -> set -> union value
      // 4. destroy: local variable
      // 5. destroy: union value
      STATIC_REQUIRE(summaries.second.move_count == 0);
      STATIC_REQUIRE(summaries.second.copy_count == 1);
      STATIC_REQUIRE(summaries.second.destroy_count == 2);
    }

    SECTION("With the first of more elements.") {
      constexpr auto summaries = analyze_lifetimes([](lifetime_summary& summary_1, lifetime_summary& summary_2) {
        lifetime element(&summary_1);
        lifetime replacement(&summary_2);

        percy::variadic_union<lifetime, int> union_0(std::move(element));
        percy::set(union_0, replacement);
        percy::destroy(union_0, 0);
      });

      // same logic as above
      STATIC_REQUIRE(summaries.first.move_count == 1);
      STATIC_REQUIRE(summaries.first.copy_count == 0);
      STATIC_REQUIRE(summaries.first.destroy_count == 1);

      // same logic as above
      STATIC_REQUIRE(summaries.second.move_count == 0);
      STATIC_REQUIRE(summaries.second.copy_count == 1);
      STATIC_REQUIRE(summaries.second.destroy_count == 2);
    }

    SECTION("With the second of more elements.") {
      constexpr auto summaries = analyze_lifetimes([](lifetime_summary& summary_1, lifetime_summary& summary_2) {
        lifetime element(&summary_1);
        lifetime replacement(&summary_2);

        percy::variadic_union<int, lifetime> union_0(std::move(element));
        percy::set(union_0, replacement);
        percy::destroy(union_0, 1);
      });

      // same logic as above
      STATIC_REQUIRE(summaries.first.move_count == 1);
      STATIC_REQUIRE(summaries.first.copy_count == 0);
      STATIC_REQUIRE(summaries.first.destroy_count == 1);

      // same logic as above
      STATIC_REQUIRE(summaries.second.move_count == 0);
      STATIC_REQUIRE(summaries.second.copy_count == 1);
      STATIC_REQUIRE(summaries.second.destroy_count == 2);
    }
  }
}
