#include <libecs/scene.hpp>

namespace ecs {

auto scene::initialize() -> void {
  auto view = _registry.create_view<script_handle>();

  for (const auto& entity : view) {
    auto& script = view.get<script_handle>(entity);
    std::invoke(script.on_create, this, entity);
  }
}

auto scene::update(std::float_t delta_time) -> void {
  auto view = _registry.create_view<script_handle>();

  for (const auto& entity : view) {
    auto& script = view.get<script_handle>(entity);
    std::invoke(script.on_update, this, entity, delta_time);
  }
}

auto scene::terminate() -> void {
  auto view = _registry.create_view<script_handle>();

  for (const auto& entity : view) {
    auto& script = view.get<script_handle>(entity);
    std::invoke(script.on_destroy, this, entity);
  }
}

} // namespace ecs
