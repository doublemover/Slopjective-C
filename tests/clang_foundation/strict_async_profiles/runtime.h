#ifndef OBJC3_STRICT_ASYNC_PROFILES_RUNTIME_H
#define OBJC3_STRICT_ASYNC_PROFILES_RUNTIME_H
#include <objc3/concurrency.h>
#include <assert.h>

// One maintained source/PCH witness for every strict profile, C17/C++20/23
// and ARC/MRC. The same frame ABI and cleanup rules apply to each profile.
static int profile_entered, profile_cleaned;
OBJC_TASK_JOIN_DECLARE(profile_join_int, int);
#ifndef __cplusplus
OBJC_TASK_GROUP_NEXT_DECLARE(profile_next_int, int);
#endif

static int profile_leaf(int value) __objc3_async_frame {
  ++profile_entered;
  defer { ++profile_cleaned; }
  return value;
}

static int profile_group(int value)
    __objc3_async_frame __attribute__((objc_task_group)) {
  objc3_task_group_t *group = objc3_task_group_scope_current();
  assert(group);
  objc3_task_t *first = NULL, *second = NULL;
  defer {
    objc3_task_release(second);
    objc3_task_release(first);
    ++profile_cleaned;
  }
  objc3_async_frame one = profile_leaf(value);
  objc3_async_frame two = profile_leaf(1);
  assert(objc3_async_frame_spawn_in_group(&one, group, &first) == OBJC3_SERVICE_OK);
  assert(objc3_async_frame_spawn_in_group(&two, group, &second) == OBJC3_SERVICE_OK);
#ifdef __cplusplus
  int left = __objc3_await profile_join_int(first);
  int right = __objc3_await profile_join_int(second);
  return left + right;
#else
  int sum = 0, count = 0;
  for (;;) {
    Optional(int) next = __objc3_await profile_next_int(group);
    if let item = next { sum += item; ++count; } else { break; }
  }
  assert(count == 2);
  return sum;
#endif
}

static void profile_pump(objc3_executor_t *executor) {
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
}

static void profile_frame_suite(objc3_executor_t *executor) {
  objc3_async_frame frame = profile_group(41);
  assert(profile_entered == 0 && profile_cleaned == 0);
  objc3_task_t *task = NULL;
  assert(objc3_async_frame_start(&frame, executor, NULL, &task) == OBJC3_SERVICE_OK);
  assert(!frame.context && profile_entered == 0);
  profile_pump(executor);
  objc3_completion completion = {0};
  assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_SUCCESS && completion.has_payload);
  assert(*(const int *)completion.payload == 42);
  assert(profile_entered == 2 && profile_cleaned == 3);
  objc3_task_release(task);

  // Cancellation before the first resume neither enters user code nor
  // invents a registered defer. Dropping an unstarted frame has the same rule.
  frame = profile_group(99);
  task = NULL;
  assert(objc3_async_frame_start(&frame, executor, NULL, &task) == OBJC3_SERVICE_OK);
  objc3_task_request_cancel(task);
  profile_pump(executor);
  assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
  objc3_task_release(task);
  frame = profile_group(99);
  assert(objc3_async_frame_dispose(&frame) == OBJC3_SERVICE_OK);
  assert(profile_entered == 2 && profile_cleaned == 3);
}

#ifdef __cplusplus
static int profile_task_entered, profile_task_cleaned;
static int profile_task_leaf(int value) __objc3_async {
  ++profile_task_entered;
  defer { ++profile_task_cleaned; }
  return value;
}
static int profile_task_parent(int value) __objc3_async {
  return 1 + __objc3_await profile_task_leaf(value);
}
struct alignas(128) ProfileAligned { int value; };
static int profile_task_aligned(ProfileAligned value) __objc3_async {
  assert(((__UINTPTR_TYPE__)&value % alignof(ProfileAligned)) == 0);
  return value.value;
}
static void profile_cpp_suite(objc3_executor_t *executor) {
  auto aligned = profile_task_aligned(ProfileAligned{42}).start(executor);
  profile_pump(executor);
  assert(aligned.result() == 42);
  auto ordinary = profile_task_parent(41).start(executor);
  profile_pump(executor);
  assert(ordinary.result() == 42);
  assert(profile_task_entered == 1 && profile_task_cleaned == 1);

  auto grouped = objc3::with_task_group<int>([](objc3::task_group_handle group)
      -> objc3::task<int> {
    auto child = objc3_task_spawn_in_group(group, [] { return profile_task_leaf(41); });
    co_return 1 + co_await child;
  }).start(executor);
  profile_pump(executor);
  assert(grouped.result() == 42);
  assert(profile_task_entered == 2 && profile_task_cleaned == 2);

  auto cancelled = profile_task_parent(99).start(executor);
  cancelled.cancel();
  profile_pump(executor);
  objc3_completion completion{};
  assert(objc3_task_get_completion(cancelled.native_handle(), &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
  assert(profile_task_entered == 2 && profile_task_cleaned == 2);
}
#endif

static int profile_async_suite(void) {
  objc3_executor_t *executor = NULL;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  profile_frame_suite(executor);
#ifdef __cplusplus
  profile_cpp_suite(executor);
#endif
  objc3_executor_release(executor);
  return 0;
}
#endif
