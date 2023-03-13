#include <iostream>

#include <libecs/ecs.hpp>

struct data {
  std::uint32_t value;
}; // struct data

auto main() -> int {
  auto registry = ecs::registry{};

  auto player = registry.create_entity();

  registry.add_component<data>(player, 42u);

  auto& component = registry.get_component<data>(player);

  component.value = 69;

  std::cout << registry.get_component<data>(player).value << '\n';

  return 0;
}
