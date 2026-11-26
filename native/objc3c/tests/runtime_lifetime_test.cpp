#include "runtime/public/objc3_task.hpp"

#include <algorithm>
#include <array>
#include <barrier>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <mutex>
#include <pthread.h>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {
const objc3_completion success{sizeof(success), OBJC3_SERVICE_ABI_VERSION,
    OBJC3_COMPLETION_SUCCESS, 0, nullptr, nullptr, nullptr, nullptr};

void cross_release(unsigned count) {
  std::array<objc3_executor_t *, 3> workers{};
  for (unsigned i = 0; i != count; ++i)
    assert(objc3_executor_create(0, &workers[i]) == OBJC3_SERVICE_OK);
  std::barrier gate(static_cast<std::ptrdiff_t>(count + 1));
  std::mutex mutex;
  std::condition_variable completed;
  unsigned done = 0, drained = 0;
  struct Context {
    objc3_executor_t *target;
    std::barrier<> *gate;
    std::mutex *mutex;
    std::condition_variable *completed;
    unsigned *done;
    unsigned *drained;
  };
  std::array<Context, 3> contexts;
  for (unsigned i = 0; i != count; ++i) {
    contexts[i] = {workers[(i + 1) % count], &gate, &mutex, &completed, &done, &drained};
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
    assert(objc3_executor_enqueue(workers[i], [](void *opaque) {
      auto &context = *static_cast<Context *>(opaque);
      std::lock_guard lock(*context.mutex);
      ++*context.drained;
      context.completed->notify_one();
    }, &contexts[i]) == OBJC3_SERVICE_OK);
  }
  gate.arrive_and_wait();
  std::unique_lock lock(mutex);
  assert(completed.wait_for(lock, std::chrono::seconds(5), [&] {
    return done == count && drained == count;
  }));
}

enum class ReleaseOwner { Direct, Frame, Actor, NamedBinding };

void manual_release(ReleaseOwner owner) {
  objc3_executor_t *manual = nullptr, *worker = nullptr;
  assert(objc3_executor_create(1, &manual) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
  std::mutex mutex;
  std::condition_variable changed;
  struct Gate {
    std::mutex &mutex;
    std::condition_variable &changed;
    bool open = false, drained = false;
  } gate{mutex, changed};
  assert(objc3_executor_enqueue(worker, [](void *context) {
    auto &gate = *static_cast<Gate *>(context);
    std::unique_lock lock(gate.mutex);
    assert(gate.changed.wait_for(lock, std::chrono::seconds(5), [&] { return gate.open; }));
    gate.drained = true;
    gate.changed.notify_one();
  }, &gate) == OBJC3_SERVICE_OK);
  objc3_task_t *task = nullptr;
  switch (owner) {
  case ReleaseOwner::Direct:
    assert(objc3_executor_enqueue(manual, [](void *context) {
      objc3_executor_release(static_cast<objc3_executor_t *>(context));
    }, worker) == OBJC3_SERVICE_OK);
    break;
  case ReleaseOwner::Frame: {
    objc3_task_start_options options{};
    options.struct_size = sizeof(options);
    options.version = OBJC3_SERVICE_ABI_VERSION;
    options.executor = manual;
    options.frame = worker;
    options.resume = [](objc3_task_t *current, void *) {
      assert(objc3_task_complete(current, &success) == OBJC3_SERVICE_OK);
    };
    options.destroy = [](void *context) {
      objc3_executor_release(static_cast<objc3_executor_t *>(context));
    };
    assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
    break;
  }
  case ReleaseOwner::Actor: {
    objc3_actor_t *actor = nullptr;
    assert(objc3_actor_create(worker, &actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
    objc3_executor_release(worker);
    assert(objc3_executor_enqueue(manual, [](void *context) {
      objc3_actor_release(static_cast<objc3_actor_t *>(context));
    }, actor) == OBJC3_SERVICE_OK);
    break;
  }
  case ReleaseOwner::NamedBinding:
    assert(objc3_executor_register_named("lifetime.worker", worker) == OBJC3_SERVICE_OK);
    objc3_executor_release(worker);
    assert(objc3_executor_enqueue(manual, [](void *) {
      assert(objc3_executor_unregister_named("lifetime.worker") == OBJC3_SERVICE_OK);
      objc3_executor_t *found = nullptr;
      assert(objc3_executor_get_named("lifetime.worker", &found) == OBJC3_SERVICE_CLOSED && !found);
    }, nullptr) == OBJC3_SERVICE_OK);
    break;
  }
  uint32_t executed = 0;
  assert(objc3_executor_pump(manual, 1, &executed) == OBJC3_SERVICE_OK && executed == 1);
  {
    std::unique_lock lock(mutex);
    assert(!gate.drained);
    gate.open = true;
    changed.notify_one();
    assert(changed.wait_for(lock, std::chrono::seconds(5), [&] { return gate.drained; }));
  }
  objc3_task_release(task);
  objc3_executor_release(manual);
}

void host_release() {
  objc3_executor_t *worker = nullptr;
  unsigned drained = 0;
  assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
  assert(objc3_executor_enqueue(worker, [](void *context) { ++*static_cast<unsigned *>(context); },
                                &drained) == OBJC3_SERVICE_OK);
  objc3_executor_release(worker);
  assert(drained == 1);
}

unsigned public_worker_capacity() {
  std::vector<objc3_executor_t *> workers;
  for (;;) {
    objc3_executor_t *worker = nullptr;
    const auto status = objc3_executor_create(0, &worker);
    if (status == OBJC3_SERVICE_NOT_READY) {
      assert(!worker);
      break;
    }
    assert(status == OBJC3_SERVICE_OK && worker && workers.size() < 32);
    workers.push_back(worker);
  }
  for (auto *worker : workers) objc3_executor_release(worker);
  return workers.size();
}

void wait_for_worker_capacity(unsigned expected) {
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  while (public_worker_capacity() != expected) {
    assert(std::chrono::steady_clock::now() < deadline);
    std::this_thread::yield();
  }
}

struct Value {
  int number;
  bool extracted = false;
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
      extracted = true;
    } else number = source.number;
  }
  ~Value() {
    if (this == payload) events.push_back(3);
    if (extracted) events.push_back(4);
  }
};
objc3::task_handle<Value> completed_value(objc3_executor_t *executor, int number = 42) {
  auto *frame = new Value(number);
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
  Value::events.reserve(4);
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
    assert(Value::events == (throws ? std::vector<int>{1, 2, 3} : std::vector<int>{1, 2, 3, 4}));
    Value::payload = nullptr;
  }
  Value::fail = false;
  for (bool another_owner : {false, true}) {
    auto handle = completed_value(executor);
    auto replacement = completed_value(executor, 17);
    auto explicit_owner = another_owner ? handle : objc3::task_handle<Value>{};
    objc3_completion completion{};
    assert(objc3_task_get_completion(handle.native_handle(), &completion) == OBJC3_SERVICE_OK);
    Value::payload = static_cast<const Value *>(completion.payload);
    Value::events.clear();
    Value::hook = [&] {
      handle = std::move(replacement);
      assert(handle.result().number == 17);
    };
    assert(handle.result().number == 42);
    assert(handle.result().number == 17 && !replacement);
    if (another_owner) {
      assert((Value::events == std::vector<int>{1, 2, 4}));
      explicit_owner = {};
    }
    assert(Value::events == (another_owner ? std::vector<int>{1, 2, 4, 3} : std::vector<int>{1, 2, 3, 4}));
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
  auto group = objc3::task_group_handle::create();
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
    auto rejected_with_status = [&](auto operation) {
      bool rejected = false;
      try {
        operation();
      } catch (const objc3::service_failure &failure) {
        assert(failure.status() == expected);
        rejected = true;
      }
      assert(rejected && resumed == 0 && destroyed == 0);
    };
    rejected_with_status([&] { (void)objc3::task<int>::from_native(&invalid); });
    rejected_with_status([&] { (void)group.spawn<int>([=] { return invalid; }); });
    rejected_with_status([&] {
      (void)objc3::task_detail::own_callable_as<objc3::task<int>>([=] { return TaskLike{invalid}; });
    });
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
  group.close();
  group.wait();
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
void bounded_group_tree(bool fail_at_leaf) {
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
  auto *sibling = start_suspended(executor, nullptr,
      new SuspendedFrame{nullptr, nullptr, &destroyed, &resumed});
  assert(objc3_task_group_add(root, sibling) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(root) == OBJC3_SERVICE_OK);
  uint32_t ran = 0;
  assert(objc3_executor_pump(executor, 2, &ran) == OBJC3_SERVICE_OK && ran == 2);
  assert(objc3_task_group_join(root) == OBJC3_SERVICE_NOT_READY);
  const objc3_completion cancelled{sizeof(cancelled), OBJC3_SERVICE_ABI_VERSION,
      OBJC3_COMPLETION_CANCELLED, 0, nullptr, nullptr, nullptr, nullptr};
  if (fail_at_leaf) {
    objc3_error_t *error = nullptr;
    assert(objc3_async_error_create_service(OBJC3_SERVICE_FAILED, &error) == OBJC3_SERVICE_OK);
    auto failed = cancelled;
    failed.kind = OBJC3_COMPLETION_ERROR;
    failed.error = error;
    assert(!objc3_task_is_cancel_requested(sibling));
    assert(objc3_task_complete(leaf, &failed) == OBJC3_SERVICE_OK);
    objc3_completion observed{};
    assert(objc3_task_get_completion(leaf, &observed) == OBJC3_SERVICE_OK && observed.error == error);
    objc3_error_release(error);
  } else {
    assert(objc3_task_group_cancel(root) == OBJC3_SERVICE_OK);
    assert(objc3_task_is_cancel_requested(leaf));
    assert(objc3_task_complete(leaf, &cancelled) == OBJC3_SERVICE_OK);
  }
  assert(objc3_task_is_cancel_requested(sibling));
  assert(objc3_task_group_join(root) == OBJC3_SERVICE_NOT_READY && destroyed == 1);
  assert(objc3_task_complete(sibling, &cancelled) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(root) == OBJC3_SERVICE_OK);
  assert(destroyed == 2 && resumed == 2);
  objc3_task_release(sibling);
  objc3_task_release(leaf);
  // Retain one midpoint in the failure case and verify both portions reclaim.
  auto *midpoint = fail_at_leaf ? objc3_task_group_retain(groups[groups.size() / 2]) : nullptr;
  for (unsigned i = 1; i < groups.size(); ++i) objc3_task_group_release(groups[i]);
  objc3_task_group_release(root);
  if (midpoint) {
    assert(objc3_task_group_join(midpoint) == OBJC3_SERVICE_OK);
    objc3_task_group_release(midpoint);
  }
  objc3_executor_release(executor);
}

struct WakeLedger {
  std::uintptr_t caller_frame;
  std::uintptr_t maximum_distance = 0;
  unsigned completed = 0;
};

void failed_wake_chain(bool closed_actor, unsigned length) {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  objc3_actor_t *actor = nullptr;
  if (closed_actor) assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  unsigned destroyed = 0, resumed = 0;
  WakeLedger ledger{reinterpret_cast<std::uintptr_t>(__builtin_frame_address(0))};
  std::vector<objc3_task_t *> tasks;
  tasks.reserve(length);
  for (unsigned i = 0; i < length; ++i) {
    tasks.push_back(start_suspended(executor, actor,
        new SuspendedFrame{tasks.empty() ? nullptr : tasks.back(), nullptr,
                           &destroyed, &resumed}));
    assert(objc3_task_add_waiter(tasks.back(), [](void *context, const objc3_completion *) {
      auto &ledger = *static_cast<WakeLedger *>(context);
      const auto address = reinterpret_cast<std::uintptr_t>(__builtin_frame_address(0));
      ledger.maximum_distance = std::max(ledger.maximum_distance,
          std::max(address, ledger.caller_frame) - std::min(address, ledger.caller_frame));
      ++ledger.completed;
    }, &ledger) == OBJC3_SERVICE_OK);
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
  assert(ledger.completed == tasks.size());
  assert(ledger.maximum_distance < 32 * 1024);
  assert(queued == 0);
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
    bounded_group_tree(false);
    bounded_group_tree(true);
    for (unsigned length : {2, 4096}) {
      failed_wake_chain(false, length);
      failed_wake_chain(true, length);
    }
    return nullptr;
  }, nullptr) == 0);
  assert(pthread_attr_destroy(&attributes) == 0);
  assert(pthread_join(thread, nullptr) == 0);
}
} // namespace
int main() {
  const auto initial_workers = public_worker_capacity();
  assert(initial_workers >= 3);
  cross_release(1);
  cross_release(2);
  cross_release(3);
  wait_for_worker_capacity(initial_workers);
  for (auto owner : {ReleaseOwner::Direct, ReleaseOwner::Frame, ReleaseOwner::Actor, ReleaseOwner::NamedBinding}) {
    manual_release(owner);
    wait_for_worker_capacity(initial_workers);
  }
  host_release();
  reentrant_result();
  invalid_callable_frames();
  bounded_stack_cleanup();
  std::puts("runtime lifetime: all focused checks passed");
}
