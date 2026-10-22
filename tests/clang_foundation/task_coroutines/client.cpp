#include "provider.h"
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#ifdef __OBJC__
extern "C" id class_createInstance(Class, __SIZE_TYPE__)
    __attribute__((ns_returns_retained));
extern "C" id object_dispose(__unsafe_unretained id);
extern "C" void objc_release(__unsafe_unretained id);
extern "C" id objc_retain(__unsafe_unretained id);
extern "C" id objc_autorelease(__unsafe_unretained id);
static std::atomic<unsigned> object_destructions{0};
__attribute__((objc_root_class))
@interface CoroutineValue {
  Class isa;
@public
  int number;
}
+ (id)new;
+ (id)temporary;
@end
@implementation CoroutineValue
+ (id)new {
  return class_createInstance(self, 0);
}
+ (id)temporary {
#if __has_feature(objc_arc)
  return [self new];
#else
  return objc_autorelease([self new]);
#endif
}
- (void)dealloc {
  ++object_destructions;
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {
}
#endif
@end
#endif

namespace {
void pump(objc3_executor_t *executor, unsigned limit = 1) {
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, limit, &executed) == OBJC3_SERVICE_OK);
}
void noop(void *) {}
objc3_service_status failing_completion_copy(void *, const void *source) {
  // Reenter completion while the parent owns an in-progress payload copy.
  // The dependency wake must survive the parent copy's subsequent failure.
  pump(*static_cast<objc3_executor_t *const *>(source));
  return OBJC3_SERVICE_OUT_OF_MEMORY;
}
objc3_service_status unsupported_completion_move(void *, void *) {
  return OBJC3_SERVICE_INVALID_ARGUMENT;
}
struct raw_await_frame {
  objc3_task_t *dependency;
  unsigned resumes = 0;
};
void raw_await_resume(objc3_task_t *task, void *opaque) {
  auto &frame = *static_cast<raw_await_frame *>(opaque);
  if (frame.resumes++ == 0) {
    assert(objc3_task_await(task, task) == OBJC3_SERVICE_INVALID_ARGUMENT);
    assert(objc3_task_await(task, frame.dependency) == OBJC3_SERVICE_OK);
    assert(objc3_task_await(task, frame.dependency) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
  } else {
    objc3_completion completion{};
    assert(objc3_task_get_completion(frame.dependency, &completion) ==
           OBJC3_SERVICE_OK);
    assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  }
}
void pending(const objc3::task_handle<int> &task) {
  objc3_completion completion{};
  completion.version = 999;
  assert(objc3_task_get_completion(task.native_handle(), &completion) ==
         OBJC3_SERVICE_NOT_READY);
  assert(completion.version == 999);
}
template <class F> void cancelled(F function) {
  bool caught = false;
  try {
    function();
  } catch (const objc3::task_cancelled &) {
    caught = true;
  }
  assert(caught);
}
objc3::task<void> consume_failure(objc3::task_handle<std::string> child) {
  auto value = co_await child;
  (void)value;
  co_return;
}
objc3::task<void> consume_foreign(objc3::task_handle<void> child) {
  co_await child;
  co_return;
}
objc3::task<bool> boolean() { co_return false; }
objc3::task<void> empty() { co_return; }
struct alignas(256) aligned_value {
  int value;
};
objc3::task<aligned_value> aligned() { co_return aligned_value{91}; }
objc3::task<void> aligned_local() {
  aligned_value local{19};
  assert(reinterpret_cast<std::uintptr_t>(&local) % alignof(aligned_value) ==
         0);
  co_await objc3::task_yield;
  assert(local.value == 19);
  assert(reinterpret_cast<std::uintptr_t>(&local) % alignof(aligned_value) ==
         0);
}
objc3::task<std::string> text() { co_return std::string("owned result"); }
objc3::task<int> checkpoint(std::atomic<unsigned> *entered) {
  co_await objc3::task_checkpoint;
  ++*entered;
  co_return 1;
}
#ifdef __OBJC__
objc3::task<int> objective_values(objc3::task_handle<int> dependency) {
#if __has_feature(objc_arc)
  CoroutineValue *object = [CoroutineValue temporary];
#else
  CoroutineValue *object = objc_retain([CoroutineValue temporary]);
#endif
#if !__has_feature(objc_arc)
  struct owner {
    CoroutineValue *value;
    ~owner() { objc_release(value); }
  } guard{object};
#endif
  object->number = 84;
  int (^callback)(void) = ^{
    return object->number;
  };
  int value = co_await dependency;
  co_return callback() + value;
}
objc3::task<CoroutineValue *> objective_result() {
  CoroutineValue *value = [CoroutineValue new];
  value->number = 96;
  co_return value;
}
objc3::task<void> objective_exception() {
  CoroutineValue *value = [CoroutineValue new];
  value->number = 73;
#if !__has_feature(objc_arc)
  struct owner {
    CoroutineValue *value;
    ~owner() { objc_release(value); }
  } guard{value};
#endif
  co_await objc3::task_yield;
  @throw value;
}
#endif

void manual_contract() {
  objc3_executor_t *executor = nullptr, *other = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &other) == OBJC3_SERVICE_OK);
  {
    auto child = empty().start(other);
    raw_await_frame frame{child.native_handle()};
    objc3_task_start_options options{};
    options.struct_size = sizeof(options);
    options.version = OBJC3_SERVICE_ABI_VERSION;
    options.executor = executor;
    options.frame = &frame;
    options.resume = raw_await_resume;
    options.destroy = noop;
    objc3_task_t *parent = nullptr;
    assert(objc3_task_start(&options, &parent) == OBJC3_SERVICE_OK);
    pump(executor);
    assert(frame.resumes == 1);
    const objc3_value_ops failing_ops{sizeof(objc3_value_ops),
                                      OBJC3_VALUE_OPS_VERSION,
                                      sizeof(other),
                                      alignof(objc3_executor_t *),
                                      failing_completion_copy,
                                      unsupported_completion_move,
                                      noop};
    objc3_completion failed_copy{};
    failed_copy.struct_size = sizeof(failed_copy);
    failed_copy.version = OBJC3_SERVICE_ABI_VERSION;
    failed_copy.kind = OBJC3_COMPLETION_SUCCESS;
    failed_copy.has_payload = 1;
    failed_copy.payload = &other;
    failed_copy.payload_ops = &failing_ops;
    assert(objc3_task_complete(parent, &failed_copy) ==
           OBJC3_SERVICE_OUT_OF_MEMORY);
    pump(executor);
    assert(frame.resumes == 2);
    objc3_completion completion{};
    assert(objc3_task_get_completion(parent, &completion) == OBJC3_SERVICE_OK);
    assert(completion.kind == OBJC3_COMPLETION_SUCCESS &&
           !completion.has_payload);
    objc3_task_release(parent);
  }
  {
    auto child_state = std::make_shared<coroutine_state>();
    auto parent_state = std::make_shared<coroutine_state>();
    auto child = start_leaf(other, child_state);
    auto parent = start_parent(executor, child, parent_state);
    pump(other);
    pump(executor);
    assert(child_state->entered == 1 && parent_state->entered == 1);
    assert(parent_state->destroyed == 0 && parent_state->continued == 0);
    pending(parent);
    assert(objc3_task_await(parent.native_handle(), child.native_handle()) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    child_state->ready = true;
    pump(other);
    assert(child.result() == 0 && child_state->destroyed == 1);
    assert(parent_state->continued ==
           0); // No inline continuation on child executor.
    pump(executor);
    assert(parent.result() == 42 && parent_state->destroyed == 1);
    assert(parent_state->continued == 1);

    // Already completed awaits still schedule a distinct continuation.
    auto ready_state = std::make_shared<coroutine_state>();
    auto ready = start_parent(executor, child, ready_state);
    pump(executor);
    assert(ready_state->continued == 0);
    pump(executor);
    assert(ready.result() == 42 && ready_state->destroyed == 1);
  }
  {
    auto child_state = std::make_shared<coroutine_state>();
    auto state = std::make_shared<coroutine_state>();
    auto child = start_leaf(other, child_state);
    auto parent = start_parent(executor, child, state);
    pump(executor);
    parent.cancel();
    pump(executor);
    cancelled([&] { (void)parent.result(); });
    assert(state->continued == 0 && state->destroyed == 1);
    assert(!objc3_task_is_cancel_requested(child.native_handle()));
    child_state->ready = true;
    pump(other, 8);
    pump(executor, 8); // Stale child callback must not revive parent.
    assert(child.result() == 0 && state->destroyed == 1);
  }
  {
    auto child_state = std::make_shared<coroutine_state>();
    auto state = std::make_shared<coroutine_state>();
    auto child = start_leaf(other, child_state);
    {
      auto abandoned = start_parent(executor, child, state);
      pump(executor);
      assert(state->destroyed == 0);
    }
    assert(state->destroyed == 1); // No parent/child retain cycle.
    child_state->ready = true;
    pump(other, 8); // Token survives the freed frame and task record.
    pump(executor, 8);
    assert(state->continued == 0);
  }
  {
    auto child_state = std::make_shared<coroutine_state>();
    auto state = std::make_shared<coroutine_state>();
    auto child = start_leaf(other, child_state);
    auto parent = start_parent(executor, child, state);
    pump(executor);
    // Queue pressure cannot strand a cancelled suspension.
    for (unsigned i = 0; i != 1024; ++i)
      assert(objc3_executor_enqueue(executor, noop, nullptr) ==
             OBJC3_SERVICE_OK);
    parent.cancel();
    cancelled([&] { (void)parent.result(); });
    assert(state->destroyed == 1);
    pump(executor, 2048);
    child_state->ready = true;
    pump(other, 8);
  }
  {
    auto failure = consume_failure(start_failure(other)).start(executor);
    auto foreign = consume_foreign(start_foreign_error(other)).start(executor);
    pump(executor, 8);
    pump(other, 8);
    pump(executor, 8);
    bool caught = false;
    try {
      failure.result();
    } catch (const std::runtime_error &error) {
      caught = std::string(error.what()) == "native coroutine error";
    }
    assert(caught);
    caught = false;
    try {
      foreign.result();
    } catch (const objc3::task_error &error) {
      caught = *static_cast<const int *>(
                   objc3_error_payload(error.native_handle())) == 73;
    }
    assert(caught);
  }
  {
    auto a = boolean().start(executor);
    auto b = empty().start(executor);
    auto c = aligned().start(executor);
    auto d = text().start(executor);
    auto f = aligned_local().start(executor);
    std::atomic<unsigned> entered{0};
    auto e = checkpoint(&entered).start(executor);
    e.cancel();
    pump(executor, 16);
    assert(!a.result());
    b.result();
    f.result();
    assert(c.result().value == 91 && d.result() == "owned result");
    cancelled([&] { (void)e.result(); });
    assert(entered == 0);
    auto copy = d;
    d = {};
    assert(copy.result() == "owned result");
  }
  {
    auto chain = start_chain(executor, 2048);
    pump(executor, 10000);
    assert(chain.result() == 2049);
  }
  {
    auto state = std::make_shared<coroutine_state>();
    auto parent = start_owned_child(executor, state);
    pump(executor, 2);
    assert(state->entered == 2 && state->destroyed == 0);
    parent.cancel();
    pump(executor, 32);
    cancelled([&] { (void)parent.result(); });
    assert(state->destroyed == 2 && state->continued == 0);
  }
  {
    auto state = std::make_shared<coroutine_state>();
    auto parent = start_owned_child(executor, state);
    pump(executor, 2);
    parent = {};
    pump(executor, 32);
    assert(state->entered == 2 && state->destroyed == 2 &&
           state->continued == 0);
  }
#ifdef __OBJC__
  {
    auto state = std::make_shared<coroutine_state>();
    auto child = start_leaf(other, state);
    auto success = objective_values(child).start(executor);
    auto cancellation = objective_values(child).start(executor);
    pump(executor, 2);
    assert(object_destructions == 0);
    cancellation.cancel();
    pump(executor, 8);
    cancelled([&] { (void)cancellation.result(); });
    assert(object_destructions == 1);
    state->ready = true;
    pump(other, 8);
    pump(executor, 8);
    assert(success.result() == 84 && object_destructions == 2);
  }
  @autoreleasepool {
    auto outcome = objective_result().start(executor);
    pump(executor, 8);
#if __has_feature(objc_arc)
    __attribute__((objc_precise_lifetime))
#endif
    CoroutineValue *value = outcome.result();
    outcome = {};
    assert(value->number == 96 && object_destructions == 2);
#if !__has_feature(objc_arc)
    objc_release(value);
#endif
  }
  assert(object_destructions == 3);
  objc3::task_handle<void> exception_outcome;
  @autoreleasepool {
    exception_outcome = objective_exception().start(executor);
    pump(executor, 8);
    // Foreign Objective-C unwinding is not a successful void return.
    objc3_completion completion{};
    assert(objc3_task_get_completion(exception_outcome.native_handle(),
                                     &completion) == OBJC3_SERVICE_OK);
    assert(completion.kind != OBJC3_COMPLETION_SUCCESS);
  }
  assert(object_destructions == 3);
  bool caught_object = false;
  @try {
    exception_outcome.result();
  } @catch (CoroutineValue *value) {
    caught_object = value->number == 73;
  }
  assert(caught_object);
  exception_outcome = {};
  assert(object_destructions == 4);
#endif
  objc3_executor_release(other);
  objc3_executor_release(executor);
}

void worker_contract() {
  objc3_executor_t *first = nullptr, *second = nullptr;
  assert(objc3_executor_create(0, &first) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(0, &second) == OBJC3_SERVICE_OK);
  for (unsigned batch = 0; batch != 8; ++batch) {
    auto child_state = std::make_shared<coroutine_state>();
    auto child = start_leaf(second, child_state);
    std::vector<std::shared_ptr<coroutine_state>> states;
    std::vector<objc3::task_handle<int>> parents;
    objc3_task_group_t *group = nullptr;
    assert(objc3_task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
    for (unsigned index = 0; index != 64; ++index) {
      auto state = std::make_shared<coroutine_state>();
      auto parent = start_parent(first, child, state);
      assert(objc3_task_group_add(group, parent.native_handle()) ==
             OBJC3_SERVICE_OK);
      states.push_back(state);
      parents.push_back(std::move(parent));
    }
    std::thread cancellation([&] {
      for (unsigned index = 0; index != parents.size(); index += 2)
        parents[index].cancel();
    });
    child_state->ready = true;
    cancellation.join();
    assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
    for (unsigned index = 0; index != parents.size(); ++index) {
      // Cancellation races success, and is cooperative. Either outcome must
      // preserve exactly-once lexical destruction and a valid terminal view.
      try {
        assert(parents[index].result() == 42);
      } catch (const objc3::task_cancelled &) {
      }
      assert(states[index]->destroyed == 1);
      assert(states[index]->continued <= 1);
    }
    child.wait();
    assert(child.result() == 0);
    objc3_task_group_release(group);
  }
#ifdef __OBJC__
  {
    auto state = std::make_shared<coroutine_state>();
    auto child = start_leaf(second, state);
    objc3_task_group_t *group = nullptr;
    assert(objc3_task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
    std::vector<objc3::task_handle<int>> values;
    for (unsigned index = 0; index != 64; ++index) {
      auto value = objective_values(child).start(index % 2 ? first : second);
      assert(objc3_task_group_add(group, value.native_handle()) ==
             OBJC3_SERVICE_OK);
      values.push_back(std::move(value));
    }
    state->ready = true;
    assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
    for (const auto &value : values)
      assert(value.result() == 84);
    // Joining includes task frame destruction. Fence both serial executors
    // before inspecting autoreleased temporaries drained after each callback.
    for (auto *executor : {first, second}) {
      struct fence {
        std::mutex mutex;
        std::condition_variable changed;
        bool done = false;
      } finished;
      assert(objc3_executor_enqueue(
                 executor,
                 [](void *opaque) {
                   auto &finished = *static_cast<fence *>(opaque);
                   std::lock_guard lock(finished.mutex);
                   finished.done = true;
                   finished.changed.notify_one();
                 },
                 &finished) == OBJC3_SERVICE_OK);
      std::unique_lock lock(finished.mutex);
      assert(finished.changed.wait_for(lock, std::chrono::seconds(5),
                                       [&] { return finished.done; }));
    }
    assert(object_destructions == 68);
    objc3_task_group_release(group);
  }
#endif
  objc3_executor_release(second);
  objc3_executor_release(first);
}
objc3::task<void> queued_actor_hop(objc3_actor_t *target,
                                   std::shared_ptr<coroutine_state> state) {
  struct guard {
    std::shared_ptr<coroutine_state> state;
    ~guard() { ++state->destroyed; }
  } lifetime{state};
  ++state->entered;
  co_await objc3::hop_to(target);
  ++state->continued;
}
objc3::task<int> owned_destination(objc3::task_destination target) {
  co_await std::move(target);
  co_await objc3::task_yield;
  co_return 42;
}
void actor_hop_contract(bool manual) {
  objc3_executor_t *first = nullptr, *other = nullptr;
  assert(objc3_executor_create(manual, &first) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(manual, &other) == OBJC3_SERVICE_OK);
  objc3_actor_t *a = nullptr, *b = nullptr, *c = nullptr;
  assert(objc3_actor_create(first, &a) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(first, &b) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(other, &c) == OBJC3_SERVICE_OK);
  objc3_task_group_t *group = nullptr;
  assert(objc3_task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::vector<objc3::task_handle<int>> tasks;
  std::vector<std::shared_ptr<coroutine_state>> states;
  for (unsigned index = 0; index != 64; ++index) {
    auto state = std::make_shared<coroutine_state>();
    auto task = start_actor_hops(a, index % 2 ? b : c, other, state, 8);
    assert(objc3_task_group_add(group, task.native_handle()) == OBJC3_SERVICE_OK);
    states.push_back(state);
    tasks.push_back(std::move(task));
  }
  assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
  if (manual) {
    for (unsigned round = 0; round != 32; ++round) {
      pump(first, 100000);
      pump(other, 100000);
    }
  }
  assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  for (unsigned index = 0; index != tasks.size(); ++index) {
    assert(tasks[index].result() == 42);
    assert(states[index]->entered == 1 && states[index]->destroyed == 1);
    assert(states[index]->continued == 8);
  }
  tasks.clear();
  objc3_task_group_release(group);
  assert(objc3_actor_close(a) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(b) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(c) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(a) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(b) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(c) == OBJC3_SERVICE_OK);
  objc3_actor_release(c);
  objc3_actor_release(b);
  objc3_actor_release(a);
  objc3_executor_release(other);
  objc3_executor_release(first);
}
void cancelled_actor_hop_contract() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  for (unsigned mode = 0; mode != 4; ++mode) {
    objc3_actor_t *actor = nullptr;
    assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
    auto state = std::make_shared<coroutine_state>();
    auto task = queued_actor_hop(actor, state).start(executor);
    pump(executor);
    assert(state->entered == 1 && state->continued == 0);
    if (mode == 0) {
      assert(objc3_actor_cancel(actor) == OBJC3_SERVICE_OK);
    } else if (mode == 1) {
      task.cancel();
    } else if (mode == 2) {
      // Closing an actor rejects the next continuation after a dependency.
      assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
      task.cancel();
    } else {
      task = {};
      assert(objc3_actor_cancel(actor) == OBJC3_SERVICE_OK);
    }
    pump(executor, 100000);
    if (task) cancelled([&] { task.result(); });
    assert(state->destroyed == 1 && state->continued == 0);
    assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
    objc3_actor_release(actor);
  }
  // The hop value retains the actor before the lazy frame starts.
  objc3_actor_t *actor = nullptr;
  assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  auto task = owned_destination(objc3::hop_to(actor)).start(executor);
  objc3_actor_release(actor);
  pump(executor, 100000);
  assert(task.result() == 42);
  objc3_executor_release(executor);
}

void failed_actor_dispatch_contract() {
  objc3_executor_t *source = nullptr, *target = nullptr;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &target) == OBJC3_SERVICE_OK);
  for (unsigned mode = 0; mode != 3; ++mode) {
    objc3_actor_t *actor = nullptr;
    assert(objc3_actor_create(target, &actor) == OBJC3_SERVICE_OK);
    if (mode == 0) {
      assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
      auto lazy = queued_actor_hop(actor, std::make_shared<coroutine_state>());
      bool rejected = false;
      try { (void)std::move(lazy).start_on(actor); }
      catch (const objc3::service_failure &error) {
        rejected = error.status() == OBJC3_SERVICE_CLOSED;
      }
      assert(rejected); // Failure leaves the unstarted frame owned by lazy.
    } else if (mode == 1) {
      objc3_actor_hop_facts facts{};
      facts.struct_size = sizeof(facts);
      facts.version = OBJC3_ACTOR_SERVICE_VERSION;
      for (unsigned count = 0; count != 1024; ++count)
        assert(objc3_actor_enqueue(actor, noop, nullptr, nullptr, &facts) ==
               OBJC3_SERVICE_OK);
    } else {
      for (unsigned count = 0; count != 1024; ++count)
        assert(objc3_executor_enqueue(target, noop, nullptr) == OBJC3_SERVICE_OK);
    }
    auto state = std::make_shared<coroutine_state>();
    auto task = queued_actor_hop(actor, state).start(source);
    pump(source, 100000);
    cancelled([&] { task.result(); });
    assert(state->entered == 1 && state->continued == 0 && state->destroyed == 1);
    pump(target, 100000);
    assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
    objc3_actor_release(actor);
  }
  objc3_executor_release(target);
  objc3_executor_release(source);
}

objc3::task<int> actor_dependency(objc3::task_handle<int> dependency,
                                  std::shared_ptr<coroutine_state> state) {
  struct guard {
    std::shared_ptr<coroutine_state> state;
    ~guard() { ++state->destroyed; }
  } lifetime{state};
  ++state->entered;
  int value = co_await dependency;
  ++state->continued;
  co_return value;
}
void group_cancel_dispatch_failure_contract() {
  objc3_executor_t *executor = nullptr, *other = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &other) == OBJC3_SERVICE_OK);
  for (unsigned mode = 0; mode != 5; ++mode) {
    objc3_actor_t *actor = nullptr;
    assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
    auto child_state = std::make_shared<coroutine_state>();
    auto state = std::make_shared<coroutine_state>();
    auto dependency = start_leaf(other, child_state);
    auto parent = actor_dependency(dependency, state).start_on(actor);
    pump(executor, 100000);
    assert(state->entered == 1 && state->continued == 0);
    objc3_task_group_t *root = nullptr, *nested = nullptr, *failed_group = nullptr;
    assert(objc3_task_group_create(nullptr, &root) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_create(root, &nested) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_create(root, &failed_group) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_add(nested, parent.native_handle()) == OBJC3_SERVICE_OK);
    objc3::task_handle<std::string> failure;
    if (mode == 1) {
      failure = start_failure(executor);
      assert(objc3_task_group_add(failed_group, failure.native_handle()) ==
             OBJC3_SERVICE_OK);
    }
    assert(objc3_task_group_close(root) == OBJC3_SERVICE_OK);
    assert(objc3_task_add_waiter(
               parent.native_handle(),
               [](void *context, const objc3_completion *completion) {
                 assert(completion->kind == OBJC3_COMPLETION_CANCELLED);
                 // Reentry must not encounter a group or failure-publication
                 // lock held by the cancellation that triggered this callback.
                 assert(objc3_task_group_cancel(
                            static_cast<objc3_task_group_t *>(context)) ==
                        OBJC3_SERVICE_OK);
               }, root) == OBJC3_SERVICE_OK);
    if (mode < 2) {
      assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    } else if (mode == 2) {
      assert(objc3_actor_cancel(actor) == OBJC3_SERVICE_OK);
    } else if (mode == 3) {
      objc3_actor_hop_facts facts{};
      facts.struct_size = sizeof(facts);
      facts.version = OBJC3_ACTOR_SERVICE_VERSION;
      for (unsigned count = 0; count != 1024; ++count)
        assert(objc3_actor_enqueue(actor, noop, nullptr, nullptr, &facts) ==
               OBJC3_SERVICE_OK);
    } else {
      for (unsigned count = 0; count != 1024; ++count)
        assert(objc3_executor_enqueue(executor, noop, nullptr) == OBJC3_SERVICE_OK);
    }
    if (mode != 1)
      assert(objc3_task_group_cancel(root) == OBJC3_SERVICE_OK);
    pump(executor, 100000);
    assert(objc3_task_group_join(root) == OBJC3_SERVICE_OK);
    cancelled([&] { parent.result(); });
    assert(state->destroyed == 1 && state->continued == 0);
    // Group cancellation never owns this shared dependency.
    assert(!objc3_task_is_cancel_requested(dependency.native_handle()));
    child_state->ready = true;
    pump(other, 100000);
    assert(dependency.result() == 0);
    objc3_task_group_release(failed_group);
    objc3_task_group_release(nested);
    objc3_task_group_release(root);
    assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
    objc3_actor_release(actor);
  }
  objc3_executor_release(other);
  objc3_executor_release(executor);
}

struct held_hop_frame {
  objc3_executor_t *target = nullptr;
  std::mutex mutex;
  std::condition_variable changed;
  bool held = false, released = false;
  unsigned resumes = 0;
  std::atomic<unsigned> destroyed{0};
};
void held_hop_resume(objc3_task_t *task, void *context) {
  auto &frame = *static_cast<held_hop_frame *>(context);
  if (++frame.resumes == 1) {
    assert(objc3_task_hop(task, frame.target, nullptr) == OBJC3_SERVICE_OK);
    std::unique_lock lock(frame.mutex);
    frame.held = true;
    frame.changed.notify_all();
    assert(frame.changed.wait_for(lock, std::chrono::seconds(5),
                                  [&] { return frame.released; }));
  } else {
    objc3_completion completion{};
    completion.struct_size = sizeof(completion);
    completion.version = OBJC3_SERVICE_ABI_VERSION;
    assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  }
}
void hop_publication_contract() {
  objc3_executor_t *source = nullptr, *target = nullptr;
  assert(objc3_executor_create(0, &source) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &target) == OBJC3_SERVICE_OK);
  held_hop_frame frame{};
  frame.target = target;
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = source;
  options.frame = &frame;
  options.resume = held_hop_resume;
  options.destroy = [](void *context) {
    ++static_cast<held_hop_frame *>(context)->destroyed;
  };
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  {
    std::unique_lock lock(frame.mutex);
    assert(frame.changed.wait_for(lock, std::chrono::seconds(5),
                                  [&] { return frame.held; }));
    uint32_t executed = 99;
    assert(objc3_executor_pump(target, 100, &executed) == OBJC3_SERVICE_OK);
    assert(executed == 0); // Nothing published while the source owns the frame.
    assert(objc3_task_hop(task, target, nullptr) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    std::vector<std::thread> wakes;
    for (unsigned index = 0; index != 8; ++index)
      wakes.emplace_back([&] {
        for (unsigned wake = 0; wake != 16; ++wake)
          assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
      });
    for (auto &wake : wakes) wake.join();
    assert(objc3_executor_pump(target, 100, &executed) == OBJC3_SERVICE_OK);
    assert(executed == 0); // Active and concurrent requests coalesce before publication.
    frame.released = true;
    frame.changed.notify_all();
  }
  // Joining the source executor observes publication of the target job.
  objc3_executor_release(source);
  uint32_t executed = 0;
  assert(objc3_executor_pump(target, 100, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1);
  assert(frame.resumes == 2 && frame.destroyed == 1);
  objc3_task_release(task);
  objc3_executor_release(target);
}
void source_free_group_await_contract() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  auto group = objc3::task_group_handle::create();
  auto nested = group.child();
  auto child_state = std::make_shared<coroutine_state>();
  auto parent_state = std::make_shared<coroutine_state>();
  auto child = start_leaf(executor, child_state);
  nested.add(child);
  group.close();
  auto joined = start_group_join(executor, group, parent_state);
  pump(executor, 2);
  assert(child_state->entered == 1 && parent_state->entered == 1);
  pending(joined);
  assert(parent_state->continued == 0);
  child_state->ready = true;
  pump(executor, 100);
  assert(joined.result() == 42 && parent_state->continued == 1);
  assert(child_state->destroyed == 1 && parent_state->destroyed == 1);
  group.wait();
  objc3_executor_release(executor);
}
} // namespace
int main() {
  manual_contract();
  worker_contract();
  actor_hop_contract(true);
  actor_hop_contract(false);
  cancelled_actor_hop_contract();
  failed_actor_dispatch_contract();
  group_cancel_dispatch_failure_contract();
  hop_publication_contract();
  source_free_group_await_contract();
}
