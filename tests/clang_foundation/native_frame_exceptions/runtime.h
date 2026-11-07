#ifndef OBJC3_NATIVE_FRAME_EXCEPTIONS_H
#define OBJC3_NATIVE_FRAME_EXCEPTIONS_H
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
#ifdef __cplusplus
#include <objc3c/runtime/objc3_task.hpp>
extern "C" {
#endif
id class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
#ifdef __cplusplus
}
#endif
static objc3_executor_t *property_caller,*property_destination,*property_main;
@protocol Sendable @end
__attribute__((objc_root_class)) @interface PropertyMain <Sendable> {
  Class isa;
#if !__has_feature(objc_arc)
  int references;
#endif
}
+ (instancetype)new __attribute__((nothrow));
#if !__has_feature(objc_arc)
- (id)retain;
- (oneway void)release;
#endif
@end
@implementation PropertyMain
+ (instancetype)new {
  PropertyMain *value=class_createInstance(self,0);
#if !__has_feature(objc_arc)
  value->references=1;
#endif
  return value;
}
#if !__has_feature(objc_arc)
- (id)retain {++references;return self;}
- (oneway void)release {if(!--references)[self dealloc];}
#endif
- (void)dealloc {object_dispose(self);}
@end
static objc3_task_t *property_start(objc3_async_frame *frame) {
  objc3_task_t *task=0;assert(objc3_async_frame_start(frame,property_caller,0,&task)==OBJC3_SERVICE_OK);return task;
}
static objc3_completion property_finish(objc3_task_t *task,int caller_only) {
  objc3_completion done={0};
  for(int turn=0;turn!=64;++turn) {
    uint32_t count=0;assert(objc3_executor_pump(property_caller,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
    if(!caller_only) {
      assert(objc3_executor_pump(property_main,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
      assert(objc3_executor_pump(property_destination,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
    }
    objc3_service_status status=objc3_task_get_completion(task,&done);
    if(status==OBJC3_SERVICE_OK)return done;
    assert(status==OBJC3_SERVICE_NOT_READY);
  }
  assert(!"native frame exception did not complete");return done;
}
#if __has_feature(objc_exceptions)
static int eh_foreign(PropertyMain *object) __objc3_async_frame {@throw object;}
static void eh_check_foreign(void) {
  PropertyMain *object=[PropertyMain new];
  objc3_async_frame frame=eh_foreign(object);objc3_task_t *task=property_start(&frame);
  objc3_completion done=property_finish(task,0);objc3_service_status status=OBJC3_SERVICE_OK;
  assert(done.kind==OBJC3_COMPLETION_ERROR && done.error);
#ifdef __cplusplus
  // GNUstep Objective-C++ gives native ObjC exceptions a C++ carrier.
  bool caught=false;
  @try {objc3::task_detail::rethrow_completion(done);}
  @catch(PropertyMain *failure) {assert(failure==object);caught=true;}
  assert(caught);
#else
  assert(objc3_async_error_get_service(done.error,&status)==OBJC3_SERVICE_OK && status==OBJC3_SERVICE_INVALID_ARGUMENT);
#endif
  objc3_task_release(task);
#if !__has_feature(objc_arc)
  [object release];
#endif
}
#else
static void eh_check_foreign(void) {}
#endif
#ifdef __cplusplus
static bool eh_reject_error_payload;
void *operator new(__SIZE_TYPE__ size,std::align_val_t alignment,const std::nothrow_t &) noexcept {
  if(eh_reject_error_payload)return nullptr;
  try {return ::operator new(size,alignment);}catch(...){return nullptr;}
}
static int eh_live, eh_created, eh_destroyed, eh_copies, eh_moves;
static int eh_throw_copy, eh_throw_move, eh_setters, eh_stored, eh_receiver_calls, eh_native_order;
static int eh_failure_live,eh_failure_created,eh_failure_destroyed;
struct FrameFailure {
  int value;
  explicit FrameFailure(int v) noexcept : value(v) {++eh_failure_live;++eh_failure_created;}
  FrameFailure(const FrameFailure &v) noexcept : value(v.value) {++eh_failure_live;++eh_failure_created;}
  ~FrameFailure() noexcept {--eh_failure_live;++eh_failure_destroyed;assert(eh_failure_live>=0);}
};
static const FrameFailure *eh_failure_address;
[[noreturn]] static void eh_throw_failure(int value) {
  try {throw FrameFailure(value);}
  catch(const FrameFailure &failure) {eh_failure_address=&failure;throw;}
}
struct __attribute__((objc_unsafe_sendable)) FrameValue {
  int value;
  explicit FrameValue(int v) noexcept : value(v) {++eh_created;++eh_live;}
  FrameValue(const FrameValue &other) : value(other.value) {
    ++eh_copies;if(eh_throw_copy && eh_copies==eh_throw_copy)eh_throw_failure(111);
    ++eh_created;++eh_live;
  }
  FrameValue(FrameValue &&other) : value(other.value) {
    ++eh_moves;if(eh_throw_move && eh_moves==eh_throw_move)eh_throw_failure(112);
    other.value=-1;++eh_created;++eh_live;
  }
  ~FrameValue() noexcept {++eh_destroyed;--eh_live;assert(eh_live>=0);}
};
struct __attribute__((objc_unsafe_sendable)) FrameMoveOnly {
  int value;
  explicit FrameMoveOnly(int v) noexcept : value(v) {++eh_created;++eh_live;}
  FrameMoveOnly(const FrameMoveOnly &)=delete;
  FrameMoveOnly(FrameMoveOnly &&other) noexcept : value(other.value) {
    other.value=-1;++eh_created;++eh_live;
  }
  ~FrameMoveOnly() noexcept {++eh_destroyed;--eh_live;assert(eh_live>=0);}
};
struct FrameGuard {
  FrameGuard() noexcept {++eh_created;++eh_live;}
  ~FrameGuard() noexcept {++eh_destroyed;--eh_live;assert(eh_live>=0);}
};
@interface PropertyMain (NativeFrameExceptions)
@property(nonatomic) FrameValue plainFrameValue;
- (FrameValue)plainFrameValue;
- (void)setPlainFrameValue:(FrameValue)value;
@property(nonatomic) FrameValue frameValue;
- (FrameValue)frameValue __attribute__((objc_executor(main)));
- (void)setFrameValue:(FrameValue)value __attribute__((objc_executor(main)));
@property(nonatomic) FrameMoveOnly moveOnly;
- (FrameMoveOnly)moveOnly __attribute__((objc_executor(main)));
- (void)setMoveOnly:(FrameMoveOnly)value __attribute__((objc_executor(main)));
@property(nonatomic,readonly) int nativeFailure;
- (int)nativeFailure __attribute__((objc_executor(main)));
- (int)nativeFailureMessage __attribute__((objc_executor(main)));
@end
@implementation PropertyMain (NativeFrameExceptions)
- (FrameValue)plainFrameValue {return FrameValue(eh_stored);}
- (void)setPlainFrameValue:(FrameValue)value {eh_stored=value.value;}
- (FrameValue)frameValue {assert(objc3_executor_is_current(property_main));return FrameValue(eh_stored);}
- (void)setFrameValue:(FrameValue)value {
  assert(objc3_executor_is_current(property_main));++eh_setters;eh_stored=value.value;
  if(value.value==501)eh_throw_failure(501);
}
- (FrameMoveOnly)moveOnly {return FrameMoveOnly(eh_stored);}
- (void)setMoveOnly:(FrameMoveOnly)value {
  assert(objc3_executor_is_current(property_main));++eh_setters;eh_stored=value.value;
}
- (int)nativeFailure {assert(objc3_executor_is_current(property_main));FrameGuard guard;eh_throw_failure(601);}
- (int)nativeFailureMessage {assert(objc3_executor_is_current(property_main));FrameGuard guard;eh_throw_failure(602);}
@end
static PropertyMain *eh_mutating_receiver(PropertyMain *object,FrameValue *value) {
  ++eh_receiver_calls;value->value=99;return object;
}
static int eh_write_lvalue(PropertyMain *object,FrameValue *value) __objc3_async_frame {
  __objc3_await (eh_mutating_receiver(object,value).frameValue=*value);
  assert(value->value==99 && eh_stored==eh_native_order);return eh_stored;
}
static void eh_write_prvalue(PropertyMain *object,int value) __objc3_async_frame {
  __objc3_await (object.frameValue=FrameValue(value));
}
static void eh_write_moveonly(PropertyMain *object) __objc3_async_frame {
  __objc3_await (object.moveOnly=FrameMoveOnly(88));
}
static int eh_read_value(PropertyMain *object) __objc3_async_frame {
  FrameValue value=__objc3_await object.frameValue;return value.value;
}
static int eh_direct(int kind) __objc3_async_frame {
  FrameGuard guard;
  if(kind==0)eh_throw_failure(701);
  if(kind==1)throw objc3::task_cancelled();
  if(kind==2) {
    objc3_error_t *error=objc3::task_detail::service_error(OBJC3_SERVICE_OUT_OF_MEMORY);
    objc3::task_error failure(error);objc3_error_release(error);throw failure;
  }
  try {eh_throw_failure(702);}catch(const FrameFailure &failure){return failure.value;}
}
static int eh_isolated_function(int value) __attribute__((objc_executor(main)));
static int eh_isolated_function(int value) {assert(objc3_executor_is_current(property_main));FrameGuard guard;eh_throw_failure(value);}
struct __attribute__((objc_unsafe_sendable)) FrameReceiver {
  int fail() const __attribute__((objc_executor(main))) {FrameGuard guard;eh_throw_failure(604);}
};
static int eh_isolated(PropertyMain *object,int kind) __objc3_async_frame {
  FrameGuard guard;
  if(kind==0)return __objc3_await object.nativeFailure;
  if(kind==1)return __objc3_await [object nativeFailureMessage];
  if(kind==2)return __objc3_await eh_isolated_function(603);
  FrameReceiver receiver;return __objc3_await receiver.fail();
}
static int eh_task_parent(PropertyMain *object,int kind) __objc3_async {
  try {return __objc3_await eh_isolated(object,kind);}
  catch(const FrameFailure &failure) {assert(&failure==eh_failure_address);return failure.value+1000;}
}
static void eh_check_failure(objc3_async_frame frame,int value) {
  auto *task=property_start(&frame);auto done=property_finish(task,0);
  assert(done.kind==OBJC3_COMPLETION_ERROR && done.error);
  bool caught=false;
  try {objc3::task_detail::rethrow_completion(done);}
  catch(const FrameFailure &failure) {assert(failure.value==value && &failure==eh_failure_address);caught=true;}
  assert(caught);objc3_task_release(task);
}
static int eh_success(objc3_async_frame frame,bool value=true) {
  auto *task=property_start(&frame);auto done=property_finish(task,0);
  if(done.kind==OBJC3_COMPLETION_ERROR)objc3::task_detail::rethrow_completion(done);
  assert(done.kind==OBJC3_COMPLETION_SUCCESS);
  int result=value?*static_cast<const int *>(done.payload):0;objc3_task_release(task);return result;
}
static void native_frame_exception_suite() {
  eh_live=eh_created=eh_destroyed=eh_copies=eh_moves=0;
  assert(objc3_executor_create(1,&property_caller)==OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1,&property_destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_get_main(&property_main)==OBJC3_SERVICE_OK);
  PropertyMain *object=[PropertyMain new];
  eh_check_foreign();
  {
    FrameValue input(42);
    eh_mutating_receiver(object,&input).plainFrameValue=input;
    eh_native_order=eh_stored;input.value=42;eh_receiver_calls=0;
    assert(eh_success(eh_write_lvalue(object,&input))==eh_native_order && eh_receiver_calls==1);
    assert(eh_success(eh_read_value(object))==eh_native_order);
    eh_success(eh_write_prvalue(object,77),false);assert(eh_stored==77);
    eh_success(eh_write_moveonly(object),false);assert(eh_stored==88);
    // Nil setters skip dispatch and destroy all independent typed snapshots.
    int before=eh_setters;
    eh_success(eh_write_prvalue((PropertyMain *)0,79),false);
    eh_success(eh_write_moveonly((PropertyMain *)0),false);assert(eh_setters==before);
    auto nil=eh_read_value((PropertyMain *)0);auto *nil_task=property_start(&nil);
    auto done=property_finish(nil_task,1);objc3_service_status status=OBJC3_SERVICE_OK;
    assert(done.kind==OBJC3_COMPLETION_ERROR);
    assert(objc3_async_error_get_service(done.error,&status)==OBJC3_SERVICE_OK && status==OBJC3_SERVICE_INVALID_ARGUMENT);
    objc3_task_release(nil_task);
  }
  assert(eh_live==0 && eh_created==eh_destroyed);
  eh_check_failure(eh_direct(0),701);
  assert(eh_success(eh_direct(3))==702);
  {
    auto frame=eh_direct(1);auto *task=property_start(&frame);auto done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_CANCELLED);objc3_task_release(task);
    frame=eh_direct(2);task=property_start(&frame);done=property_finish(task,0);
    objc3_service_status status=OBJC3_SERVICE_OK;
    assert(done.kind==OBJC3_COMPLETION_ERROR);
    assert(objc3_async_error_get_service(done.error,&status)==OBJC3_SERVICE_OK && status==OBJC3_SERVICE_OUT_OF_MEMORY);
    objc3_task_release(task);
  }
  for(int kind=0;kind!=4;++kind) {
    eh_check_failure(eh_isolated(object,kind),601+kind);
    auto task=eh_task_parent(object,kind);assert(eh_success(std::move(task).release_native())==1601+kind);
  }
  {
    FrameValue input(42);int before=eh_receiver_calls;
    eh_throw_copy=eh_copies+1;
    eh_check_failure(eh_write_lvalue(object,&input),111);
    assert(eh_receiver_calls==before+1 && input.value==99);eh_throw_copy=0;
    eh_throw_move=eh_moves+1;eh_check_failure(eh_write_prvalue(object,90),112);eh_throw_move=0;
  }
  eh_check_failure(eh_write_prvalue(object,501),501);
  int before_sets=eh_setters;
  auto abandoned=eh_write_prvalue(object,87);
  assert(objc3_async_frame_dispose(&abandoned)==OBJC3_SERVICE_OK && eh_live==0);
  for(int boundary=0;boundary!=2;++boundary) {
    auto pending=eh_write_prvalue(object,86);auto *pending_task=property_start(&pending);
    if(boundary) {
      uint32_t count=0;assert(objc3_executor_pump(property_caller,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
      assert(count && eh_live>0);
    }
    objc3_task_request_cancel(pending_task);auto result=property_finish(pending_task,0);
    assert(result.kind==OBJC3_COMPLETION_CANCELLED && eh_setters==before_sets);
    objc3_task_release(pending_task);assert(eh_live==0);
  }
  // Fail only owned error-payload allocation after the exception exists.
  auto frame=eh_direct(0);auto *task=property_start(&frame);
  eh_reject_error_payload=true;auto done=property_finish(task,0);eh_reject_error_payload=false;
  objc3_service_status status=OBJC3_SERVICE_OK;
  assert(done.kind==OBJC3_COMPLETION_ERROR);
  assert(objc3_async_error_get_service(done.error,&status)==OBJC3_SERVICE_OK && status==OBJC3_SERVICE_OUT_OF_MEMORY);
  objc3_task_release(task);
  assert(eh_live==0 && eh_created==eh_destroyed);
  assert(eh_failure_live==0 && eh_failure_created==eh_failure_destroyed);
#if !__has_feature(objc_arc)
  [object release];
#else
  object=(id)0;
#endif
  objc3_executor_release(property_main);objc3_executor_release(property_destination);objc3_executor_release(property_caller);
}
#else
static void native_frame_exception_suite(void) {
  assert(objc3_executor_create(1,&property_caller)==OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1,&property_destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_get_main(&property_main)==OBJC3_SERVICE_OK);
  eh_check_foreign();
  objc3_executor_release(property_main);objc3_executor_release(property_destination);objc3_executor_release(property_caller);
}
#endif
#endif
