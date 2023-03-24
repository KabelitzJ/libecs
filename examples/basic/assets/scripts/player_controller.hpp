#ifndef BASIC_SCRIPTS_PLAYER_CONTROLLER_HPP_
#define BASIC_SCRIPTS_PLAYER_CONTROLLER_HPP_

#include <iostream>
#include <cmath>

#include <libecs/script.hpp>
#include <libecs/vector3.hpp>
#include <libecs/component_handle.hpp>

namespace scripts {

class player_controller : public ecs::script<player_controller> {

public:

  auto on_create() -> void {
    _position = get_component<ecs::vector3>();
  }

  auto on_update(std::float_t delta_time) -> void {
    std::cout << "player_controller::on_update(" << delta_time << ")\n";
    std::cout << "(" << _position->x << ", " << _position->y << ", " << _position->z << ")\n";
  }

private:

  ecs::component_handle<ecs::vector3> _position{};

}; // class player_controller

} // namespace scripts

#endif // BASIC_SCRIPTS_PLAYER_CONTROLLER_HPP_
