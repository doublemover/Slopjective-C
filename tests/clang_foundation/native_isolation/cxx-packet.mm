#include "provider.h"
#include "objc3_task.hpp"
#include <assert.h>
#include <objc/runtime.h>
extern "C" void task_abi_release(void *) __asm__("objc_release");

extern "C" int hop_throwing_argument(void) { throw 77; }

static int live_packets, packet_entries, move_count, fail_move;
struct __attribute__((objc_unsafe_sendable)) MovePacket {
  int value;
  explicit MovePacket(int value):value(value) {
    __atomic_fetch_add(&live_packets,1,__ATOMIC_RELEASE);
  }
  MovePacket(const MovePacket &)=delete;
  MovePacket(MovePacket &&source):value(source.value) {
    int ordinal=__atomic_add_fetch(&move_count,1,__ATOMIC_ACQ_REL);
    if (ordinal==__atomic_load_n(&fail_move,__ATOMIC_ACQUIRE)) throw 77;
    source.value=0;
    __atomic_fetch_add(&live_packets,1,__ATOMIC_RELEASE);
  }
  ~MovePacket() noexcept {__atomic_fetch_sub(&live_packets,1,__ATOMIC_RELEASE);}
};

static int move_entry(MovePacket packet) noexcept
    __attribute__((objc_executor(main)));
static int move_entry(MovePacket packet) noexcept {
  objc3_executor_t *destination=nullptr;
  assert(objc3_executor_get_main(&destination)==OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(destination));
  objc3_executor_release(destination);
  __atomic_fetch_add(&packet_entries,1,__ATOMIC_RELEASE);
  return packet.value;
}

extern "C" int hop_cxx_packet_roundtrip(objc3_executor_t *caller)
    __objc3_async_frame {
  MovePacket source(42);
  int result=__objc3_await move_entry(static_cast<MovePacket &&>(source));
  assert(source.value==0 && objc3_executor_is_current(caller));
  return result;
}
extern "C" int hop_cxx_packet_live(void) {
  return __atomic_load_n(&live_packets,__ATOMIC_ACQUIRE);
}
extern "C" int hop_cxx_packet_entries(void) {
  return __atomic_load_n(&packet_entries,__ATOMIC_ACQUIRE);
}
extern "C" void hop_cxx_packet_fail_move(int ordinal) {
  assert(hop_cxx_packet_live()==0);
  __atomic_store_n(&move_count,0,__ATOMIC_RELEASE);
  __atomic_store_n(&fail_move,ordinal,__ATOMIC_RELEASE);
}

static int live_values;
struct __attribute__((objc_unsafe_sendable)) CopyPacket {
  int value;
  explicit CopyPacket(int value):value(value) {
    __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  CopyPacket(const CopyPacket &source):value(source.value) {
    __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  CopyPacket(CopyPacket &&source) noexcept:value(source.value) {
    source.value=0;
    __atomic_fetch_add(&live_values,1,__ATOMIC_RELEASE);
  }
  ~CopyPacket() noexcept {__atomic_fetch_sub(&live_values,1,__ATOMIC_RELEASE);}
};
static CopyPacket value_entry(CopyPacket value) noexcept
    __attribute__((objc_executor(main)));
static CopyPacket value_entry(CopyPacket value) noexcept {
  value.value+=2;
  return value;
}
extern "C" int hop_cxx_value_roundtrip(objc3_executor_t *caller)
    __objc3_async_frame {
  CopyPacket source(40);
  CopyPacket result=__objc3_await value_entry(static_cast<CopyPacket &&>(source));
  assert(source.value==0 && objc3_executor_is_current(caller));
  return result.value;
}
extern "C" int hop_cxx_value_live(void) {
  return __atomic_load_n(&live_values,__ATOMIC_ACQUIRE);
}

@interface HopCounter (CxxValues)
- (CopyPacket)copyPacket:(CopyPacket)value __attribute__((nothrow));
- (int)movePacket:(MovePacket)value __attribute__((nothrow));
@end
@implementation HopCounter (CxxValues)
- (CopyPacket)copyPacket:(CopyPacket)value {
  value.value+=2;
  return value;
}
- (int)movePacket:(MovePacket)value {
  __atomic_fetch_add(&packet_entries,1,__ATOMIC_RELEASE);
  return value.value;
}
@end
extern "C" int hop_cxx_actor_roundtrip(HopCounter *counter,
                                       objc3_executor_t *caller)
    __objc3_async_frame {
  CopyPacket source(40);
  CopyPacket result=__objc3_await [counter copyPacket:static_cast<CopyPacket &&>(source)];
  assert(source.value==0 && objc3_executor_is_current(caller));
  MovePacket moved(42);
  int value=__objc3_await [counter movePacket:static_cast<MovePacket &&>(moved)];
  assert(moved.value==0 && objc3_executor_is_current(caller));
  return result.value+value-42;
}

static int task_abi_entries;
static int task_abi_leaf(int value) __objc3_async {
  __objc3_await objc3::task_yield;
  return value+1;
}
static CopyPacket task_abi_entry(CopyPacket value) noexcept __objc3_async
    __attribute__((objc_executor(main)));
static CopyPacket task_abi_entry(CopyPacket value) noexcept __objc3_async {
  objc3_executor_t *destination=nullptr;
  assert(objc3_executor_get_main(&destination)==OBJC3_SERVICE_OK);
  objc3::task_destination owner(destination);
  objc3_executor_release(destination);
  assert(objc3_executor_is_current(owner.executor()));
  __atomic_fetch_add(&task_abi_entries,1,__ATOMIC_RELEASE);
  __objc3_await objc3::task_yield;
  assert(objc3_executor_is_current(owner.executor()));
  value.value+=2;
  return value;
}
@interface HopCounter (TaskABI)
- (int)taskABIValue:(int)amount __objc3_async __attribute__((nothrow));
@end
@implementation HopCounter (TaskABI)
- (int)taskABIValue:(int)amount __objc3_async {
  objc3_actor_t *actor=nullptr;
  assert(objc3_actor_get_for_object((__bridge const void *)self,&actor)==OBJC3_SERVICE_OK);
  objc3::task_destination owner(actor);
  objc3_actor_release(actor);
  objc3_current_actor_facts facts{sizeof(facts),OBJC3_ACTOR_SERVICE_VERSION};
  assert(objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK &&
         facts.actor_identity==objc3_actor_identity(owner.actor()));
  __objc3_await objc3::task_yield;
  assert(objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK &&
         facts.actor_identity==objc3_actor_identity(owner.actor()));
  return amount+2;
}
@end
@protocol Error @end
@protocol TaskABIError <Error,Sendable> @end
@interface HopTaskError : HopObject <TaskABIError> @end
@implementation HopTaskError @end
#if !__has_feature(objc_arc)
struct TaskObjectOwner {
  HopObject *value;
  ~TaskObjectOwner() { task_abi_release((__bridge void *)value); }
};
#endif
static HopObject *task_abi_object(void) noexcept __objc3_async
    __attribute__((objc_executor(main)));
static HopObject *task_abi_object(void) noexcept __objc3_async {
  HopObject *value=[HopObject new];
#if !__has_feature(objc_arc)
  TaskObjectOwner owner{value};
#endif
  __objc3_await objc3::task_yield;
  return value;
}
static int task_abi_checked(bool fail) noexcept __objc3_async
    __objc3_throws(id<TaskABIError>) __attribute__((objc_executor(main)));
static int task_abi_checked(bool fail) noexcept __objc3_async
    __objc3_throws(id<TaskABIError>) {
  HopTaskError *error=[HopTaskError new];
#if !__has_feature(objc_arc)
  TaskObjectOwner owner{error};
#endif
  __objc3_await objc3::task_yield;
  if (fail) __objc3_throw((id<TaskABIError>)error);
  return 42;
}
static int task_abi_recover(bool fail,objc3_executor_t *caller) __objc3_async_frame {
  __objc3_do {
    int result=__objc3_try(__objc3_await task_abi_checked(fail));
    assert(objc3_executor_is_current(caller));
    return result;
  } __objc3_catch (id<TaskABIError> error) {
    __objc3_await task_abi_leaf(0);
    assert(objc3_executor_is_current(caller));
    return [(HopObject *)error number];
  }
}
#if __has_feature(objc_arc)
static int task_abi_consume(__attribute__((ns_consumed)) HopObject *value)
    noexcept __objc3_async __attribute__((objc_executor(main)));
static int task_abi_consume(__attribute__((ns_consumed)) HopObject *value)
    noexcept __objc3_async {
  __objc3_await objc3::task_yield;
  return [value number];
}
typedef int (^TaskABIWork)(void) __attribute__((objc_unsafe_sendable));
static int task_abi_consume_block(__attribute__((ns_consumed)) TaskABIWork work)
    noexcept __objc3_async __attribute__((objc_executor(main)));
static int task_abi_consume_block(__attribute__((ns_consumed)) TaskABIWork work)
    noexcept __objc3_async {
  __objc3_await objc3::task_yield;
  return work();
}
#endif
extern "C" int hop_task_abi_roundtrip(HopCounter *counter,
                                      objc3_executor_t *caller)
    __objc3_async_frame {
  int plain=__objc3_await task_abi_leaf(41);
  assert(plain==42 && objc3_executor_is_current(caller));
  CopyPacket source(40);
  CopyPacket value=__objc3_await task_abi_entry(static_cast<CopyPacket &&>(source));
  assert(value.value==42 && source.value==0 && objc3_executor_is_current(caller));
  int actor=__objc3_await [counter taskABIValue:40];
  assert(actor==42 && objc3_executor_is_current(caller));
  HopObject *object=__objc3_await task_abi_object();
  assert([object number]==42 && objc3_executor_is_current(caller));
  assert((__objc3_await task_abi_recover(false,caller))==42);
  assert((__objc3_await task_abi_recover(true,caller))==42);
#if __has_feature(objc_arc)
  assert((__objc3_await task_abi_consume([HopObject new]))==42);
  TaskABIWork work=^{ return [object number]; };
  assert((__objc3_await task_abi_consume_block(work))==42);
#endif
  return plain+value.value+actor-84;
}
extern "C" int hop_task_abi_entries(void) {
  return __atomic_load_n(&task_abi_entries,__ATOMIC_ACQUIRE);
}

@interface HopTaskBase : HopObject
- (int)taskValue:(int)value __objc3_async
    __attribute__((nothrow,objc_executor(main)));
+ (int)classTaskValue:(int)value __objc3_async
    __attribute__((nothrow,objc_executor(main)));
@end
@interface HopTaskChild : HopTaskBase
- (int)taskValue:(int)value __objc3_async
    __attribute__((nothrow,objc_executor(main)));
- (int)taskSuperFrame:(objc3_executor_t *)caller __objc3_async_frame;
+ (int)taskSuperClassFrame:(objc3_executor_t *)caller __objc3_async_frame;
@end
@implementation HopTaskBase
- (int)taskValue:(int)value __objc3_async {
  hop_super_enter_main();
  __objc3_await objc3::task_yield;
  hop_super_enter_main();
  return value+2;
}
+ (int)classTaskValue:(int)value __objc3_async {
  assert(self==objc_getClass("HopTaskChild"));
  hop_super_enter_main();
  __objc3_await objc3::task_yield;
  hop_super_enter_main();
  return value+2;
}
@end
@implementation HopTaskChild
- (int)taskValue:(int)value __objc3_async { return value-1000; }
- (int)taskSuperFrame:(objc3_executor_t *)caller __objc3_async_frame {
  int value=__objc3_await [super taskValue:40];
  assert(objc3_executor_is_current(caller));
  return value;
}
+ (int)taskSuperClassFrame:(objc3_executor_t *)caller __objc3_async_frame {
  int value=__objc3_await [super classTaskValue:40];
  assert(objc3_executor_is_current(caller));
  return value;
}
@end
extern "C" objc3_async_frame hop_task_abi_super_frame(int kind,
                                                      objc3_executor_t *caller) {
  if (kind) return [HopTaskChild taskSuperClassFrame:caller];
  HopTaskChild *object=[HopTaskChild new];
  objc3_async_frame frame=[object taskSuperFrame:caller];
#if !__has_feature(objc_arc)
  task_abi_release((__bridge void *)object);
#endif
  return frame;
}
extern "C" int hop_task_abi_class_roundtrip(objc3_executor_t *caller)
    __objc3_async_frame {
  int value=__objc3_await [HopTaskChild classTaskValue:40];
  assert(objc3_executor_is_current(caller));
  return value;
}

extern "C" int hop_task_abi_failure_frame(int fail,objc3_executor_t *caller)
    __objc3_async_frame {
  CopyPacket source(40);
  if (fail) hop_fail_next_frame_allocation();
  CopyPacket value=__objc3_await task_abi_entry(static_cast<CopyPacket &&>(source));
  assert(objc3_executor_is_current(caller));
  return value.value;
}
