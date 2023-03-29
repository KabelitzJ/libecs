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
#include <variant>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <libecs/ecs.hpp>

#include <basic/delegate.hpp>
#include <basic/logger.hpp>

#include <assets/scripts/camera_controller.hpp>
#include <assets/scripts/player_controller.hpp>

class node;

class graph {

  friend class node;
  friend class flow_builder;

public:

  graph() = default;

  graph(const graph&) = delete;

  graph(graph&& other) noexcept;

  ~graph();

  auto operator=(const graph& ) -> graph& = delete;

  auto operator=(graph&& other) noexcept -> graph&;

  auto is_empty() const noexcept -> bool;

  auto size() -> std::size_t;

  auto clear() -> void;

private:

  auto _clear() -> void;

  auto _erase(node* node) -> void;

  template<typename ...Args>
  auto _emplace_back(Args&&... args) -> node*;

  auto _emplace_back() -> node*;

  std::vector<node*> _nodes;

}; // class graph

class node {

  friend class task;
  friend class flow_builder;

  struct fixed {
    template<typename C>
    fixed(C&&);

    std::variant<std::function<void()>> work;
  }; // struct fixed

  struct dynamic {
    template<typename C>
    dynamic(C&&);

    std::variant<std::function<void()>> work;
  }; // struct dynamic

  struct condition {
    template<typename C>
    condition(C&&);

    std::variant<std::function<int()>> work;
  }; // struct condition


  using handle_type = std::variant<std::monostate, fixed, dynamic, condition>;

public:

  template<typename... Args>
  node(Args&&... args);

  ~node() = default;

  auto name() const noexcept -> const std::string&;

private:

  auto _precede(node* node) -> void;

  std::string _name;
  handle_type _handle;
  node* _parent;
  std::vector<node*> _successors;
  std::vector<node*> _dependents;

}; // class node

template<typename Callable>
constexpr auto is_fixed_task_v = std::is_invocable_r_v<void, Callable>;

template<typename Callable>
constexpr auto is_dynamic_task = std::is_invocable_r_v<void, Callable> && !is_fixed_task_v<Callable>;

template<typename Callable>
constexpr auto is_condition_task = std::is_invocable_r_v<int, Callable> && !is_dynamic_task<Callable>;

template<typename... Types>
struct dependent_false {
  inline static constexpr auto value = false;
}; // struct dependent_false

template<typename... Types>
constexpr auto dependent_false_v = dependent_false<Types...>::value;

class task {

  friend class node;
  friend class flow_builder;

public:

  task() = default;

  ~task();

  auto name() const noexcept -> const std::string&;

  template<typename Callable>
  auto work(Callable&& callable) -> task&;

  template<typename... Tasks>
  auto precede(Tasks&&... tasks) -> task&;

  template<typename... Tasks>
  auto succeed(Tasks&&... tasks) -> task&;

  template <typename Visitor>
  requires (std::is_invocable_r_v<void, Visitor, node*>)
  auto for_each_successor(Visitor&& visitor) const -> void;

  template <typename Visitor>
  requires (std::is_invocable_r_v<void, Visitor, node*>)
  auto for_each_dependent(Visitor&& visitor) const -> void;

private:

  task(node* node);

  node* _node;

}; // class task

class flow_builder {

  friend class task;

public:

  flow_builder(graph* graph);

  template<typename Callable>
  requires (is_fixed_task_v<Callable>)
  auto emplace(Callable&& callable) -> task;

  template<typename Callable>
  requires (is_dynamic_task<Callable>)
  auto emplace(Callable&& callable) -> task;

  template<typename Callable>
  requires (is_condition_task<Callable>)
  auto emplace(Callable&& callable) -> task;

  template<typename... Callables>
  requires (sizeof...(Callables) > 1)
  auto emplace(Callables&&... callables) -> decltype(auto) {
    return std::make_tuple(emplace(std::forward<Callables>(callables))...);
  }

  auto erase(const task& task) -> void;

private:

  graph* _graph;

}; // class flow_builder

class task_flow : public flow_builder {

public:

  task_flow(const std::string& name);

  auto name() const noexcept -> const std::string&;

  auto clear() -> void;

  auto is_empty() const noexcept -> bool;

private:

  std::string _name;
  graph _graph;

}; // class task_flow

template<typename C>
node::fixed::fixed(C&& c) : work{std::forward<C>(c)} { }

template<typename C>
node::dynamic::dynamic(C&& c) : work{std::forward<C>(c)} { }

template<typename C>
node::condition::condition(C&& c) : work{std::forward<C>(c)} { }

template<typename... Args>
node::node(Args&&... args) : _handle{std::forward<Args>(args)...} { }

auto node::name() const noexcept -> const std::string& {
  return _name;
}

auto node::_precede(node* node) -> void {
  _successors.push_back(node);
  node->_dependents.push_back(this);
}

graph::graph(graph&& other) noexcept
: _nodes{std::move(other._nodes)} {

}

graph::~graph() {
  _clear();
}

auto graph::operator=(graph&& other) noexcept -> graph& {
  _clear();
  _nodes = std::move(other._nodes);
  return *this;
}

auto graph::is_empty() const noexcept -> bool {
  return _nodes.empty();
}

auto graph::size() -> std::size_t {
  return _nodes.size();
}

auto graph::clear() -> void {
  _clear();
}

auto graph::_clear() -> void {
  for (auto node : _nodes) {
    delete node;
  }

  _nodes.clear();
}

auto graph::_erase(node* node) -> void {
  if (auto entry = std::find(_nodes.begin(), _nodes.end(), node); entry != _nodes.end()) {
    _nodes.erase(entry);
    delete node;
  }
}

template<typename ...Args>
auto graph::_emplace_back(Args&&... args) -> node* {
  _nodes.push_back(new node{std::forward<Args>(args)...});
  return _nodes.back();
}

auto graph::_emplace_back() -> node* {
  _nodes.push_back(new node{});
  return _nodes.back();
}

task::~task() {

}

auto task::name() const noexcept -> const std::string& {
  return _node->name();
}

template<typename Callable>
auto task::work(Callable&& callable) -> task& {
  if constexpr (is_fixed_task_v<Callable>) {
    _node->_handle.emplace<node::fixed>(std::forward<Callable>(callable));
  } else if constexpr (is_dynamic_task<Callable>) {
    _node->_handle.emplace<node::dynamic>(std::forward<Callable>(callable));
  } else if constexpr (is_condition_task<Callable>) {
    _node->_handle.emplace<node::condition>(std::forward<Callable>(callable));
  } else {
    static_assert(dependent_false_v<Callable>, "Invalid task type");
  }

  return *this;
}

template<typename... Tasks>
auto task::precede(Tasks&&... tasks) -> task& {
  (_node->_precede(tasks._node), ...);
  return *this;
}

template<typename... Tasks>
auto task::succeed(Tasks&&... tasks) -> task& {
  (_node->_precede(tasks._node), ...);
  return *this;
}

template <typename Visitor>
requires (std::is_invocable_r_v<void, Visitor, node*>)
auto task::for_each_successor(Visitor&& visitor) const -> void {
  for (auto* successor : _node->_successors) {
    std::invoke(visitor, successor);
  }
}

template <typename Visitor>
requires (std::is_invocable_r_v<void, Visitor, node*>)
auto task::for_each_dependent(Visitor&& visitor) const -> void {
  for (auto* dependent : _node->_dependents) {
    std::invoke(visitor, dependent);
  }
}

task::task(node* node)
: _node{node} { } 

flow_builder::flow_builder(graph* graph)
: _graph{graph} { }

template<typename Callable>
requires (is_fixed_task_v<Callable>)
auto flow_builder::emplace(Callable&& callable) -> task {
  return task{_graph->_emplace_back(std::in_place_type_t<node::fixed>{}, std::forward<Callable>(callable))};
}

template<typename Callable>
requires (is_dynamic_task<Callable>)
auto flow_builder::emplace(Callable&& callable) -> task {
  return task{_graph->_emplace_back(std::in_place_type_t<node::dynamic>{}, std::forward<Callable>(callable))};
}

template<typename Callable>
requires (is_condition_task<Callable>)
auto flow_builder::emplace(Callable&& callable) -> task {
  return task{_graph->_emplace_back(std::in_place_type_t<node::condition>{}, std::forward<Callable>(callable))};
}

auto flow_builder::erase(const task& task) -> void {
  if (!task._node) {
    return;
  }

  task.for_each_dependent([&](node* dependent){
    auto& successors = dependent->_successors;

    if(auto entry = std::find(successors.begin(), successors.end(), dependent); entry != successors.end()) {
      successors.erase(entry);
    }
  });

  task.for_each_successor([&](node* successor){
    auto& dependents = successor->_dependents;

    if(auto entry = std::find(dependents.begin(), dependents.end(), successor); entry != dependents.end()) {
      dependents.erase(entry);
    }
  });

  _graph->_erase(task._node);
}

task_flow::task_flow(const std::string& name)
: flow_builder{&_graph},
  _name{name} { }

auto task_flow::name() const noexcept -> const std::string& {
  return _name;
}

auto task_flow::clear() -> void {
  _graph.clear();
}

auto task_flow::is_empty() const noexcept -> bool {
  return _graph.is_empty();
}

auto main() -> int {
  auto flow = task_flow{"main"};

  fmt::print("{}\n", flow.name());
  fmt::print("{}\n", flow.is_empty());

  auto [a, b, c, d] = flow.emplace(
    [](){ fmt::print("Task a\n"); },
    [](){ fmt::print("Task b\n"); },
    [](){ fmt::print("Task c\n"); },
    [](){ fmt::print("Task d\n"); }
  );

  a.precede(b, c);
  d.succeed(b, c);

  // auto scene = ecs::scene{};

  // auto camera = scene.create_node();
  // camera.add_script<scripts::camera_controller>();
  
  // auto player = scene.create_node();
  // player.add_script<scripts::player_controller>();

  // scene.initialize();
  // scene.update(0.1f);
  // scene.terminate();

  return 0;
}













