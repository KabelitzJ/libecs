#include <chrono>
#include <cmath>
#include <concepts>
#include <condition_variable>
#include <cstring>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <numbers>
#include <queue>
#include <random>
#include <ranges>
#include <string_view>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <chrono>

#include <cstdio>
#include <cstdarg>

// #include <dlfcn.h>

// #include <fmt/format.h>
#include <fmt/printf.h>

#include <libecs/ecs.hpp>

// #include <basic/delegate.hpp>
// #include <basic/logger.hpp>
// #include <basic/sparse_set.hpp>
// #include <basic/taskgraph.hpp>

// #include <assets/scripts/camera_controller.hpp>
// #include <assets/scripts/player_controller.hpp>

#include <basic/log.hpp>

#include <yaml-cpp/yaml.h>

struct vector3 {
  std::float_t x;
  std::float_t y;
  std::float_t z;
}; // struct vector3

template<>
struct YAML::convert<vector3> {
  static auto encode(const vector3& vector) -> YAML::Node {
    auto node = YAML::Node{};

    node.SetStyle(YAML::EmitterStyle::Flow);

    node["x"] = vector.x;
    node["y"] = vector.y;
    node["z"] = vector.z;

    return node;
  }

  static auto decode(const YAML::Node& node, vector3& vector) {
    if (!node.IsMap() || node.size() != 3) {
      return false;
    }

    vector.x = node["x"].as<std::float_t>();
    vector.y = node["y"].as<std::float_t>();
    vector.z = node["z"].as<std::float_t>();

    return true;
  }
}; // struct YAML::convert

struct transform {
  vector3 position;
  vector3 rotation;
  vector3 scale;
}; // struct transform

template<>
struct YAML::convert<transform> {
  static auto encode(const transform& transform) -> YAML::Node {
    auto node = YAML::Node{};
    
    node["position"] = transform.position;
    node["rotation"] = transform.rotation;
    node["scale"] = transform.scale;

    return node;
  }

  static auto decode(const YAML::Node& node, transform& transform) {
    if (!node.IsMap() || node.size() != 3) {
      return false;
    }

    transform.position = node["position"].as<vector3>();
    transform.rotation = node["rotation"].as<vector3>();
    transform.scale = node["scale"].as<vector3>();

    return true;
  }
}; // struct YAML::convert

struct tag {
  std::string value;
}; // struct tag

template<>
struct YAML::convert<tag> {
  static auto encode(const tag& tag) -> YAML::Node {
    auto node = YAML::Node{};

    node = tag.value;

    return node;
  }

  static auto decode(const YAML::Node& node, tag& tag) {
    if (node.IsMap() || node.IsSequence()) {
      return false;
    }

    tag.value = node.as<std::string>();

    return true;
  }
}; // struct YAML::convert

struct rigidbody {
  bool is_active;
  vector3 velocity;

  rigidbody() : is_active{true} { }
}; // struct rigidbody

template<>
struct YAML::convert<rigidbody> {
  static auto encode(const rigidbody& rigidbody) -> YAML::Node {
    auto node = YAML::Node{};

    node["is_active"] = rigidbody.is_active;
    node["velocity"] = rigidbody.velocity;

    return node;
  }

  static auto decode(const YAML::Node& node, rigidbody& rigidbody) {
    if (!node.IsMap() || node.size() != 2) {
      return false;
    }

    rigidbody.is_active = node["is_active"].as<bool>();
    rigidbody.velocity = node["velocity"].as<vector3>();

    return true;
  }
}; // struct YAML::convert

auto main() -> int {
  auto registry = ecs::registry{};

  auto e1 = registry.create_entity();
  registry.add_component<tag>(e1, "entity1");
  registry.add_component<transform>(e1);
  registry.add_component<rigidbody>(e1);

  auto e2 = registry.create_entity();
  registry.add_component<tag>(e2, "entity2");
  registry.add_component<transform>(e2);
  registry.add_component<rigidbody>(e2);

  auto e3 = registry.create_entity();
  registry.add_component<tag>(e3, "entity3");
  registry.add_component<transform>(e3);
  registry.add_component<rigidbody>(e3);

  auto v = registry.create_view<const tag, const transform, const rigidbody>();

  auto scene = YAML::Node{};

  for (auto [entity, tag, transform, rigidbody] : v.each()) {
    auto node = YAML::Node{};

    node["tag"] = tag;
    node["transform"] = transform;
    node["rigidbody"] = rigidbody;

    scene["entities"].push_back(node);
  }

  auto emitter = YAML::Emitter{};

  emitter << scene;

  fmt::print("{}\n", emitter.c_str());

  auto file = std::ofstream{"/home/jxk10011/development/libecs/examples/basic/basic/player.yaml"};

  file.write(emitter.c_str(), emitter.size());

  return 0;
}













