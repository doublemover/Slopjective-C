#include "runtime/public/objc3_task.hpp"

#include <array>
#include <barrier>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <pthread.h>
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
struct SuspendedFrame {
  objc3_task_t *dependency{};
  objc3_task_group_t *membership_group{};
  unsigned *destroyed{};
  unsigned *resumed{};
};
objc3_task_t *start_suspended(objc3_executor_t *executor, objc3_actor_t *actor,
                             SuspendedFrame *frame) {
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = actor ? nullptr : executor;
  options.actor = actor;
  options.frame = frame;
  options.resume = [](objc3_task_t *task, void *opaque) {
    auto *frame = static_cast<SuspendedFrame *>(opaque);
    ++*frame->resumed;
    if (frame->dependency)
      assert(objc3_task_await(task, frame->dependency) == OBJC3_SERVICE_OK);
    if (frame->membership_group) {
      objc3_service_status status = OBJC3_SERVICE_OK;
      assert(objc3_task_group_await(frame->membership_group, task, &status) ==
             OBJC3_SERVICE_INVALID_ARGUMENT);
    }
  };
  options.destroy = [](void *opaque) {
    auto *frame = static_cast<SuspendedFrame *>(opaque);
    ++*frame->destroyed;
    delete frame;
  };
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  return task;
}
void bounded_group_tree() {
  objc3_task_group_t *root = nullptr;
  assert(objc3_task_group_create(nullptr, &root) == OBJC3_SERVICE_OK);
  std::vector<objc3_task_group_t *> groups{root};
  for (unsigned depth = 1; depth < OBJC3_TASK_GROUP_MAX_DEPTH; ++depth) {
    objc3_task_group_t *child = nullptr;
    assert(objc3_task_group_create(groups.back(), &child) == OBJC3_SERVICE_OK);
    groups.push_back(child);
  }
  for (unsigned attempt = 0; attempt < 32; ++attempt) {
    auto *rejected = reinterpret_cast<objc3_task_group_t *>(1);
    assert(objc3_task_group_create(groups.back(), &rejected) == OBJC3_SERVICE_INVALID_ARGUMENT);
    assert(rejected == nullptr);
  }
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  unsigned destroyed = 0, resumed = 0;
  auto *leaf = start_suspended(executor, nullptr,
      new SuspendedFrame{nullptr, root, &destroyed, &resumed});
  assert(objc3_task_group_add(groups.back(), leaf) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(root) == OBJC3_SERVICE_OK);
  uint32_t ran = 0;
  assert(objc3_executor_pump(executor, 1, &ran) == OBJC3_SERVICE_OK && ran == 1);
  assert(objc3_task_group_cancel(root) == OBJC3_SERVICE_OK);
  assert(objc3_task_is_cancel_requested(leaf));
  assert(objc3_task_group_join(root) == OBJC3_SERVICE_NOT_READY);
  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(leaf, &cancelled) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(root) == OBJC3_SERVICE_OK);
  assert(destroyed == 1 && resumed == 1);
  objc3_task_release(leaf);
  // The parent association is the sole owner of every remaining child.
  for (unsigned i = 1; i < groups.size(); ++i) objc3_task_group_release(groups[i]);
  objc3_task_group_release(root);
  objc3_executor_release(executor);
}
void failed_wake_chain(bool closed_actor) {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  objc3_actor_t *actor = nullptr;
  if (closed_actor) assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  unsigned destroyed = 0, resumed = 0;
  std::vector<objc3_task_t *> tasks;
  for (unsigned i = 0; i < 4096; ++i) {
    tasks.push_back(start_suspended(executor, actor,
        new SuspendedFrame{tasks.empty() ? nullptr : tasks.back(), nullptr,
                           &destroyed, &resumed}));
    uint32_t ran = 0;
    assert(objc3_executor_pump(executor, 1, &ran) == OBJC3_SERVICE_OK && ran == 1);
  }
  unsigned queued = 0;
  if (closed_actor) assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  else for (unsigned i = 0; i < 1024; ++i)
    assert(objc3_executor_enqueue(executor, [](void *p) { ++*static_cast<unsigned *>(p); },
                                  &queued) == OBJC3_SERVICE_OK);
  objc3_completion success{};
  success.struct_size = sizeof(success);
  success.version = OBJC3_SERVICE_ABI_VERSION;
  success.kind = OBJC3_COMPLETION_SUCCESS;
  assert(objc3_task_complete(tasks.front(), &success) == OBJC3_SERVICE_OK);
  assert(resumed == tasks.size() && destroyed == tasks.size());
  for (unsigned i = 0; i < tasks.size(); ++i) {
    objc3_completion completion{};
    assert(objc3_task_get_completion(tasks[i], &completion) == OBJC3_SERVICE_OK);
    assert(completion.kind == (i == 0 ? OBJC3_COMPLETION_SUCCESS : OBJC3_COMPLETION_CANCELLED));
    objc3_task_release(tasks[i]);
  }
  if (!closed_actor) {
    uint32_t ran = 0;
    assert(objc3_executor_pump(executor, 1024, &ran) == OBJC3_SERVICE_OK && ran == 1024);
    assert(queued == 1024);
  }
  if (actor) { assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK); objc3_actor_release(actor); }
  objc3_executor_release(executor);
}
void bounded_stack_cleanup() {
  pthread_attr_t attributes;
  assert(pthread_attr_init(&attributes) == 0);
  assert(pthread_attr_setstacksize(&attributes, 512 * 1024) == 0);
  pthread_t thread;
  assert(pthread_create(&thread, &attributes, [](void *) -> void * {
    bounded_group_tree();
    failed_wake_chain(false);
    failed_wake_chain(true);
    return nullptr;
  }, nullptr) == 0);
  assert(pthread_attr_destroy(&attributes) == 0);
  assert(pthread_join(thread, nullptr) == 0);
}
} // namespace
int main() {
  cross_release(1);
  cross_release(2);
  cross_release(3);
  reentrant_result();
  invalid_callable_frames();
  bounded_stack_cleanup();
}
