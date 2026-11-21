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

static int er_live,er_opened,er_closed,er_next,er_copies,er_moves,er_copy_fault,er_move_fault,er_setters,er_stored;
static bool er_active[1024],er_union_owned;
static int er_payload[1024];
static void er_close_handle(int handle) noexcept {
  if(handle==-1)return;
  assert(handle>0 && handle<1024 && er_active[handle]);er_active[handle]=false;--er_live;++er_closed;
}
typedef int FrameResourceHandle __attribute__((objc_requires_cleanup(er_close_handle),objc_resource_family(FrameResourceHandles),objc_invalid(-1)));
static int er_new_handle(int value) {
  int handle=++er_next;assert(handle<1024);er_active[handle]=true;er_payload[handle]=value;++er_live;++er_opened;return handle;
}
static int er_clone_handle(int handle) {if(handle==-1)return -1;assert(er_active[handle]);return er_new_handle(er_payload[handle]);}
static void er_before_copy(void) {if(++er_copies==er_copy_fault)eh_throw_failure(811);}
static void er_before_move(void) {if(++er_moves==er_move_fault)eh_throw_failure(812);}
struct __attribute__((objc_unsafe_sendable)) FrameResourceValue {
  FrameResourceHandle handle;int value;
  explicit FrameResourceValue(int value):handle(er_new_handle(value)),value(value) {}
  FrameResourceValue(const FrameResourceValue &other):handle((er_before_copy(),er_clone_handle(other.handle))),value(other.value) {}
  FrameResourceValue(FrameResourceValue &&other):handle((er_before_move(),other.handle)),value(other.value) {other.handle=-1;}
  ~FrameResourceValue() noexcept {er_close_handle(handle);}
};
struct __attribute__((objc_unsafe_sendable)) FrameResourceMoveOnly {
  FrameResourceValue item;
  explicit FrameResourceMoveOnly(int value):item(value) {}
  FrameResourceMoveOnly(const FrameResourceMoveOnly &)=delete;
  FrameResourceMoveOnly(FrameResourceMoveOnly &&)=default;
};
struct __attribute__((objc_unsafe_sendable)) FrameResourcePair {
  FrameResourceValue values[2];
  explicit FrameResourcePair(int value):values{FrameResourceValue(value),FrameResourceValue(value+1)} {}
};
union __attribute__((objc_unsafe_sendable)) FrameResourceUnion {
  struct ResourceArm {int tag;FrameResourceHandle handle;} owned;
  struct ScalarArm {int tag,value;} plain;
  FrameResourceUnion(int value,bool resource) {if(resource)owned={1,er_new_handle(value)};else plain={0,value};}
  FrameResourceUnion(const FrameResourceUnion &other) {
    er_before_copy();if(other.owned.tag)owned={1,er_clone_handle(other.owned.handle)};else plain=other.plain;
  }
  FrameResourceUnion(FrameResourceUnion &&other) {
    er_before_move();if(other.owned.tag){owned=other.owned;other.owned.handle=-1;}else plain=other.plain;
  }
  ~FrameResourceUnion() noexcept {if(owned.tag)er_close_handle(owned.handle);}
  int value() const {if(!owned.tag)return plain.value;assert(owned.handle>0 && er_active[owned.handle]);return er_payload[owned.handle];}
};

struct FrameGuard {
  FrameGuard() noexcept {++eh_created;++eh_live;}
  ~FrameGuard() noexcept {++eh_destroyed;--eh_live;assert(eh_live>=0);}
};
@interface PropertyMain (NativeFrameExceptions)
@property(nonatomic) FrameResourceValue resourceValue;
- (FrameResourceValue)resourceValue __attribute__((objc_executor(main)));
- (void)setResourceValue:(FrameResourceValue)value __attribute__((objc_executor(main)));
@property(nonatomic) FrameResourceMoveOnly resourceMove;
- (FrameResourceMoveOnly)resourceMove __attribute__((objc_executor(main)));
- (void)setResourceMove:(FrameResourceMoveOnly)value __attribute__((objc_executor(main)));
@property(nonatomic) FrameResourcePair resourcePair;
- (FrameResourcePair)resourcePair __attribute__((objc_executor(main)));
- (void)setResourcePair:(FrameResourcePair)value __attribute__((objc_executor(main)));
@property(nonatomic) FrameResourceUnion resourceUnion;
- (FrameResourceUnion)resourceUnion __attribute__((objc_executor(main)));
- (void)setResourceUnion:(FrameResourceUnion)value __attribute__((objc_executor(main)));
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
- (FrameResourceValue)resourceValue {assert(objc3_executor_is_current(property_main));return FrameResourceValue(er_stored);}
- (void)setResourceValue:(FrameResourceValue)value {
  assert(objc3_executor_is_current(property_main) && value.handle>0 && er_active[value.handle]);
  ++er_setters;er_stored=value.value;if(value.value==801)eh_throw_failure(801);
}
- (FrameResourceMoveOnly)resourceMove {return FrameResourceMoveOnly(er_stored);}
- (void)setResourceMove:(FrameResourceMoveOnly)value {
  assert(objc3_executor_is_current(property_main) && value.item.handle>0 && er_active[value.item.handle]);++er_setters;er_stored=value.item.value;
}
- (FrameResourcePair)resourcePair {assert(objc3_executor_is_current(property_main));return FrameResourcePair(er_stored);}
- (void)setResourcePair:(FrameResourcePair)value {
  assert(objc3_executor_is_current(property_main));
  assert(er_active[value.values[0].handle] && er_active[value.values[1].handle]);++er_setters;er_stored=value.values[0].value+value.values[1].value;
}
- (FrameResourceUnion)resourceUnion {assert(objc3_executor_is_current(property_main));return FrameResourceUnion(er_stored,er_union_owned);}
- (void)setResourceUnion:(FrameResourceUnion)value {
  assert(objc3_executor_is_current(property_main));++er_setters;er_stored=value.value();er_union_owned=value.owned.tag;
}
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

static void er_write_value(PropertyMain *object,FrameResourceValue *value) __objc3_async_frame {__objc3_await (object.resourceValue=*value);}
static void er_write_prvalue(PropertyMain *object,int value) __objc3_async_frame {__objc3_await (object.resourceValue=FrameResourceValue(value));}
static void er_write_move(PropertyMain *object) __objc3_async_frame {__objc3_await (object.resourceMove=FrameResourceMoveOnly(88));}
static void er_write_pair(PropertyMain *object,FrameResourcePair *value) __objc3_async_frame {__objc3_await (object.resourcePair=*value);}
static void er_write_union(PropertyMain *object,int value,bool owned) __objc3_async_frame {__objc3_await (object.resourceUnion=FrameResourceUnion(value,owned));}
static int er_read_value(PropertyMain *object) __objc3_async_frame {FrameResourceValue value=__objc3_await object.resourceValue;return value.value;}
static int er_read_pair(PropertyMain *object) __objc3_async_frame {FrameResourcePair value=__objc3_await object.resourcePair;return value.values[0].value+value.values[1].value;}
static int er_read_union(PropertyMain *object) __objc3_async_frame {FrameResourceUnion value=__objc3_await object.resourceUnion;return value.value();}
static void er_hold_value(PropertyMain *object,FrameResourceValue value) __objc3_async_frame {
  __objc3_await (object.resourceValue=static_cast<FrameResourceValue &&>(value));
}
static void native_resource_property_suite(PropertyMain *object) {
  er_live=er_opened=er_closed=er_next=er_copies=er_moves=er_copy_fault=er_move_fault=er_setters=0;
  {
    FrameResourceValue input(42);eh_success(er_write_value(object,&input),false);assert(er_stored==42 && er_live==1);
    assert(eh_success(er_read_value(object))==42 && er_live==1);
    int before=er_setters;er_copy_fault=er_copies+1;eh_check_failure(er_write_value(object,&input),811);er_copy_fault=0;
    assert(er_setters==before && er_live==1 && er_active[input.handle]);
  }
  assert(er_live==0);
  eh_success(er_write_prvalue(object,77),false);assert(er_stored==77 && er_live==0);
  eh_success(er_write_move(object),false);assert(er_stored==88 && er_live==0);
  for(bool owned:{false,true}) {
    eh_success(er_write_union(object,84,owned),false);assert(er_stored==84 && er_union_owned==owned && er_live==0);
    assert(eh_success(er_read_union(object))==84 && er_live==0);
  }
  {
    FrameResourcePair input(42);eh_success(er_write_pair(object,&input),false);assert(er_stored==85 && er_live==2);
    assert(eh_success(er_read_pair(object))==171 && er_live==2);
    // The second member copy fails after the first acquired its handle.
    int before=er_setters,closed=er_closed;er_copy_fault=er_copies+2;
    eh_check_failure(er_write_pair(object,&input),811);er_copy_fault=0;
    assert(er_setters==before && er_live==2 && er_closed>closed);
    assert(er_active[input.values[0].handle] && er_active[input.values[1].handle]);
  }
  assert(er_live==0);
  er_move_fault=er_moves+1;eh_check_failure(er_write_prvalue(object,90),812);er_move_fault=0;assert(er_live==0);
  eh_check_failure(er_write_prvalue(object,801),801);assert(er_live==0);
  int before=er_setters;
  eh_success(er_write_prvalue((PropertyMain *)0,79),false);eh_success(er_write_move((PropertyMain *)0),false);
  eh_success(er_write_union((PropertyMain *)0,80,true),false);assert(er_setters==before && er_live==0);
  {
    FrameResourcePair input(42);eh_success(er_write_pair((PropertyMain *)0,&input),false);assert(er_live==2 && er_setters==before);
  }
  assert(er_live==0);
  for(int kind=0;kind!=3;++kind) {
    int opened=er_opened;
    auto frame=kind==0?er_read_value((PropertyMain *)0):kind==1?er_read_pair((PropertyMain *)0):er_read_union((PropertyMain *)0);
    auto *task=property_start(&frame);auto done=property_finish(task,1);objc3_service_status status=OBJC3_SERVICE_OK;
    assert(done.kind==OBJC3_COMPLETION_ERROR && objc3_async_error_get_service(done.error,&status)==OBJC3_SERVICE_OK && status==OBJC3_SERVICE_INVALID_ARGUMENT);
    objc3_task_release(task);assert(er_opened==opened && er_live==0);
  }
  {
    auto abandoned=er_hold_value(object,FrameResourceValue(87));assert(er_live==1);
    assert(objc3_async_frame_dispose(&abandoned)==OBJC3_SERVICE_OK && er_live==0);
  }
  before=er_setters;
  for(int boundary=0;boundary!=2;++boundary) {
    auto pending=er_hold_value(object,FrameResourceValue(86));assert(er_live==1);auto *task=property_start(&pending);
    if(boundary){uint32_t count=0;assert(objc3_executor_pump(property_caller,UINT32_MAX,&count)==OBJC3_SERVICE_OK);assert(count && er_live>0);}
    objc3_task_request_cancel(task);auto done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_CANCELLED && er_setters==before);objc3_task_release(task);assert(er_live==0);
  }
  assert(er_opened==er_closed && er_live==0);
}

static void native_frame_exception_suite() {
  eh_live=eh_created=eh_destroyed=eh_copies=eh_moves=0;
  assert(objc3_executor_create(1,&property_caller)==OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1,&property_destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_get_main(&property_main)==OBJC3_SERVICE_OK);
  PropertyMain *object=[PropertyMain new];
  eh_check_foreign();
  native_resource_property_suite(object);
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
