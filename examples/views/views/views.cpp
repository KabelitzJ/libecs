#include <iostream>

#include <fmt/format.h>

#include <libecs/ecs.hpp>

struct data {
  std::uint32_t value;
};

auto main() -> int {
  auto registry = ecs::registry{};

  for (auto i : std::ranges::iota_view(0u, 10u)) {
    auto entity = registry.create_entity();
    registry.add_component<data>(entity, i);
  }

  auto view = registry.create_view<data>();

  for (const auto& entity : view) {
    auto& data_component = view.get<data>(entity);
    fmt::print("{}", data_component.value);
  }

  return 0;
}
