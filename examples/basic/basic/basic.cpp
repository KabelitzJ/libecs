#include <iostream>
#include <memory>
#include <concepts>
#include <type_traits>
#include <cmath>
#include <limits>
#include <ranges>
#include <cstring>
#include <forward_list>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <future>
#include <chrono>
#include <numbers>
#include <random>

#include <libecs/ecs.hpp>

#include <basic/delegate.hpp>

struct random {

  template<typename Type>
  requires ((std::is_integral_v<Type> && ! std::is_same_v<Type, bool>) || std::is_floating_point_v<Type>)
  static auto next(Type min = std::numeric_limits<Type>::min(), Type max = std::numeric_limits<Type>::max()) -> Type {
    using distribution_type = std::conditional_t<std::is_floating_point_v<Type>, std::uniform_real_distribution<Type>, std::uniform_int_distribution<Type>>;

    static auto device = std::random_device{};
    static auto engine = std::default_random_engine{device()};

    auto distribution = distribution_type{min, max};

    return distribution(engine);
  }

}; // class random

struct data {
  std::uint32_t value;
}; // struct data

auto free_fn(const std::uint32_t& value) -> void {
  std::cout << "free_fn: " << value << '\n';
}

struct handler {
  auto method(const std::uint32_t& value) -> void {
    std::cout << "method: " << value << '\n';
  }

  auto const_method(const std::uint32_t& value) const -> void {
    std::cout << "const_method: " << value << '\n';
  }

  static auto static_method(const std::uint32_t& value) -> void {
    std::cout << "static_method: " << value << '\n';
  }

  auto operator()(const std::uint32_t& value) -> void {
    std::cout << "operator(): " << value << '\n';
  }

  auto operator()(const std::uint32_t& value) const -> void {
    std::cout << "const_operator(): " << value << '\n';
  }
}; // struct handler

template<typename Class, typename Return, typename... Args>
auto wrap_method(Class* instance, Return(Class::*fn)(Args...)) {
  return [instance, fn](Args... args){ std::invoke(fn, instance, std::forward<Args>(args)...); };
}

template<typename Class, typename Return, typename... Args>
auto wrap_method(Class* instance, Return(Class::*fn)(Args...)const) {
  return [instance, fn](Args... args){ std::invoke(fn, instance, std::forward<Args>(args)...); };
}

template<typename Class, typename Return, typename... Args>
auto wrap_method(const Class* instance, Return(Class::*fn)(Args...)const) {
  return [instance, fn](Args... args){ std::invoke(fn, instance, std::forward<Args>(args)...); };
}

auto main() -> int {
  auto instance = handler{};
  const auto const_instance = handler{};

  auto method1 = ecs::delegate<void(const std::uint32_t&)>{instance, &handler::method};
  auto method2 = ecs::delegate<void(const std::uint32_t&)>{instance, &handler::const_method};
  auto const_method = ecs::delegate<void(const std::uint32_t&)>{const_instance, &handler::const_method};

  method1(42u);
  method2(42u);
  const_method(42u);

  return 0;
}
