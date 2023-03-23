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
  const auto type = std::type_index{typeid(Type)};

  auto* script = new Type{std::forward<Args>(args)...};
  script->_set_node(this);

  auto handle = script_handle{
    .instance = script_handle::instance_type{script, &scene::_deleter<Type>},
    .on_create = [](scene& scene, std::type_index type){ scene::_on_create<Type>(scene, type); },
    .on_destroy = [](scene& scene, std::type_index type){ scene::_on_destroy<Type>(scene, type); },
    .on_update = [](scene& scene, std::type_index type, std::float_t delta_time){ scene::_on_update<Type>(scene, type, delta_time); }
  };

  _scene->_scripts.insert({type, std::move(handle)});
}

template<typename Type>
auto scene::_on_create(scene& scene, std::type_index type) -> void {
  auto* script = static_cast<Type*>(scene._scripts.at(type).instance.get());
  script->_create();
}

template<typename Type>
auto scene::_on_destroy(scene& scene, std::type_index type) -> void {
  auto* script = static_cast<Type*>(scene._scripts.at(type).instance.get());
  script->_destroy();
}

template<typename Type>
auto scene::_on_update(scene& scene, std::type_index type, std::float_t delta_time) -> void {
  auto* script = static_cast<Type*>(scene._scripts.at(type).instance.get());
  script->_update(delta_time);
}

template<typename Type>
auto scene::_deleter(void* ptr) -> void {
  delete static_cast<Type*>(ptr);
}

} // namespace ecs
