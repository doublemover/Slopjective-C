#ifndef OBJC3_ISOLATED_PROPERTIES_H
#define OBJC3_ISOLATED_PROPERTIES_H
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
#include <stdio.h>
#ifdef __cplusplus
#include <objc3c/runtime/objc3_task.hpp>
extern "C" {
#endif
id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
#ifdef __cplusplus
}
#endif
static objc3_executor_t *property_caller, *property_destination, *property_main;
static objc3_actor_t *property_mailbox;
static int property_created, property_destroyed, property_gets, property_sets;
static int property_receiver_evaluated, property_rhs_evaluated, property_event;
static int property_receiver_event, property_rhs_event, property_atomic, property_interloper;
static int property_expected_rhs_first;
static int property_packet_receiver_evaluated;
static int property_after_set;
struct PropertyPacket {int value; double weight;};
@protocol Sendable @end
__attribute__((objc_root_class)) @interface PropertyObject <Sendable> {
  Class isa;
  int ordinary_value;
#if !__has_feature(objc_arc)
  int references;
#endif
}
+ (instancetype)new __attribute__((nothrow));
@property(nonatomic) int ordinary;
#if !__has_feature(objc_arc)
- (id)retain;
- (oneway void)release;
#endif
@end
@implementation PropertyObject
- (int)ordinary {return ordinary_value;}
- (void)setOrdinary:(int)value {ordinary_value=value;}
+ (instancetype)new {
  PropertyObject *object=class_createInstance(self,0);
#if !__has_feature(objc_arc)
  object->references=1;
#endif
  ++property_created;return object;
}
#if !__has_feature(objc_arc)
- (id)retain {++references;return self;}
- (oneway void)release {if(!--references)[self dealloc];}
#endif
- (void)dealloc {++property_destroyed;object_dispose(self);}
@end
static void property_interloper_job(void *context) {(void)context;++property_interloper;}
static void property_check_actor(void) {
  objc3_current_actor_facts facts={sizeof(facts),OBJC3_ACTOR_SERVICE_VERSION};
  assert(objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK);
  assert(facts.actor_identity==objc3_actor_identity(property_mailbox));
  assert(objc3_executor_is_current(property_destination));
}
actor class PropertyActor : PropertyObject {
  int stored;
  struct PropertyPacket packet;
  PropertyObject *owned;
}
@property(nonatomic) int value;
@property(nonatomic) struct PropertyPacket packet;
@property(nonatomic,strong) PropertyObject *object;
@property(nonatomic,getter=customValue,setter=customSet:) int custom;
@property(nonatomic) int preferred;
- (int)value __attribute__((nothrow));
- (void)setValue:(int)value __attribute__((nothrow));
- (struct PropertyPacket)packet __attribute__((nothrow));
- (void)setPacket:(struct PropertyPacket)value __attribute__((nothrow));
- (PropertyObject *)object __attribute__((nothrow));
- (void)setObject:(PropertyObject *)value __attribute__((nothrow));
- (int)customValue __attribute__((nothrow));
- (void)customSet:(int)value __attribute__((nothrow));
- (int)preferred __attribute__((nothrow,objc_executor(named("properties.actor"))));
- (void)setPreferred:(int)value __attribute__((nothrow,objc_executor(named("properties.actor"))));
- (int)localValue __attribute__((nothrow));
- (int)sameActorValue __objc3_async_frame __attribute__((nothrow));
@end
@implementation PropertyActor
- (int)value {
  property_check_actor();++property_gets;
  if(property_atomic) {
    assert(property_interloper==0);
    objc3_actor_hop_facts facts={sizeof(facts),OBJC3_ACTOR_SERVICE_VERSION};
    assert(objc3_actor_enqueue(property_mailbox,property_interloper_job,0,0,&facts)==OBJC3_SERVICE_OK);
  }
  return stored;
}
- (void)setValue:(int)value {
  property_check_actor();++property_sets;
  if(property_atomic) {assert(property_interloper==0);property_atomic=0;}
  stored=value;
}
- (struct PropertyPacket)packet {property_check_actor();return packet;}
- (void)setPacket:(struct PropertyPacket)value {property_check_actor();packet=value;}
- (PropertyObject *)object {property_check_actor();return owned;}
- (void)setObject:(PropertyObject *)value {
  property_check_actor();
#if !__has_feature(objc_arc)
  [value retain];[owned release];
#endif
  owned=value;
}
- (int)customValue {property_check_actor();return stored;}
- (void)customSet:(int)value {property_check_actor();stored=value;}
- (int)preferred {property_check_actor();return stored;}
- (void)setPreferred:(int)value {property_check_actor();stored=value;}
- (int)localValue {return self.value;}
- (int)sameActorValue __objc3_async_frame {return __objc3_await self.value;}
@end
#ifdef __cplusplus
static int property_constructed;
struct PropertyConstructed {
  int value;
  explicit PropertyConstructed(int number) noexcept : value(number) {++property_constructed;}
};
#endif
@interface PropertyMain : PropertyObject {int stored;}
@property(class,nonatomic) int classValue;
+ (int)classValue __attribute__((nothrow,objc_executor(main)));
+ (void)setClassValue:(int)value __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) int value;
- (int)value __attribute__((nothrow,objc_executor(main)));
- (void)setValue:(int)value __attribute__((nothrow,objc_executor(main)));
@property(nonatomic) int unregistered;
- (int)unregistered __attribute__((nothrow,objc_executor(named("properties.unregistered"))));
- (void)setUnregistered:(int)value __attribute__((nothrow,objc_executor(named("properties.unregistered"))));
#ifdef __cplusplus
@property(nonatomic) PropertyConstructed constructed;
- (PropertyConstructed)constructed __attribute__((nothrow,objc_executor(main)));
- (void)setConstructed:(PropertyConstructed)value __attribute__((nothrow,objc_executor(main)));
#endif
@end
static int property_class_value;
@implementation PropertyMain
+ (int)classValue {assert(objc3_executor_is_current(property_main));return property_class_value;}
+ (void)setClassValue:(int)value {assert(objc3_executor_is_current(property_main));property_class_value=value;}
- (int)value {assert(objc3_executor_is_current(property_main));return stored;}
- (void)setValue:(int)value {assert(objc3_executor_is_current(property_main));stored=value;}
- (int)unregistered {assert(0);return stored;}
- (void)setUnregistered:(int)value {(void)value;assert(0);}
#ifdef __cplusplus
- (PropertyConstructed)constructed {assert(objc3_executor_is_current(property_main));return PropertyConstructed(stored);}
- (void)setConstructed:(PropertyConstructed)value {assert(objc3_executor_is_current(property_main));stored=value.value;}
#endif
@end
@interface PropertyChild : PropertyMain
- (int)value __attribute__((nothrow,objc_executor(main)));
- (void)setValue:(int)value __attribute__((nothrow,objc_executor(main)));
+ (int)classValue __attribute__((nothrow,objc_executor(main)));
- (int)superRoundtrip __objc3_async_frame __attribute__((nothrow));
+ (int)superClassRoundtrip __objc3_async_frame __attribute__((nothrow));
@end
@implementation PropertyChild
- (int)value {return 999;}
- (void)setValue:(int)value {[super setValue:value+1000];}
+ (int)classValue {return 999;}
- (int)superRoundtrip __objc3_async_frame {
  int value=__objc3_await (super.value=38);
  assert(value==38 && (__objc3_await super.value++)==38);
  return __objc3_await super.value;
}
+ (int)superClassRoundtrip __objc3_async_frame {
  __objc3_await (super.classValue=91);
  return __objc3_await super.classValue;
}
@end
static PropertyActor *property_receiver(PropertyActor *object) {
  ++property_receiver_evaluated;property_receiver_event=++property_event;return object;
}
static PropertyObject *property_plain_receiver(PropertyObject *object) {
  ++property_receiver_evaluated;property_receiver_event=++property_event;return object;
}
static int property_rhs(int value) {
  ++property_rhs_evaluated;property_rhs_event=++property_event;return value;
}
static PropertyActor *property_mutating_receiver(PropertyActor *object,struct PropertyPacket *packet) {
  ++property_packet_receiver_evaluated;packet->value=99;return object;
}
#if __has_feature(objc_arc)
struct PropertyOwnedPacket {PropertyObject *object;int value;};
static struct PropertyOwnedPacket property_owned_packet(struct PropertyOwnedPacket packet,int ignored)
    __attribute__((nothrow,objc_executor(main)));
static struct PropertyOwnedPacket __attribute__((nothrow)) property_owned_packet(
    struct PropertyOwnedPacket packet,int ignored) {
  assert(objc3_executor_is_current(property_main));assert(ignored==1);
  assert(packet.object && packet.value==42);return packet;
}
static int property_clear_packet(struct PropertyOwnedPacket *packet) {
  packet->object=(id)0;packet->value=99;return 1;
}
#endif
static int property_roundtrip(PropertyActor *actor,PropertyMain *main_object,PropertyChild *child,
                              PropertyObject *payload) __objc3_async_frame {
  int ordinary=property_plain_receiver(payload).ordinary=property_rhs(40);
  assert(ordinary==40 && (property_rhs_event<property_receiver_event)==property_expected_rhs_first);
  property_receiver_evaluated=property_rhs_evaluated=property_event=0;
  int assigned=__objc3_await (property_receiver(actor).value=property_rhs(40));
  assert(assigned==40 && property_receiver_evaluated==1 && property_rhs_evaluated==1);
  assert((property_rhs_event<property_receiver_event)==property_expected_rhs_first);
  property_atomic=1;
  int sum=__objc3_await (actor.value+=property_rhs(3));
  assert(sum==43 && property_atomic==0);
  assert(objc3_executor_is_current(property_caller));
  int post=__objc3_await actor.value++;
  int pre=__objc3_await ++actor.value;
  int result=__objc3_await actor.value;
  assert(post==43 && pre==45 && result==45);
  int custom=__objc3_await (actor.custom=46);
  assert(custom==46 && (__objc3_await actor.custom)==46);
  int preferred=__objc3_await (actor.preferred=47);
  assert(preferred==47 && (__objc3_await actor.preferred)==47);
  assert((__objc3_await [actor localValue])==47);
  assert((__objc3_await [actor sameActorValue])==47);
  struct PropertyPacket input={42,2.5};
  struct PropertyPacket assigned_packet=__objc3_await (property_mutating_receiver(actor,&input).packet=input);
  struct PropertyPacket output=__objc3_await actor.packet;
  assert(input.value==99 && property_packet_receiver_evaluated==1);
  assert(assigned_packet.value==42 && output.value==42 && output.weight==2.5);
#if __has_feature(objc_arc)
  struct PropertyOwnedPacket owned_input={payload,42};
  struct PropertyOwnedPacket owned_output=__objc3_await property_owned_packet(owned_input,property_clear_packet(&owned_input));
  assert(owned_input.object==(id)0 && owned_input.value==99);
  assert(owned_output.object==payload && owned_output.value==42);
#endif
  PropertyObject *assignment=__objc3_await (actor.object=payload);
  PropertyObject *read=__objc3_await actor.object;
  assert(assignment==payload && read==payload);
  __objc3_await (actor.object=(PropertyObject *)0);
  int main_value=__objc3_await (main_object.value=39);
  int main_old=__objc3_await main_object.value++;
  assert(main_value==39 && main_old==39 && (__objc3_await main_object.value)==40);
  assert((__objc3_await (PropertyMain.classValue=88))==88);
  assert((__objc3_await PropertyMain.classValue)==88);
  assert((__objc3_await [child superRoundtrip])==39);
  assert((__objc3_await [PropertyChild superClassRoundtrip])==91);
  assert(objc3_executor_is_current(property_caller));
  return result;
}
static int property_nil(int operation) __objc3_async_frame {
  PropertyMain *object=(PropertyMain *)0;
  if(operation==0)return __objc3_await object.value;
  if(operation==1)return __objc3_await (object.value=42);
  if(operation==2)return __objc3_await (object.unregistered+=3);
  return __objc3_await object.unregistered++;
}
static int property_cancelled(PropertyActor *object) __objc3_async_frame {
  int result=__objc3_await (property_receiver(object).value=property_rhs(99));
  ++property_after_set;return result;
}
#ifdef __cplusplus
static int property_task_roundtrip(PropertyActor *object) __objc3_async {
  int first=__objc3_await (object.value=12);
  int second=__objc3_await ++object.value;
  return first+second;
}
static PropertyConstructed property_nil_constructed(int assignment) __objc3_async_frame {
  PropertyMain *object=(PropertyMain *)0;
  if(assignment)return __objc3_await (object.constructed=PropertyConstructed(73));
  return __objc3_await object.constructed;
}
#endif
static void property_pump(objc3_executor_t *executor) {
  uint32_t executed=0;assert(objc3_executor_pump(executor,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
}
static objc3_task_t *property_start(objc3_async_frame *frame) {
  objc3_task_t *task=0;assert(objc3_async_frame_start(frame,property_caller,0,&task)==OBJC3_SERVICE_OK);return task;
}
static objc3_completion property_finish(objc3_task_t *task,int caller_only) {
  objc3_completion done={0};
  for(int turn=0;turn!=64;++turn) {
    property_pump(property_caller);
    if(!caller_only){property_pump(property_main);property_pump(property_destination);}
    objc3_service_status status=objc3_task_get_completion(task,&done);
    if(status==OBJC3_SERVICE_OK)return done;
    assert(status==OBJC3_SERVICE_NOT_READY);
  }
  assert(!"isolated property did not complete");return done;
}
#include "checked.h"
#include "async.h"
#include "checked_results.h"
#include "checked_messages.h"
#include "checked_regions.h"
#include "checked_optional_results.h"
#include "checked_optional_storage.h"
#include "checked_static_references.h"
#include "carrier_calls.h"
#include "carrier_record_payloads.h"
#include "carrier_composed_allocations.h"
static void property_suite(void) {
  assert(objc3_executor_create(1,&property_caller)==OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1,&property_destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_get_main(&property_main)==OBJC3_SERVICE_OK);
  assert(objc3_executor_register_named("properties.actor",property_destination)==OBJC3_SERVICE_OK);
  assert(objc3_actor_create(property_destination,&property_mailbox)==OBJC3_SERVICE_OK);
  PropertyActor *actor=[PropertyActor new];PropertyMain *main_object=[PropertyMain new];
  PropertyChild *child=[PropertyChild new];
  PropertyObject *payload=[PropertyObject new];
  // An ordinary property assignment is the independent native ordering oracle.
  property_plain_receiver(payload).ordinary=property_rhs(40);
  property_expected_rhs_first=property_rhs_event<property_receiver_event;
  property_receiver_evaluated=property_rhs_evaluated=property_event=0;
#if __has_feature(objc_arc)
  void *actor_pointer=(__bridge void *)actor;
#else
  void *actor_pointer=actor;
#endif
  assert(objc3_actor_bind_object(actor_pointer,property_mailbox)==OBJC3_SERVICE_OK);
  objc3_async_frame frame=property_roundtrip(actor,main_object,child,payload);
  objc3_task_t *task=property_start(&frame);
  objc3_completion done=property_finish(task,0);
  assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==45);
  assert(property_interloper==1 && property_receiver_evaluated==1 && property_rhs_evaluated==2);
  objc3_task_release(task);
#ifdef __cplusplus
  auto lazy=property_task_roundtrip(actor);
  frame=std::move(lazy).release_native();task=property_start(&frame);done=property_finish(task,0);
  assert(done.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)done.payload==25);
  objc3_task_release(task);
#endif
  int before_receivers=property_receiver_evaluated,before_rhs=property_rhs_evaluated;
  frame=property_cancelled(actor);
  assert(objc3_async_frame_dispose(&frame)==OBJC3_SERVICE_OK);
  assert(property_receiver_evaluated==before_receivers && property_rhs_evaluated==before_rhs);
  for(int boundary=0;boundary!=3;++boundary) {
    int before_sets=property_sets;
    before_receivers=property_receiver_evaluated;before_rhs=property_rhs_evaluated;
    frame=property_cancelled(actor);task=property_start(&frame);
    if(boundary) {
      uint32_t executed=0;
      assert(objc3_executor_pump(property_caller,boundary==1?1:UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
      assert(executed && property_receiver_evaluated==before_receivers+1 && property_rhs_evaluated==before_rhs+1);
    }
    objc3_task_request_cancel(task);done=property_finish(task,0);
    assert(done.kind==OBJC3_COMPLETION_CANCELLED && property_sets==before_sets && property_after_set==0);
    if(!boundary)assert(property_receiver_evaluated==before_receivers && property_rhs_evaluated==before_rhs);
    objc3_task_release(task);
  }
  for(int operation=0;operation!=4;++operation) {
    frame=property_nil(operation);task=property_start(&frame);done=property_finish(task,1);
    assert(done.kind==OBJC3_COMPLETION_SUCCESS);
    assert(*(const int *)done.payload==(operation==1?42:operation==2?3:0));
    objc3_task_release(task);
  }
#ifdef __cplusplus
  int before=property_constructed;
  frame=property_nil_constructed(0);task=property_start(&frame);done=property_finish(task,1);
  assert(done.kind==OBJC3_COMPLETION_ERROR && property_constructed==before);
  objc3_service_status error=OBJC3_SERVICE_OK;
  assert(objc3_async_error_get_service(done.error,&error)==OBJC3_SERVICE_OK && error==OBJC3_SERVICE_INVALID_ARGUMENT);
  objc3_task_release(task);
  frame=property_nil_constructed(1);task=property_start(&frame);done=property_finish(task,1);
  assert(done.kind==OBJC3_COMPLETION_SUCCESS && property_constructed==before+1);
  assert(((const PropertyConstructed *)done.payload)->value==73);objc3_task_release(task);
#endif
  checked_property_suite(main_object,actor,child);
  async_property_suite(main_object,actor,child);
  checked_result_suite(main_object);
  checked_message_suite(main_object,child);
  checked_region_suite(main_object);
  checked_optional_suite(main_object);
  checked_optional_storage_suite(main_object);
  checked_static_reference_suite(main_object);
  carrier_record_payload_suite();
  carrier_composed_allocation_suite();
  carrier_call_suite(main_object);
  assert(objc3_actor_close(property_mailbox)==OBJC3_SERVICE_OK);
  assert(objc3_actor_join(property_mailbox)==OBJC3_SERVICE_OK);
  objc3_service_status unbind=objc3_actor_unbind_object(actor_pointer);
  if(unbind!=OBJC3_SERVICE_OK)
    fprintf(stderr,"property unbind status=%d created=%d destroyed=%d gets=%d sets=%d\n",
        (int)unbind,property_created,property_destroyed,property_gets,property_sets);
  assert(unbind==OBJC3_SERVICE_OK);
#if __has_feature(objc_arc)
  payload=(id)0;main_object=(id)0;child=(id)0;actor=(id)0;
#else
  [payload release];[main_object release];[child release];[actor release];
#endif
  assert(property_created==4 && property_destroyed==4);
  assert(objc3_executor_unregister_named("properties.actor")==OBJC3_SERVICE_OK);
  objc3_actor_release(property_mailbox);objc3_executor_release(property_main);
  objc3_executor_release(property_destination);objc3_executor_release(property_caller);
}
#endif
