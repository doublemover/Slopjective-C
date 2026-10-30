#include "objc3_runtime_services.h"
#include "runtime/concurrency/service_internal.h"
#include "runtime/concurrency/service_group.h"
#include "runtime/concurrency/service_actor.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <condition_variable>
#include <limits>
#include <mutex>
#include <thread>
#include <vector>

static objc3_service_status copy_int(void *destination, const void *source) {
  *static_cast<int *>(destination) = *static_cast<const int *>(source);
  return OBJC3_SERVICE_OK;
}
static objc3_service_status move_int(void *destination, void *source) {
  *static_cast<int *>(destination) = *static_cast<int *>(source);
  return OBJC3_SERVICE_OK;
}
static void destroy_int(void *) {}
static const objc3_value_ops int_ops = {
    sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION, sizeof(int), alignof(int),
    copy_int, move_int, destroy_int};
static objc3_service_status copy_bool(void *destination, const void *source) {
  *static_cast<bool *>(destination) = *static_cast<const bool *>(source);
  return OBJC3_SERVICE_OK;
}
static objc3_service_status move_bool(void *destination, void *source) {
  *static_cast<bool *>(destination) = *static_cast<bool *>(source);
  return OBJC3_SERVICE_OK;
}
static void destroy_bool(void *) {}
static const objc3_value_ops bool_ops = {
    sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION, sizeof(bool), alignof(bool),
    copy_bool, move_bool, destroy_bool};
static std::atomic<int> empty_copies{0};
static std::atomic<int> empty_destroys{0};
static objc3_service_status copy_empty(void *, const void *) {
  ++empty_copies;
  return OBJC3_SERVICE_OK;
}
static objc3_service_status move_empty(void *, void *) {
  return OBJC3_SERVICE_OK;
}
static void destroy_empty(void *) { ++empty_destroys; }
static const objc3_value_ops empty_ops = {
    sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION, 0, 1,
    copy_empty, move_empty, destroy_empty};

struct ReentrantErrorPayload {
  objc3_task_t *target{};
  std::atomic<int> *destroyed{};
  std::atomic<int> *completion_status{};
};

static objc3_service_status copy_reentrant_error(void *destination,
                                                const void *source) {
  *static_cast<ReentrantErrorPayload *>(destination) =
      *static_cast<const ReentrantErrorPayload *>(source);
  return OBJC3_SERVICE_OK;
}

static objc3_service_status move_reentrant_error(void *destination,
                                                void *source) {
  *static_cast<ReentrantErrorPayload *>(destination) =
      *static_cast<ReentrantErrorPayload *>(source);
  return OBJC3_SERVICE_OK;
}

static objc3_service_status complete_with_int_error(objc3_task_t *task,
                                                    int value) {
  objc3_error_t *error = nullptr;
  objc3_service_status status = objc3_error_create(&value, &int_ops, &error);
  if (status != OBJC3_SERVICE_OK) return status;
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_ERROR;
  completion.error = error;
  status = objc3_task_complete(task, &completion);
  objc3_error_release(error);
  return status;
}

static void destroy_reentrant_error(void *opaque) {
  auto *payload = static_cast<ReentrantErrorPayload *>(opaque);
  if (payload->destroyed) ++*payload->destroyed;
  if (payload->target && payload->completion_status)
    *payload->completion_status = complete_with_int_error(payload->target, 219);
}

static const objc3_value_ops reentrant_error_ops = {
    sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
    sizeof(ReentrantErrorPayload), alignof(ReentrantErrorPayload),
    copy_reentrant_error, move_reentrant_error, destroy_reentrant_error};

namespace {

struct ResumeGate {
  std::mutex mutex;
  std::condition_variable condition;
  bool started{};
  bool proceed{};
};

struct CallbackGate {
  std::mutex mutex;
  std::condition_variable condition;
  bool entered{};
  bool proceed{};
};

struct Frame {
  objc3_executor_t *executor{};
  int resumes{};
  int value{};
  bool complete_inside_resume{};
  std::atomic<int> *destroyed{};
  std::atomic<bool> *destroyed_during_resume{};
  ResumeGate *gate{};
  CallbackGate *destroy_gate{};
};

void destroy_frame(void *opaque) {
  auto *frame = static_cast<Frame *>(opaque);
  if (frame->destroyed_during_resume && frame->destroyed_during_resume->load())
    frame->destroyed_during_resume->store(true);
  if (frame->destroyed) ++*frame->destroyed;
  if (frame->destroy_gate) {
    std::unique_lock<std::mutex> lock(frame->destroy_gate->mutex);
    frame->destroy_gate->entered = true;
    frame->destroy_gate->condition.notify_all();
    frame->destroy_gate->condition.wait(lock,
                                        [&] { return frame->destroy_gate->proceed; });
  }
  delete frame;
}

void resume_task(objc3_task_t *task, void *opaque) {
  auto *frame = static_cast<Frame *>(opaque);
  ++frame->resumes;
  if (frame->gate) {
    std::unique_lock<std::mutex> lock(frame->gate->mutex);
    frame->gate->started = true;
    frame->gate->condition.notify_all();
    frame->gate->condition.wait(lock, [&] { return frame->gate->proceed; });
  }
  if (frame->complete_inside_resume) {
    int result = frame->value;
    objc3_completion completion{};
    completion.struct_size = sizeof(completion);
    completion.version = OBJC3_SERVICE_ABI_VERSION;
    completion.kind = OBJC3_COMPLETION_SUCCESS;
    completion.has_payload = 1;
    completion.payload = &result;
    completion.payload_ops = &int_ops;
    assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
    if (frame->destroyed_during_resume && frame->destroyed)
      assert(frame->destroyed->load() == 0);
  }
}

struct Results {
  std::atomic<int> calls{0};
  std::atomic<int> kind{-1};
  std::atomic<int> has_payload{-1};
  std::atomic<int> value{-1};
  objc3_task_t *task{};
  std::atomic<int> reentry_status{-1};
  std::atomic<int> saw_error{0};
  std::atomic<int> error_value{-1};
};

struct BoolResult { std::atomic<int> calls{0}; std::atomic<int> value{1}; };
void record_bool(void *opaque, const objc3_completion *completion) {
  auto *result = static_cast<BoolResult *>(opaque);
  ++result->calls;
  assert(completion->has_payload == 1);
  result->value = *static_cast<const bool *>(completion->payload) ? 1 : 0;
}

void record_result(void *opaque, const objc3_completion *completion) {
  auto *results = static_cast<Results *>(opaque);
  ++results->calls;
  results->kind = completion->kind;
  results->has_payload = completion->has_payload;
  results->saw_error = completion->error ? 1 : 0;
  if (completion->error) {
    const void *payload = objc3_error_payload(completion->error);
    const objc3_value_ops *ops = objc3_error_payload_ops(completion->error);
    assert(payload && ops && objc3_value_ops_validate(ops) == OBJC3_SERVICE_OK);
    results->error_value = *static_cast<const int *>(payload);
  }
  if (completion->has_payload)
    results->value = *static_cast<const int *>(completion->payload);
  if (results->task) {
    results->reentry_status = objc3_task_schedule_resume(results->task);
    assert(objc3_task_wait(results->task) == OBJC3_SERVICE_OK);
  }
}

objc3_executor_t *manual_executor() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  return executor;
}

objc3_task_t *start_task(objc3_executor_t *executor, Frame *frame,
                         Results *results = nullptr) {
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = frame;
  options.resume = resume_task;
  options.destroy = destroy_frame;
  options.completion = results ? record_result : nullptr;
  options.completion_context = results;
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  if (results) results->task = task;
  return task;
}

struct ExecutorWaitTrace {
  objc3_task_t *task{};
  objc3_service_status wait_status{OBJC3_SERVICE_CLOSED};
  objc3_service_status quiescent_status{OBJC3_SERVICE_CLOSED};
};

void wait_from_executor(void *opaque) {
  auto *trace = static_cast<ExecutorWaitTrace *>(opaque);
  trace->wait_status = objc3_task_wait(trace->task);
  trace->quiescent_status = objc3_task_wait_quiescent(trace->task);
}

void test_same_executor_pending_wait_returns_not_ready() {
  auto *executor = manual_executor();
  ExecutorWaitTrace trace;
  // This callback precedes the task's queued resume on the same serial
  // executor. Blocking here would prevent that resume from completing.
  assert(objc3_executor_enqueue(executor, wait_from_executor, &trace) ==
         OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  trace.task = start_task(executor, new Frame{executor, 0, 7, true, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && destroyed == 0);
  assert(trace.wait_status == OBJC3_SERVICE_NOT_READY);
  assert(trace.quiescent_status == OBJC3_SERVICE_NOT_READY);
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && destroyed == 1);
  assert(objc3_executor_enqueue(executor, wait_from_executor, &trace) ==
         OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && trace.wait_status == OBJC3_SERVICE_OK);
  assert(trace.quiescent_status == OBJC3_SERVICE_OK);
  objc3_task_release(trace.task);
  objc3_executor_release(executor);
}

void wait_then_complete_self(objc3_task_t *task, void *opaque) {
  auto *frame = static_cast<Frame *>(opaque);
  assert(objc3_task_wait(task) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_task_wait_quiescent(task) == OBJC3_SERVICE_NOT_READY);
  resume_task(task, frame);
  assert(objc3_task_wait(task) == OBJC3_SERVICE_OK);
}

void test_task_pending_self_wait_returns_not_ready() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = new Frame{executor, 0, 11, true, &destroyed};
  options.resume = wait_then_complete_self;
  options.destroy = destroy_frame;
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  // The owner of a manual destination must remain able to pump its work.
  assert(objc3_task_wait(task) == OBJC3_SERVICE_NOT_READY);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && destroyed == 1);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_zero_payload_and_reentry() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  Results result;
  auto *frame = new Frame{executor, 0, 0, true, &destroyed};
  auto *task = start_task(executor, frame, &result);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && result.calls == 1);
  assert(result.kind == OBJC3_COMPLETION_SUCCESS && result.has_payload == 1);
  assert(result.value == 0 && destroyed == 1);
  assert(result.reentry_status == OBJC3_SERVICE_ALREADY_COMPLETED);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_false_payload_is_present() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  BoolResult result;
  assert(objc3_task_add_waiter(task, record_bool, &result) == OBJC3_SERVICE_OK);
  bool false_value = false;
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.has_payload = 1;
  completion.payload = &false_value;
  completion.payload_ops = &bool_ops;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  assert(result.calls == 1 && result.value == 0);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_invalid_value_descriptors_and_presence_rules() {
  auto invalid_ops = int_ops;
  invalid_ops.version = 99;
  assert(objc3_value_ops_validate(&invalid_ops) ==
         OBJC3_SERVICE_UNSUPPORTED_VERSION);
  int payload = 6;
  objc3_error_t *error = reinterpret_cast<objc3_error_t *>(1);
  assert(objc3_error_create(&payload, &invalid_ops, &error) ==
         OBJC3_SERVICE_UNSUPPORTED_VERSION);
  assert(error == nullptr);

  auto missing_move = int_ops;
  missing_move.move_init = nullptr;
  assert(objc3_value_ops_validate(&missing_move) == OBJC3_SERVICE_INVALID_ARGUMENT);
  error = reinterpret_cast<objc3_error_t *>(1);
  assert(objc3_error_create(&payload, &missing_move, &error) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(error == nullptr);
  auto invalid_alignment = int_ops;
  invalid_alignment.payload_alignment = 3;
  assert(objc3_value_ops_validate(&invalid_alignment) == OBJC3_SERVICE_INVALID_ARGUMENT);
  error = reinterpret_cast<objc3_error_t *>(1);
  assert(objc3_error_create(&payload, &invalid_alignment, &error) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(error == nullptr);

  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);

  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.has_payload = 1;
  completion.payload = &payload;
  completion.payload_ops = &invalid_ops;
  assert(objc3_task_complete(task, &completion) ==
         OBJC3_SERVICE_UNSUPPORTED_VERSION);
  completion.has_payload = 0;
  completion.payload_ops = nullptr;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_INVALID_ARGUMENT);

  completion.kind = OBJC3_COMPLETION_CANCELLED;
  completion.payload = nullptr;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void record_empty(void *opaque, const objc3_completion *completion) {
  auto *calls = static_cast<std::atomic<int> *>(opaque);
  assert(completion->kind == OBJC3_COMPLETION_SUCCESS);
  assert(completion->has_payload == 1 && completion->payload);
  assert(completion->payload_ops && completion->payload_ops->payload_size == 0);
  ++*calls;
}

void test_present_zero_sized_payload() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  std::atomic<int> calls{0};
  assert(objc3_task_add_waiter(task, record_empty, &calls) == OBJC3_SERVICE_OK);
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.has_payload = 1;
  completion.payload = nullptr; // a zero-sized present value needs no source storage
  completion.payload_ops = &empty_ops;
  empty_copies = 0;
  empty_destroys = 0;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  assert(calls == 1 && empty_copies == 1 && empty_destroys == 0);
  objc3_task_release(task);
  assert(empty_destroys == 1);
  objc3_executor_release(executor);
}

struct WaiterReleaseContext {
  objc3_task_t *task;
  int observed{-1};
  objc3_service_status resume_status{OBJC3_SERVICE_OK};
};

void release_task_then_read(void *opaque, const objc3_completion *completion) {
  auto *context = static_cast<WaiterReleaseContext *>(opaque);
  objc3_task_release(context->task); // drops the caller's final handle
  assert(objc3_task_wait(context->task) == OBJC3_SERVICE_OK);
  context->resume_status = objc3_task_schedule_resume(context->task);
  assert(completion->has_payload == 1);
  context->observed = *static_cast<const int *>(completion->payload);
}

void test_terminal_waiter_holds_task_lifetime() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  int result = 64;
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.has_payload = 1;
  completion.payload = &result;
  completion.payload_ops = &int_ops;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  WaiterReleaseContext context{task};
  assert(objc3_task_add_waiter(task, release_task_then_read, &context) ==
         OBJC3_SERVICE_OK);
  assert(context.observed == 64);
  assert(context.resume_status == OBJC3_SERVICE_ALREADY_COMPLETED);
  assert(destroyed == 1);
  objc3_executor_release(executor);
}

struct PumpProbe {
  objc3_executor_t *executor;
  std::mutex mutex;
  std::condition_variable condition;
  bool entered{};
  bool proceed{};
  objc3_service_status recursive_status{OBJC3_SERVICE_OK};
  std::atomic<int> active{0};
  std::atomic<int> max_active{0};
};

void pump_probe_callback(void *opaque) {
  auto *probe = static_cast<PumpProbe *>(opaque);
  const int active = ++probe->active;
  int maximum = probe->max_active.load();
  while (active > maximum &&
         !probe->max_active.compare_exchange_weak(maximum, active)) {}
  uint32_t recursive_executed = 99;
  probe->recursive_status = objc3_executor_pump(
      probe->executor, 1, &recursive_executed);
  assert(recursive_executed == 0);
  {
    std::unique_lock<std::mutex> lock(probe->mutex);
    probe->entered = true;
    probe->condition.notify_all();
    probe->condition.wait(lock, [&] { return probe->proceed; });
  }
  --probe->active;
}

void test_manual_pump_is_serial_and_reentrant_safe() {
  auto *executor = manual_executor();
  PumpProbe probe{executor};
  assert(objc3_executor_enqueue(executor, pump_probe_callback, &probe) ==
         OBJC3_SERVICE_OK);
  uint32_t first_executed = 0;
  objc3_service_status first_status{};
  std::thread first([&] {
    first_status = objc3_executor_pump(executor, 1, &first_executed);
  });
  {
    std::unique_lock<std::mutex> lock(probe.mutex);
    probe.condition.wait(lock, [&] { return probe.entered; });
  }
  uint32_t second_executed = 99;
  const auto second_status = objc3_executor_pump(executor, 1, &second_executed);
  assert(second_status == OBJC3_SERVICE_NOT_READY && second_executed == 0);
  {
    std::lock_guard<std::mutex> lock(probe.mutex);
    probe.proceed = true;
  }
  probe.condition.notify_all();
  first.join();
  assert(first_status == OBJC3_SERVICE_OK && first_executed == 1);
  assert(probe.recursive_status == OBJC3_SERVICE_NOT_READY);
  assert(probe.max_active == 1);
  objc3_executor_release(executor);
}

void release_executor_from_callback(void *opaque) {
  objc3_executor_release(static_cast<objc3_executor_t *>(opaque));
}

void test_manual_pump_last_release_from_callback() {
  auto *executor = manual_executor();
  assert(objc3_executor_enqueue(executor, release_executor_from_callback,
                               executor) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1);
  // The pump's temporary lifetime reference keeps the executor valid until
  // both its callback and the pump serialization lock have been released.
}

void test_start_closed_executor_failure_keeps_frame_with_caller() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  {
    objc3::services::ExecutorQueueLock lock(executor->mutex);
    executor->stopping = true; // closed-path injection without invalid handle use
  }
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = frame;
  options.resume = resume_task;
  options.destroy = destroy_frame;
  objc3_task_t *task = reinterpret_cast<objc3_task_t *>(1);
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_CLOSED);
  assert(task == nullptr && destroyed == 0);
  assert(executor->public_references.load() == 1);
  destroy_frame(frame); // frame ownership stayed with the caller
  assert(destroyed == 1);
  objc3_executor_release(executor);
}

void test_suspension_resume_and_competing_completers() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  Results waiter;
  assert(objc3_task_add_waiter(task, record_result, &waiter) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(frame->resumes == 1 && destroyed == 0);
  assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(frame->resumes == 2);

  int first = 17, second = 23;
  auto complete = [task](int *value) {
    objc3_completion completion{};
    completion.struct_size = sizeof(completion);
    completion.version = OBJC3_SERVICE_ABI_VERSION;
    completion.kind = OBJC3_COMPLETION_SUCCESS;
    completion.has_payload = 1;
    completion.payload = value;
    completion.payload_ops = &int_ops;
    return objc3_task_complete(task, &completion);
  };
  objc3_service_status one{}, two{};
  std::thread a([&] { one = complete(&first); });
  std::thread b([&] { two = complete(&second); });
  a.join(); b.join();
  assert((one == OBJC3_SERVICE_OK && two == OBJC3_SERVICE_ALREADY_COMPLETED) ||
         (two == OBJC3_SERVICE_OK && one == OBJC3_SERVICE_ALREADY_COMPLETED));
  assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_ALREADY_COMPLETED);
  assert(objc3_task_wait(task) == OBJC3_SERVICE_OK);
  assert(waiter.calls == 1 && waiter.has_payload == 1);
  assert(objc3_task_add_waiter(task, record_result, &waiter) == OBJC3_SERVICE_OK);
  assert(waiter.calls == 2);
  assert(destroyed == 1);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_error_outcome_owns_error() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  Results result;
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame, &result);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);

  int payload = 71;
  objc3_error_t *error = nullptr;
  assert(objc3_error_create(&payload, &int_ops, &error) == OBJC3_SERVICE_OK);
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_ERROR;
  completion.error = error;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  objc3_error_release(error); // The task retains the stored error.
  assert(result.calls == 1 && result.kind == OBJC3_COMPLETION_ERROR);
  assert(result.saw_error == 1 && result.error_value == 71);
  assert(result.has_payload == 0 && destroyed == 1);
  assert(objc3_task_add_waiter(task, record_result, &result) == OBJC3_SERVICE_OK);
  assert(result.calls == 2 && result.error_value == 71);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

struct WorkerSignal {
  std::mutex mutex;
  std::condition_variable condition;
  bool ready{};
};

void signal_worker(void *opaque) {
  auto *signal = static_cast<WorkerSignal *>(opaque);
  std::lock_guard<std::mutex> lock(signal->mutex);
  signal->ready = true;
  // The waiter may destroy this stack state as soon as it reacquires the lock.
  // Finish notifying before handing that lifetime back to the waiter.
  signal->condition.notify_one();
}

void test_worker_executor_self_release() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(0, &executor) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  ResumeGate gate;
  auto *frame = new Frame{executor, 0, 9, true, &destroyed, nullptr, &gate};
  auto *task = start_task(executor, frame);
  WorkerSignal signal;
  assert(objc3_executor_enqueue(executor, signal_worker, &signal) ==
         OBJC3_SERVICE_OK);
  {
    std::unique_lock<std::mutex> lock(gate.mutex);
    gate.condition.wait(lock, [&] { return gate.started; });
  }
  objc3_task_release(task);
  objc3_executor_release(executor);
  {
    std::lock_guard<std::mutex> lock(gate.mutex);
    gate.proceed = true;
  }
  gate.condition.notify_all();
  std::unique_lock<std::mutex> lock(signal.mutex);
  signal.condition.wait(lock, [&] { return signal.ready; });
  lock.unlock();
  // The task's final executor release occurs on the worker after the queued
  // barrier. It must detach safely instead of joining itself or freeing early.
  for (int i = 0; i < 100 && destroyed.load() == 0; ++i)
    std::this_thread::yield();
  assert(destroyed == 1);
}

void test_queued_lifetime_and_cancellation() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  Results result;
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame, &result);
  objc3_task_request_cancel(task);
  assert(objc3_task_is_cancel_requested(task));
  objc3_task_release(task); // queued resume owns the task and frame
  assert(destroyed == 0);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && destroyed == 1 && result.calls == 1);
  assert(result.kind == OBJC3_COMPLETION_CANCELLED && result.has_payload == 0);
  objc3_executor_release(executor);
}

void test_frame_destroy_after_active_resume() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  std::atomic<bool> destroyed_during_resume{false};
  auto *frame = new Frame{executor, 0, 41, true, &destroyed,
                          &destroyed_during_resume};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  assert(!destroyed_during_resume);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

struct GroupWaiterContext {
  objc3::services::TaskGroup *group;
  CallbackGate *gate;
  objc3_service_status self_join_status{OBJC3_SERVICE_OK};
};

void blocked_group_waiter(void *opaque, const objc3_completion *) {
  auto *context = static_cast<GroupWaiterContext *>(opaque);
  context->self_join_status = objc3::services::task_group_join(context->group);
  std::unique_lock<std::mutex> lock(context->gate->mutex);
  context->gate->entered = true;
  context->gate->condition.notify_all();
  context->gate->condition.wait(lock, [&] { return context->gate->proceed; });
}

struct GroupJoinThread {
  objc3::services::TaskGroup *group;
  std::atomic<bool> started{false};
  std::atomic<bool> finished{false};
  objc3_service_status status{OBJC3_SERVICE_OK};
};

void test_group_join_waits_for_frame_and_observers() {
  using namespace objc3::services;
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(0, &executor) == OBJC3_SERVICE_OK);
  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  ResumeGate resume_gate;
  CallbackGate callback_gate, destroy_gate;
  auto *frame = new Frame{executor, 0, 3, true, &destroyed, nullptr,
                          &resume_gate, &destroy_gate};
  auto *task = start_task(executor, frame);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_OK);
  GroupWaiterContext waiter{group, &callback_gate};
  assert(objc3_task_add_waiter(task, blocked_group_waiter, &waiter) ==
         OBJC3_SERVICE_OK);
  assert(task_group_close(group) == OBJC3_SERVICE_OK);
  GroupJoinThread join{group};
  std::thread joiner([&] {
    join.started = true;
    join.status = task_group_join(group);
    join.finished = true;
  });
  while (!join.started.load()) std::this_thread::yield();

  {
    std::unique_lock<std::mutex> lock(resume_gate.mutex);
    resume_gate.condition.wait(lock, [&] { return resume_gate.started; });
    resume_gate.proceed = true;
  }
  resume_gate.condition.notify_all();
  {
    std::unique_lock<std::mutex> lock(callback_gate.mutex);
    callback_gate.condition.wait(lock, [&] { return callback_gate.entered; });
    assert(waiter.self_join_status == OBJC3_SERVICE_NOT_READY);
    assert(!join.finished.load());
    callback_gate.proceed = true;
  }
  callback_gate.condition.notify_all();
  {
    std::unique_lock<std::mutex> lock(destroy_gate.mutex);
    destroy_gate.condition.wait(lock, [&] { return destroy_gate.entered; });
    assert(!join.finished.load());
    destroy_gate.proceed = true;
  }
  destroy_gate.condition.notify_all();
  joiner.join();
  assert(join.status == OBJC3_SERVICE_OK && join.finished);
  assert(destroyed == 1);
  task_group_release(group);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_group_release_with_live_child() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_OK);
  auto *completer = objc3_task_retain(task);
  task_group_release(group); // observer keeps the group alive until completion
  objc3_task_release(task);
  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(completer, &cancelled) == OBJC3_SERVICE_OK);
  assert(objc3_task_wait_quiescent(completer) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  objc3_task_release(completer);
  objc3_executor_release(executor);
}

void test_group_add_racing_close() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  std::atomic<bool> go{false};
  objc3_service_status add_status{};
  std::thread add([&] {
    while (!go.load()) std::this_thread::yield();
    add_status = task_group_add_task(group, task);
  });
  std::thread close([&] {
    while (!go.load()) std::this_thread::yield();
    assert(task_group_close(group) == OBJC3_SERVICE_OK);
  });
  go = true;
  add.join(); close.join();
  assert(add_status == OBJC3_SERVICE_OK || add_status == OBJC3_SERVICE_CLOSED);
  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  assert(task_group_join(group) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  task_group_release(group);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_group_rejects_duplicate_task_association() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *task = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);

  std::atomic<bool> go{false};
  objc3_service_status statuses[2]{};
  std::thread first([&] {
    while (!go.load()) std::this_thread::yield();
    statuses[0] = task_group_add_task(group, task);
  });
  std::thread second([&] {
    while (!go.load()) std::this_thread::yield();
    statuses[1] = task_group_add_task(group, task);
  });
  go = true;
  first.join();
  second.join();
  assert((statuses[0] == OBJC3_SERVICE_OK &&
          statuses[1] == OBJC3_SERVICE_INVALID_ARGUMENT) ||
         (statuses[1] == OBJC3_SERVICE_OK &&
          statuses[0] == OBJC3_SERVICE_INVALID_ARGUMENT));

  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(task_group_close(group) == OBJC3_SERVICE_OK);
  objc3_task_t *completed = nullptr;
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_OK);
  assert(completed == task);
  objc3_task_release(completed);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_CLOSED);
  assert(completed == nullptr);
  assert(task_group_join(group) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  task_group_release(group);
  objc3_task_release(task);
  objc3_executor_release(executor);

  // A task that is already terminal must also be rejected on its second
  // association; duplicate observers otherwise leave a phantom entry pending.
  executor = manual_executor();
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  task = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(task_group_close(group) == OBJC3_SERVICE_OK);
  assert(task_group_join(group) == OBJC3_SERVICE_OK);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_OK);
  assert(completed == task);
  objc3_task_release(completed);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_CLOSED);
  assert(destroyed == 2);
  task_group_release(group);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

struct GroupNextWaitContext {
  objc3::services::TaskGroup *group;
  objc3_service_status status{OBJC3_SERVICE_OK};
  objc3_task_t *task{reinterpret_cast<objc3_task_t *>(1)};
};

void try_group_next_from_child_callback(void *opaque, const objc3_completion *) {
  auto *context = static_cast<GroupNextWaitContext *>(opaque);
  context->status = objc3::services::task_group_next_completed(
      context->group, &context->task);
}

void test_group_next_completed_self_wait_returns_not_ready() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *task = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  GroupNextWaitContext context{group};
  assert(objc3_task_add_waiter(task, try_group_next_from_child_callback, &context) ==
         OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_OK);
  assert(task_group_close(group) == OBJC3_SERVICE_OK);
  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  assert(context.status == OBJC3_SERVICE_NOT_READY && context.task == nullptr);
  objc3_task_t *completed = nullptr;
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_OK);
  assert(completed == task);
  objc3_task_release(completed);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_CLOSED);
  assert(task_group_join(group) == OBJC3_SERVICE_OK && destroyed == 1);
  task_group_release(group);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_nested_group_failure_cancels_ancestor_siblings() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *root = nullptr, *middle = nullptr, *leaf = nullptr;
  assert(task_group_create(nullptr, &root) == OBJC3_SERVICE_OK);
  assert(task_group_create(root, &middle) == OBJC3_SERVICE_OK);
  assert(task_group_create(middle, &leaf) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *failure = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  auto *middle_sibling = start_task(executor,
      new Frame{executor, 0, 0, false, &destroyed});
  auto *root_sibling = start_task(executor,
      new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 3, &executed) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(leaf, failure) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(middle, middle_sibling) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(root, root_sibling) == OBJC3_SERVICE_OK);
  assert(task_group_close(root) == OBJC3_SERVICE_OK);

  int payload = 101;
  objc3_error_t *error = nullptr;
  assert(objc3_error_create(&payload, &int_ops, &error) == OBJC3_SERVICE_OK);
  objc3_completion failed{};
  failed.struct_size = sizeof(failed);
  failed.version = OBJC3_SERVICE_ABI_VERSION;
  failed.kind = OBJC3_COMPLETION_ERROR;
  failed.error = error;
  assert(objc3_task_complete(failure, &failed) == OBJC3_SERVICE_OK);
  objc3_error_release(error);
  assert(objc3_task_is_cancel_requested(middle_sibling));
  assert(objc3_task_is_cancel_requested(root_sibling));

  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(middle_sibling, &cancelled) == OBJC3_SERVICE_OK);
  assert(objc3_task_complete(root_sibling, &cancelled) == OBJC3_SERVICE_OK);
  assert(task_group_join(root) == OBJC3_SERVICE_OK);
  assert(destroyed == 3);
  task_group_release(leaf);
  task_group_release(middle);
  task_group_release(root);
  objc3_task_release(failure);
  objc3_task_release(middle_sibling);
  objc3_task_release(root_sibling);
  objc3_executor_release(executor);
}

void test_nested_group_failure_after_parent_release_is_safe() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *root = nullptr, *child = nullptr;
  assert(task_group_create(nullptr, &root) == OBJC3_SERVICE_OK);
  assert(task_group_create(root, &child) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *task = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(child, task) == OBJC3_SERVICE_OK);
  task_group_release(root); // Observer keeps child alive, parent may be destroyed.

  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  assert(objc3_task_wait_quiescent(task) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  task_group_release(child);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

struct FailurePropagationGate {
  std::mutex mutex;
  std::condition_variable condition;
  bool entered{false};
  bool proceed{false};
};

void pause_first_failure_propagation(void *opaque) {
  auto *gate = static_cast<FailurePropagationGate *>(opaque);
  std::unique_lock<std::mutex> lock(gate->mutex);
  gate->entered = true;
  gate->condition.notify_all();
  gate->condition.wait(lock, [&] { return gate->proceed; });
}

struct DropGroupReferenceOnce {
  objc3::services::TaskGroup **group;
  std::atomic<bool> dropped{false};
};

void release_public_group_reference(void *opaque) {
  auto *context = static_cast<DropGroupReferenceOnce *>(opaque);
  if (context->dropped.exchange(true)) return;
  assert(context->group && *context->group);
  objc3::services::TaskGroup *released = *context->group;
  *context->group = nullptr;
  task_group_release(released);
}

void test_completed_group_registration_survives_reentrant_last_owner_release() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *task = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(complete_with_int_error(task, 321) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  DropGroupReferenceOnce drop{&group};
  task_group_test_set_failure_propagation_hook(release_public_group_reference,
                                               &drop);
  // The terminal observer runs inside add(), drops the last caller owner, and
  // releases its own association before add() finishes registration cleanup.
  const auto status = task_group_add_task(group, task);
  task_group_test_set_failure_propagation_hook(nullptr, nullptr);
  assert(status == OBJC3_SERVICE_OK && drop.dropped && !group);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_service_status_errors_are_owned_and_distinct_from_cancellation() {
  for (int value = OBJC3_SERVICE_INVALID_ARGUMENT;
       value <= OBJC3_SERVICE_TYPE_MISMATCH; ++value) {
    const auto status = static_cast<objc3_service_status>(value);
    if (status == OBJC3_SERVICE_CANCELLED) continue;
    objc3_error_t *error = nullptr, *same = nullptr;
    assert(objc3_async_error_create_service(status, &error) == OBJC3_SERVICE_OK);
    assert(objc3_async_error_create_service(status, &same) == OBJC3_SERVICE_OK);
    assert(error && same == error);
    const auto *ops = objc3_error_payload_ops(error);
    assert(ops && objc3_value_ops_validate(ops) == OBJC3_SERVICE_OK);
    assert(*static_cast<const objc3_service_status *>(objc3_error_payload(error)) == status);
    auto *retained = objc3_error_retain(error);
    objc3_error_release(error);
    objc3_error_release(same);
    objc3_service_status recovered = OBJC3_SERVICE_OK;
    assert(objc3_async_error_get_service(retained, &recovered) == OBJC3_SERVICE_OK);
    assert(recovered == status);
    objc3_error_release(retained);
  }
  objc3_error_t *invalid = reinterpret_cast<objc3_error_t *>(1);
  assert(objc3_async_error_create_service(OBJC3_SERVICE_OK, &invalid) ==
         OBJC3_SERVICE_INVALID_ARGUMENT && !invalid);
  assert(objc3_async_error_create_service(OBJC3_SERVICE_CANCELLED, &invalid) ==
         OBJC3_SERVICE_INVALID_ARGUMENT && !invalid);
  int ordinary_payload = 91;
  objc3_error_t *ordinary = nullptr;
  assert(objc3_error_create(&ordinary_payload, &int_ops, &ordinary) == OBJC3_SERVICE_OK);
  objc3_service_status unchanged = OBJC3_SERVICE_CLOSED;
  assert(objc3_async_error_get_service(ordinary, &unchanged) ==
         OBJC3_SERVICE_TYPE_MISMATCH && unchanged == OBJC3_SERVICE_CLOSED);
  objc3_error_release(ordinary);
}

void test_group_failure_release_defers_reentrant_destruction_until_unlocked() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *root = nullptr;
  TaskGroup *child = nullptr;
  assert(task_group_create(nullptr, &root) == OBJC3_SERVICE_OK);
  assert(task_group_create(root, &child) == OBJC3_SERVICE_OK);

  std::atomic<int> destroyed{0};
  std::atomic<int> reentrant_error_destroyed{0};
  std::atomic<int> reentrant_completion_status{-1};
  auto *root_error_owner = start_task(
      executor, new Frame{executor, 0, 0, false, &destroyed});
  auto *primary_failure = start_task(
      executor, new Frame{executor, 0, 0, false, &destroyed});
  auto *reentrant_failure = start_task(
      executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 3, &executed) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(root, root_error_owner) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(child, primary_failure) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(child, reentrant_failure) == OBJC3_SERVICE_OK);
  assert(task_group_close(root) == OBJC3_SERVICE_OK);

  ReentrantErrorPayload payload{reentrant_failure, &reentrant_error_destroyed,
                                &reentrant_completion_status};
  objc3_error_t *reentrant_error = nullptr;
  assert(objc3_error_create(&payload, &reentrant_error_ops, &reentrant_error) ==
         OBJC3_SERVICE_OK);
  objc3_completion initial_failure{};
  initial_failure.struct_size = sizeof(initial_failure);
  initial_failure.version = OBJC3_SERVICE_ABI_VERSION;
  initial_failure.kind = OBJC3_COMPLETION_ERROR;
  initial_failure.error = reentrant_error;
  assert(objc3_task_complete(root_error_owner, &initial_failure) ==
         OBJC3_SERVICE_OK);
  objc3_error_release(reentrant_error);
  objc3_task_release(root_error_owner);

  // The hook drops the public root reference while failure propagation still
  // owns an ancestor lease. Releasing that lease must happen after the tree's
  // publication lock is unlocked: root destruction releases the stored task,
  // whose error payload completes another task in this same group tree.
  DropGroupReferenceOnce drop_root{&root};
  task_group_test_set_failure_propagation_hook(
      release_public_group_reference, &drop_root);
  const objc3_service_status primary_status =
      complete_with_int_error(primary_failure, 218);
  task_group_test_set_failure_propagation_hook(nullptr, nullptr);

  assert(primary_status == OBJC3_SERVICE_OK);
  assert(root == nullptr);
  assert(reentrant_error_destroyed == 1);
  assert(reentrant_completion_status == OBJC3_SERVICE_OK);
  assert(task_group_join(child) == OBJC3_SERVICE_OK);
  task_group_release(child);
  objc3_task_release(reentrant_failure);
  objc3_task_release(primary_failure);
  assert(destroyed == 3);
  objc3_executor_release(executor);
}

struct FailurePoll {
  objc3::services::TaskGroup *group;
  objc3_task_t *root_sibling;
  std::atomic<bool> started{false};
  std::atomic<bool> finished{false};
  std::atomic<bool> saw_before_ancestor_cancel{false};
  objc3_service_status status{OBJC3_SERVICE_OK};
  objc3_task_t *observed{nullptr};
};

void test_concurrent_group_failures_publish_after_ancestor_cancellation() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *root = nullptr;
  assert(task_group_create(nullptr, &root) == OBJC3_SERVICE_OK);
  std::vector<TaskGroup *> chain{root};
  for (int depth = 0; depth < 32; ++depth) {
    TaskGroup *child = nullptr;
    assert(task_group_create(chain.back(), &child) == OBJC3_SERVICE_OK);
    chain.push_back(child);
  }
  TaskGroup *leaf = chain.back();
  std::atomic<int> destroyed{0};
  auto *first_failure = start_task(
      executor, new Frame{executor, 0, 0, false, &destroyed});
  auto *second_failure = start_task(
      executor, new Frame{executor, 0, 0, false, &destroyed});
  auto *root_sibling = start_task(
      executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 3, &executed) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(leaf, first_failure) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(leaf, second_failure) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(root, root_sibling) == OBJC3_SERVICE_OK);
  assert(task_group_close(root) == OBJC3_SERVICE_OK);

  FailurePropagationGate gate;
  task_group_test_set_failure_propagation_hook(
      pause_first_failure_propagation, &gate);
  auto complete_error = [](objc3_task_t *task, int code) {
    objc3_error_t *error = nullptr;
    assert(objc3_error_create(&code, &int_ops, &error) == OBJC3_SERVICE_OK);
    objc3_completion failure{};
    failure.struct_size = sizeof(failure);
    failure.version = OBJC3_SERVICE_ABI_VERSION;
    failure.kind = OBJC3_COMPLETION_ERROR;
    failure.error = error;
    const auto status = objc3_task_complete(task, &failure);
    objc3_error_release(error);
    return status;
  };

  objc3_service_status first_status = OBJC3_SERVICE_OK;
  objc3_service_status second_status = OBJC3_SERVICE_OK;
  std::atomic<bool> second_started{false};
  std::atomic<bool> second_finished{false};
  std::thread first([&] { first_status = complete_error(first_failure, 211); });
  {
    std::unique_lock<std::mutex> lock(gate.mutex);
    gate.condition.wait(lock, [&] { return gate.entered; });
  }
  FailurePoll poll{leaf, root_sibling};
  std::thread poller([&] {
    poll.started = true;
    poll.status = task_group_next_completed(leaf, &poll.observed);
    poll.saw_before_ancestor_cancel =
        !objc3_task_is_cancel_requested(root_sibling);
    poll.finished = true;
  });
  while (!poll.started.load()) std::this_thread::yield();
  std::thread second([&] {
    second_started = true;
    second_status = complete_error(second_failure, 212);
    second_finished = true;
  });
  while (!second_started.load()) std::this_thread::yield();
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  assert(!second_finished.load());
  assert(!poll.finished.load());

  {
    std::lock_guard<std::mutex> lock(gate.mutex);
    gate.proceed = true;
  }
  gate.condition.notify_all();
  first.join();
  second.join();
  poller.join();
  task_group_test_set_failure_propagation_hook(nullptr, nullptr);

  assert(first_status == OBJC3_SERVICE_OK);
  assert(second_status == OBJC3_SERVICE_OK);
  assert(poll.status == OBJC3_SERVICE_OK && poll.observed);
  assert(!poll.saw_before_ancestor_cancel.load());
  assert(objc3_task_is_cancel_requested(root_sibling));
  objc3_task_t *another = nullptr;
  assert(task_group_next_completed(leaf, &another) == OBJC3_SERVICE_OK);
  assert(another && another != poll.observed);
  objc3_task_release(another);
  objc3_task_release(poll.observed);

  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(root_sibling, &cancelled) == OBJC3_SERVICE_OK);
  assert(task_group_join(root) == OBJC3_SERVICE_OK);
  assert(destroyed == 3);
  for (auto it = chain.rbegin(); it != chain.rend(); ++it)
    task_group_release(*it);
  objc3_task_release(root_sibling);
  objc3_task_release(second_failure);
  objc3_task_release(first_failure);
  objc3_executor_release(executor);
}

void test_group_failure_cancels_siblings_before_delivery() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *first = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  auto *sibling = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 2, &executed) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, first) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, sibling) == OBJC3_SERVICE_OK);
  assert(task_group_close(group) == OBJC3_SERVICE_OK);

  int error_payload = 99;
  objc3_error_t *error = nullptr;
  assert(objc3_error_create(&error_payload, &int_ops, &error) ==
         OBJC3_SERVICE_OK);
  objc3_completion failed{};
  failed.struct_size = sizeof(failed);
  failed.version = OBJC3_SERVICE_ABI_VERSION;
  failed.kind = OBJC3_COMPLETION_ERROR;
  failed.error = error;
  assert(objc3_task_complete(first, &failed) == OBJC3_SERVICE_OK);
  assert(objc3_task_is_cancel_requested(sibling));
  objc3_error_release(error);

  objc3_task_t *completed = nullptr;
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_OK);
  assert(completed == first);
  Results error_result;
  assert(objc3_task_add_waiter(completed, record_result, &error_result) ==
         OBJC3_SERVICE_OK);
  assert(error_result.kind == OBJC3_COMPLETION_ERROR &&
         error_result.error_value == 99);
  objc3_task_release(completed);

  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(sibling, &cancelled) == OBJC3_SERVICE_OK);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_OK);
  assert(completed == sibling);
  objc3_task_release(completed);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_CLOSED);
  assert(completed == nullptr);
  assert(task_group_join(group) == OBJC3_SERVICE_OK);
  assert(destroyed == 2);
  task_group_release(group);
  objc3_task_release(first);
  objc3_task_release(sibling);
  objc3_executor_release(executor);
}

void test_recursive_group_close_and_join() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  TaskGroup *root = nullptr, *child = nullptr, *grandchild = nullptr;
  assert(task_group_create(nullptr, &root) == OBJC3_SERVICE_OK);
  assert(task_group_create(root, &child) == OBJC3_SERVICE_OK);
  assert(task_group_create(child, &grandchild) == OBJC3_SERVICE_OK);
  std::atomic<int> destroyed{0};
  auto *task = start_task(executor, new Frame{executor, 0, 0, false, &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(grandchild, task) == OBJC3_SERVICE_OK);
  assert(task_group_close(root) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(grandchild, task) == OBJC3_SERVICE_CLOSED);
  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  objc3_task_t *completed = nullptr;
  assert(task_group_next_completed(grandchild, &completed) == OBJC3_SERVICE_OK);
  assert(completed == task);
  objc3_task_release(completed);
  assert(task_group_join(root) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  task_group_release(grandchild);
  task_group_release(child);
  task_group_release(root);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void noop_executor_callback(void *opaque) {
  ++*static_cast<std::atomic<int> *>(opaque);
}

void test_executor_admission_and_queue_saturation() {
  std::vector<objc3_executor_t *> workers;
  for (int i = 0; i < 8; ++i) {
    objc3_executor_t *worker = nullptr;
    assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
    workers.push_back(worker);
  }
  objc3_executor_t *extra_worker = nullptr;
  assert(objc3_executor_create(0, &extra_worker) == OBJC3_SERVICE_NOT_READY);
  assert(extra_worker == nullptr);
  for (auto *worker : workers) objc3_executor_release(worker);
  objc3_executor_t *replacement = nullptr;
  assert(objc3_executor_create(0, &replacement) == OBJC3_SERVICE_OK);
  objc3_executor_release(replacement);

  auto *executor = manual_executor();
  std::atomic<int> callback_count{0};
  for (size_t i = 0; i < 1024; ++i)
    assert(objc3_executor_enqueue(executor, noop_executor_callback,
                                  &callback_count) == OBJC3_SERVICE_OK);
  assert(objc3_executor_enqueue(executor, noop_executor_callback,
                               &callback_count) == OBJC3_SERVICE_NOT_READY);

  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = frame;
  options.resume = resume_task;
  options.destroy = destroy_frame;
  objc3_task_t *task = reinterpret_cast<objc3_task_t *>(1);
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_NOT_READY);
  assert(task == nullptr && destroyed == 0);
  assert(executor->public_references.load() == 1);
  destroy_frame(frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1024, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1024 && callback_count == 1024 && destroyed == 1);
  objc3_executor_release(executor);
}

void test_saturated_resume_can_retry_without_lost_wakeup() {
  auto *executor = manual_executor();
  std::atomic<int> destroyed{0};
  auto *frame = new Frame{executor, 0, 0, false, &destroyed};
  auto *task = start_task(executor, frame);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  std::atomic<int> callback_count{0};
  for (size_t i = 0; i < 1024; ++i)
    assert(objc3_executor_enqueue(executor, noop_executor_callback,
                                  &callback_count) == OBJC3_SERVICE_OK);
  assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_executor_pump(executor, 512, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 512);
  assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(executor, 1024, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 513 && callback_count == 1024 && frame->resumes == 2);
  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  assert(destroyed == 1);
  objc3_task_release(task);
  objc3_executor_release(executor);
}

void test_refcount_saturation_fails_without_losing_ownership() {
  using namespace objc3::services;
  constexpr uint32_t maximum = std::numeric_limits<uint32_t>::max();
  std::atomic<uint32_t> primitive{maximum};
  assert(!try_retain(primitive) && primitive.load() == maximum);
  primitive.store(maximum - 1);
  assert(try_retain(primitive) && primitive.load() == maximum);
  assert(!try_retain(primitive) && primitive.load() == maximum);
  primitive.store(0);
  assert(!try_retain(primitive));

  auto *executor = manual_executor();
  const uint32_t executor_references = executor->references.load();
  executor->references.store(maximum);
  assert(objc3_executor_retain(executor) == nullptr);
  assert(executor->references.load() == maximum);
  executor->references.store(executor_references);

  const uint32_t public_references = executor->public_references.load();
  executor->public_references.store(maximum);
  assert(objc3_executor_retain(executor) == nullptr);
  assert(executor->references.load() == executor_references);
  executor->public_references.store(public_references);

  Actor *actor = reinterpret_cast<Actor *>(1);
  executor->public_references.store(maximum);
  assert(actor_create(executor, &actor) == OBJC3_SERVICE_NOT_READY);
  assert(actor == nullptr);
  executor->public_references.store(public_references);

  std::atomic<int> destroyed{0};
  auto *unsubmitted_frame = new Frame{executor, 0, 0, false, &destroyed};
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = unsubmitted_frame;
  options.resume = resume_task;
  options.destroy = destroy_frame;
  objc3_task_t *unsubmitted = nullptr;
  executor->public_references.store(maximum);
  assert(objc3_task_start(&options, &unsubmitted) == OBJC3_SERVICE_NOT_READY);
  assert(unsubmitted == nullptr && destroyed == 0);
  executor->public_references.store(public_references);
  destroy_frame(unsubmitted_frame); // start failure left the frame with caller
  assert(destroyed == 1);

  auto *task = start_task(executor, new Frame{executor, 0, 0, false,
                                               &destroyed});
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  const uint32_t task_references = task->references.load();
  task->references.store(maximum);
  assert(objc3_task_retain(task) == nullptr);
  assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_task_add_waiter(task, record_result, nullptr) ==
         OBJC3_SERVICE_NOT_READY);
  assert(task->references.load() == maximum);
  task->references.store(task_references);

  TaskGroup *group = nullptr;
  assert(task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  assert(task_group_test_set_references(group, maximum) == OBJC3_SERVICE_OK);
  assert(task_group_retain(group) == nullptr);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_NOT_READY);
  assert(task_group_test_set_references(group, 1) == OBJC3_SERVICE_OK);
  assert(task_group_add_task(group, task) == OBJC3_SERVICE_OK);

  objc3_completion cancelled{};
  cancelled.struct_size = sizeof(cancelled);
  cancelled.version = OBJC3_SERVICE_ABI_VERSION;
  cancelled.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &cancelled) == OBJC3_SERVICE_OK);
  assert(task_group_close(group) == OBJC3_SERVICE_OK);
  objc3_task_t *completed = nullptr;
  const uint32_t grouped_task_references = task->references.load();
  task->references.store(maximum);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_NOT_READY);
  assert(completed == nullptr);
  task->references.store(grouped_task_references);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_OK);
  assert(completed == task);
  objc3_task_release(completed);
  assert(task_group_next_completed(group, &completed) == OBJC3_SERVICE_CLOSED);
  assert(task_group_join(group) == OBJC3_SERVICE_OK);

  TaskGroup *parent = nullptr;
  TaskGroup *child = nullptr;
  assert(task_group_create(nullptr, &parent) == OBJC3_SERVICE_OK);
  assert(task_group_create(parent, &child) == OBJC3_SERVICE_OK);
  assert(task_group_test_set_references(child, maximum) == OBJC3_SERVICE_OK);
  assert(task_group_close(parent) == OBJC3_SERVICE_NOT_READY);
  assert(task_group_cancel(parent) == OBJC3_SERVICE_NOT_READY);
  assert(task_group_test_set_references(child, 2) == OBJC3_SERVICE_OK);
  assert(task_group_close(parent) == OBJC3_SERVICE_OK);
  assert(task_group_test_set_references(child, maximum) == OBJC3_SERVICE_OK);
  assert(task_group_join(parent) == OBJC3_SERVICE_NOT_READY);
  assert(task_group_test_set_references(child, 2) == OBJC3_SERVICE_OK);
  assert(task_group_join(parent) == OBJC3_SERVICE_OK);
  task_group_release(child);
  task_group_release(parent);

  task_group_release(group);
  objc3_task_release(task);
  assert(destroyed == 2);
  objc3_executor_release(executor);
}

struct ActorTrace {
  std::vector<int> sequence;
  int active{0};
  int max_active{0};
  int destroyed{0};
  objc3::services::Actor *cross_actor{nullptr};
  objc3::services::ActorHopFacts cross_facts{};
  objc3_service_status cross_status{OBJC3_SERVICE_OK};
  objc3::services::ActorHopFacts resume_facts{};
  objc3_service_status resume_status{OBJC3_SERVICE_OK};
};

struct ActorJobContext {
  ActorTrace *trace;
  objc3::services::Actor *actor;
  int marker;
  objc3::services::Actor *cross_actor;
  objc3::services::Actor *followup_actor;
};

void destroy_actor_test_job(void *opaque);

void actor_test_job(void *opaque) {
  auto *context = static_cast<ActorJobContext *>(opaque);
  auto *trace = context->trace;
  ++trace->active;
  if (trace->active > trace->max_active) trace->max_active = trace->active;
  trace->sequence.push_back(context->marker);
  objc3::services::CurrentActorFacts current{
      sizeof(objc3::services::CurrentActorFacts),
      objc3::services::kActorServiceVersion, 0, 0};
  assert(objc3::services::actor_current_facts(&current) == OBJC3_SERVICE_OK);
  assert(current.actor_identity ==
         objc3::services::actor_identity(context->actor));
  assert(current.executor_identity ==
         objc3::services::actor_executor_identity(context->actor));
  assert(objc3::services::executor_is_current_identity(
      current.executor_identity));
  assert(current.executor_identity ==
         objc3::services::actor_executor_identity(context->actor));
  assert(objc3::services::executor_is_current_identity(
      current.executor_identity));
  if (context->cross_actor) {
    trace->cross_facts = {sizeof(trace->cross_facts),
                          objc3::services::kActorServiceVersion, 0, 0,
                          0, 0, 0, 0};
    auto *next = new ActorJobContext{trace, context->cross_actor, 4, nullptr,
                                     context->actor};
    trace->cross_status = objc3::services::actor_enqueue(
        context->cross_actor, actor_test_job, next,
        [](void *value) {
          auto *job = static_cast<ActorJobContext *>(value);
          ++job->trace->destroyed;
          delete job;
        }, &trace->cross_facts);
    if (trace->cross_status != OBJC3_SERVICE_OK) delete next;
  }
  if (context->followup_actor) {
    trace->resume_facts = {sizeof(trace->resume_facts),
                           objc3::services::kActorServiceVersion, 0, 0,
                           0, 0, 0, 0};
    auto *next = new ActorJobContext{trace, context->followup_actor, 3,
                                     nullptr, nullptr};
    trace->resume_status = objc3::services::actor_enqueue(
        context->followup_actor, actor_test_job, next,
        destroy_actor_test_job, &trace->resume_facts);
    if (trace->resume_status != OBJC3_SERVICE_OK) delete next;
  }
  --trace->active;
}

void destroy_actor_test_job(void *opaque) {
  auto *context = static_cast<ActorJobContext *>(opaque);
  objc3::services::CurrentActorFacts current{
      sizeof(objc3::services::CurrentActorFacts),
      objc3::services::kActorServiceVersion, 0, 0};
  assert(objc3::services::actor_current_facts(&current) == OBJC3_SERVICE_OK);
  assert(current.actor_identity ==
         objc3::services::actor_identity(context->actor));
  assert(objc3::services::actor_join(context->actor) ==
         OBJC3_SERVICE_NOT_READY);
  ++context->trace->destroyed;
  delete context;
}

void test_actor_serial_mailbox_hops_and_cancellation() {
  using namespace objc3::services;
  auto *executor = manual_executor();
  Actor *first = nullptr;
  Actor *second = nullptr;
  assert(actor_create(executor, &first) == OBJC3_SERVICE_OK);
  assert(actor_create(executor, &second) == OBJC3_SERVICE_OK);
  assert(actor_identity(first) != actor_identity(second));
  assert(actor_executor_identity(first) == actor_executor_identity(second));

  ActorTrace trace;
  trace.cross_actor = second;
  ActorHopFacts facts{sizeof(ActorHopFacts), kActorServiceVersion};
  assert(actor_enqueue(first, actor_test_job,
                       new ActorJobContext{&trace, first, 1, second, nullptr},
                       destroy_actor_test_job, &facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == actor_identity(first));
  assert(facts.actor_hop == 1 && facts.executor_hop == 1);
  facts = {sizeof(ActorHopFacts), kActorServiceVersion};
  assert(actor_enqueue(first, actor_test_job,
                       new ActorJobContext{&trace, first, 2, nullptr, nullptr},
                       destroy_actor_test_job, &facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_hop == 1 && facts.executor_hop == 1);

  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1);
  assert((trace.sequence == std::vector<int>{1}));
  assert(trace.max_active == 1 && trace.active == 0);
  assert(trace.cross_status == OBJC3_SERVICE_OK);
  assert(trace.cross_facts.actor_hop == 1);
  assert(trace.cross_facts.executor_hop == 0);
  assert(trace.cross_facts.current_actor_identity == actor_identity(first));
  assert(trace.cross_facts.current_executor_identity ==
         actor_executor_identity(first));
  assert(trace.destroyed == 1);
  assert(actor_close(second) == OBJC3_SERVICE_OK);
  assert(actor_join(second) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert((trace.sequence == std::vector<int>{1, 4}));
  assert(trace.resume_status == OBJC3_SERVICE_OK);
  assert(trace.resume_facts.actor_hop == 1 &&
         trace.resume_facts.executor_hop == 0);
  assert(actor_join(second) == OBJC3_SERVICE_OK);
  assert(actor_close(first) == OBJC3_SERVICE_OK);
  assert(actor_join(first) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_executor_pump(executor, 2, &executed) == OBJC3_SERVICE_OK);
  assert((trace.sequence == std::vector<int>{1, 4, 2, 3}));
  assert(trace.destroyed == 4);
  assert(actor_join(first) == OBJC3_SERVICE_OK);

  Actor *cancelled = nullptr;
  assert(actor_create(executor, &cancelled) == OBJC3_SERVICE_OK);
  ActorTrace cancelled_trace;
  facts = {sizeof(ActorHopFacts), kActorServiceVersion};
  assert(actor_enqueue(cancelled, actor_test_job,
                       new ActorJobContext{&cancelled_trace, cancelled, 9,
                                           nullptr, nullptr},
                       destroy_actor_test_job, &facts) == OBJC3_SERVICE_OK);
  assert(actor_cancel(cancelled) == OBJC3_SERVICE_OK);
  assert(cancelled_trace.destroyed == 0 && cancelled_trace.sequence.empty());
  assert(actor_join(cancelled) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(cancelled_trace.destroyed == 1 && cancelled_trace.sequence.empty());
  assert(actor_join(cancelled) == OBJC3_SERVICE_OK);
  actor_release(cancelled);
  actor_release(second);
  actor_release(first);
  objc3_executor_release(executor);
}

} // namespace

void test_actor_scheduling();

int main() {
  test_actor_scheduling();
  test_completed_group_registration_survives_reentrant_last_owner_release();
  test_service_status_errors_are_owned_and_distinct_from_cancellation();
  test_same_executor_pending_wait_returns_not_ready();
  test_task_pending_self_wait_returns_not_ready();
  test_zero_payload_and_reentry();
  test_false_payload_is_present();
  test_invalid_value_descriptors_and_presence_rules();
  test_present_zero_sized_payload();
  test_terminal_waiter_holds_task_lifetime();
  test_manual_pump_is_serial_and_reentrant_safe();
  test_manual_pump_last_release_from_callback();
  test_start_closed_executor_failure_keeps_frame_with_caller();
  test_suspension_resume_and_competing_completers();
  test_error_outcome_owns_error();
  test_queued_lifetime_and_cancellation();
  test_frame_destroy_after_active_resume();
  test_group_join_waits_for_frame_and_observers();
  test_group_release_with_live_child();
  test_group_add_racing_close();
  test_group_rejects_duplicate_task_association();
  test_group_next_completed_self_wait_returns_not_ready();
  test_nested_group_failure_cancels_ancestor_siblings();
  test_nested_group_failure_after_parent_release_is_safe();
  test_concurrent_group_failures_publish_after_ancestor_cancellation();
  test_group_failure_release_defers_reentrant_destruction_until_unlocked();
  test_group_failure_cancels_siblings_before_delivery();
  test_recursive_group_close_and_join();
  test_executor_admission_and_queue_saturation();
  test_saturated_resume_can_retry_without_lost_wakeup();
  test_refcount_saturation_fails_without_losing_ownership();
  test_actor_serial_mailbox_hops_and_cancellation();
  test_worker_executor_self_release();
  std::puts("task services: all focused checks passed");
  return 0;
}
