#ifndef LIBECS_REGISTRY_HPP_
#define LIBECS_REGISTRY_HPP_

#include <vector>
#include <memory>

#include <libecs/entity.hpp>

namespace ecs {

template<typename Entity, typename Allocator = std::allocator<Entity>>
class basic_registry {

  using allocator_traits = std::allocator_traits<Allocator>;

  template<typename Type>
  using rebound_allocator = typename allocator_traits::rebind_alloc<Type>;

  static_assert(std::is_same_v<typename allocator_traits::value_type, Entity>, "");

  using entity_storage_type = std::vector<Entity, Allocator>;
  using free_list_type = std::vector<std::size_t, rebound_allocator<std::size_t>>;

public:

  using entity_type = Entity;
  using allocator_type = Allocator;

  basic_registry() {

  }

  ~basic_registry() {

  }

  auto create_entity() -> entity_type {
    if (!_free_entities.empty()) {
      auto index = _free_entities.back();
      _free_entities.pop_back();

      return _entities.at(index);
    }

    const auto id = static_cast<entity_type::id_type>(_entities.size());

    auto new_entity = entity{id, static_cast<entity_type::version_type>(0)};

    _entities.push_back(new_entity);
    return new_entity;
  }

  auto destroy_entity(const entity_type& entity) -> void {
    auto index = static_cast<std::size_t>(entity._id());
    _free_entities.push_back(index);
    _entities.at(index)._increment_version();
  }

  auto is_valid_entity(const entity_type& entity) -> bool {
    auto index = static_cast<std::size_t>(entity._id());
    return index < _entities.size() && entity == _entities.at(index);
  }

private:

  entity_storage_type _entities;
  free_list_type _free_entities;

};

using registry = basic_registry<entity>;

} // namespace ecs

#endif // LIBECS_REGISTRY_HPP_
