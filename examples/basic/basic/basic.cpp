#include <iostream>
#include <memory>
#include <concepts>
#include <type_traits>
#include <cmath>
#include <limits>
#include <ranges>
#include <cstring>
#include <forward_list>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <future>
#include <chrono>
#include <numbers>
#include <random>
#include <string>
#include <string_view>
#include <filesystem>
#include <unordered_set>
#include <fstream>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <libecs/ecs.hpp>

#include <basic/delegate.hpp>
#include <basic/logger.hpp>

#include <assets/scripts/camera_controller.hpp>
#include <assets/scripts/player_controller.hpp>

auto main() -> int {
  auto scene = ecs::scene{};

  auto n1 = scene.create_node(ecs::vector3{1.0, 2.0, 3.0});
  n1.add_component<std::uint32_t>(std::uint32_t{1u});

  auto n2 = scene.create_node();
  // n2.add_component<std::uint32_t>(2u);

  auto n3 = scene.create_node();
  n3.add_component<std::uint32_t>(std::uint32_t{3u});

  auto view1 = scene.create_view<const std::uint32_t, const ecs::vector3>();

  for (const auto& entity : view1) {
    const auto& i = view1.get<const std::uint32_t>(entity);
    const auto& v = view1.get<const ecs::vector3>(entity);

    fmt::print("{} : [{} {} {}]\n", i, v.x, v.y, v.z);
  }

  fmt::print("\n");

  auto view2 = scene.create_view<const ecs::vector3>();

  for (const auto& entity : view2) {
    const auto& v = view2.get<const ecs::vector3>(entity);

    fmt::print("[{} {} {}]\n", v.x, v.y, v.z);
  }

  fmt::print("\n");

  auto camera = scene.create_node();
  camera.add_script<scripts::camera_controller>();
  
  auto player = scene.create_node();
  player.add_script<scripts::player_controller>();

  scene.initialize();
  scene.update(0.1f);
  scene.terminate();

  return 0;
}













