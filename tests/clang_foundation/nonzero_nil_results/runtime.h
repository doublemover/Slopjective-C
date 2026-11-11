#ifndef OBJC3_NONZERO_NIL_RESULTS_H
#define OBJC3_NONZERO_NIL_RESULTS_H
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
#include <stdio.h>
#ifdef __cplusplus
#include <objc3c/runtime/objc3_task.hpp>
#endif

static int nil_constructed, nil_copied, nil_destroyed, nil_resource_closed;
static int nil_argument_evaluated, nil_foreign_released, nil_foreign_retained;
struct NilForeign { int claims; };
typedef const struct NilForeign *NilForeignRef;
#ifdef __cplusplus
extern "C" {
#endif
void nil_reject_frame_allocation(int);
int nil_frame_allocations(void);
void CFRelease(const void *pointer) {
  struct NilForeign *owner = (struct NilForeign *)pointer;
  assert(owner && owner->claims > 0);
  --owner->claims;
  ++nil_foreign_released;
}
const void *CFRetain(const void *pointer) {
  struct NilForeign *owner = (struct NilForeign *)pointer;
  assert(owner && owner->claims > 0);
  ++owner->claims;
  ++nil_foreign_retained;
  return pointer;
}
void nil_close_handle(int value) { (void)value; ++nil_resource_closed; }
#ifdef __cplusplus
}
#endif
typedef int NilOwnedHandle __attribute__((objc_requires_cleanup(nil_close_handle),
    objc_resource_family(NilHandles), objc_invalid(-1)));
struct NilResourceRecord { NilOwnedHandle value; };
struct NilResourceArray { NilOwnedHandle values[2]; };
struct NilResourceView { NilOwnedHandle *value; };
static NilForeignRef nil_argument(struct NilForeign *owner) {
  ++nil_argument_evaluated;
  return owner;
}

#ifdef __cplusplus
struct NilNontrivial {
  int value;
  explicit NilNontrivial(int value) : value(value) { ++nil_constructed; }
  NilNontrivial(const NilNontrivial &other) : value(other.value) { ++nil_copied; }
  ~NilNontrivial() noexcept { ++nil_destroyed; }
};
struct NilConstructorOnly {
  int value;
  explicit NilConstructorOnly(int value) : value(value) { ++nil_constructed; }
};
struct NilReferenceRecord { int &value; };
struct NilDefaultMember { int value = ++nil_constructed; };
struct NilResourceBase { NilOwnedHandle value; };
struct NilResourceDerived : NilResourceBase { int count; };
namespace {
struct NilPrivateValue {
  explicit NilPrivateValue(int) { ++nil_constructed; }
  NilPrivateValue(const NilPrivateValue &) { ++nil_copied; }
  ~NilPrivateValue() noexcept { ++nil_destroyed; }
};
}
#endif

__attribute__((objc_root_class)) @interface NilOwnedResultRoot
- (NilOwnedHandle)resourceFrame:(NilForeignRef __attribute__((cf_consumed)))owner __objc3_async_frame;
- (struct NilResourceRecord)resourceRecord __objc3_async_frame;
- (struct NilResourceArray)resourceArray __objc3_async_frame;
- (struct NilResourceView)resourceView __objc3_async_frame;
- (int)scalarFrame __objc3_async_frame;
- (NilOwnedHandle)isolatedResource:(NilForeignRef __attribute__((cf_consumed)))owner
    __attribute__((nothrow, objc_executor(main)));
#ifdef __cplusplus
- (NilNontrivial)valueFrame:(NilForeignRef __attribute__((cf_consumed)))owner __objc3_async_frame;
- (NilNontrivial)valueTask:(NilForeignRef __attribute__((cf_consumed)))owner __objc3_async;
- (NilConstructorOnly)constructorTask __objc3_async;
- (NilReferenceRecord)referenceFrame __objc3_async_frame;
- (NilDefaultMember)defaultTask __objc3_async;
- (NilPrivateValue)privateFrame __objc3_async_frame;
- (NilPrivateValue)privateTask __objc3_async;
- (NilOwnedHandle)resourceTask __objc3_async;
- (NilResourceRecord)recordTask __objc3_async;
- (NilResourceDerived)baseTask __objc3_async;
- (NilNontrivial)isolatedValue __attribute__((nothrow, objc_executor(main)));
#endif
@end

actor class NilOwnedActor : NilOwnedResultRoot
- (NilOwnedHandle)actorResource __attribute__((nothrow));
@end

static NilOwnedHandle nil_isolated_resource(NilOwnedResultRoot *receiver,
    NilForeignRef __attribute__((cf_consumed)) owner) __objc3_async_frame {
  return __objc3_await [receiver isolatedResource:owner];
}
static NilOwnedHandle nil_actor_resource(NilOwnedActor *receiver) __objc3_async_frame {
  return __objc3_await [receiver actorResource];
}
#ifdef __cplusplus
static NilNontrivial nil_isolated_value(NilOwnedResultRoot *receiver) __objc3_async_frame {
  return __objc3_await [receiver isolatedValue];
}
#endif

static int nil_free_entry_called;
static NilOwnedHandle nil_free_entry(NilOwnedResultRoot *argument)
    __attribute__((nothrow, objc_executor(main)));
static NilOwnedHandle __attribute__((nothrow)) nil_free_entry(NilOwnedResultRoot *argument) {
  assert(argument == 0);
  ++nil_free_entry_called;
  return 42;
}
static NilOwnedHandle nil_free_parent(void) __objc3_async_frame {
  return __objc3_await nil_free_entry((NilOwnedResultRoot *)0);
}

static void nil_pump(objc3_executor_t *executor) {
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
}
static void nil_assert_untouched(void) {
  assert(nil_constructed == 0 && nil_copied == 0 && nil_destroyed == 0);
  assert(nil_resource_closed == 0);
}
static void nil_check_failure(objc3_executor_t *executor, objc3_async_frame *frame,
                              int cancel, unsigned expected_size) {
  static int case_index;
  ++case_index;
  const objc3_service_status validation = objc3_async_frame_validate(frame);
  if (validation != OBJC3_SERVICE_OK)
    fprintf(stderr, "nil case %d: status=%d size=%u version=%u context=%p resume=%p destroy=%p type=%p\n",
        case_index, (int)validation, frame->struct_size, frame->version, frame->context,
        (void *)frame->resume, (void *)frame->destroy, (const void *)frame->result_type);
  assert(validation == OBJC3_SERVICE_OK);
  assert(frame->result_type->storage_size == expected_size);
  const objc3_async_result_type *original = frame->result_type;
  objc3_task_t *task = NULL;
  assert(objc3_async_frame_start(frame, executor, NULL, &task) == OBJC3_SERVICE_OK);
  if (cancel) objc3_task_request_cancel(task);
  nil_pump(executor);
  objc3_completion completion = {0};
  assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
  assert(!completion.has_payload && !completion.payload && !completion.payload_ops);
  if (cancel) assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
  else {
    assert(completion.kind == OBJC3_COMPLETION_ERROR);
    objc3_service_status status = OBJC3_SERVICE_OK;
    assert(objc3_async_error_get_service(completion.error, &status) == OBJC3_SERVICE_OK);
    assert(status == OBJC3_SERVICE_INVALID_ARGUMENT);
  }
  const objc3_async_result_type *declared = NULL;
  assert(objc3_task_get_result_type(task, &declared) == OBJC3_SERVICE_OK);
  assert(objc3_async_result_type_equal(original, declared));
  objc3_task_release(task);
  nil_assert_untouched();
}

static int nil_results_suite(void) {
  objc3_executor_t *executor = NULL;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  NilOwnedResultRoot *receiver = (NilOwnedResultRoot *)0;
  struct NilForeign foreign = {1};
  int before = nil_frame_allocations();
  nil_reject_frame_allocation(1);
  objc3_async_frame resource = [receiver resourceFrame:nil_argument(&foreign)];
  assert(foreign.claims == 0 && nil_argument_evaluated == 1 && nil_foreign_released == 1);
  assert(nil_frame_allocations() == before);
  nil_reject_frame_allocation(0);
  nil_check_failure(executor, &resource, 0, sizeof(NilOwnedHandle));
  assert(nil_foreign_released == 1);
  objc3_async_frame record = [receiver resourceRecord];
  nil_check_failure(executor, &record, 1, sizeof(struct NilResourceRecord));
  record = [receiver resourceRecord];
  assert(objc3_async_frame_dispose(&record) == OBJC3_SERVICE_OK);
  record = [receiver resourceArray];
  nil_check_failure(executor, &record, 0, sizeof(struct NilResourceArray));

#ifdef __cplusplus
  foreign.claims = 1;
  before = nil_frame_allocations();
  nil_reject_frame_allocation(1);
  objc3_async_frame value = [receiver valueFrame:nil_argument(&foreign)];
  assert(foreign.claims == 0 && nil_foreign_released == 2 && nil_argument_evaluated == 2);
  assert(nil_frame_allocations() == before);
  foreign.claims = 1;
  auto lazy = [receiver valueTask:nil_argument(&foreign)];
  assert(foreign.claims == 0 && nil_foreign_released == 3 && nil_argument_evaluated == 3);
  auto empty = [receiver valueTask:(NilForeignRef)nullptr];
  assert(nil_frame_allocations() == before && nil_foreign_released == 3);
  nil_reject_frame_allocation(0);
  nil_check_failure(executor, &value, 0, sizeof(NilNontrivial));
  // Native CF consumption skips null. The task constructor keeps its declared
  // type and does not invoke a missing default constructor or a result copier.
  auto task = std::move(lazy).start(executor);
  nil_pump(executor);
  bool failed = false;
  try { (void)task.result(); }
  catch (const objc3::service_failure &error) {
    failed = error.status() == OBJC3_SERVICE_INVALID_ARGUMENT;
  }
  assert(failed);
  value = std::move(empty).release_native();
  assert(objc3_async_frame_dispose(&value) == OBJC3_SERVICE_OK);
  assert(nil_foreign_released == 3);
  auto constructor = [receiver constructorTask];
  value = std::move(constructor).release_native();
  nil_check_failure(executor, &value, 0, sizeof(NilConstructorOnly));
  value = [receiver referenceFrame];
  nil_check_failure(executor, &value, 0, sizeof(NilReferenceRecord));
  auto defaults = [receiver defaultTask];
  value = std::move(defaults).release_native();
  nil_check_failure(executor, &value, 1, sizeof(NilDefaultMember));
  value = [receiver privateFrame];
  nil_check_failure(executor, &value, 0, sizeof(NilPrivateValue));
  auto private_task = [receiver privateTask];
  value = std::move(private_task).release_native();
  nil_check_failure(executor, &value, 0, sizeof(NilPrivateValue));
  auto resource_task = [receiver resourceTask];
  value = std::move(resource_task).release_native();
  nil_check_failure(executor, &value, 0, sizeof(NilOwnedHandle));
  auto record_task = [receiver recordTask];
  value = std::move(record_task).release_native();
  nil_check_failure(executor, &value, 0, sizeof(NilResourceRecord));
  auto base_task = [receiver baseTask];
  value = std::move(base_task).release_native();
  nil_check_failure(executor, &value, 0, sizeof(NilResourceDerived));
  nil_assert_untouched();
#endif

  // A plain scalar retains ordinary nil success, independently of the new
  // failure policy for values with construction/ownership invariants.
  objc3_async_frame scalar = [receiver scalarFrame];
  objc3_task_t *plain = NULL;
  assert(objc3_async_frame_start(&scalar, executor, NULL, &plain) == OBJC3_SERVICE_OK);
  nil_pump(executor);
  objc3_completion done = {0};
  assert(objc3_task_get_completion(plain, &done) == OBJC3_SERVICE_OK);
  assert(done.kind == OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload == 0);
  objc3_task_release(plain);
  scalar = [receiver resourceView];
  assert(objc3_async_frame_start(&scalar, executor, NULL, &plain) == OBJC3_SERVICE_OK);
  nil_pump(executor);
  assert(objc3_task_get_completion(plain, &done) == OBJC3_SERVICE_OK);
  assert(done.kind == OBJC3_COMPLETION_SUCCESS);
  assert(((const struct NilResourceView *)done.payload)->value == NULL);
  objc3_task_release(plain);
  nil_assert_untouched();

  // An awaited synchronous isolated method has the same nil-result contract.
  // Its independently pinned CF argument must be balanced without entering a
  // body, constructing a result, or hopping to the destination executor.
  foreign.claims = 1;
  scalar = nil_isolated_resource(receiver, nil_argument(&foreign));
  nil_check_failure(executor, &scalar, 0, sizeof(NilOwnedHandle));
  assert(foreign.claims == 0);
  foreign.claims = 1;
  scalar = nil_isolated_resource(receiver, nil_argument(&foreign));
  nil_check_failure(executor, &scalar, 1, sizeof(NilOwnedHandle));
  assert(foreign.claims == 0);
  foreign.claims = 1;
  scalar = nil_isolated_resource(receiver, nil_argument(&foreign));
  assert(objc3_async_frame_dispose(&scalar) == OBJC3_SERVICE_OK);
  assert(foreign.claims == 0);
#ifdef __cplusplus
  scalar = nil_isolated_value(receiver);
  nil_check_failure(executor, &scalar, 0, sizeof(NilNontrivial));
#endif
  assert(nil_foreign_released == nil_foreign_retained + nil_argument_evaluated);
  scalar = nil_actor_resource((NilOwnedActor *)0);
  nil_check_failure(executor, &scalar, 0, sizeof(NilOwnedHandle));
  nil_assert_untouched();

  // A free executor function's first ObjC argument is not its receiver. Nil is
  // a valid argument and must not trigger the method-only nil guard.
  objc3_executor_t *main_executor = NULL;
  assert(objc3_executor_get_main(&main_executor) == OBJC3_SERVICE_OK);
  scalar = nil_free_parent();
  assert(objc3_async_frame_start(&scalar, executor, NULL, &plain) == OBJC3_SERVICE_OK);
  for (int turn = 0; turn != 8; ++turn) {
    nil_pump(executor);
    nil_pump(main_executor);
  }
  assert(objc3_task_get_completion(plain, &done) == OBJC3_SERVICE_OK);
  assert(done.kind == OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload == 42);
  assert(nil_free_entry_called == 1);
  objc3_task_release(plain);
  objc3_executor_release(main_executor);
  objc3_executor_release(executor);
  return 0;
}
#endif
