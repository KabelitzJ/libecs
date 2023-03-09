#include <iostream>

#include <libecs/ecs.hpp>

class node {

public:

  node(std::shared_ptr<ecs::registry> registry)
  : _registry{registry},
    _entity{registry->create_entity()} { }

  ~node() {
    _registry->destroy_entity(_entity);
  }

  auto operator==(const node& other) const noexcept -> bool {
    return _entity == other._entity;
  }

private:

  std::shared_ptr<ecs::registry> _registry;
  ecs::entity _entity;

};

auto main() -> int {
  auto registry = std::make_shared<ecs::registry>();

  auto player = node{registry};
  auto monster = node{registry};

  if (player == monster) {
    std::cout << "The player is a monster :(\n";
  } else {
    std::cout << "The player is not a monster :)\n";
  }

  return 0;
}
