#include <libecs/scene.hpp>

namespace ecs {

auto scene::initialize() -> void {
  auto view = _registry.create_view<script_handle>();

  for (auto& [entity, script] : view) {
    // (*script->on_create)(*this, entity);
    std::invoke(script->on_create, this, entity);
  }
}

auto scene::update(std::float_t delta_time) -> void {
  auto view = _registry.create_view<script_handle>();

  for (auto& [entity, script] : view) {
    (*script->on_update)(this, entity, delta_time);
  }
}

auto scene::terminate() -> void {
  auto view = _registry.create_view<script_handle>();

  for (auto& [entity, script] : view) {
    (*script->on_destroy)(this, entity);
  }
}

} // namespace ecs
