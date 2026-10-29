#include "runtime/public/objc3_task.hpp"

#include <array>
#include <barrier>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace {
void cross_release(unsigned count) {
  std::array<objc3_executor_t *, 3> workers{};
  for (unsigned i = 0; i != count; ++i)
    assert(objc3_executor_create(0, &workers[i]) == OBJC3_SERVICE_OK);
  std::barrier gate(static_cast<std::ptrdiff_t>(count));
  std::mutex mutex;
  std::condition_variable completed;
  unsigned done = 0;
  struct Context {
    objc3_executor_t *target;
    std::barrier<> *gate;
    std::mutex *mutex;
    std::condition_variable *completed;
    unsigned *done;
  };
  std::array<Context, 3> contexts;
  for (unsigned i = 0; i != count; ++i) {
    contexts[i] = {workers[(i + 1) % count], &gate, &mutex, &completed, &done};
    assert(objc3_executor_enqueue(workers[i], [](void *opaque) {
      auto &context = *static_cast<Context *>(opaque);
      context.gate->arrive_and_wait();
      objc3_executor_release(context.target);
      // The notification and final context access occur while holding the
      // mutex, so the waiting host cannot destroy contexts before return.
      std::lock_guard lock(*context.mutex);
      ++*context.done;
      context.completed->notify_one();
    }, &contexts[i]) == OBJC3_SERVICE_OK);
  }
  std::unique_lock lock(mutex);
  assert(completed.wait_for(lock, std::chrono::seconds(5), [&] { return done == count; }));
}

struct Value {
  int number;
  static inline std::function<void()> hook;
  static inline const Value *payload = nullptr;
  static inline bool fail = false;
  static inline std::vector<int> events;
  explicit Value(int value) : number(value) {}
  Value(const Value &source) {
    if (hook) {
      events.push_back(1);
      auto action = std::move(hook);
      hook = {};
      action();
      // This read must happen before destruction of the task-owned source.
      number = source.number;
      assert(number == 42);
      events.push_back(2);
      if (fail) throw std::runtime_error("reentrant result copy");
    } else number = source.number;
  }
  ~Value() { if (this == payload) events.push_back(3); }
};
objc3::task_handle<Value> completed_value(objc3_executor_t *executor) {
  auto *frame = new Value(42);
  objc3_task_start_options options{sizeof(options), OBJC3_SERVICE_ABI_VERSION,
      executor, frame, [](objc3_task_t *task, void *context) {
        objc3_completion completion{sizeof(completion), OBJC3_SERVICE_ABI_VERSION,
            OBJC3_COMPLETION_SUCCESS, 1, context, objc3::task_detail::result_ops<Value>(),
            nullptr, objc3::task_detail::result_type<Value>()};
        assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
      }, [](void *context) { delete static_cast<Value *>(context); }, nullptr, nullptr,
      nullptr, objc3::task_detail::result_type<Value>()};
  objc3_task_t *raw = nullptr;
  assert(objc3_task_start(&options, &raw) == OBJC3_SERVICE_OK);
  uint32_t ran = 0;
  assert(objc3_executor_pump(executor, 1, &ran) == OBJC3_SERVICE_OK && ran == 1);
  return objc3::task_handle<Value>::adopt_native(&raw);
}
void reentrant_result() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  for (bool throws : {false, true}) {
    auto handle = completed_value(executor);
    objc3_completion completion{};
    assert(objc3_task_get_completion(handle.native_handle(), &completion) == OBJC3_SERVICE_OK);
    Value::payload = static_cast<const Value *>(completion.payload);
    Value::events.clear();
    Value::fail = throws;
    Value::hook = [&] { handle = {}; };
    bool caught = false;
    try {
      auto value = handle.result();
      assert(value.number == 42);
    } catch (const std::runtime_error &) { caught = true; }
    assert(caught == throws && !handle);
    assert((Value::events == std::vector<int>{1, 2, 3}));
    Value::payload = nullptr;
  }
  objc3_executor_release(executor);
}
void invalid_callable_frames() {
  unsigned resumed = 0, destroyed = 0;
  struct Context { unsigned *resumed, *destroyed; } context{&resumed, &destroyed};
  const objc3_async_frame valid{sizeof(objc3_async_frame), OBJC3_ASYNC_ABI_VERSION,
      &context, [](objc3_task_t *, void *opaque) { ++*static_cast<Context *>(opaque)->resumed; },
      [](void *opaque) { ++*static_cast<Context *>(opaque)->destroyed; },
      objc3::task_detail::result_type<int>()};
  struct TaskLike {
    objc3_async_frame frame;
    objc3_async_frame release_native() && { return frame; }
  };
  for (unsigned field = 0; field != 5; ++field) {
    auto invalid = valid;
    switch (field) {
    case 0: invalid.struct_size = 0; break;
    case 1: invalid.version = OBJC3_ASYNC_ABI_VERSION + 1; break;
    case 2: invalid.context = nullptr; break;
    case 3: invalid.resume = nullptr; break;
    case 4: invalid.destroy = nullptr; break;
    }
    const auto expected = objc3_async_frame_validate(&invalid);
    assert(expected != OBJC3_SERVICE_OK);
    for (bool task_like : {false, true}) {
      bool rejected = false;
      try {
        if (task_like)
          (void)objc3::task_detail::own_callable_as<objc3::task<int>>([=] { return TaskLike{invalid}; });
        else
          (void)objc3::task_detail::own_callable_as<objc3::task<int>>([=] { return invalid; });
      } catch (const objc3::service_failure &failure) {
        assert(failure.status() == expected);
        rejected = true;
      }
      assert(rejected && resumed == 0 && destroyed == 0);
    }
  }
  { auto accepted = objc3::task_detail::own_callable_as<objc3::task<int>>([=] { return valid; }); }
  assert(resumed == 0 && destroyed == 1);
  bool mismatch = false;
  try {
    (void)objc3::task_detail::own_callable_as<objc3::task<void>>([=] { return valid; });
  } catch (const objc3::service_failure &failure) {
    assert(failure.status() == OBJC3_SERVICE_TYPE_MISMATCH);
    mismatch = true;
  }
  assert(mismatch && resumed == 0 && destroyed == 2);
}
} // namespace
int main() {
  cross_release(1);
  cross_release(2);
  cross_release(3);
  reentrant_result();
  invalid_callable_frames();
}
