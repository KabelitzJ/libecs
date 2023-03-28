#include <assets/scripts/camera_controller.hpp>

#include <iostream>
#include <cmath>

namespace scripts {

auto camera_controller::on_create() -> void {
  _position = get_component<ecs::vector3>();
}

auto camera_controller::on_update(std::float_t delta_time) -> void {
  std::cout << "camera_controller::on_update(" << delta_time << ") -> (" << _position->x << ", " << _position->y << ", " << _position->z << ")\n";
}

} // namespace scripts
