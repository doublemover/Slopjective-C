#ifndef TASK_SPAWNING_GROUP_SCOPE_H
#define TASK_SPAWNING_GROUP_SCOPE_H

typedef struct SourceScope {
  objc3_executor_t *children;
  objc3_actor_t *actor;
  objc3_task_t *parent, *child;
  objc3_task_group_t *group;
  unsigned exited, child_destroyed, ready, completed;
} SourceScope;
static const objc3_async_result_type scope_void_type = {
    sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION, 0, 1,
    OBJC3_ASYNC_VOID, 0, "objc3.async.v1:_ZTSv", sizeof("objc3.async.v1:_ZTSv") - 1, NULL};
static void source_scope_child_destroy(void *opaque) {
  __atomic_fetch_add(&((SourceScope *)opaque)->child_destroyed, 1u, __ATOMIC_RELEASE);
}
static void source_scope_child_resume(objc3_task_t *task, void *opaque) {
  SourceScope *state = opaque;
  if (!__atomic_load_n(&state->ready, __ATOMIC_ACQUIRE)) return;
  objc3_completion done = {0};
  done.struct_size = sizeof(done); done.version = OBJC3_SERVICE_ABI_VERSION;
  done.kind = objc3_task_is_cancel_requested(task) ? OBJC3_COMPLETION_CANCELLED
                                                 : OBJC3_COMPLETION_SUCCESS;
  if (done.kind == OBJC3_COMPLETION_SUCCESS) done.result_type = &scope_void_type;
  assert(objc3_task_complete(task, &done) == OBJC3_SERVICE_OK);
}
static void source_scope_add(SourceScope *state) {
  objc3_task_group_t *group = objc3_task_group_scope_current();
  assert(group);
  if (state->actor) {
    objc3_current_actor_facts facts = {0};
    facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
    assert(facts.actor_identity == objc3_actor_identity(state->actor));
  }
  state->group = objc3_task_group_retain(group);
  objc3_task_start_options options = {0};
  options.struct_size = sizeof(options); options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = state->children; options.frame = state;
  options.result_type = &scope_void_type;
  options.resume = source_scope_child_resume; options.destroy = source_scope_child_destroy;
  assert(objc3_task_start_child_on(state->parent, &options, &state->child) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group, state->child) == OBJC3_SERVICE_OK);
}
static int source_scope_value(SourceScope *state)
    __objc3_async_frame __attribute__((objc_task_group)) {
  source_scope_add(state);
  defer { ++state->exited; }
  return 42;
}
static int source_scope_suspended(SourceScope *state)
    __objc3_async_frame __attribute__((objc_task_group)) {
  source_scope_add(state);
  defer { ++state->exited; }
  __objc3_await join_existing_void(state->child);
  return 42;
}
static void source_scope_abandoned(void *opaque, const objc3_completion *done) {
  SourceScope *state = opaque;
  assert(done->kind == OBJC3_COMPLETION_CANCELLED && state->child_destroyed == 1);
  ++state->completed;
}
static int source_scope_error(SourceScope *state, SpawnOwner *error)
    __objc3_async_frame __objc3_throws(id<Error>)
    __attribute__((objc_task_group)) {
  source_scope_add(state);
  defer { ++state->exited; }
  __objc3_throw((id<Error>)error);
}
#if __has_feature(objc_arc)
typedef Optional(SpawnOwner *) ScopeOwnerMaybe;
static ScopeOwnerMaybe source_scope_owned(SourceScope *state, SpawnOwner *owner)
    __objc3_async_frame __attribute__((objc_task_group)) {
  source_scope_add(state);
  defer { ++state->exited; }
  return Some(SpawnOwner *, owner);
}
#endif
@interface SpawnOwner (StructuredScope)
- (int)scopeValue:(SourceScope *)state __objc3_async_frame
    __attribute__((objc_task_group));
@end
@implementation SpawnOwner (StructuredScope)
- (int)scopeValue:(SourceScope *)state __objc3_async_frame {
  source_scope_add(state);
  defer { ++state->exited; }
  return self->number;
}
@end
extern int sched_yield(void);
static void source_scope_worker_completed(void *opaque, const objc3_completion *done) {
  SourceScope *state = opaque;
  assert(done->kind == OBJC3_COMPLETION_SUCCESS);
  objc3_current_actor_facts facts = {0};
  facts.struct_size = sizeof(facts); facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == objc3_actor_identity(state->actor));
  ++state->completed;
}
static void source_scope_worker(objc3_executor_t *source, int ready) {
  SourceScope state = {0}; state.ready = (unsigned)ready;
  assert(objc3_executor_create(0, &state.children) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(source, &state.actor) == OBJC3_SERVICE_OK);
  objc3_async_frame frame = source_scope_value(&state);
  assert(objc3_async_frame_start(&frame, NULL, state.actor, &state.parent) == OBJC3_SERVICE_OK);
  assert(objc3_task_add_waiter(state.parent, source_scope_worker_completed, &state) == OBJC3_SERVICE_OK);
  objc3_task_group_t *fence = NULL;
  assert(objc3_task_group_create(NULL, &fence) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(fence, state.parent) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(fence) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  assert(state.exited == 1 && state.child && state.group);
  if (!ready) {
    __atomic_store_n(&state.ready, 1u, __ATOMIC_RELEASE);
    const objc3_service_status resumed = objc3_task_schedule_resume(state.child);
    assert(resumed == OBJC3_SERVICE_OK || resumed == OBJC3_SERVICE_ALREADY_COMPLETED);
  }
  objc3_completion completion = {0};
  while (objc3_task_get_completion(state.parent, &completion) == OBJC3_SERVICE_NOT_READY) {
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    sched_yield();
  }
  assert(completion.kind == OBJC3_COMPLETION_SUCCESS && *(const int *)completion.payload == 42);
  assert(state.completed == 1 && __atomic_load_n(&state.child_destroyed, __ATOMIC_ACQUIRE) == 1);
  assert(objc3_task_group_join(fence) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(state.actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(state.actor) == OBJC3_SERVICE_OK);
  objc3_task_release(state.parent); objc3_task_release(state.child);
  objc3_task_group_release(state.group); objc3_task_group_release(fence);
  objc3_actor_release(state.actor); objc3_executor_release(state.children);
}
static void group_scope_witnesses(objc3_executor_t *source) {
  assert(!objc3_task_group_scope_current());
  source_scope_worker(source, 0);
  source_scope_worker(source, 1);
  const unsigned modes = 6;
  for (unsigned mode = 0; mode != modes; ++mode) {
    if (mode == 3 && !__has_feature(objc_arc)) continue;
    unsigned destroyed = 0;
    @autoreleasepool {
      SourceScope state = {0};
      assert(objc3_executor_create(1, &state.children) == OBJC3_SERVICE_OK);
      SpawnOwner *owner = [SpawnOwner new]; owner->destroyed = &destroyed;
      owner->number = 42;
      objc3_async_frame frame;
#if __has_feature(objc_arc)
      if (mode == 3) frame = source_scope_owned(&state, owner);
      else
#endif
      frame = mode == 5 ? [owner scopeValue:&state] :
              mode == 1 ? source_scope_error(&state, owner) :
              mode == 4 ? source_scope_suspended(&state) : source_scope_value(&state);
      state.parent = join_start(source, frame);
      uint32_t executed = 0;
      assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
      assert(state.exited == (mode == 4 ? 0u : 1u) && state.child && state.group && !state.child_destroyed);
      objc3_completion completion = {0};
      assert(objc3_task_get_completion(state.parent, &completion) == OBJC3_SERVICE_NOT_READY);
      if (mode == 2) objc3_task_request_cancel(state.parent);
      if (mode == 4) {
        assert(objc3_task_add_waiter(state.parent, source_scope_abandoned, &state) == OBJC3_SERVICE_OK);
        objc3_task_release(state.parent); state.parent = NULL;
        assert(!state.exited && !state.completed);
      }
#if __has_feature(objc_arc)
      if (mode == 3 || mode == 5) { owner = NULL; assert(!destroyed); }
#endif
      __atomic_store_n(&state.ready, 1u, __ATOMIC_RELEASE);
      assert(objc3_executor_pump(state.children, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
      if (mode == 4) {
        assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
        assert(state.completed == 1 && state.exited == 1);
      } else completion = join_completion(source, state.parent);
      assert(state.child_destroyed == 1);
      if (mode != 4) assert(completion.kind == (mode == 1 ? OBJC3_COMPLETION_ERROR :
                                mode == 2 ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS));
      if (!mode || mode == 5) assert(*(const int *)completion.payload == 42);
      if (mode == 1) {
        assert(objc3_error_payload(completion.error));
        assert(*(void * const *)objc3_error_payload(completion.error) == (__bridge void *)owner);
      }
#if __has_feature(objc_arc)
      if (mode == 3) {
        const ScopeOwnerMaybe *value = completion.payload;
        if let object = *value { assert(object->number == 42); } else { assert(0); }
      }
#endif
      assert(objc3_task_group_join(state.group) == OBJC3_SERVICE_OK);
      objc3_task_release(state.parent); objc3_task_release(state.child);
      objc3_task_group_release(state.group);
#if !__has_feature(objc_arc)
      release_source((__bridge void *)owner);
#else
      owner = NULL;
#endif
      objc3_executor_release(state.children);
    }
    assert(destroyed == 1);
  }
}
#endif
