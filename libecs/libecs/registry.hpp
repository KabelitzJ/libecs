#ifndef LIBECS_REGISTRY_HPP_
#define LIBECS_REGISTRY_HPP_

#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <optional>
#include <algorithm>
#include <tuple>
#include <ranges>

#include <libecs/memory.hpp>
#include <libecs/entity.hpp>
#include <libecs/sparse_set.hpp>
#include <libecs/storage.hpp>
#include <libecs/view.hpp>
#include <libecs/component_handle.hpp>

namespace ecs {

template<typename To, typename From>
struct constness_as {
  using type = std::remove_const_t<To>;
};

template<typename To, typename From>
struct constness_as<To, const From> {
  using type = const To;
};

template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;

template<typename Entity, allocator_for<Entity> Allocator = std::allocator<Entity>>
class basic_registry {

  using allocator_traits = std::allocator_traits<Allocator>;

  static_assert(allocator_for<Allocator, Entity>, "Invalid allocator type");

  using entity_storage_type = std::vector<Entity, Allocator>;
  using free_list_type = std::vector<std::size_t, rebound_allocator_t<Allocator, std::size_t>>;

  using basic_storage_type = sparse_set<Entity, Allocator>;

  template<typename Type>
  using storage_type = constness_as_t<storage<Entity, std::remove_const_t<Type>, rebound_allocator_t<Allocator, std::remove_const_t<Type>>>, Type>;

public:

  using entity_type = Entity;
  using allocator_type = Allocator;
  using size_type = std::size_t;

  basic_registry() = default;

  basic_registry(const basic_registry&) = delete;

  basic_registry(basic_registry&& other) noexcept
  : _entities{std::move(other._entities)},
    _free_entities{std::move(other._free_entities)},
    _component_storages{std::move(other._component_storages)} { }

  ~basic_registry() {
    clear();
  }

  auto operator=(const basic_registry&) -> basic_registry& = delete;

  auto operator=(basic_registry&& other) noexcept -> basic_registry& {
    if (this != &other) {
      _entities = std::move(other._entities);
      _free_entities = std::move(other._free_entities);
      _component_storages = std::move(other._component_storages);
    }

    return *this;
  }

  auto clear() -> void {
    for (auto& [key, storage] : _component_storages) {
      storage->clear();
    }

    _entities.clear();
    _free_entities.clear();
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
    // [NOTE] : Clear out all components that are owned by this entity
    for (auto& [type, storage] : _component_storages) {
      storage->remove(entity);
    }

    auto index = static_cast<std::size_t>(entity._id());
    _free_entities.push_back(index);
    _entities.at(index)._increment_version();
  }

  auto is_valid_entity(const entity_type& entity) -> bool {
    auto index = static_cast<std::size_t>(entity._id());
    return index < _entities.size() && entity == _entities.at(index);
  }

  template<typename Component>
  auto has_component(const entity_type& entity) const -> bool {
    if (const auto storage = _try_get_storage<std::remove_const_t<Component>>(); storage) {
      return storage->get().contains(entity);
    }

    return false;
  }

  template<typename Component, typename... Args>
  auto add_component(const entity_type& entity, Args&&... args) -> component_handle<Component> {
    auto& storage = _get_or_create_storage<std::remove_const_t<Component>>();

    return storage.add(entity, std::forward<Args>(args)...);
  }

  template<typename Component>
  auto get_component(const entity_type& entity) const -> component_handle<const Component> {
    if (const auto component = try_get_component<std::remove_const_t<Component>>(entity); component) {
      return *component;
    }

    throw std::runtime_error{"Entity does not have component assigned to it"};
  }

  template<typename Component>
  auto get_component(const entity_type& entity) -> component_handle<Component> {
    if (auto component = try_get_component<std::remove_const_t<Component>>(entity); component) {
      return *component;
    }

    throw std::runtime_error{"Entity does not have component assigned to it"};
  }

  template<typename Component>
  auto try_get_component(const entity_type& entity) const -> component_handle<const Component> {
    if (const auto storage = _try_get_storage<std::remove_const_t<Component>>(); storage) {
      if (auto entry = storage->get().find(entity); entry != storage->get().cend()) {
        return *entry;
      }
    }

    return component_handle<const Component>{};
  }

  template<typename Component>
  auto try_get_component(const entity_type& entity) -> component_handle<Component> {
    if (auto storage = _try_get_storage<std::remove_const_t<Component>>(); storage) {
      if (auto entry = storage->get().find(entity); entry != storage->get().end()) {
        return *entry;
      }
    }

    return component_handle<Component>{};
  }

  template<typename... Components>
  auto create_view() -> basic_view<storage_type<Components>...> {
    return {_get_or_create_storage<std::remove_const_t<Components>>()...};
  }

  template<typename... Components>
  auto create_view() const -> basic_view<storage_type<const Components>...> {
    return {_get_or_create_storage<std::remove_const_t<Components>>()...};
  }

private:

  template<typename Component>
  auto _get_or_create_storage() -> storage_type<Component>& {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _component_storages.find(type); entry != _component_storages.end()) {
      return static_cast<storage_type<Component>&>(*entry->second);
    }

    auto entry = _component_storages.insert({type, std::make_unique<storage_type<Component>>()}).first;

    return static_cast<storage_type<Component>&>(*entry->second);
  }

  template<typename Component>
  auto _try_get_storage() const -> std::optional<std::reference_wrapper<const storage_type<Component>>> {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _component_storages.find(type); entry != _component_storages.cend()) {
      return std::cref(static_cast<storage_type<Component>&>(*entry->second));
    }

    return std::nullopt;
  }

  template<typename Component>
  auto _try_get_storage() -> std::optional<std::reference_wrapper<storage_type<Component>>> {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _component_storages.find(type); entry != _component_storages.end()) {
      return std::ref(static_cast<storage_type<Component>&>(*entry->second));
    }

    return std::nullopt;
  }

  entity_storage_type _entities;
  free_list_type _free_entities;

  std::unordered_map<std::type_index, std::unique_ptr<basic_storage_type>> _component_storages;

};

using registry = basic_registry<entity>;

} // namespace ecs

#endif // LIBECS_REGISTRY_HPP_
