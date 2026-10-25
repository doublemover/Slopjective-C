#include <objc3/concurrency.h>
#include <objc3/errors.h>
#include <assert.h>
#include "ownership_wait.h"

@protocol Sendable @end
extern id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern id object_dispose(__unsafe_unretained id);
extern void release_source(void *) __asm__("objc_release");
__attribute__((objc_root_class))
@interface SpawnOwner <Sendable, Error> {
  Class isa;
@public
  unsigned *destroyed;
  int number;
}
+ (id)new;
@end
@implementation SpawnOwner
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc {
  __atomic_fetch_add(destroyed, 1u, __ATOMIC_RELEASE);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

#include "handle_joins.h"
#include "group_next.h"

typedef struct SpawnState {
  objc3_actor_t *actor;
  objc3_executor_t *source, *global;
  objc3_task_group_t *group;
  objc3_task_t *parent, *children[2], *detached[2];
  unsigned ran[4], deferred[4];
  unsigned owners_destroyed;
  int cancel_parent;
} SpawnState;

static void check_destination(uint64_t actor, uint64_t executor) {
  objc3_current_actor_facts facts = {0};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == actor && facts.executor_identity == executor);
}
static void leaf(void) __objc3_async_frame {}

static void spawn_parent(SpawnState *state) __objc3_async_frame {
  if (state->cancel_parent) objc3_task_request_cancel(state->parent);
  state->children[0] = objc3_task_spawn(^void(void) __objc3_async_frame {
    defer { ++state->deferred[0]; }
    check_destination(objc3_actor_identity(state->actor),
                      objc3_executor_identity(state->source));
    ++state->ran[0];
    __objc3_await leaf();
    check_destination(objc3_actor_identity(state->actor),
                      objc3_executor_identity(state->source));
  });
  state->children[1] = OBJC_TASK_SPAWN(^void(void) __objc3_async_frame {
    defer { ++state->deferred[1]; }
    check_destination(objc3_actor_identity(state->actor),
                      objc3_executor_identity(state->source));
    ++state->ran[1];
    __objc3_await leaf();
  });
  state->detached[0] = objc3_task_detached(^void(void) __objc3_async_frame {
    defer { ++state->deferred[2]; }
    check_destination(0, objc3_executor_identity(state->global));
    ++state->ran[2];
    __objc3_await leaf();
  });
  state->detached[1] = OBJC_TASK_DETACHED(^void(void) __objc3_async_frame {
    defer { ++state->deferred[3]; }
    check_destination(0, objc3_executor_identity(state->global));
    ++state->ran[3];
    __objc3_await leaf();
    check_destination(0, objc3_executor_identity(state->global));
  });
  for (unsigned index = 0; index != 2; ++index) {
    assert(state->children[index] && state->detached[index]);
    assert(!!objc3_task_is_cancel_requested(state->children[index]) ==
           state->cancel_parent);
    assert(!objc3_task_is_cancel_requested(state->detached[index]));
    assert(objc3_task_group_add(state->group, state->children[index]) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_add(state->group, state->detached[index]) == OBJC3_SERVICE_OK);
  }
  SpawnOwner *owner = [SpawnOwner new];
  owner->destroyed = &state->owners_destroyed;
  owner->number = 42;
  objc3_task_t *owned_child = OBJC_TASK_SPAWN(^void(void) __objc3_async_frame {
    assert(owner->number == 42);
    __objc3_await leaf();
    assert(owner->number == 42);
  });
  objc3_task_t *owned_detached = OBJC_TASK_DETACHED(^void(void) __objc3_async_frame {
    assert(owner->number == 42);
    __objc3_await leaf();
    assert(owner->number == 42);
  });
  assert(owned_child && owned_detached);
  assert(objc3_task_group_add(state->group, owned_child) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(state->group, owned_detached) == OBJC3_SERVICE_OK);
  objc3_task_release(owned_child);
  objc3_task_release(owned_detached);
#if !__has_feature(objc_arc)
  release_source((__bridge void *)owner);
#endif
  check_destination(objc3_actor_identity(state->actor),
                    objc3_executor_identity(state->source));
}

static objc3_task_t *failed_owned_start(unsigned *destroyed) {
  SpawnOwner *owner = [SpawnOwner new];
  owner->destroyed = destroyed;
  objc3_task_t *task = OBJC_TASK_SPAWN(^void(void) __objc3_async_frame {
    (void)owner->number;
    assert(0);
  });
#if !__has_feature(objc_arc)
  release_source((__bridge void *)owner);
#endif
  return task;
}

extern void *__objc3_task_error_object(const objc3_error_t *);
static void inspect_checked_task(objc3_task_t *task, id expected) {
  assert(task);
  objc3_task_group_t *group = NULL;
  assert(objc3_task_group_create(NULL, &group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group, task) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  objc3_completion completion = {0};
  assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_ERROR && completion.error);
  assert(__objc3_task_error_object(completion.error) == (__bridge void *)expected);
  objc3_task_release(task);
  objc3_task_group_release(group);
}
static void checked_spawn_error_ownership(unsigned *destroyed) {
  SpawnOwner *error = [SpawnOwner new];
  error->destroyed = destroyed;
  inspect_checked_task(objc3_task_detached(^void(void) __objc3_async_frame __objc3_throws {
    throwError(error);
  }), error);
  inspect_checked_task(OBJC_TASK_DETACHED(^void(void) __objc3_async_frame __objc3_throws {
    throwError(error);
  }), error);
#if !__has_feature(objc_arc)
  release_source((__bridge void *)error);
#endif
}

int main(void) {
  objc3_executor_t *source = NULL, *global = NULL;
  objc3_actor_t *actor = NULL;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(source, &actor) == OBJC3_SERVICE_OK);
  assert(!objc3_task_spawn((objc3_task_work)0));
  assert(!objc3_task_detached((objc3_task_work)0));
  // A failed outside-parent child start owns and disposes its native frame.
  assert(!OBJC_TASK_SPAWN(^void(void) __objc3_async_frame { assert(0); }));
  unsigned failed_destroyed = 0;
  assert(!failed_owned_start(&failed_destroyed));
  assert(__atomic_load_n(&failed_destroyed, __ATOMIC_ACQUIRE) == 1);
  for (int cancelled = 0; cancelled != 2; ++cancelled) {
    SpawnState state = {0};
    state.actor = actor;
    state.source = source;
    state.global = global;
    state.cancel_parent = cancelled;
    assert(objc3_task_group_create(NULL, &state.group) == OBJC3_SERVICE_OK);
    objc3_async_frame frame = spawn_parent(&state);
    assert(objc3_async_frame_start(&frame, NULL, actor, &state.parent) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_add(state.group, state.parent) == OBJC3_SERVICE_OK);
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_close(state.group) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_join(state.group) == OBJC3_SERVICE_OK);
    assert(__atomic_load_n(&state.owners_destroyed, __ATOMIC_ACQUIRE) == 1);
    for (unsigned index = 0; index != 4; ++index) {
      const unsigned expected = index < 2 && cancelled ? 0u : 1u;
      assert(state.ran[index] == expected && state.deferred[index] == expected);
    }
    for (unsigned index = 0; index != 2; ++index) {
      objc3_task_release(state.children[index]);
      objc3_task_release(state.detached[index]);
    }
    objc3_task_release(state.parent);
    objc3_task_group_release(state.group);
  }
  handle_join_witnesses(source);
  group_next_witnesses(source);
  unsigned error_destroyed = 0;
  checked_spawn_error_ownership(&error_destroyed);
  wait_for_destructions(&error_destroyed, 1);
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  objc3_actor_release(actor);
  objc3_executor_release(global);
  objc3_executor_release(source);
}
