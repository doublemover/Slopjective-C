#include <objc3/concurrency.h>
#include <objc3/errors.h>
#include <atomic>
#include <cassert>
#include <initializer_list>
#include <memory>
#include "ownership_wait.h"

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

int main() {
  objc3_executor_t *source = nullptr, *global = nullptr;
  objc3_actor_t *actor = nullptr;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(source, &actor) == OBJC3_SERVICE_OK);
  checked_cxx_scope_exits(source);
  callback_group_owns_callable_until_quiescence(source);
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
