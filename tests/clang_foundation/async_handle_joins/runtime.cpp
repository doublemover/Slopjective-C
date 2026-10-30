#include "objc3_runtime_services.h"
#include <cassert>
#include <cstring>
#include <new>
#include <initializer_list>

static bool reject_allocation, reject_copy, reject_aligned_allocation, reject_optional_copy;
extern "C" void *__wrap__ZnwmRKSt9nothrow_t(
    std::size_t size, const std::nothrow_t &) noexcept {
  if (reject_allocation) return nullptr;
  try { return ::operator new(size); } catch (...) { return nullptr; }
}
extern "C" void *__wrap__ZnwmSt11align_val_tRKSt9nothrow_t(
    std::size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
  if (reject_aligned_allocation) return nullptr;
  try { return ::operator new(size, alignment); } catch (...) { return nullptr; }
}
extern "C" objc3_async_frame __objc3_task_join_frame(objc3_task_t *);
extern "C" objc3_async_frame __objc3_task_group_join_frame(objc3_task_group_t *);

static constexpr char int_key[] = "objc3.async.v1:_ZTSi";
static const objc3_async_result_type int_type{
    sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION, sizeof(int),
    alignof(int), OBJC3_ASYNC_VALUE, 0, int_key, sizeof(int_key)-1, nullptr};
static objc3_service_status copy_int(void *out, const void *in) {
  if (reject_copy) return OBJC3_SERVICE_OUT_OF_MEMORY;
  std::memcpy(out, in, sizeof(int)); return OBJC3_SERVICE_OK;
}
static objc3_service_status move_int(void *out, void *in) { return copy_int(out,in); }
static void destroy_int(void *) {}
static const objc3_value_ops int_ops{sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
    sizeof(int), alignof(int), copy_int, move_int, destroy_int};
struct Source { bool ready = false; unsigned destroyed = 0, resumes = 0; int value = 42; };
static void source_destroy(void *context) { ++static_cast<Source *>(context)->destroyed; }
static void source_resume(objc3_task_t *owner, void *context) {
  auto &source = *static_cast<Source *>(context); ++source.resumes;
  if (!source.ready && !objc3_task_is_cancel_requested(owner)) return;
  objc3_completion completion{};
  completion.struct_size=sizeof(completion); completion.version=OBJC3_SERVICE_ABI_VERSION;
  completion.kind=objc3_task_is_cancel_requested(owner) ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS;
  completion.has_payload=completion.kind==OBJC3_COMPLETION_SUCCESS;
  completion.payload=&source.value; completion.payload_ops=&int_ops; completion.result_type=&int_type;
  if (!completion.has_payload) { completion.payload=nullptr; completion.payload_ops=nullptr; completion.result_type=nullptr; }
  assert(objc3_task_complete(owner,&completion)==OBJC3_SERVICE_OK);
}
static void pump(objc3_executor_t *executor, uint32_t limit=UINT32_MAX) {
  uint32_t count=0; assert(objc3_executor_pump(executor,limit,&count)==OBJC3_SERVICE_OK);
}
static objc3_task_t *start_source(objc3_executor_t *executor, Source &source, bool typed=true) {
  objc3_task_start_options options{}; options.struct_size=sizeof(options);
  options.version=OBJC3_SERVICE_ABI_VERSION; options.executor=executor;
  options.frame=&source; options.resume=source_resume; options.destroy=source_destroy;
  options.result_type=typed ? &int_type : nullptr;
  objc3_task_t *task=nullptr; assert(objc3_task_start(&options,&task)==OBJC3_SERVICE_OK); return task;
}
static objc3_task_t *start_frame(objc3_executor_t *executor, objc3_async_frame &frame) {
  objc3_task_t *task=nullptr;
  assert(objc3_async_frame_start(&frame,executor,nullptr,&task)==OBJC3_SERVICE_OK);
  assert(!frame.context); return task;
}
static objc3_completion view(objc3_task_t *task) {
  objc3_completion completion{}; assert(objc3_task_get_completion(task,&completion)==OBJC3_SERVICE_OK); return completion;
}
static void service_error(objc3_task_t *task, objc3_service_status expected) {
  auto completion=view(task); assert(completion.kind==OBJC3_COMPLETION_ERROR);
  objc3_service_status actual=OBJC3_SERVICE_OK;
  assert(objc3_async_error_get_service(completion.error,&actual)==OBJC3_SERVICE_OK && actual==expected);
}
static void validation_and_disposal(objc3_executor_t *executor) {
  objc3_async_frame untouched{}; untouched.context=reinterpret_cast<void *>(42);
  auto before=untouched;
  assert(objc3_async_frame_from_task(nullptr,&untouched)==OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(std::memcmp(&before,&untouched,sizeof(before))==0);
  assert(objc3_async_frame_from_group(nullptr,&untouched)==OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(std::memcmp(&before,&untouched,sizeof(before))==0);
  Source source;
  auto *task=start_source(executor,source,false); pump(executor);
  assert(objc3_async_frame_from_task(task,&untouched)==OBJC3_SERVICE_NOT_READY);
  assert(std::memcmp(&before,&untouched,sizeof(before))==0);
  source.ready=true; assert(objc3_task_schedule_resume(task)==OBJC3_SERVICE_OK); pump(executor);
  objc3_async_frame frame{}; assert(objc3_async_frame_from_task(task,&frame)==OBJC3_SERVICE_OK);
  objc3_task_release(task);
  auto *joined=start_frame(executor,frame); pump(executor);
  int value=0; assert(objc3_task_copy_result(joined,&int_type,&int_ops,&value)==OBJC3_SERVICE_OK && value==42);
  objc3_task_release(joined); assert(source.destroyed==1);

  Source pending; task=start_source(executor,pending); pump(executor);
  assert(objc3_async_frame_from_task(task,&frame)==OBJC3_SERVICE_OK);
  assert(objc3_async_frame_dispose(&frame)==OBJC3_SERVICE_OK);
  assert(!objc3_task_is_cancel_requested(task) && pending.destroyed==0);
  objc3_task_release(task); assert(pending.destroyed==1);
}
static void dependency_and_copy_failures(objc3_executor_t *executor) {
  for (bool cancel : {false,true}) {
    Source source; auto *task=start_source(executor,source); pump(executor);
    objc3_async_frame frame{}; assert(objc3_async_frame_from_task(task,&frame)==OBJC3_SERVICE_OK);
    auto *joined=start_frame(executor,frame); pump(executor);
    objc3_completion none{}; assert(objc3_task_get_completion(joined,&none)==OBJC3_SERVICE_NOT_READY);
    if (cancel) {
      objc3_task_request_cancel(joined); pump(executor);
      assert(view(joined).kind==OBJC3_COMPLETION_CANCELLED);
      assert(!objc3_task_is_cancel_requested(task));
    }
    source.ready=true; assert(objc3_task_schedule_resume(task)==OBJC3_SERVICE_OK); pump(executor);
    if (!cancel) assert(view(joined).kind==OBJC3_COMPLETION_SUCCESS);
    objc3_task_release(joined); objc3_task_release(task); assert(source.destroyed==1);
  }
  Source source; source.ready=true; auto *task=start_source(executor,source); pump(executor);
  objc3_async_frame frame{}; assert(objc3_async_frame_from_task(task,&frame)==OBJC3_SERVICE_OK);
  auto *joined=start_frame(executor,frame);
  reject_copy=true; pump(executor); reject_copy=false;
  service_error(joined,OBJC3_SERVICE_OUT_OF_MEMORY);
  assert(view(task).kind==OBJC3_COMPLETION_SUCCESS);
  objc3_task_release(joined); objc3_task_release(task);
}
static void factory_failures(objc3_executor_t *executor) {
  Source source; auto *task=start_source(executor,source); pump(executor);
  objc3_async_frame frame{};
  reject_allocation=true;
  assert(objc3_async_frame_from_task(task,&frame)==OBJC3_SERVICE_OUT_OF_MEMORY && !frame.context);
  frame=__objc3_task_join_frame(task);
  reject_allocation=false;
  assert(objc3_async_frame_validate(&frame)==OBJC3_SERVICE_OK);
  auto *failed=start_frame(executor,frame); pump(executor);
  service_error(failed,OBJC3_SERVICE_OUT_OF_MEMORY); objc3_task_release(failed);
  frame=__objc3_task_join_frame(nullptr); failed=start_frame(executor,frame); pump(executor);
  service_error(failed,OBJC3_SERVICE_INVALID_ARGUMENT); objc3_task_release(failed);
  frame=__objc3_task_group_join_frame(nullptr); failed=start_frame(executor,frame);
  objc3_task_request_cancel(failed); pump(executor);
  assert(view(failed).kind==OBJC3_COMPLETION_CANCELLED); objc3_task_release(failed);
  objc3_task_release(task);
}
static void group_join(objc3_executor_t *executor) {
  for (bool cancel : {false,true}) {
    objc3_task_group_t *group=nullptr; assert(objc3_task_group_create(nullptr,&group)==OBJC3_SERVICE_OK);
    Source source; auto *task=start_source(executor,source);
    assert(objc3_task_group_add(group,task)==OBJC3_SERVICE_OK);
    objc3_async_frame frame{}; assert(objc3_async_frame_from_group(group,&frame)==OBJC3_SERVICE_OK);
    // Construction is lazy: registration remains open until first resume.
    Source extra; extra.ready=true; auto *other=start_source(executor,extra);
    assert(objc3_task_group_add(group,other)==OBJC3_SERVICE_OK);
    auto *joined=start_frame(executor,frame); pump(executor);
    assert(objc3_task_group_add(group,other)==OBJC3_SERVICE_CLOSED);
    if (cancel) {
      objc3_task_request_cancel(joined); pump(executor);
      assert(view(joined).kind==OBJC3_COMPLETION_CANCELLED);
      assert(!objc3_task_is_cancel_requested(task));
    }
    source.ready=true; assert(objc3_task_schedule_resume(task)==OBJC3_SERVICE_OK); pump(executor);
    if (!cancel) {
      auto completion=view(joined); assert(completion.kind==OBJC3_COMPLETION_SUCCESS && !completion.has_payload);
      assert(completion.result_type->kind==OBJC3_ASYNC_VOID && source.destroyed==1 && extra.destroyed==1);
      assert(std::strcmp(completion.result_type->identity,"objc3.async.v1:_ZTSv")==0 && !completion.result_type->identity_domain);
    }
    assert(objc3_task_group_join(group)==OBJC3_SERVICE_OK);
    objc3_task_release(joined); objc3_task_release(task); objc3_task_release(other); objc3_task_group_release(group);
  }
  objc3_task_group_t *empty=nullptr; assert(objc3_task_group_create(nullptr,&empty)==OBJC3_SERVICE_OK);
  objc3_async_frame frame{}; assert(objc3_async_frame_from_group(empty,&frame)==OBJC3_SERVICE_OK);
  objc3_task_group_release(empty); auto *joined=start_frame(executor,frame); pump(executor);
  assert(view(joined).kind==OBJC3_COMPLETION_SUCCESS); objc3_task_release(joined);
}

struct MaybeInt { unsigned char tag; int value; };
static unsigned some_destroyed, none_destroyed;
static objc3_service_status copy_maybe(void *out, const void *in) {
  if (reject_optional_copy) return OBJC3_SERVICE_OUT_OF_MEMORY;
  std::memcpy(out,in,sizeof(MaybeInt)); return OBJC3_SERVICE_OK;
}
static objc3_service_status move_maybe(void *out, void *in) { return copy_maybe(out,in); }
static void destroy_maybe(void *value) {
  (static_cast<MaybeInt *>(value)->tag ? some_destroyed : none_destroyed)++;
}
static const objc3_value_ops maybe_ops{sizeof(objc3_value_ops),OBJC3_VALUE_OPS_VERSION,
    sizeof(MaybeInt),alignof(MaybeInt),copy_maybe,move_maybe,destroy_maybe};
static constexpr char maybe_key[]="objc3.async.v1:_ZTS8MaybeInt";
static const objc3_async_result_type maybe_type{sizeof(objc3_async_result_type),
    OBJC3_ASYNC_ABI_VERSION,sizeof(MaybeInt),alignof(MaybeInt),OBJC3_ASYNC_VALUE,0,
    maybe_key,sizeof(maybe_key)-1,nullptr};
static objc3_async_group_next_plan next_plan() {
  return {sizeof(objc3_async_group_next_plan),1,&int_type,&maybe_type,&maybe_ops,
          offsetof(MaybeInt,value),offsetof(MaybeInt,tag),1,0};
}
static objc3_task_t *start_next(objc3_executor_t *executor, objc3_task_group_t *group) {
  auto plan=next_plan(); objc3_async_frame frame{};
  assert(objc3_async_frame_from_group_next(group,&plan,&frame)==OBJC3_SERVICE_OK);
  return start_frame(executor,frame);
}
static void check_next(objc3_executor_t *executor,objc3_task_group_t *group,
                        bool present,int expected=0) {
  auto *task=start_next(executor,group);pump(executor);
  auto completion=view(task);assert(completion.kind==OBJC3_COMPLETION_SUCCESS);
  auto &value=*static_cast<const MaybeInt *>(completion.payload);
  assert(!!value.tag==present);if(present)assert(value.value==expected);
  objc3_task_release(task);
}
static void next_validation_and_fifo(objc3_executor_t *executor) {
  objc3_task_group_t *group=nullptr;assert(objc3_task_group_create(nullptr,&group)==OBJC3_SERVICE_OK);
  auto plan=next_plan();objc3_async_frame output{};output.context=reinterpret_cast<void *>(42);auto before=output;
  assert(objc3_async_frame_from_group_next(nullptr,&plan,&output)==OBJC3_SERVICE_INVALID_ARGUMENT);
  for(int invalid=0;invalid!=5;++invalid) {
    auto bad=plan;
    if(invalid==0)bad.version=2;
    if(invalid==1)bad.tag_offset=sizeof(MaybeInt);
    if(invalid==2)bad.payload_offset=UINT64_MAX;
    if(invalid==3)bad.tag_offset=bad.payload_offset;
    if(invalid==4)bad.reserved=1;
    assert(objc3_async_frame_from_group_next(group,&bad,&output)==
           (invalid==0?OBJC3_SERVICE_UNSUPPORTED_VERSION:OBJC3_SERVICE_INVALID_ARGUMENT));
    assert(std::memcmp(&output,&before,sizeof(before))==0);
  }
  reject_allocation=true;
  assert(objc3_async_frame_from_group_next(group,&plan,&output)==OBJC3_SERVICE_OUT_OF_MEMORY);
  reject_allocation=false;assert(std::memcmp(&output,&before,sizeof(before))==0);
  Source first,second;first.value=19;second.value=23;
  auto *a=start_source(executor,first),*b=start_source(executor,second);pump(executor);
  assert(objc3_task_group_add(group,a)==OBJC3_SERVICE_OK);
  // The lazy factory must copy identity bytes, descriptors and operation tables.
  char element_key[sizeof(int_key)];std::memcpy(element_key,int_key,sizeof(int_key));
  auto type=int_type;type.identity=element_key;auto result=maybe_type;auto ops=maybe_ops;
  plan.element_type=&type;plan.result_type=&result;plan.result_ops=&ops;
  objc3_async_frame frame{};assert(objc3_async_frame_from_group_next(group,&plan,&frame)==OBJC3_SERVICE_OK);
  std::memset(element_key,'?',sizeof(element_key));type.kind=OBJC3_ASYNC_VOID;ops.copy_init=nullptr;result.kind=OBJC3_ASYNC_VOID;
  assert(objc3_task_group_add(group,b)==OBJC3_SERVICE_OK);
  auto *next=start_frame(executor,frame);pump(executor);
  assert(objc3_task_group_add(group,a)==OBJC3_SERVICE_CLOSED);
  second.ready=true;assert(objc3_task_schedule_resume(b)==OBJC3_SERVICE_OK);pump(executor);
  assert(static_cast<const MaybeInt *>(view(next).payload)->value==23);objc3_task_release(next);
  first.ready=true;assert(objc3_task_schedule_resume(a)==OBJC3_SERVICE_OK);pump(executor);
  // A different canonical type rejects without taking the ready item.
  auto mismatch=next_plan();auto wrong=int_type;wrong.identity="objc3.async.v1:_ZTSj";mismatch.element_type=&wrong;
  assert(objc3_async_frame_from_group_next(group,&mismatch,&frame)==OBJC3_SERVICE_OK);
  next=start_frame(executor,frame);pump(executor);service_error(next,OBJC3_SERVICE_TYPE_MISMATCH);objc3_task_release(next);
  check_next(executor,group,true,19);check_next(executor,group,false);
  assert(objc3_task_group_join(group)==OBJC3_SERVICE_OK);
  objc3_task_release(a);objc3_task_release(b);objc3_task_group_release(group);
  assert(first.destroyed==1 && second.destroyed==1);
}
static void next_cancellation_and_failures(objc3_executor_t *executor) {
  for(bool reserved:{false,true}) {
    Source source;source.ready=reserved;auto *task=start_source(executor,source);pump(executor);
    objc3_task_group_t *group=nullptr;assert(objc3_task_group_create(nullptr,&group)==OBJC3_SERVICE_OK);
    assert(objc3_task_group_add(group,task)==OBJC3_SERVICE_OK);
    auto *next=start_next(executor,group);pump(executor,1);
    objc3_task_request_cancel(next);pump(executor);assert(view(next).kind==OBJC3_COMPLETION_CANCELLED);
    objc3_task_release(next);assert(!objc3_task_is_cancel_requested(task));
    if(!reserved) {source.ready=true;assert(objc3_task_schedule_resume(task)==OBJC3_SERVICE_OK);pump(executor);}
    check_next(executor,group,!reserved,42);
    assert(objc3_task_group_join(group)==OBJC3_SERVICE_OK);
    objc3_task_release(task);objc3_task_group_release(group);
  }
  for(int failure=0;failure!=3;++failure) {
    Source source;source.ready=true;auto *task=start_source(executor,source);pump(executor);
    objc3_task_group_t *group=nullptr;assert(objc3_task_group_create(nullptr,&group)==OBJC3_SERVICE_OK);
    assert(objc3_task_group_add(group,task)==OBJC3_SERVICE_OK);
    auto *next=start_next(executor,group);pump(executor,1);
    reject_copy=failure==0;reject_optional_copy=failure==1;reject_aligned_allocation=failure==2;
    pump(executor);reject_copy=reject_optional_copy=reject_aligned_allocation=false;
    service_error(next,OBJC3_SERVICE_OUT_OF_MEMORY);objc3_task_release(next);
    check_next(executor,group,false);
    assert(objc3_task_group_join(group)==OBJC3_SERVICE_OK);
    objc3_task_release(task);objc3_task_group_release(group);
  }
  objc3_task_group_t *empty=nullptr;assert(objc3_task_group_create(nullptr,&empty)==OBJC3_SERVICE_OK);
  auto plan=next_plan();objc3_async_frame lazy{};
  assert(objc3_async_frame_from_group_next(empty,&plan,&lazy)==OBJC3_SERVICE_OK);
  objc3_task_group_release(empty);auto *next=start_frame(executor,lazy);pump(executor);
  assert(!static_cast<const MaybeInt *>(view(next).payload)->tag);objc3_task_release(next);
}

static void error_resume(objc3_task_t *owner,void *) {
  objc3_completion completion{};completion.struct_size=sizeof(completion);
  completion.version=OBJC3_SERVICE_ABI_VERSION;completion.kind=OBJC3_COMPLETION_ERROR;
  assert(objc3_async_error_create_service(OBJC3_SERVICE_FAILED,&completion.error)==OBJC3_SERVICE_OK);
  assert(objc3_task_complete(owner,&completion)==OBJC3_SERVICE_OK);
  objc3_error_release(completion.error);
}
static void next_terminal_outcomes(objc3_executor_t *executor) {
  for(bool cancelled:{false,true}) {
    Source source;objc3_task_t *task=nullptr;
    if(cancelled) {task=start_source(executor,source);objc3_task_request_cancel(task);}
    else {
      objc3_task_start_options options{};options.struct_size=sizeof(options);
      options.version=OBJC3_SERVICE_ABI_VERSION;options.executor=executor;
      options.resume=error_resume;options.result_type=&int_type;
      assert(objc3_task_start(&options,&task)==OBJC3_SERVICE_OK);
    }
    pump(executor);objc3_task_group_t *group=nullptr;
    assert(objc3_task_group_create(nullptr,&group)==OBJC3_SERVICE_OK);
    assert(objc3_task_group_add(group,task)==OBJC3_SERVICE_OK);
    auto *next=start_next(executor,group);pump(executor);
    if(cancelled)assert(view(next).kind==OBJC3_COMPLETION_CANCELLED);
    else {service_error(next,OBJC3_SERVICE_FAILED);assert(view(next).error==view(task).error);}
    const objc3_async_result_type *declared=nullptr;
    assert(objc3_task_get_result_type(next,&declared)==OBJC3_SERVICE_OK);
    assert(objc3_async_result_type_equal(declared,&maybe_type));
    assert(objc3_task_group_join(group)==OBJC3_SERVICE_OK);
    objc3_task_release(task);objc3_task_group_release(group);objc3_task_release(next);
  }
}
int main() {
  objc3_executor_t *executor=nullptr; assert(objc3_executor_create(1,&executor)==OBJC3_SERVICE_OK);
  validation_and_disposal(executor); dependency_and_copy_failures(executor);
  factory_failures(executor); group_join(executor);
  next_validation_and_fifo(executor);next_cancellation_and_failures(executor);
  next_terminal_outcomes(executor);
  objc3_executor_release(executor);
}
