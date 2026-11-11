#ifndef OBJC3_ASYNC_PROPERTY_RUNTIME_H
#define OBJC3_ASYNC_PROPERTY_RUNTIME_H
static int async_property_set_entries;
static int async_property_pause(int value) __objc3_async_frame {return value;}
@interface PropertyMain (Async)
@property(nonatomic) int asyncValue;
- (int)asyncValue __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (void)setAsyncValue:(int)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) int asyncGetterOnly;
- (int)asyncGetterOnly __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (void)setAsyncGetterOnly:(int)value __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) int asyncSetterOnly;
- (int)asyncSetterOnly __attribute__((nothrow,objc_executor(main)));
- (void)setAsyncSetterOnly:(int)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) int asyncPlain;
- (int)asyncPlain __objc3_async_frame __attribute__((nothrow));
- (void)setAsyncPlain:(int)value __objc3_async_frame __attribute__((nothrow));
@property(class,nonatomic) int classAsync;
+ (int)classAsync __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
+ (void)setClassAsync:(int)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
#ifdef __cplusplus
@property(nonatomic) int taskValue;
- (int)taskValue __objc3_async __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (void)setTaskValue:(int)value __objc3_async __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) CheckedPropertyOwner asyncOwner;
- (CheckedPropertyOwner)asyncOwner __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (void)setAsyncOwner:(CheckedPropertyOwner)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
#endif
@end
@implementation PropertyMain (Async)
- (int)asyncValue __objc3_async_frame __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));__objc3_await async_property_pause(0);
  assert(objc3_executor_is_current(property_main));return __objc3_try(checked_property_get(1));
}
- (void)setAsyncValue:(int)value __objc3_async_frame __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));++async_property_set_entries;
  value=__objc3_await async_property_pause(value);
  assert(objc3_executor_is_current(property_main));__objc3_try(checked_property_set(value,1));
}
- (int)asyncGetterOnly __objc3_async_frame __objc3_throws(id<Error>) {
  __objc3_await async_property_pause(0);return __objc3_try(checked_property_get(1));
}
- (void)setAsyncGetterOnly:(int)value {__objc3_try_force(checked_property_set(value,0));}
- (int)asyncSetterOnly {return __objc3_try_force(checked_property_get(0));}
- (void)setAsyncSetterOnly:(int)value __objc3_async_frame __objc3_throws(id<Error>) {
  __objc3_await async_property_pause(0);__objc3_try(checked_property_set(value,1));
}
- (int)asyncPlain __objc3_async_frame {__objc3_await async_property_pause(0);return checked_property_stored;}
- (void)setAsyncPlain:(int)value __objc3_async_frame {checked_property_stored=__objc3_await async_property_pause(value);}
+ (int)classAsync __objc3_async_frame __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));__objc3_await async_property_pause(0);return __objc3_try(checked_property_get(1));
}
+ (void)setClassAsync:(int)value __objc3_async_frame __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));__objc3_await async_property_pause(0);__objc3_try(checked_property_set(value,1));
}
#ifdef __cplusplus
- (int)taskValue __objc3_async __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));__objc3_await async_property_pause(0);return __objc3_try(checked_property_get(1));
}
- (void)setTaskValue:(int)value __objc3_async __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));__objc3_await async_property_pause(0);__objc3_try(checked_property_set(value,1));
}
- (CheckedPropertyOwner)asyncOwner __objc3_async_frame __objc3_throws(id<Error>) {
  __objc3_await async_property_pause(0);
  if(checked_property_fault==1)__objc3_throw(checked_property_error);
  return CheckedPropertyOwner(40);
}
- (void)setAsyncOwner:(CheckedPropertyOwner)value __objc3_async_frame __objc3_throws(id<Error>) {
  __objc3_await async_property_pause(0);++checked_owner_setters;assert(value.value==47);
  if(checked_property_fault==2)__objc3_throw(checked_property_error);
}
#endif
@end
@interface PropertyActor (Async)
@property(nonatomic) int actorAsync;
- (int)actorAsync __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow));
- (void)setActorAsync:(int)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow));
- (int)sameCheckedRoundtrip __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow));
@end
@implementation PropertyActor (Async)
- (int)actorAsync __objc3_async_frame __objc3_throws(id<Error>) {
  property_check_actor();__objc3_await async_property_pause(0);property_check_actor();return __objc3_try(checked_property_get(1));
}
- (void)setActorAsync:(int)value __objc3_async_frame __objc3_throws(id<Error>) {
  property_check_actor();__objc3_await async_property_pause(0);property_check_actor();__objc3_try(checked_property_set(value,1));
}
- (int)sameCheckedRoundtrip __objc3_async_frame __objc3_throws(id<Error>) {
  return __objc3_try(__objc3_await (self.actorChecked+=7));
}
@end
@interface PropertyChild (Async)
- (int)asyncValue __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (int)asyncSuperRoundtrip __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow));
@end
@implementation PropertyChild (Async)
- (int)asyncValue __objc3_async_frame __objc3_throws(id<Error>) {return 999;}
- (int)asyncSuperRoundtrip __objc3_async_frame __objc3_throws(id<Error>) {
  return __objc3_try(__objc3_await (super.asyncValue+=7));
}
@end
static int async_property_operation(PropertyMain *object,int operation) __objc3_async_frame __objc3_throws(id<Error>) {
  if(operation==0)return __objc3_try(__objc3_await object.asyncValue);
  if(operation==1)return __objc3_try(__objc3_await (checked_property_receiver(object).asyncValue=checked_property_rhs(7)));
  if(operation==2)return __objc3_try(__objc3_await (object.asyncValue+=7));
  if(operation==3)return __objc3_try(__objc3_await object.asyncValue++);
  if(operation==4)return __objc3_try(__objc3_await --object.asyncValue);
  if(operation==5)return __objc3_try(__objc3_await (object.asyncGetterOnly+=7));
  return __objc3_try(__objc3_await (object.asyncSetterOnly+=7));
}
static int async_property_recover(PropertyMain *object,int operation) __objc3_async_frame {
  __objc3_do {return __objc3_try(__objc3_await async_property_operation(object,operation));}
  __objc3_catch(id<Error> error) {assert(error==checked_property_error);return -100-checked_property_fault;}
}
static int async_property_same_executor(PropertyMain *object) __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
static int async_property_same_executor(PropertyMain *object) __objc3_async_frame __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));return __objc3_try(__objc3_await (object.checkedValue+=7));
}
static int async_property_entries(PropertyActor *actor,PropertyChild *child,PropertyMain *object,int operation) __objc3_async_frame __objc3_throws(id<Error>) {
  if(operation==0)return __objc3_try(__objc3_await (actor.actorAsync+=7));
  if(operation==1)return __objc3_try(__objc3_await (PropertyMain.classAsync+=7));
  if(operation==2)return __objc3_try(__objc3_await [child asyncSuperRoundtrip]);
  if(operation==3)return __objc3_try(__objc3_await [actor sameCheckedRoundtrip]);
  if(operation==4)return __objc3_try(__objc3_await async_property_same_executor(object));
  return __objc3_await (object.asyncPlain+=7);
}
#ifdef __cplusplus
static int async_property_owner(PropertyMain *object,int operation) __objc3_async_frame {
  __objc3_do {
    if(operation==0) {CheckedPropertyOwner value=__objc3_try(__objc3_await object.asyncOwner);return value.value;}
    if(operation==1) {CheckedPropertyOwner value(47);__objc3_try(__objc3_await (object.asyncOwner=value));}
    else __objc3_try(__objc3_await (object.asyncOwner=CheckedPropertyOwner(47)));
    return 47;
  } __objc3_catch(id<Error> error) {assert(error==checked_property_error);return -100-checked_property_fault;}
}
static int async_property_task(PropertyMain *object) __objc3_async __objc3_throws(id<Error>) {
  return __objc3_try(__objc3_await (object.taskValue+=7));
}
#endif
static void async_property_suite(PropertyMain *object,PropertyActor *actor,PropertyChild *child) {
  checked_property_error=object;
  for(int operation=0;operation!=7;++operation)for(int fault=0;fault!=3;++fault) {
    checked_property_fault=fault;checked_property_stored=40;
    checked_property_gets=checked_property_sets=checked_property_phase=0;
    checked_property_receivers=checked_property_rhs_calls=0;
    objc3_async_frame frame=async_property_recover(object,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);assert(done.kind==OBJC3_COMPLETION_SUCCESS);
    int getter=operation!=1,setter=operation!=0;
    int getter_failed=getter && fault==1 && operation!=6;
    int setter_failed=setter && !getter_failed && fault==2 && operation!=5;
    int result=getter_failed?-101:setter_failed?-102:operation==1?7:operation==4?39:operation>=2 && operation!=3?47:40;
    assert(*(const int *)done.payload==result);
    assert(checked_property_gets==getter && checked_property_sets==(setter && !getter_failed));
    assert(checked_property_phase==(getter_failed || !setter?1:getter?12:2));
    assert(checked_property_stored==(!setter || getter_failed || setter_failed?40:operation==1?7:operation==3?41:operation==4?39:47));
    assert(checked_property_receivers==(operation==1) && checked_property_rhs_calls==(operation==1));
    objc3_task_release(task);
  }
  checked_property_fault=0;
  for(int operation=0;operation!=6;++operation) {
    checked_property_stored=40;checked_property_phase=0;
    objc3_async_frame frame=async_property_entries(actor,child,object,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==47);objc3_task_release(task);
  }
  for(int operation=0;operation!=7;++operation) {
    checked_property_phase=0;
    objc3_async_frame frame=async_property_operation((PropertyMain *)0,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,1);
    assert(done.kind==OBJC3_COMPLETION_SUCCESS && checked_property_phase==0);
    assert(*(const int *)done.payload==(operation==1 || operation==2 || operation>=5?7:operation==4?-1:0));
    objc3_task_release(task);
  }
  for(int start=0;start!=2;++start) {
    checked_property_phase=checked_property_receivers=checked_property_rhs_calls=0;
    objc3_async_frame frame=async_property_operation(object,1);
    if(!start) {assert(objc3_async_frame_dispose(&frame)==OBJC3_SERVICE_OK);}
    else {
      objc3_task_t *task=property_start(&frame);objc3_task_request_cancel(task);
      objc3_completion done=property_finish(task,0);assert(done.kind==OBJC3_COMPLETION_CANCELLED);
      objc3_task_release(task);
    }
    assert(checked_property_phase==0 && checked_property_receivers==0 && checked_property_rhs_calls==0);
  }
  for(int boundary=0;boundary!=2;++boundary) {
    checked_property_stored=40;checked_property_gets=checked_property_sets=async_property_set_entries=checked_property_phase=0;
    objc3_async_frame frame=async_property_operation(object,2);objc3_task_t *task=property_start(&frame);
    for(int turn=0;turn!=128;++turn) {
      uint32_t count=0;
      assert(objc3_executor_pump(property_caller,1,&count)==OBJC3_SERVICE_OK);
      assert(objc3_executor_pump(property_main,1,&count)==OBJC3_SERVICE_OK);
      if(checked_property_gets && (!boundary || async_property_set_entries))break;
    }
    assert(checked_property_gets==1 && checked_property_sets==0);
    objc3_task_request_cancel(task);objc3_completion done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_CANCELLED && checked_property_stored==40 && checked_property_sets==0);
    objc3_task_release(task);
  }
#ifdef __cplusplus
  for(int operation=0;operation!=3;++operation)for(int fault=0;fault!=3;++fault) {
    checked_property_fault=fault;checked_owner_setters=0;
    objc3_async_frame frame=async_property_owner(object,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);assert(done.kind==OBJC3_COMPLETION_SUCCESS);
    assert(*(const int *)done.payload==(operation==0?(fault==1?-101:40):(fault==2?-102:47)));
    assert(checked_owner_setters==(operation!=0));objc3_task_release(task);
    assert(checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  checked_property_fault=0;
  for(int operation=0;operation!=3;++operation) {
    const int before=checked_owner_created;checked_owner_setters=0;
    objc3_async_frame frame=async_property_owner((PropertyMain *)0,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,1);
    if(!operation) {
      objc3_service_status status=OBJC3_SERVICE_OK;
      assert(done.kind==OBJC3_COMPLETION_ERROR && checked_owner_created==before);
      assert(objc3_async_error_get_service(done.error,&status)==OBJC3_SERVICE_OK && status==OBJC3_SERVICE_INVALID_ARGUMENT);
    } else assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==47 && checked_owner_created>before);
    assert(checked_owner_setters==0);objc3_task_release(task);
    assert(checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  checked_owner_setters=0;
  {
    objc3_async_frame frame=async_property_owner(object,2);objc3_task_t *task=property_start(&frame);
    property_pump(property_caller);assert(checked_owner_live>0);
    objc3_task_request_cancel(task);objc3_completion done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_CANCELLED && checked_owner_setters==0);objc3_task_release(task);
    assert(checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  checked_property_fault=0;checked_property_stored=40;
  auto lazy=async_property_task(object);objc3_async_frame frame=std::move(lazy).release_native();
  objc3_task_t *task=property_start(&frame);objc3_completion done=property_finish(task,0);
  assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==47);objc3_task_release(task);
#endif
  checked_property_error=(id)0;
}
#endif
