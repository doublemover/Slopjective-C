#include <objc3/concurrency.h>
#include <objc3/errors.h>
#include <atomic>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "ownership_wait.h"
#include "cxx_frame_values.h"
#include "cxx_frame_borrows.h"

@protocol Sendable @end
@protocol SpawnFailure <Error, Sendable> @end
extern "C" id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern "C" id object_dispose(__unsafe_unretained id);
extern "C" void release_source(void *) __asm__("objc_release");
__attribute__((objc_root_class))
@interface SpawnError <SpawnFailure> {
  Class isa;
@public
  unsigned *destroyed;
}
+ (id)new;
@end
@implementation SpawnError
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc {
  __atomic_fetch_add(destroyed, 1u, __ATOMIC_RELEASE);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

namespace {
struct State {
  objc3_task_t *parent{};
  objc3_actor_t *actor{};
  objc3_executor_t *source{}, *global{};
  objc3::task_group_handle group;
  objc3::task_handle<int> direct, macro, raw, detached, raw_detached;
  std::atomic<unsigned> ran{0}, deferred{0};
  bool cancelled{};
};
void check_destination(objc3_actor_t *actor, objc3_executor_t *executor) {
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == objc3_actor_identity(actor));
  assert(facts.executor_identity == objc3_executor_identity(executor));
}
int pause(int value) __objc3_async { return value; }
int raw_pause(int value) __objc3_async_frame { return value; }
int raw_scope_child(void) __objc3_async_frame __attribute__((objc_task_group)) {
  assert(objc3_task_group_scope_current());
  return 21;
}
int raw_scope_parent(void) __objc3_async_frame __attribute__((objc_task_group)) {
  auto *group = objc3_task_group_scope_current();
  assert(group);
  auto child = std::move(__objc3_async_from_frame<int>(raw_scope_child())).start_child();
  assert(objc3_task_group_add(group, child.native_handle()) == OBJC3_SERVICE_OK);
  return 42;
}
int parent(State *state) __objc3_async {
  if (state->cancelled) objc3_task_request_cancel(state->parent);
  state->direct = objc3_task_spawn(^int(void) __objc3_async {
    defer { ++state->deferred; }
    ++state->ran;
    check_destination(state->actor, state->source);
    return __objc3_await pause(11);
  });
  state->macro = OBJC_TASK_SPAWN(^int(void) __objc3_async {
    defer { ++state->deferred; }
    ++state->ran;
    check_destination(state->actor, state->source);
    return __objc3_await pause(13);
  });
  state->raw = OBJC_TASK_SPAWN(^int(void) __objc3_async_frame {
    defer { ++state->deferred; }
    ++state->ran;
    check_destination(state->actor, state->source);
    return __objc3_await raw_pause(18);
  });
  state->detached = objc3_task_detached(^int(void) __objc3_async {
    defer { ++state->deferred; }
    check_destination(nullptr, state->global);
    return __objc3_await pause(42);
  });
  state->raw_detached = OBJC_TASK_DETACHED(^int(void) __objc3_async_frame {
    defer { ++state->deferred; }
    check_destination(nullptr, state->global);
    return __objc3_await raw_pause(43);
  });
  state->group.add(state->direct);
  state->group.add(state->macro);
  state->group.add(state->raw);
  state->group.add(state->detached);
  state->group.add(state->raw_detached);
  check_destination(state->actor, state->source);
  if (state->cancelled) return 0;
  return (__objc3_await state->direct) + (__objc3_await state->macro) +
         (__objc3_await state->raw);
}

void inspect_checked_task(objc3::task_handle<int> task, id expected) {
  auto group = objc3::task_group_handle::create();
  group.add(task);
  group.close();
  group.wait();
  bool caught = false;
  try { (void)task.result(); }
  catch (const objc3::task_error &failure) {
    caught = objc3::task_detail::__objc3_task_error_object(failure.native_handle()) ==
             (__bridge void *)expected;
  }
  assert(caught);
}
void checked_spawn_error_ownership(unsigned *destroyed) {
  SpawnError *object = [SpawnError new];
  object->destroyed = destroyed;
  id<SpawnFailure> error = object;
  // These overloads require native deduction of E from the Block effect type.
  // Each failed task has its own joined group, so first-failure cancellation
  // cannot hide a later task's checked error transport.
  inspect_checked_task(objc3_task_detached(
      ^int(void) __objc3_async __objc3_throws(id<SpawnFailure>) {
        throwError(error);
      }), error);
  inspect_checked_task(OBJC_TASK_DETACHED(
      ^int(void) __objc3_async_frame __objc3_throws(id<SpawnFailure>) {
        throwError(error);
      }), error);
  inspect_checked_task(OBJC_TASK_DETACHED(
      ^int(void) __objc3_async __objc3_throws {
        throwError(error);
      }), error);
  inspect_checked_task(objc3_task_detached(
      ^int(void) __objc3_async_frame __objc3_throws {
        throwError(error);
      }), error);
#if !__has_feature(objc_arc)
  release_source((__bridge void *)object);
#endif
}
}
namespace {
struct CxxScopeState {
  objc3::task_handle<int> child;
  unsigned exited = 0;
};
struct ScopedCallback {
  struct State {
    objc3::task_handle<void> gate;
    unsigned ran = 0, destroyed = 0;
    bool fail = false;
  } *state;
  std::shared_ptr<int> owner;
  ~ScopedCallback() { if (owner) ++state->destroyed; }
  ScopedCallback(State *state, std::shared_ptr<int> owner)
      : state(state), owner(std::move(owner)) {}
  ScopedCallback(ScopedCallback &&) = default;
  ScopedCallback(const ScopedCallback &) = delete;
  objc3::task<void> child() {
    co_await state->gate;
    assert(owner && *owner == 42);
    ++state->ran;
    co_return;
  }
  objc3::task<int> operator()(objc3::task_group_handle group) {
    auto task = child().start_child();
    group.add(task);
    if (state->fail) throw 73;
    co_return 42;
  }
};
void callback_group_owns_callable_until_quiescence(objc3_executor_t *source) {
  for (bool fail : {false, true}) {
    ScopedCallback::State state;
    state.fail = fail;
    objc3_task_start_options options{};
    options.struct_size = sizeof(options);
    options.version = OBJC3_SERVICE_ABI_VERSION;
    options.executor = source;
    options.resume = [](objc3_task_t *, void *) {};
    options.result_type = objc3::task_detail::result_type<void>();
    objc3_task_t *gate = nullptr;
    assert(objc3_task_start(&options, &gate) == OBJC3_SERVICE_OK);
    auto *owned = objc3_task_retain(gate);
    state.gate = objc3::task_handle<void>::adopt_native(&owned);
    auto value = std::make_shared<int>(42);
    std::weak_ptr<int> witness = value;
    auto task = objc3::with_task_group<int>(ScopedCallback(&state, std::move(value))).start(source);
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    if (!fail) {
      objc3_completion completion{};
      assert(objc3_task_get_completion(task.native_handle(), &completion) ==
             OBJC3_SERVICE_NOT_READY);
      assert(!witness.expired() && state.destroyed == 0 && state.ran == 0);
    }
    objc3_completion completion{};
    completion.struct_size = sizeof(completion);
    completion.version = OBJC3_SERVICE_ABI_VERSION;
    completion.kind = OBJC3_COMPLETION_SUCCESS;
    completion.result_type = objc3::task_detail::result_type<void>();
    assert(objc3_task_complete(gate, &completion) == OBJC3_SERVICE_OK);
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    if (fail) {
      bool caught = false;
      try { (void)task.result(); }
      catch (int error) { caught = error == 73; }
      assert(caught && state.ran == 0);
    } else {
      assert(task.result() == 42 && state.ran == 1);
    }
    assert(witness.expired() && state.destroyed == 1);
    objc3_task_release(gate);
  }
  auto empty_result = objc3::with_task_group([](objc3::task_group_handle group)
                                               -> objc3::task<void> {
    auto child = pause(42).start_child();
    group.add(child);
    co_return;
  }).start(source);
  uint32_t executed = 0;
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  empty_result.result();
  auto iterated = objc3::with_task_group<int>([](objc3::task_group_handle group)
                                                -> objc3::task<int> {
    (void)pause(21).start_child(group);
    (void)pause(23).start_child(group);
    group.close();
    auto first = co_await group.next<int>();
    auto second = co_await group.next<int>();
    assert(first && second);
    auto exhausted = co_await group.next<int>();
    assert(!exhausted);
    co_await group;
    co_return first.result() + second.result();
  }).start(source);
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  assert(iterated.result() == 44);
  for (bool recover : {false, true}) {
    auto errors = objc3::with_task_group<int>([recover](objc3::task_group_handle group)
                                                 -> objc3::task<int> {
      auto failed = []() -> objc3::task<int> {
        throw 73;
        co_return 0;
      };
      (void)failed().start_child(group);
      group.close();
      auto member = co_await group.next<int>();
      try { co_return member.result(); }
      catch (int error) {
        if (recover) co_return error + 1;
        throw;
      }
    }).start(source);
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    if (recover) {
      assert(errors.result() == 74);
    } else {
      bool caught = false;
      try { (void)errors.result(); }
      catch (int error) { caught = error == 73; }
      assert(caught);
    }
  }
}

struct CallbackFenceState {
  objc3_executor_t *worker = nullptr;
  std::mutex mutex;
  std::condition_variable changed;
  bool entered = false, release = false, fence = false;
  std::atomic<unsigned> callable_destroyed{0};
};
struct CallbackFenceValue {
  CallbackFenceState *state;
  bool original = true;
  explicit CallbackFenceValue(CallbackFenceState *state) : state(state) {}
  CallbackFenceValue(const CallbackFenceValue &other)
      : state(other.state), original(false) {}
  CallbackFenceValue(CallbackFenceValue &&other) noexcept
      : state(other.state), original(std::exchange(other.original, false)) {}
  ~CallbackFenceValue() {
    if (!original) return;
    std::unique_lock<std::mutex> lock(state->mutex);
    state->entered = true;
    state->changed.notify_all();
    assert(state->changed.wait_for(lock, std::chrono::seconds(5),
                                   [&] { return state->release; }));
    assert(state->callable_destroyed == 0);
  }
};
struct CallbackFenceWork {
  CallbackFenceState *state;
  std::shared_ptr<int> owner = std::make_shared<int>(42);
  explicit CallbackFenceWork(CallbackFenceState *state) : state(state) {}
  CallbackFenceWork(CallbackFenceWork &&) = default;
  ~CallbackFenceWork() { if (owner) ++state->callable_destroyed; }
  objc3::task<CallbackFenceValue> operator()(objc3::task_group_handle) {
    co_await objc3::hop_to(state->worker);
    co_return CallbackFenceValue(state);
  }
};
void callback_frame_cleanup_is_scope_work(objc3_executor_t *source) {
  CallbackFenceState state;
  assert(objc3_executor_create(0, &state.worker) == OBJC3_SERVICE_OK);
  auto task = objc3::with_task_group<CallbackFenceValue>(CallbackFenceWork(&state)).start(source);
  uint32_t executed = 0;
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  {
    std::unique_lock<std::mutex> lock(state.mutex);
    assert(state.changed.wait_for(lock, std::chrono::seconds(5), [&] { return state.entered; }));
  }
  // The callback's terminal result is available; its frame is still inside a
  // value destructor on another worker. Joining only terminal completion would
  // destroy the callable too soon and let the enclosing scope return here.
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  objc3_completion completion{};
  assert(objc3_task_get_completion(task.native_handle(), &completion) == OBJC3_SERVICE_NOT_READY);
  assert(state.callable_destroyed == 0);
  assert(objc3_executor_enqueue(state.worker, [](void *opaque) {
    auto &state = *static_cast<CallbackFenceState *>(opaque);
    std::lock_guard<std::mutex> lock(state.mutex);
    state.fence = true;
    state.changed.notify_all();
  }, &state) == OBJC3_SERVICE_OK);
  {
    std::unique_lock<std::mutex> lock(state.mutex);
    state.release = true;
    state.changed.notify_all();
    assert(state.changed.wait_for(lock, std::chrono::seconds(5), [&] { return state.fence; }));
  }
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  assert(task.result().state == &state && state.callable_destroyed == 1);
  task = {};
  objc3_executor_release(state.worker);
}

objc3::task<int> rejected_callback(unsigned *entered) {
  ++*entered;
  co_return 42;
}
struct ClosedGroupFactory {
  unsigned *entered;
  objc3::task<int> operator()(objc3::task_group_handle group) {
    auto scope = objc3::task_group_handle::current_scope();
    assert(scope.native_handle() != group.native_handle());
    scope.close();
    return rejected_callback(entered);
  }
};
struct RejectedQueueFactory {
  objc3_actor_t *actor;
  unsigned *entered;
  objc3::task<int> operator()(objc3::task_group_handle) {
    assert(objc3_actor_cancel(actor) == OBJC3_SERVICE_OK);
    return rejected_callback(entered);
  }
};
struct CancelledScopeFactory {
  unsigned *entered;
  objc3::task<int> operator()(objc3::task_group_handle) {
    objc3::task_group_handle::current_scope().cancel();
    return rejected_callback(entered);
  }
};
void callback_registration_failure_runs_no_body(objc3_executor_t *source) {
  unsigned entered = 0;
  auto task = objc3::with_task_group<int>(ClosedGroupFactory{&entered}).start(source);
  uint32_t executed = 0;
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  bool caught = false;
  try { (void)task.result(); }
  catch (const objc3::service_failure &failure) {
    caught = failure.status() == OBJC3_SERVICE_CLOSED;
  }
  assert(caught && entered == 0);
  auto cancelled = objc3::with_task_group<int>(CancelledScopeFactory{&entered}).start(source);
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  caught = false;
  try { (void)cancelled.result(); }
  catch (const objc3::task_cancelled &) { caught = true; }
  assert(caught && entered == 0);
  objc3_actor_t *actor = nullptr;
  assert(objc3_actor_create(source, &actor) == OBJC3_SERVICE_OK);
  frame_cxx_value_contract(source);
  cxx_frame_borrow_contract(source);
  auto rejected = objc3::with_task_group<int>(RejectedQueueFactory{actor, &entered}).start_on(actor);
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  caught = false;
  try { (void)rejected.result(); }
  catch (const objc3::task_cancelled &) { caught = true; }
  assert(caught && entered == 0);
  rejected = {};
  objc3_actor_release(actor);
}
int cxx_checked_scope(CxxScopeState *state, id<SpawnFailure> error, bool fail)
    __objc3_async __objc3_throws(id<SpawnFailure>)
    __attribute__((objc_task_group)) {
  defer { ++state->exited; }
  state->child = OBJC_TASK_SPAWN(^int(void) __objc3_async {
    return __objc3_await pause(21);
  });
  objc3::task_group_handle::current_scope().add(state->child);
  if (fail) throwError(error);
  return 42;
}
void checked_cxx_scope_exits(objc3_executor_t *source) {
  for (bool fail : {false, true}) {
    unsigned destroyed = 0;
    SpawnError *object = [SpawnError new];
    object->destroyed = &destroyed;
    CxxScopeState state;
    auto task = cxx_checked_scope(&state, object, fail).start(source);
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, 1, &executed) == OBJC3_SERVICE_OK);
    assert(state.exited == 1);
    objc3_completion completion{};
    assert(objc3_task_get_completion(task.native_handle(), &completion) ==
           OBJC3_SERVICE_NOT_READY);
    assert(bool(objc3_task_is_cancel_requested(state.child.native_handle())) == fail);
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    if (fail) {
      bool caught = false;
      try { (void)task.result(); }
      catch (const objc3::task_error &failure) {
        caught = objc3::task_detail::__objc3_task_error_object(failure.native_handle()) ==
                 (__bridge void *)object;
      }
      assert(caught);
    } else {
      assert(task.result() == 42 && state.child.result() == 21);
    }
    task = {};
#if !__has_feature(objc_arc)
    release_source((__bridge void *)object);
#endif
    object = nullptr;
    wait_for_destructions(&destroyed, 1);
  }
}
} // namespace

#include "dynamic_frame_storage.h"


// The explicit unsafe admission covers only an atomic observer and exclusively
// owned scalar state. It does not admit a borrowed/reference capture.
struct __attribute__((objc_unsafe_sendable)) OwnedCallableWork {
  std::atomic<int> *live;
  int value=42;
  explicit OwnedCallableWork(std::atomic<int> *counter):live(counter){++*live;}
  OwnedCallableWork(const OwnedCallableWork &other):live(other.live),value(other.value){++*live;}
  OwnedCallableWork(OwnedCallableWork &&other):live(other.live),value(other.value){++*live;}
  ~OwnedCallableWork(){--*live;}
  objc3::task<int> operator()() {
    co_await objc3::task_yield;
    assert(value==42);
    co_return value;
  }
};
int owned_callable_parent(std::atomic<int> *live) __objc3_async
    __attribute__((objc_task_group)) {
  auto scope=objc3::task_group_handle::current_scope();
  auto direct=objc3_task_spawn(OwnedCallableWork(live));
  auto framed=objc3_task_spawn<int>([value=42]() -> int __objc3_async_frame {
    return __objc3_await raw_pause(value);
  });
  auto grouped=scope.spawn([value=42]() -> int __objc3_async {
    return __objc3_await pause(value);
  });
  auto detached=objc3_task_detached([value=42]() -> int __objc3_async {
    return __objc3_await pause(value);
  });
  int result=__objc3_await direct;
  result+=__objc3_await framed;
  result+=__objc3_await grouped;
  return result+__objc3_await detached;
}
void owned_callable_spawn_contract(objc3_executor_t *source) {
  std::atomic<int> live{0};
  auto finish=[&](const auto &parent) {
    for(unsigned step=0;step!=1000000;++step) {
      uint32_t count=0;
      assert(objc3_executor_pump(source,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
      objc3_completion completion{};
      auto status=objc3_task_get_completion(parent.native_handle(),&completion);
      if(status==OBJC3_SERVICE_OK)return;
      assert(status==OBJC3_SERVICE_NOT_READY);
      std::this_thread::yield();
    }
    assert(!"owned callable did not quiesce");
  };
  {
    auto parent=owned_callable_parent(&live).start(source);
    uint32_t count=0;
    assert(objc3_executor_pump(source,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
    finish(parent);
    assert(parent.result()==168 && live.load()==0);
  }
  {
    auto parent=owned_callable_parent(&live).start(source);
    uint32_t count=0;
    assert(objc3_executor_pump(source,1,&count)==OBJC3_SERVICE_OK);
    parent.cancel();
    assert(objc3_executor_pump(source,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
    finish(parent);
    bool cancelled=false;try{(void)parent.result();}catch(const objc3::task_cancelled &){cancelled=true;}
    assert(cancelled && live.load()==0);
  }
  bool failed=false;
  try{(void)objc3_task_spawn(OwnedCallableWork(&live));}
  catch(const objc3::service_failure &error){failed=error.status()==OBJC3_SERVICE_NOT_READY;}
  assert(failed && live.load()==0);
}

int main() {
  objc3_executor_t *source = nullptr, *global = nullptr;
  objc3_actor_t *actor = nullptr;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  dynamic_frame_storage_contract(source);
  assert(objc3_actor_create(source, &actor) == OBJC3_SERVICE_OK);
  owned_callable_spawn_contract(source);
  checked_cxx_scope_exits(source);
  callback_group_owns_callable_until_quiescence(source);
  callback_frame_cleanup_is_scope_work(source);
  callback_registration_failure_runs_no_body(source);
  {
    auto scoped = std::move(__objc3_async_from_frame<int>(raw_scope_parent())).start_on(actor);
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    assert(scoped.result() == 42);
  }
  bool failed = false;
  try { (void)OBJC_TASK_SPAWN(^int(void) __objc3_async { return 0; }); }
  catch (const objc3::service_failure &failure) {
    failed = failure.status() == OBJC3_SERVICE_NOT_READY;
  }
  unsigned error_destroyed = 0;
  checked_spawn_error_ownership(&error_destroyed);
  wait_for_destructions(&error_destroyed, 1);
  assert(failed);
  for (bool cancelled : {false, true}) {
    State state;
    state.actor = actor;
    state.source = source;
    state.global = global;
    state.cancelled = cancelled;
    state.group = objc3::task_group_handle::create();
    auto task = parent(&state).start_on(actor);
    state.parent = task.native_handle();
    state.group.add(task);
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    state.group.close();
    state.group.wait();
    assert(state.ran == (cancelled ? 0u : 3u));
    assert(state.deferred == (cancelled ? 2u : 5u));
    assert(state.detached.result() == 42 && state.raw_detached.result() == 43);
    if (!cancelled) {
      assert(task.result() == 42);
      assert(state.direct.result() == 11 && state.macro.result() == 13);
      assert(state.raw.result() == 18);
    } else {
      for (auto handle : {state.direct, state.macro, state.raw}) {
        bool caught = false;
        try { (void)handle.result(); }
        catch (const objc3::task_cancelled &) { caught = true; }
        assert(caught);
      }
    }
  }
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  objc3_actor_release(actor);
  objc3_executor_release(global);
  objc3_executor_release(source);
}
