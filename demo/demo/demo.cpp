#include <iostream>
#include <vector>
#include <memory>
#include <bitset>
#include <functional>
#include <algorithm>
#include <cstring>

#include <libecs/ecs.hpp>

struct data {
  std::uint32_t value;
};

auto main() -> int {

  auto registry = ecs::registry{};

  auto e1 = registry.create_entity();
  auto e2 = registry.create_entity();

  std::cout << std::boolalpha << registry.has_component<data>(e1) << '\n';

  registry.add_component<data>(e1, std::uint32_t{42});

  std::cout << std::boolalpha << registry.has_component<data>(e1) << '\n';

  auto& d1 = registry.get_component<data>(e1);
  const auto& d2 = registry.get_component<data>(e1);

  std::cout << d1.value << '\n';
  std::cout << d2.value << '\n';

  d1.value = 69;

  std::cout << registry.get_component<data>(e1).value << '\n';

  return 0;
}
