#ifndef PERCY_TESTING
#define PERCY_TESTING

#include <array>
#include <memory>
#include <utility>

#ifdef RUNTIME_TESTS
#define CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define PERCY_CONSTEXPR
#else
#define PERCY_CONSTEXPR constexpr
#endif

namespace testing {
struct tracker_id {
  std::size_t value;

  constexpr tracker_id() : value(424242) {}

  constexpr tracker_id(std::size_t id) : value(id) {}

  constexpr bool is_valid() const {
    return value != 424242;
  }

  constexpr bool operator==(const tracker_id& other) const {
    return value == other.value;
  }
};

struct explicit_constructor {
  tracker_id ths;
  constexpr explicit explicit_constructor(tracker_id ths_id) : ths(ths_id){};
  constexpr bool operator==(const explicit_constructor&) const = default;
};

struct copy_constructor {
  tracker_id ths;
  tracker_id other;
  constexpr explicit copy_constructor(tracker_id ths_id, tracker_id other_id)
      : ths(ths_id), other(other_id){};
  constexpr bool operator==(const copy_constructor&) const = default;
};

struct move_constructor {
  tracker_id ths;
  tracker_id other;
  constexpr explicit move_constructor(tracker_id ths_id, tracker_id other_id)
      : ths(ths_id), other(other_id){};
  constexpr bool operator==(const move_constructor&) const = default;
};

struct copy_assignment {
  tracker_id ths;
  tracker_id other;
  constexpr explicit copy_assignment(tracker_id ths_id, tracker_id other_id)
      : ths(ths_id), other(other_id){};
  constexpr bool operator==(const copy_assignment&) const = default;
};

struct move_assignment {
  tracker_id ths;
  tracker_id other;
  constexpr explicit move_assignment(tracker_id ths_id, tracker_id other_id)
      : ths(ths_id), other(other_id){};
  constexpr bool operator==(const move_assignment&) const = default;
};

struct destructor {
  tracker_id id;
  constexpr explicit destructor(tracker_id t_id) : id(t_id){};
  constexpr bool operator==(const destructor&) const = default;
};

struct event {
  union event_types {
    explicit_constructor ec;
    copy_constructor cc;
    move_constructor mc;
    copy_assignment ca;
    move_assignment ma;
    destructor d;
    constexpr event_types() : d(tracker_id()) {}
  } event_type;

  enum class event_type_ids { ec, cc, mc, ca, ma, d } event_type_id;

  constexpr event() : event_type(), event_type_id(event_type_ids::d) {}

  constexpr event(explicit_constructor ec) {
    std::construct_at(&event_type.ec, ec);
    event_type_id = event_type_ids::ec;
  }

  constexpr event(copy_constructor cc) {
    std::construct_at(&event_type.cc, cc);
    event_type_id = event_type_ids::cc;
  }

  constexpr event(move_constructor mc) {
    std::construct_at(&event_type.mc, mc);
    event_type_id = event_type_ids::mc;
  }

  constexpr event(copy_assignment ca) {
    std::construct_at(&event_type.ca, ca);
    event_type_id = event_type_ids::ca;
  }

  constexpr event(move_assignment ma) {
    std::construct_at(&event_type.ma, ma);
    event_type_id = event_type_ids::ma;
  }

  constexpr event(destructor d) {
    std::construct_at(&event_type.d, d);
    event_type_id = event_type_ids::d;
  }

  constexpr bool operator==(const event& other) const {
    if (event_type_id != other.event_type_id) {
      return false;
    }

    switch (event_type_id) {
    case event_type_ids::ec:
      return event_type.ec == other.event_type.ec;
    case event_type_ids::cc:
      return event_type.cc == other.event_type.cc;
    case event_type_ids::mc:
      return event_type.mc == other.event_type.mc;
    case event_type_ids::ca:
      return event_type.ca == other.event_type.ca;
    case event_type_ids::ma:
      return event_type.ma == other.event_type.ma;
    case event_type_ids::d:
      return event_type.d == other.event_type.d;
    default:
      return false;
    }
  }
};

struct event_log {
  std::array<event, 64> events;
  std::size_t cursor;

  constexpr event_log() : events(), cursor(0) {}

  constexpr void add(const event& e) {
    events[cursor++] = e;
  }

  constexpr const event& operator[](std::size_t index) const {
    return events[index];
  }

  constexpr std::size_t size() const {
    return cursor;
  }

  constexpr bool operator==(const event_log& other) const {
    if (size() != other.size()) {
      return false;
    }

    for (std::size_t i = 0; i < size(); ++i) {
      if (!((*this)[i] == other[i])) {
        return false;
      }
    }

    return true;
  }
};

struct context {
  event_log events;

  std::array<std::pair<const void*, tracker_id>, 64> ptr_id_map;
  std::size_t ptr_id_count;
  std::size_t highest_id;

  constexpr context() : events(), ptr_id_map(), ptr_id_count(0), highest_id(0) {}

  constexpr void explicitly_construct(const void* ths) {
    events.add(explicit_constructor(make_id(ths)));
  }

  constexpr void move_construct(const void* ths, const void* other) {
    events.add(move_constructor(make_id(ths), get_id(other)));
  }

  constexpr void copy_construct(const void* ths, const void* other) {
    events.add(copy_constructor(make_id(ths), get_id(other)));
  }

  constexpr void move_assign(const void* ths, const void* other) {
    events.add(move_assignment(get_or_make_id(ths), get_id(other)));
  }

  constexpr void copy_assign(const void* ths, const void* other) {
    events.add(copy_assignment(get_or_make_id(ths), get_id(other)));
  }

  constexpr void destroy(const void* ths) {
    events.add(destructor(get_id(ths)));
  }

private:
  constexpr tracker_id make_id(const void* t) {
    tracker_id t_id = highest_id++;
    ptr_id_map[ptr_id_count++] = std::make_pair(t, t_id);
    return t_id;
  }

  constexpr tracker_id get_id(const void* t) {
    for (std::size_t i = 0; i < ptr_id_count; ++i) {
      if (ptr_id_map[i].first == t) {
        return ptr_id_map[i].second;
      }
    }

    return tracker_id();
  }

  constexpr tracker_id get_or_make_id(const void* t) {
    tracker_id id = get_id(t);
    return id.is_valid() ? id : make_id(t);
  }
};

// forward declaration
struct tracker2;

struct tracker1 {
  context* ctx;

  constexpr tracker1(const tracker2&) = delete;
  constexpr tracker1(tracker2&&) = delete;

  constexpr tracker1& operator=(const tracker2&) = delete;
  constexpr tracker1& operator=(tracker2&&) = delete;

  constexpr explicit tracker1(context& context) : ctx(&context) {
    ctx->explicitly_construct(this);
  }

  constexpr tracker1(tracker1&& other) noexcept : ctx(other.ctx) {
    ctx->move_construct(this, &other);
  }

  constexpr tracker1(const tracker1& other) : ctx(other.ctx) {
    ctx->copy_construct(this, &other);
  }

  constexpr tracker1& operator=(tracker1&& other) noexcept {
    ctx = other.ctx;
    ctx->move_assign(this, &other);
    return *this;
  }

  constexpr tracker1& operator=(const tracker1& other) {
    ctx = other.ctx;
    ctx->copy_assign(this, &other);
    return *this;
  }

  constexpr ~tracker1() {
    ctx->destroy(this);
  }
};

struct tracker2 : public tracker1 {
  constexpr tracker2(const tracker1& other) = delete;
  constexpr tracker2(tracker1&& other) = delete;

  constexpr tracker2& operator=(const tracker1& other) = delete;
  constexpr tracker2& operator=(tracker1&& other) = delete;

  constexpr explicit tracker2(context& context) : tracker1(context) {}
};

template <typename Function>
constexpr event_log capture_events(Function&& function) {
  context ctx;
  function(ctx);
  return ctx.events;
}
} // namespace testing

#endif
