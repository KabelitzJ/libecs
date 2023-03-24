#ifndef BASIC_SCRIPTS_CAMERA_CONTROLLER_HPP_
#define BASIC_SCRIPTS_CAMERA_CONTROLLER_HPP_

#include <libecs/script.hpp>
#include <libecs/vector3.hpp>
#include <libecs/component_handle.hpp>

namespace scripts {

auto special_function() -> void;

class camera_controller : public ecs::script<camera_controller> {

public:

  auto on_create() -> void;

  auto on_update(std::float_t delta_time) -> void;

private:

  ecs::component_handle<ecs::vector3> _position{};

}; // class camera_controller

} // namespace scripts

#endif // BASIC_SCRIPTS_CAMERA_CONTROLLER_HPP_
