#ifndef BASIC_SCRIPTS_CAMERA_CONTROLLER_HPP_
#define BASIC_SCRIPTS_CAMERA_CONTROLLER_HPP_

#include <iostream>
#include <cmath>

#include <libecs/script.hpp>
#include <libecs/vector3.hpp>
#include <libecs/component_handle.hpp>

namespace scripts {

class camera_controller : public ecs::script<camera_controller> {

public:

  camera_controller() = default;

  auto on_create() -> void {
    std::cout << "camera_controller::on_create()\n";
    _position = get_component<ecs::vector3>();
  }

  auto on_destroy() -> void {
    std::cout << "camera_controller::on_destroy()\n";
  }

  auto on_update(std::float_t delta_time) -> void {
    std::cout << "camera_controller::on_update(" << delta_time << ")\n";
    std::cout << "(" << _position->x << ", " << _position->y << ", " << _position->z << ")\n";
  }

private:

  ecs::component_handle<ecs::vector3> _position{};

}; // class camera_controller

} // namespace scripts

#endif // BASIC_SCRIPTS_CAMERA_CONTROLLER_HPP_
