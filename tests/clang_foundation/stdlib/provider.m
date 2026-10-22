#include "provider.h"
#include <assert.h>

Result(int, int) stdlib_optional(Optional(int) value, int (^factory)(void)) {
  return okOr(value, factory);
}
Result(int, int) stdlib_status(int status, int value, int (^factory)(int)) {
  return resultFromStatus(status, value, factory);
}
void stdlib_status_throwing(int status, id<Error> (^factory)(int)) throws(id<Error>) {
  tryValue(statusOrThrow(status, factory));
}
int stdlib_throwing(Optional(int) value, id<Error> (^factory)(void))
    throws(id<Error>) {
  return tryValue(orThrow(value, factory));
}

int stdlib_records(void) {
  const char bytes[] = {'A', (char)0xf0, (char)0x9f, (char)0x99, (char)0x82, 0};
  objc3_text_v2_t text = 0, prefix = 0, joined = 0;
  assert(objc3_text_from_utf8_record(bytes, sizeof(bytes), 4, &text) == OBJC3_STDLIB_V2_INVALID_ARGUMENT && !text);
  assert(objc3_text_from_utf8_record(bytes, sizeof(bytes), 3, &text) == 0);
  uint64_t count = 0;
  assert(objc3_runtime_stdlib_text_v2_byte_count_checked(text, &count) == 0 && count == 6);
  assert(objc3_runtime_stdlib_text_v2_scalar_count_checked(text, &count) == 0 && count == 3);
  assert(objc3_text_prefix(text, 2, &prefix) == 0);
  assert(objc3_runtime_stdlib_text_v2_byte_count_checked(prefix, &count) == 0 && count == 5);
  assert(objc3_runtime_stdlib_text_v2_concat(prefix, text, &joined) == 0);
  assert(objc3_runtime_stdlib_text_v2_scalar_count_checked(joined, &count) == 0 && count == 5);
  assert(objc3_runtime_stdlib_text_v2_release_checked(joined) == 0);
  assert(objc3_runtime_stdlib_text_v2_release_checked(prefix) == 0);
  assert(objc3_text_prefix(text, UINT64_MAX, &prefix) == 0);
  assert(objc3_runtime_stdlib_text_v2_byte_count_checked(prefix, &count) == 0 && count == 6);
  assert(objc3_runtime_stdlib_text_v2_release_checked(prefix) == 0);
  assert(objc3_runtime_stdlib_text_v2_release_checked(text) == 0);
  assert(objc3_text_prefix(text, 1, &prefix) == OBJC3_STDLIB_V2_STALE_HANDLE && !prefix);
  const char malformed[] = {(char)0xc0, (char)0x80};
  assert(objc3_runtime_stdlib_text_v2_create_utf8_checked(malformed, 2, &text) == OBJC3_STDLIB_V2_INVALID_UTF8 && !text);

  int32_t values[] = {11, 22, 33}, value = 0;
  objc3_array_i32_v2_t array = 0;
  assert(objc3_runtime_stdlib_array_i32_v2_create(values, 3, &array) == 0);
  assert(objc3_array_i32_get_or(array, 1, -1, &value) == 0 && value == 22);
  assert(objc3_array_i32_get_or(array, 3, -7, &value) == OBJC3_STDLIB_V2_OUT_OF_BOUNDS && value == -7);
  assert(objc3_array_i32_prefix_count(array, UINT64_MAX, &count) == 0 && count == 3);
  assert(objc3_array_i32_prefix_count(array, 1, &count) == 0 && count == 1);
  assert(objc3_runtime_stdlib_array_i32_v2_release(array) == 0);
  objc3_map_i32_i32_v2_t map = 0;
  assert(objc3_runtime_stdlib_map_i32_i32_v2_create(&map) == 0);
  assert(objc3_map_i32_insert_count(map, 8, 41, &count) == 0 && count == 1);
  assert(objc3_map_i32_insert_count(map, 8, 42, &count) == 0 && count == 1);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_count(map, &count) == 0 && count == 1);
  assert(objc3_map_i32_get_or(map, 8, -1, &value) == 0 && value == 42);
  assert(objc3_map_i32_get_or(map, 9, -3, &value) == OBJC3_STDLIB_V2_NOT_FOUND && value == -3);
  assert(objc3_map_i32_insert_count(map, 9, 43, &count) == 0 && count == 2);
  assert(objc3_map_i32_insert_count(map, 9, 43, 0) == OBJC3_STDLIB_V2_INVALID_ARGUMENT);
  assert(objc3_runtime_stdlib_map_i32_i32_v2_release(map) == 0);
  count = 99;
  assert(objc3_map_i32_insert_count(map, 8, 1, &count) == OBJC3_STDLIB_V2_STALE_HANDLE && count == 0);
  assert(objc3_map_i32_get_or(map, 8, -5, &value) == OBJC3_STDLIB_V2_STALE_HANDLE && value == -5);
  return 42;
}

static void resumed(objc3_task_t *task, void *context) {
  ++*(int *)context;
  assert(objc3_task_is_cancel_requested(task));
  objc3_completion cancelled = {0};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
}
static void destroy_frame(void *context) { (void)context; }
int stdlib_tasks(void) {
  objc3_executor_t *executor = 0;
  objc3_task_group_t *group = 0;
  objc3_task_t *task = 0;
  int called = 0;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_create(0, &group) == OBJC3_SERVICE_OK);
  objc3_task_start_options options = {0};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = &called;
  options.resume = resumed;
  options.destroy = destroy_frame;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group, task) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_cancel(group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK && executed == 1);
  assert(objc3_task_wait(task) == OBJC3_SERVICE_OK);
  objc3_completion completion = {0};
  assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_CANCELLED && called == 1);
  assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  objc3_task_release(task);
  objc3_task_group_release(group);
  objc3_executor_release(executor);
  return called;
}
