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

#include <libecs/ecs.hpp>

#include <basic/delegate.hpp>

struct data {
  std::uint32_t value;
}; // struct data

auto free_fn(const std::uint32_t& value) -> void {
  std::cout << "free_fn: " << value << '\n';
}

class thread_pool {

public:

  thread_pool(std::size_t size = 1)
  : _is_finished{false} {
    for (auto i = 0u; i < size; ++i) {
      _threads.emplace_back([this](){ _run(); });
    }
  }

  ~thread_pool() {
    {
      auto lock = std::scoped_lock{_mutex}; // lock
      _is_finished = false;
    } // unlock

    _condition_variable.notify_all();

    for (auto& thread : _threads) {
      thread.join();
    }
  }

  template<typename Callable, typename Return, typename... Args>
  requires (std::is_invocable_r_v<Return, Callable, Args...>)
  [[nodiscard]] auto enqueue(Callable&& callable, Args&&... args) -> std::future<Return> {
    auto promise = std::make_shared<std::promise<Return>>();
    auto future = promise->get_future();

    {
      auto lock = std::scoped_lock{_mutex};

      _tasks.emplace_back([promise, callable = std::forward<Callable>(callable), ...args = std::forward<Args>(args)](){
        try {
          if constexpr (std::is_void_v<Return>) {
            std::invoke(callable, std::forward<Args>(args)...);
            promise->set_value();
          } else {
            promise->set_value(std::invoke(callable, std::forward<Args>(args)...));
          }
        } catch (...) {
          promise->set_exception(std::current_exception());
        }
      });
    }

    _condition_variable.notify_one();

    return future;
  }

private:

  auto _run() -> void {
    static auto thread_id = std::this_thread::get_id();

    {
      auto lock = std::scoped_lock{_mutex};
      std::cout << "ThreadId: " << thread_id << "\n";
    }

    while (!_is_finished) {
      auto lock = std::unique_lock{_mutex};

      _condition_variable.wait(lock, [this](){ return !_tasks.empty() || _is_finished; });

      if (_is_finished) {
        break;
      }

      auto task = std::move(_tasks.back());
      _tasks.pop_back();

      lock.unlock();

      std::invoke(task);
    }
  }

  std::vector<std::thread> _threads;
  std::condition_variable _condition_variable;
  std::mutex _mutex;
  bool _is_finished;
  std::vector<std::function<void()>> _tasks{};

}; // class thread_pool

struct handler {
  auto method(const std::uint32_t& value) -> void {
    std::cout << "method: " << value << '\n';
  }

  auto const_method(const std::uint32_t& value) const -> void {
    std::cout << "const_method: " << value << '\n';
  }

  static auto static_method(const std::uint32_t& value) -> void {
    std::cout << "static_method: " << value << '\n';
  }

  auto operator()(const std::uint32_t& value) -> void {
    std::cout << "operator(): " << value << '\n';
  }

  auto operator()(const std::uint32_t& value) const -> void {
    std::cout << "const_operator(): " << value << '\n';
  }
}; // struct handler

auto main() -> int {
  auto instance = handler{};
  const auto const_instance = handler{};

  auto lambda = ecs::delegate<void(const std::uint32_t&)>{[](const std::uint32_t& value){ std::cout << "lambda: " << value << '\n'; }};
  lambda(42u);

  auto method = ecs::delegate<void(const std::uint32_t&)>{[&instance](const std::uint32_t& value){ instance.method(value); }};
  method(42u);

  auto const_method = ecs::delegate<void(const std::uint32_t&)>{[&const_instance](const std::uint32_t& value){ const_instance.const_method(value); }};
  const_method(42u);

  auto free = ecs::delegate<void(const std::uint32_t&)>{free_fn};
  free(42u);

  auto static_method = ecs::delegate<void(const std::uint32_t&)>{&handler::static_method};
  static_method(42u);

  auto op = ecs::delegate<void(const std::uint32_t&)>{instance};
  op(42u);


  auto registry = ecs::registry{};

  auto player = registry.create_entity();

  registry.add_component<data>(player, 42u);

  auto& component = registry.get_component<data>(player);
  
  component.value = 69u;

  std::cout << registry.get_component<data>(player).value << '\n';

  return 0;
}
