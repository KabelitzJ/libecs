#include <libecs/scene.hpp>

#include <libecs/script.hpp>

namespace ecs {

template<component Component>
auto scene::node::get_component() -> component_handle<Component> {
  return _scene->_registry.get_component<Component>(_entity);
}

template<typename Type, typename... Args>
requires (std::is_base_of_v<script<Type>, Type> && std::is_constructible_v<Type, Args...>)
auto scene::node::add_script(Args&&... args) -> void {
  auto* script = new Type{std::forward<Args>(args)...};
  script->_set_node(this);

  auto handle = script_handle{
    .instance = script_handle::instance_type{script, [](void* ptr){ delete static_cast<Type*>(ptr); }},
    .on_create = [](scene& scene, const entity& entity){ scene::_on_create<Type>(scene, entity); },
    .on_destroy = [](scene& scene, const entity& entity){ scene::_on_destroy<Type>(scene, entity); },
    .on_update = [](scene& scene, const entity& entity, std::float_t delta_time){ scene::_on_update<Type>(scene, entity, delta_time); }
  };

  _scene->_registry.add_component<script_handle>(_entity, std::move(handle));
}

template<typename Type>
auto scene::_on_create(scene& scene, const entity& entity) -> void {
  auto* script = static_cast<Type*>(scene._registry.get_component<script_handle>(entity)->instance.get());
  script->_create();
}

template<typename Type>
auto scene::_on_destroy(scene& scene, const entity& entity) -> void {
  auto* script = static_cast<Type*>(scene._registry.get_component<script_handle>(entity)->instance.get());
  script->_destroy();
}

template<typename Type>
auto scene::_on_update(scene& scene, const entity& entity, std::float_t delta_time) -> void {
  auto* script = static_cast<Type*>(scene._registry.get_component<script_handle>(entity)->instance.get());
  script->_update(delta_time);
}

} // namespace ecs
