#ifndef TASK_SPAWNING_GROUP_SCOPE_H
#define TASK_SPAWNING_GROUP_SCOPE_H

typedef struct SourceScope {
  objc3_executor_t *children;
  objc3_task_t *parent, *child;
  objc3_task_group_t *group;
  unsigned exited, child_destroyed, ready, completed;
} SourceScope;
static const objc3_async_result_type scope_void_type = {
    sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION, 0, 1,
    OBJC3_ASYNC_VOID, 0, "objc3.async.v1:_ZTSv", sizeof("objc3.async.v1:_ZTSv") - 1, NULL};
static void source_scope_child_destroy(void *opaque) {
  ++((SourceScope *)opaque)->child_destroyed;
}
static void source_scope_child_resume(objc3_task_t *task, void *opaque) {
  SourceScope *state = opaque;
  if (!state->ready) return;
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
static void group_scope_witnesses(objc3_executor_t *source) {
  assert(!objc3_task_group_scope_current());
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
      state.ready = 1;
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
