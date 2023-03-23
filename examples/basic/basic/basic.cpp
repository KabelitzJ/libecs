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
#include <fstream>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <libecs/ecs.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <basic/delegate.hpp>
#include <basic/logger.hpp>

#include <assets/scripts/camera_controller.hpp>

auto main() -> int {
  auto scene = ecs::scene{};

  auto camera = scene.create_node();

  camera.add_script<scripts::camera_controller>();

  scene.initialize();
  scene.update(0.1f);
  scene.terminate();

  return 0;
}













