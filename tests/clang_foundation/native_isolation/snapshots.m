#include "provider.h"
#include <assert.h>
extern void snapshot_release(void *) __asm__("objc_release");
extern void *snapshot_block_copy(const void *) __asm__("_Block_copy");
extern void snapshot_block_release(const void *) __asm__("_Block_release");
static int snapshot_entries, snapshot_arguments, snapshot_before, snapshot_fail;
void hop_snapshot_reset(int fail) {
  snapshot_entries=snapshot_arguments=0;
  snapshot_before=hop_destroyed();snapshot_fail=fail;
}
int hop_snapshot_entries(void) {return snapshot_entries;}
int hop_snapshot_arguments(void) {return snapshot_arguments;}
static HopObject *snapshot_source(void) __attribute__((ns_returns_retained));
static HopObject *snapshot_source(void) {++snapshot_arguments;return [HopObject new];}
static void snapshot_entered(void) {
  objc3_executor_t *main=0;
  assert(objc3_executor_get_main(&main)==OBJC3_SERVICE_OK);
  assert(objc3_executor_is_current(main));objc3_executor_release(main);
  ++snapshot_entries;
}
static int drop_snapshot(HopObject *__strong *slot) {
#if !__has_feature(objc_arc)
  snapshot_release(*slot);
#endif
  *slot=0;
  // This checks the ownership before frame creation, without dereferencing a
  // possibly freed receiver when the compiler misses the operand snapshot.
  assert(hop_destroyed()==snapshot_before);
  if(snapshot_fail)hop_fail_next_frame_allocation();
  return 0;
}
@interface HopObject (Snapshots)
- (int)snapshotAfter:(int)value __attribute__((nothrow,objc_executor(main)));
+ (int)snapshotObject:(HopObject *)object after:(int)value
    __attribute__((nothrow,objc_executor(main)));
@end
@implementation HopObject (Snapshots)
- (int)snapshotAfter:(int)value {snapshot_entered();return [self number]+value;}
+ (int)snapshotObject:(HopObject *)object after:(int)value {
  snapshot_entered();return [object number]+value;
}
@end
static int snapshot_function(HopObject *object,int value)
    __attribute__((nothrow,objc_executor(main)));
static int snapshot_function(HopObject *object,int value) {
  snapshot_entered();return [object number]+value;
}
typedef int (^SnapshotWork)(void) __attribute__((objc_unsafe_sendable));
static int snapshot_work(SnapshotWork work,int value)
    __attribute__((nothrow,objc_executor(main)));
static int snapshot_work(SnapshotWork work,int value) {
  snapshot_entered();return work()+value;
}
static int drop_snapshot_work(SnapshotWork __strong *slot) {
#if !__has_feature(objc_arc)
  snapshot_block_release(*slot);
#endif
  *slot=0;assert(hop_destroyed()==snapshot_before);
  if(snapshot_fail)hop_fail_next_frame_allocation();
  return 0;
}
int hop_snapshot_roundtrip(int kind,objc3_executor_t *caller) __objc3_async_frame {
  HopObject *object=snapshot_source();
#if !__has_feature(objc_arc)
  __objc3_defer {if(object)snapshot_release(object);}
#endif
  int result=0;
  switch(kind) {
  case 0: result=__objc3_await [object snapshotAfter:drop_snapshot(&object)];break;
  case 1: result=__objc3_await [HopObject snapshotObject:object after:drop_snapshot(&object)];break;
  case 2: result=__objc3_await snapshot_function(object,drop_snapshot(&object));break;
  case 3: {
    SnapshotWork work=^{return [object number];};
#if !__has_feature(objc_arc)
    work=(SnapshotWork)snapshot_block_copy(work);snapshot_release(object);
#endif
    object=0;
    result=__objc3_await snapshot_work(work,drop_snapshot_work(&work));
    break;
  }
  case 4: result=__objc3_await [object snapshotAfter:hop_throwing_argument()];break;
  case 5: result=__objc3_await [HopObject snapshotObject:object after:hop_throwing_argument()];break;
  case 6: result=__objc3_await snapshot_function(object,hop_throwing_argument());break;
  case 7: {
    SnapshotWork work=^{return [object number];};
#if !__has_feature(objc_arc)
    work=(SnapshotWork)snapshot_block_copy(work);
    __objc3_defer {snapshot_block_release(work);}
    snapshot_release(object);
#endif
    object=0;
    result=__objc3_await snapshot_work(work,hop_throwing_argument());
    break;
  }
  default:assert(!"unknown isolation snapshot case");
  }
  assert(objc3_executor_is_current(caller));return result;
}
