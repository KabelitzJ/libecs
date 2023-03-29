#ifndef LIBECS_SCENE_HPP_
#define LIBECS_SCENE_HPP_

#include <memory>
#include <typeindex>
#include <cmath>

#include <libecs/entity.hpp>
#include <libecs/registry.hpp>
#include <libecs/vector3.hpp>

namespace ecs {

template<typename>
class script;

class scene {

  struct script_handle {
    using instance_type = std::unique_ptr<void, void(*)(void*)>;
    using on_create_fn_type = void(*)(scene*, const entity&);
    using on_destroy_fn_type = void(*)(scene*, const entity&);
    using on_update_fn_type = void(*)(scene*, const entity&, std::float_t);

    instance_type instance;
    on_create_fn_type on_create;
    on_destroy_fn_type on_destroy;
    on_update_fn_type on_update;
  }; // struct script_handle

public:

  class node {

    friend class scene;

  public:

    ~node() {
      _scene->_registry.destroy_entity(_entity);
    }

    template<typename Component>
    auto get_component() -> component_handle<Component>;

    template<typename Component, typename... Args>
    auto add_component(Args&&... args) -> component_handle<Component>;

    template<typename Type, typename... Args>
    requires (std::is_base_of_v<script<Type>, Type> && std::is_constructible_v<Type, Args...>)
    auto add_script(Args&&... args) -> void;

  private:

    node(scene* scene)
    : _scene{scene}, _entity{scene->_registry.create_entity()} { }

    scene* _scene;
    ecs::entity _entity{null_entity};

  }; // class node

  auto create_node(const vector3& position = {0.0f, 0.0f, 0.0f}) -> node {
    auto new_node = node{this};
    new_node.add_component<vector3>(position);

    return new_node;
  }

  template<typename... Components>
  auto create_view() -> decltype(auto);

  auto initialize() -> void;

  auto update(std::float_t delta_time) -> void;

  auto terminate() -> void;

private:

  template<typename Type>
  static auto _on_create(scene* scene, const entity& entity) -> void;

  template<typename Type>
  static auto _on_destroy(scene* scene, const entity& entity) -> void;

  template<typename Type>
  static auto _on_update(scene* scene, const entity& entity, std::float_t delta_time) -> void;

  ecs::registry _registry{};

}; // class scene

} // namespace ecs

#include <libecs/scene.ipp>

#endif // LIBECS_SCENE_HPP_
