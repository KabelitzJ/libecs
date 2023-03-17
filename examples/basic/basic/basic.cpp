#include <iostream>
#include <memory>
#include <concepts>
#include <type_traits>
#include <cmath>
#include <limits>
#include <ranges>
#include <cstring>
#include <forward_list>

#include <libecs/ecs.hpp>

#include <basic/delegate.hpp>

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

auto main() -> int {
  auto instance = handler{};
  const auto const_instance = handler{};

  auto lambda = ecs::delegate<void(const std::uint32_t&)>{[](const std::uint32_t& value){ std::cout << "lambda: " << value << '\n'; }};
  lambda(42u);

  auto method = ecs::delegate<void(const std::uint32_t&)>{[&instance](const std::uint32_t& value){ instance.method(value); }};
  method(42u);

  auto const_method = ecs::delegate<void(const std::uint32_t&)>{[&const_instance](const std::uint32_t& value){ const_instance.const_method(value); }};
  const_method(42u);

  auto free = ecs::delegate<void(const std::uint32_t&)>{free_fn};
  free(42u);

  auto static_method = ecs::delegate<void(const std::uint32_t&)>{&handler::static_method};
  static_method(42u);

  auto op = ecs::delegate<void(const std::uint32_t&)>{instance};
  op(42u);


  auto registry = ecs::registry{};

  auto player = registry.create_entity();

  registry.add_component<data>(player, 42u);

  auto& component = registry.get_component<data>(player);
  
  component.value = 69u;

  std::cout << registry.get_component<data>(player).value << '\n';

  return 0;
}
