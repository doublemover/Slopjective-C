#include "provider.h"
#include <assert.h>
#include <pthread.h>
extern Class objc_getClass(const char *);
extern void release_object(void *) __asm__("objc_release");
struct Lookup {void *object;objc3_actor_t *actor;};
static void *lookup_thread(void *opaque) {
  struct Lookup *lookup=(struct Lookup *)opaque;
  @autoreleasepool {
    assert(objc3_actor_get_or_create_for_object(lookup->object,0,&lookup->actor)==OBJC3_SERVICE_OK);
  }
  return 0;
}
static void unused_job(void *context) {(void)context;}
void hop_automatic_association_checks(void) {
  objc3_actor_t *actor=0;
  assert(objc3_actor_get_or_create_for_object(0,0,&actor)==OBJC3_SERVICE_INVALID_ARGUMENT && !actor);
  @autoreleasepool {
    void *object;
#if __has_feature(objc_arc)
    object=(__bridge_retained void *)[HopObject new];
#else
    object=(void *)[HopObject new];
#endif
    enum {Count=12};pthread_t threads[Count];struct Lookup lookups[Count];
    for(unsigned i=0;i<Count;++i) {
      lookups[i]=(struct Lookup){object,0};
      assert(!pthread_create(&threads[i],0,lookup_thread,&lookups[i]));
    }
    for(unsigned i=0;i<Count;++i)assert(!pthread_join(threads[i],0));
    actor=lookups[0].actor;
    for(unsigned i=1;i<Count;++i) {
      assert(objc3_actor_identity(actor)==objc3_actor_identity(lookups[i].actor));
      objc3_actor_release(lookups[i].actor);
    }
    objc3_executor_t *destination=0,*other_destination=0;
    assert(objc3_executor_get_global(&destination)==OBJC3_SERVICE_OK);
    assert(objc3_executor_get_named("automatic.other",&other_destination)==OBJC3_SERVICE_OK);
    objc3_actor_t *same=0;
    assert(objc3_actor_get_or_create_for_object(object,destination,&same)==OBJC3_SERVICE_OK && same==actor);
    objc3_actor_release(same);
    same=(objc3_actor_t *)1;
    assert(objc3_actor_get_or_create_for_object(object,other_destination,&same)==OBJC3_SERVICE_INVALID_ARGUMENT && !same);
    objc3_actor_t *conflict=0;
    assert(objc3_actor_create(destination,&conflict)==OBJC3_SERVICE_OK);
    assert(objc3_actor_bind_object(object,conflict)==OBJC3_SERVICE_INVALID_ARGUMENT);
    objc3_actor_release(conflict);
    objc3_executor_release(other_destination);objc3_executor_release(destination);
    release_object(object);
  }
  // Outstanding actor leases do not keep the source object alive. Association
  // teardown closes the mailbox even while this independent lease remains.
  objc3_actor_hop_facts facts={sizeof(facts),OBJC3_ACTOR_SERVICE_VERSION};
  assert(objc3_actor_enqueue(actor,unused_job,0,0,&facts)==OBJC3_SERVICE_CLOSED);
  assert(objc3_actor_join(actor)==OBJC3_SERVICE_OK);
  objc3_actor_release(actor);
}

actor class AutomaticCounter : HopObject {int total;}
- (int)add:(int)n __attribute__((nothrow));
+ (int)isolatedValue __attribute__((nothrow,objc_executor(global)));
@end
@implementation AutomaticCounter
- (int)add:(int)n {
  objc3_current_actor_facts facts={sizeof(facts),OBJC3_ACTOR_SERVICE_VERSION};
  assert(objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK && facts.actor_identity);
  total+=n;return total;
}
+ (int)isolatedValue {return 42;}
@end
typedef union AutomaticPacket {long whole;double fraction;} AutomaticPacket;
static AutomaticPacket union_entry(AutomaticPacket value)
    __attribute__((nothrow,objc_executor(global)));
static AutomaticPacket union_entry(AutomaticPacket value) {value.whole+=2;return value;}
int hop_automatic_transfer_roundtrip(void) __objc3_async_frame {
  AutomaticCounter *counter=[AutomaticCounter new];
  int first=__objc3_await [counter add:40];
  int second=__objc3_await [counter add:2];
  Class receiver=objc_getClass("AutomaticCounter");
  int class_value=__objc3_await [receiver isolatedValue];
  AutomaticPacket input={.whole=40};
  AutomaticPacket result=__objc3_await union_entry(input);
#if !__has_feature(objc_arc)
  release_object(counter);
#endif
  assert(first==40 && second==42 && class_value==42 && input.whole==40 && result.whole==42);
  return second+class_value+(int)result.whole;
}
