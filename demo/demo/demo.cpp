#include <iostream>

#include <libecs/ecs.hpp>

auto main() -> int {
  auto registry = ecs::registry{};

  auto player = registry.create_entity();
  auto monster = registry.create_entity();

  if (player == monster) {
    std::cout << "The player is a monster :(\n";
  } else {
    std::cout << "The player is not a monster :)\n";
  }

  return 0;  
}
