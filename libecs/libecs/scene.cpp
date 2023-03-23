#include <libecs/scene.hpp>

namespace ecs {

auto scene::initialize() -> void {
  for (auto& [type, script] : _scripts) {
    (*script.on_create)(*this, type);
  }
}

auto scene::update(std::float_t delta_time) -> void {
  for (auto& [type, script] : _scripts) {
    (*script.on_update)(*this, type, delta_time);
  }
}

auto scene::terminate() -> void {
  for (auto& [type, script] : _scripts) {
    (*script.on_destroy)(*this, type);
  }
}

} // namespace ecs
