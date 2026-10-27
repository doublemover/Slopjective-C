#include "provider.h"
#include <assert.h>
extern id class_createInstance(Class, __SIZE_TYPE__)
    __attribute__((ns_returns_retained));
extern id object_dispose(__unsafe_unretained id);
static objc3_executor_t *caller_executor, *main_executor, *global_executor,
    *named_executor;
static uint64_t expected_actor;
static int arguments, entries, cleanups, destroyed;
static int packet_arguments, packet_entries;
void hop_configure(objc3_executor_t *caller, objc3_executor_t *main,
                   objc3_executor_t *global, objc3_executor_t *named,
                   uint64_t actor_identity) {
  caller_executor = caller;
  main_executor = main;
  global_executor = global;
  named_executor = named;
  expected_actor = actor_identity;
  arguments = entries = cleanups = 0;
  packet_arguments = packet_entries = 0;
}
int hop_arguments(void) {
  return __atomic_load_n(&arguments, __ATOMIC_ACQUIRE);
}
int hop_entries(void) { return __atomic_load_n(&entries, __ATOMIC_ACQUIRE); }
int hop_cleanups(void) { return __atomic_load_n(&cleanups, __ATOMIC_ACQUIRE); }
int hop_packet_arguments(void) {
  return __atomic_load_n(&packet_arguments, __ATOMIC_ACQUIRE);
}
int hop_packet_entries(void) {
  return __atomic_load_n(&packet_entries, __ATOMIC_ACQUIRE);
}
int hop_destroyed(void) {
  return __atomic_load_n(&destroyed, __ATOMIC_ACQUIRE);
}
@implementation HopObject
+ (id)new {
  return class_createInstance(self, 0);
}
- (int)number {
  return 42;
}
- (void)dealloc {
  __atomic_fetch_add(&destroyed, 1, __ATOMIC_RELEASE);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {
}
#endif
@end
@implementation HopCounter
- (int)add:(int)amount {
  objc3_current_actor_facts facts = {sizeof(facts), OBJC3_ACTOR_SERVICE_VERSION};
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == expected_actor);
  assert(facts.executor_identity == objc3_executor_identity(named_executor));
  value += amount;
  return value;
}
- (int)read {
  return value;
}
- (HopPacket)transform:(HopPacket)packet {
  objc3_current_actor_facts facts = {sizeof(facts), OBJC3_ACTOR_SERVICE_VERSION};
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == expected_actor);
  __atomic_fetch_add(&packet_entries, 1, __ATOMIC_RELEASE);
  packet.value += 1;
  packet.weights[2] += 1;
  return packet;
}
@end
void *hop_new_counter(void) {
#if __has_feature(objc_arc)
  return (__bridge_retained void *)[HopCounter new];
#else
  return (void *)[HopCounter new];
#endif
}
static int source_argument(void) {
  assert(objc3_executor_is_current(caller_executor));
  __atomic_fetch_add(&arguments, 1, __ATOMIC_RELEASE);
  return 1;
}
static int on_main(int value) __attribute__((objc_executor(main), nothrow));
static int on_main(int value) {
  assert(objc3_executor_is_current(main_executor));
  __atomic_fetch_add(&entries, 1, __ATOMIC_RELEASE);
  return value + 1;
}
static int on_global(int value) __attribute__((objc_executor(global), nothrow));
static int on_global(int value) {
  assert(objc3_executor_is_current(global_executor));
  __atomic_fetch_add(&entries, 1, __ATOMIC_RELEASE);
  return value + 10;
}
static int on_named(int value)
    __attribute__((objc_executor(named("native.isolation")), nothrow));
static int on_named(int value) {
  assert(objc3_executor_is_current(named_executor));
  __atomic_fetch_add(&entries, 1, __ATOMIC_RELEASE);
  return value + 20;
}
int hop_roundtrip(HopCounter *counter,
                  objc3_executor_t *caller) __objc3_async_frame {
  __objc3_defer { __atomic_fetch_add(&cleanups, 1, __ATOMIC_RELEASE); }
  int value = __objc3_await on_main(source_argument());
  assert(objc3_executor_is_current(caller));
  value = __objc3_await on_global(value);
  assert(objc3_executor_is_current(caller));
  value = __objc3_await on_named(value);
  assert(objc3_executor_is_current(caller));
  value = __objc3_await [counter add:value + 10];
  assert(objc3_executor_is_current(caller));
  return value;
}
int hop_nil_roundtrip(void) __objc3_async_frame {
  return __objc3_await[(HopCounter *)0 read];
}
static HopObject *owned_object(void)
    __attribute__((objc_executor(global), nothrow, ns_returns_retained));
static HopObject *owned_object(void) {
  assert(objc3_executor_is_current(global_executor));
  return [HopObject new];
}
HopObject *hop_owned_roundtrip(void) __objc3_async_frame {
  return __objc3_await owned_object();
}
static HopPacket packet_argument(void) {
  assert(objc3_executor_is_current(caller_executor));
  __atomic_fetch_add(&packet_arguments, 1, __ATOMIC_RELEASE);
  return (HopPacket){40,{10,20,30}};
}
HopPacket hop_packet_on_main(HopPacket packet) {
  assert(objc3_executor_is_current(main_executor));
  __atomic_fetch_add(&packet_entries, 1, __ATOMIC_RELEASE);
  packet.value += 1;
  packet.weights[2] += 1;
  return packet;
}
int hop_packet_roundtrip(HopCounter *counter, objc3_executor_t *caller)
    __objc3_async_frame {
  HopPacket packet=__objc3_await hop_packet_on_main(packet_argument());
  assert(objc3_executor_is_current(caller));
  packet=__objc3_await [counter transform:packet];
  assert(objc3_executor_is_current(caller));
  assert(packet.weights[0]==10 && packet.weights[1]==20 && packet.weights[2]==32);
  return packet.value;
}

static int class_arguments, class_entries;
int hop_class_argument(objc3_executor_t *caller) {
  assert(objc3_executor_is_current(caller));
  __atomic_fetch_add(&class_arguments,1,__ATOMIC_RELEASE);
  return 1;
}
int hop_class_arguments(void) {
  return __atomic_load_n(&class_arguments,__ATOMIC_ACQUIRE);
}
int hop_class_entries(void) {
  return __atomic_load_n(&class_entries,__ATOMIC_ACQUIRE);
}
extern Class objc_getClass(const char *);
@implementation HopObject (ClassEntries)
+ (int)classScalar:(int)value {
  assert(objc3_executor_is_current(main_executor));
  __atomic_fetch_add(&class_entries,1,__ATOMIC_RELEASE);
  return value+41;
}
+ (HopPacket)classPacket:(HopPacket)value {
  assert(objc3_executor_is_current(global_executor));
  assert(self==objc_getClass("HopCounter"));
  __atomic_fetch_add(&class_entries,1,__ATOMIC_RELEASE);
  value.value+=1;
  return value;
}
+ (int)classFrame:(int)value __objc3_async_frame {
  assert(objc3_executor_is_current(named_executor));
  __atomic_fetch_add(&class_entries,1,__ATOMIC_RELEASE);
  return value+41;
}
+ (HopObject *)newOnMain {
  assert(objc3_executor_is_current(main_executor));
  __atomic_fetch_add(&class_entries,1,__ATOMIC_RELEASE);
  return [self new];
}
+ (void)classVoid {
  assert(objc3_executor_is_current(main_executor));
  __atomic_fetch_add(&class_entries,1,__ATOMIC_RELEASE);
}
@end
