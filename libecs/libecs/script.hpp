#ifndef LIBECS_SCRIPT_HPP_
#define LIBECS_SCRIPT_HPP_

#include <utility>
#include <cmath>
#include <memory>

#include <libecs/scene.hpp>

namespace ecs {

template<typename Derived>
class script {

  friend class scene;
  friend class scene::node;

public:

  script() = default;

  virtual ~script() = default;

  template<component Component>
  auto get_component() -> component_handle<Component> {
    return _node->get_component<Component>();
  }

private:

  enum class hook : std::uint8_t {
    on_create,
    on_destroy,
    on_update
  }; // enum class hooks

  auto _set_node(scene::node* node) -> void {
    _node = node;
  }

  auto _invoke_hook(...) -> void { }

  template<typename Target = Derived>
  auto _invoke_hook(std::integral_constant<hook, hook::on_create>) -> decltype(std::declval<Target>().on_create(), void()) {
    static_cast<Derived*>(this)->on_create();
  }

  template<typename Target = Derived>
  auto _invoke_hook(std::integral_constant<hook, hook::on_destroy>) -> decltype(std::declval<Target>().on_destroy(), void()) {
    static_cast<Derived*>(this)->on_destroy();
  }

  template<typename Target = Derived>
  auto _invoke_hook(std::integral_constant<hook, hook::on_update>, std::float_t delta_time) -> decltype(std::declval<Target>().on_update(delta_time), void()) {
    static_cast<Derived*>(this)->on_update(delta_time);
  }

  auto _create() -> void {
    _invoke_hook(std::integral_constant<hook, hook::on_create>{});
  }

  auto _destroy() -> void {
    _invoke_hook(std::integral_constant<hook, hook::on_destroy>{});
  }

  auto _update(std::float_t delta_time) -> void {
    _invoke_hook(std::integral_constant<hook, hook::on_update>{}, delta_time);
  }

  scene::node* _node;

}; // class script

} // namespace ecs

#endif // LIBECS_SCRIPT_HPP_
