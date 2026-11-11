#ifndef OBJC3_CHECKED_PROPERTY_RUNTIME_H
#define OBJC3_CHECKED_PROPERTY_RUNTIME_H
@protocol Error <Sendable> @end
@interface PropertyMain (CheckedError) <Error> @end
static id<Error> checked_property_error;
static int checked_property_stored,checked_property_fault,checked_property_gets,checked_property_sets;
static int checked_property_phase,checked_property_receivers,checked_property_rhs_calls;
static int checked_property_get(int checked) __objc3_throws(id<Error>) __attribute__((nothrow));
static int checked_property_get(int checked) __objc3_throws(id<Error>) {
  ++checked_property_gets;checked_property_phase=checked_property_phase*10+1;
  if(checked && checked_property_fault==1)__objc3_throw(checked_property_error);
  return checked_property_stored;
}
static void checked_property_set(int value,int checked) __objc3_throws(id<Error>) __attribute__((nothrow));
static void checked_property_set(int value,int checked) __objc3_throws(id<Error>) {
  ++checked_property_sets;checked_property_phase=checked_property_phase*10+2;
  if(checked && checked_property_fault==2)__objc3_throw(checked_property_error);
  checked_property_stored=value;
}
@interface PropertyMain (Checked)
@property(nonatomic) int checkedValue;
- (int)checkedValue __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (void)setCheckedValue:(int)value __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) int getterChecked;
- (int)getterChecked __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (void)setGetterChecked:(int)value __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) int setterChecked;
- (int)setterChecked __attribute__((nothrow,objc_executor(main)));
- (void)setSetterChecked:(int)value __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
@property(class,nonatomic) int classChecked;
+ (int)classChecked __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
+ (void)setClassChecked:(int)value __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
@end
@implementation PropertyMain (Checked)
- (int)checkedValue __objc3_throws(id<Error>) {assert(objc3_executor_is_current(property_main));return __objc3_try(checked_property_get(1));}
- (void)setCheckedValue:(int)value __objc3_throws(id<Error>) {assert(objc3_executor_is_current(property_main));__objc3_try(checked_property_set(value,1));}
- (int)getterChecked __objc3_throws(id<Error>) {return __objc3_try(checked_property_get(1));}
- (void)setGetterChecked:(int)value {__objc3_try_force(checked_property_set(value,0));}
- (int)setterChecked {return __objc3_try_force(checked_property_get(0));}
- (void)setSetterChecked:(int)value __objc3_throws(id<Error>) {__objc3_try(checked_property_set(value,1));}
+ (int)classChecked __objc3_throws(id<Error>) {assert(objc3_executor_is_current(property_main));return __objc3_try(checked_property_get(1));}
+ (void)setClassChecked:(int)value __objc3_throws(id<Error>) {assert(objc3_executor_is_current(property_main));__objc3_try(checked_property_set(value,1));}
@end
@interface PropertyActor (Checked)
@property(nonatomic) int actorChecked;
- (int)actorChecked __objc3_throws(id<Error>) __attribute__((nothrow));
- (void)setActorChecked:(int)value __objc3_throws(id<Error>) __attribute__((nothrow));
@end
@implementation PropertyActor (Checked)
- (int)actorChecked __objc3_throws(id<Error>) {property_check_actor();return __objc3_try(checked_property_get(1));}
- (void)setActorChecked:(int)value __objc3_throws(id<Error>) {property_check_actor();__objc3_try(checked_property_set(value,1));}
@end
@interface PropertyChild (Checked)
- (int)checkedValue __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (int)checkedSuperRoundtrip __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow));
@end
@implementation PropertyChild (Checked)
- (int)checkedValue __objc3_throws(id<Error>) {return 999;}
- (int)checkedSuperRoundtrip __objc3_async_frame __objc3_throws(id<Error>) {
  return __objc3_try(__objc3_await (super.checkedValue+=7));
}
@end
static PropertyMain *checked_property_receiver(PropertyMain *object) {++checked_property_receivers;return object;}
static int checked_property_rhs(int value) {++checked_property_rhs_calls;return value;}
static int checked_property_operation(PropertyMain *object,int operation) __objc3_async_frame __objc3_throws(id<Error>) {
  if(operation==0)return __objc3_try(__objc3_await object.checkedValue);
  if(operation==1)return __objc3_try(__objc3_await (checked_property_receiver(object).checkedValue=checked_property_rhs(7)));
  if(operation==2)return __objc3_try(__objc3_await (object.checkedValue+=checked_property_rhs(7)));
  if(operation==3)return __objc3_try(__objc3_await object.checkedValue++);
  if(operation==4)return __objc3_try(__objc3_await --object.checkedValue);
  if(operation==5)return __objc3_try(__objc3_await (object.getterChecked+=7));
  return __objc3_try(__objc3_await (object.setterChecked+=7));
}
static int checked_property_recover(PropertyMain *object,int operation) __objc3_async_frame {
  __objc3_do {return __objc3_try(__objc3_await checked_property_operation(object,operation));}
  __objc3_catch(id<Error> error) {assert(error==checked_property_error);return -100-checked_property_fault;}
}
static int checked_property_other_entries(PropertyActor *actor,PropertyChild *child,int operation) __objc3_async_frame __objc3_throws(id<Error>) {
  if(operation==0)return __objc3_try(__objc3_await (actor.actorChecked+=7));
  if(operation==1)return __objc3_try(__objc3_await (PropertyMain.classChecked+=7));
  return __objc3_try(__objc3_await [child checkedSuperRoundtrip]);
}
#ifdef __cplusplus
static int checked_owner_stage=-1;
static int checked_owner_live,checked_owner_created,checked_owner_destroyed,checked_owner_setters;
struct __attribute__((objc_unsafe_sendable)) CheckedPropertyOwner {
  int value;
  explicit CheckedPropertyOwner(int number) noexcept:value(number) {++checked_owner_live;++checked_owner_created;}
  CheckedPropertyOwner(const CheckedPropertyOwner &other) noexcept:value(other.value) {++checked_owner_live;++checked_owner_created;}
  CheckedPropertyOwner(CheckedPropertyOwner &&other) noexcept:value(other.value) {other.value=-1;++checked_owner_live;++checked_owner_created;}
  ~CheckedPropertyOwner() noexcept {--checked_owner_live;++checked_owner_destroyed;
    if(checked_owner_live<0)fprintf(stderr,"checked owner stage=%d created=%d destroyed=%d\n",checked_owner_stage,checked_owner_created,checked_owner_destroyed);
    assert(checked_owner_live>=0);}
};
@interface PropertyMain (CheckedOwner)
@property(nonatomic) CheckedPropertyOwner checkedOwner;
- (CheckedPropertyOwner)checkedOwner __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
- (void)setCheckedOwner:(CheckedPropertyOwner)value __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
@end
@implementation PropertyMain (CheckedOwner)
- (CheckedPropertyOwner)checkedOwner __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));
  if(checked_property_fault==1)__objc3_throw(checked_property_error);
  return CheckedPropertyOwner(40);
}
- (void)setCheckedOwner:(CheckedPropertyOwner)value __objc3_throws(id<Error>) {
  assert(objc3_executor_is_current(property_main));++checked_owner_setters;assert(value.value==47);
  if(checked_property_fault==2)__objc3_throw(checked_property_error);
}
@end
static int checked_property_owner(PropertyMain *object,int operation) __objc3_async_frame {
  __objc3_do {
    if(operation==0) {CheckedPropertyOwner value=__objc3_try(__objc3_await object.checkedOwner);return value.value;}
    if(operation==1) {CheckedPropertyOwner value(47);__objc3_try(__objc3_await (object.checkedOwner=value));}
    else __objc3_try(__objc3_await (object.checkedOwner=CheckedPropertyOwner(47)));
    return 47;
  } __objc3_catch(id<Error> error) {assert(error==checked_property_error);return -100-checked_property_fault;}
}
static int checked_property_task(PropertyMain *object) __objc3_async __objc3_throws(id<Error>) {
  return __objc3_try(__objc3_await (object.checkedValue+=7));
}
#endif
static void checked_property_suite(PropertyMain *object,PropertyActor *actor,PropertyChild *child) {
  checked_property_error=object;
  for(int operation=0;operation!=7;++operation)for(int fault=0;fault!=3;++fault) {
    checked_property_fault=fault;checked_property_stored=40;
    checked_property_gets=checked_property_sets=checked_property_phase=0;
    checked_property_receivers=checked_property_rhs_calls=0;
    objc3_async_frame frame=checked_property_recover(object,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);assert(done.kind==OBJC3_COMPLETION_SUCCESS);
    int getter=operation!=1,setter=operation!=0;
    int getter_failed=getter && fault==1 && operation!=6;
    int setter_failed=setter && !getter_failed && fault==2 && operation!=5;
    int value=getter_failed?-101:setter_failed?-102:operation==1?7:operation==4?39:operation>=2 && operation!=3?47:40;
    assert(*(const int *)done.payload==value);
    assert(checked_property_gets==getter && checked_property_sets==(setter && !getter_failed));
    assert(checked_property_phase==(getter_failed || !setter?1:getter?12:2));
    assert(checked_property_stored==(!setter || getter_failed || setter_failed?40:operation==1?7:operation==3?41:operation==4?39:47));
    assert(checked_property_receivers==(operation==1));
    assert(checked_property_rhs_calls==(operation==1 || operation==2));
    objc3_task_release(task);
  }
  checked_property_fault=0;
  for(int operation=0;operation!=3;++operation) {
    checked_property_stored=40;checked_property_phase=0;
    objc3_async_frame frame=checked_property_other_entries(actor,child,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==47 && checked_property_phase==12);
    objc3_task_release(task);
  }
  for(int operation=0;operation!=7;++operation) {
    checked_property_phase=0;
    objc3_async_frame frame=checked_property_operation((PropertyMain *)0,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,1);
    assert(done.kind==OBJC3_COMPLETION_SUCCESS && checked_property_phase==0);
    assert(*(const int *)done.payload==(operation==1 || operation==2 || operation>=5?7:operation==4?-1:0));
    objc3_task_release(task);
  }
  for(int boundary=0;boundary!=3;++boundary) {
    checked_property_phase=checked_property_receivers=checked_property_rhs_calls=0;
    objc3_async_frame frame=checked_property_operation(object,1);
    if(!boundary) {assert(objc3_async_frame_dispose(&frame)==OBJC3_SERVICE_OK);continue;}
    objc3_task_t *task=property_start(&frame);
    if(boundary==2)property_pump(property_caller);
    objc3_task_request_cancel(task);objc3_completion done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_CANCELLED && checked_property_phase==0);
    assert(checked_property_receivers==(boundary==2) && checked_property_rhs_calls==(boundary==2));
    objc3_task_release(task);
  }
#ifdef __cplusplus
  for(int operation=0;operation!=3;++operation)for(int fault=0;fault!=3;++fault) {
    checked_property_fault=fault;checked_owner_setters=0;
    objc3_async_frame frame=checked_property_owner(object,operation);objc3_task_t *task=property_start(&frame);
    objc3_completion done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_SUCCESS);
    assert(*(const int *)done.payload==(operation==0?(fault==1?-101:40):(fault==2?-102:47)));
    assert(checked_owner_setters==(operation!=0));objc3_task_release(task);
    assert(checked_owner_live==0 && checked_owner_created==checked_owner_destroyed);
  }
  checked_property_fault=0;
  checked_property_stored=40;
  auto lazy=checked_property_task(object);objc3_async_frame frame=std::move(lazy).release_native();
  objc3_task_t *task=property_start(&frame);objc3_completion done=property_finish(task,0);
  assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==47);objc3_task_release(task);
#endif
  checked_property_error=(id)0;
}
#endif
