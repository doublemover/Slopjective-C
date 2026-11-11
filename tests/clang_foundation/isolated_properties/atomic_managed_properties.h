#ifndef OBJC3_NATIVE_ATOMIC_MANAGED_PROPERTIES_H
#define OBJC3_NATIVE_ATOMIC_MANAGED_PROPERTIES_H
#include <pthread.h>
#if __has_feature(objc_arc)
@class NativeAtomicHost;
static int native_atomic_created,native_atomic_destroyed,native_atomic_reentered;
static __unsafe_unretained NativeAtomicHost *native_atomic_reentry_host;
__attribute__((objc_root_class)) @interface NativeAtomicOwner { @public Class isa;int stamp; }
+ (instancetype)newValue:(int)value;
@end
@implementation NativeAtomicOwner
+ (instancetype)newValue:(int)value {
 NativeAtomicOwner *object=(NativeAtomicOwner *)class_createInstance(self,0);object->stamp=value;
 __atomic_add_fetch(&native_atomic_created,1,__ATOMIC_SEQ_CST);return object;
}
@end
/* The deallocation body follows the host interface so its reentrant read has
   the same declared atomic accessor contract as ordinary callers. */
typedef NativeAtomicOwner *NativeAtomicStrong;
typedef NativeAtomicOwner *__weak NativeAtomicWeak;
typedef int (^NativeAtomicBlock)(void);
typedef __objc3_optional(NativeAtomicStrong) NativeAtomicMaybe;
typedef __objc3_optional(NativeAtomicWeak) NativeAtomicWeakMaybe;
typedef __objc3_optional(NativeAtomicBlock) NativeAtomicBlockMaybe;
struct NativeAtomicRecord;
struct NativeAtomicRecord {int generation;NativeAtomicMaybe owner;NativeAtomicWeakMaybe observer;};
struct NativeAtomicRawRecord {int generation;NativeAtomicStrong owner;NativeAtomicWeak observer;};
__attribute__((objc_root_class)) @interface NativeAtomicHost {
 Class isa;NativeAtomicMaybe _value;NativeAtomicWeakMaybe _weak;
 NativeAtomicBlockMaybe _block;struct NativeAtomicRecord _record;struct NativeAtomicRawRecord _raw;
}
+ (instancetype)new;
@property(atomic) NativeAtomicMaybe value;
@property(atomic) NativeAtomicWeakMaybe weakValue;
@property(atomic) NativeAtomicBlockMaybe blockValue;
@property(atomic) struct NativeAtomicRecord record;
@property(atomic) struct NativeAtomicRawRecord rawRecord;
@end
@implementation NativeAtomicHost
+ (instancetype)new{return (NativeAtomicHost *)class_createInstance(self,0);}
@synthesize value=_value,weakValue=_weak,blockValue=_block,record=_record,rawRecord=_raw;
- (void)dealloc{object_dispose(self);}
@end
static int native_atomic_stamp(NativeAtomicMaybe value) {
 return __objc3_match(value){case __objc3_case_some(let owner)=>owner?owner->stamp:-2;case __objc3_case_none=>-1;};
}
@implementation NativeAtomicOwner (AtomicDeallocation)
- (void)dealloc {
 if(stamp==1 && native_atomic_reentry_host && !__atomic_exchange_n(&native_atomic_reentered,1,__ATOMIC_SEQ_CST))
  assert(native_atomic_stamp(native_atomic_reentry_host.value)==2);
 if(stamp==3 && native_atomic_reentry_host) {
  struct NativeAtomicRawRecord value=native_atomic_reentry_host.rawRecord;
  assert(value.owner && value.owner->stamp==4);
  __atomic_fetch_or(&native_atomic_reentered,2,__ATOMIC_SEQ_CST);
 }
 __atomic_add_fetch(&native_atomic_destroyed,1,__ATOMIC_SEQ_CST);object_dispose(self);
}
@end
struct NativeAtomicThread {__unsafe_unretained NativeAtomicHost *host;int writer;};
static void *native_atomic_thread(void *context) {
 struct NativeAtomicThread *thread=(struct NativeAtomicThread *)context;
 for(int i=1;i<=80;++i) {
  @autoreleasepool {
   if(thread->writer) {
    int generation=1000*thread->writer+i;
    struct NativeAtomicRecord value={generation,
       __objc3_some(NativeAtomicStrong,[NativeAtomicOwner newValue:generation]),
       __objc3_none(NativeAtomicWeak)};
    thread->host.record=value;
   }else {
    struct NativeAtomicRecord value=thread->host.record;
    assert(native_atomic_stamp(value.owner)==value.generation);
   }
  }
 }
 return 0;
}
static pthread_mutex_t native_atomic_phase_mutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t native_atomic_phase_condition=PTHREAD_COND_INITIALIZER;
static int native_atomic_phase;
static void *native_atomic_snapshot_reader(void *context) {
 struct NativeAtomicThread *thread=(struct NativeAtomicThread *)context;
 struct NativeAtomicRecord snapshot=thread->host.record;
 pthread_mutex_lock(&native_atomic_phase_mutex);native_atomic_phase=1;
 pthread_cond_broadcast(&native_atomic_phase_condition);
 while(native_atomic_phase!=2)pthread_cond_wait(&native_atomic_phase_condition,&native_atomic_phase_mutex);
 pthread_mutex_unlock(&native_atomic_phase_mutex);
 assert(snapshot.generation==500 && native_atomic_stamp(snapshot.owner)==500);
 return 0;
}
static void native_atomic_snapshot_lifetime(NativeAtomicHost *host) {
 {
  struct NativeAtomicRecord initial={500,__objc3_some(NativeAtomicStrong,[NativeAtomicOwner newValue:500]),__objc3_none(NativeAtomicWeak)};
  host.record=initial;
 }
 native_atomic_phase=0;struct NativeAtomicThread context={host,0};pthread_t reader;
 assert(pthread_create(&reader,0,native_atomic_snapshot_reader,&context)==0);
 pthread_mutex_lock(&native_atomic_phase_mutex);
 while(native_atomic_phase!=1)pthread_cond_wait(&native_atomic_phase_condition,&native_atomic_phase_mutex);
 pthread_mutex_unlock(&native_atomic_phase_mutex);
 {
  struct NativeAtomicRecord replacement={501,__objc3_some(NativeAtomicStrong,[NativeAtomicOwner newValue:501]),__objc3_none(NativeAtomicWeak)};
  host.record=replacement;
 }
 pthread_mutex_lock(&native_atomic_phase_mutex);native_atomic_phase=2;
 pthread_cond_broadcast(&native_atomic_phase_condition);pthread_mutex_unlock(&native_atomic_phase_mutex);
 assert(pthread_join(reader,0)==0);
}
#ifdef __cplusplus
extern "C" {
#endif
void objc_copyCppObjectAtomic(void *,const void *,void (*)(void *,const void *));
void objc_getCppObjectAtomic(void *,const void *,void (*)(void *,const void *));
void objc_setCppObjectAtomic(void *,const void *,void (*)(void *,const void *));
#ifdef __cplusplus
}
static bool native_atomic_copy_throw;
#endif
static void native_atomic_copy_int(void *destination,const void *source) {
#ifdef __cplusplus
 if(native_atomic_copy_throw){native_atomic_copy_throw=false;throw 755;}
#endif
 *(int *)destination=*(const int *)source;
}
static void native_atomic_runtime_lock_suite(void) {
 int cells[2] __attribute__((aligned(256)))={17,19};
 /* Both pointers intentionally share a lock slot. A double acquisition would
    hang even without concurrency. */
 objc_copyCppObjectAtomic(&cells[1],&cells[0],native_atomic_copy_int);assert(cells[1]==17);
#ifdef __cplusplus
 for(int route=0;route!=3;++route) {
  native_atomic_copy_throw=true;bool caught=false;
  try {
   if(route==0)objc_getCppObjectAtomic(&cells[1],&cells[0],native_atomic_copy_int);
   else if(route==1)objc_setCppObjectAtomic(&cells[1],&cells[0],native_atomic_copy_int);
   else objc_copyCppObjectAtomic(&cells[1],&cells[0],native_atomic_copy_int);
  }catch(int error){assert(error==755);caught=true;}
  assert(caught && !native_atomic_copy_throw);
  objc_copyCppObjectAtomic(&cells[1],&cells[0],native_atomic_copy_int);assert(cells[1]==17);
 }
#endif
}
static void atomic_managed_property_suite(void) {
 @autoreleasepool {
  NativeAtomicHost *host=[NativeAtomicHost new];
  native_atomic_reentry_host=host;native_atomic_reentered=0;
  __weak NativeAtomicOwner *observer;
  {
   NativeAtomicOwner *object=[NativeAtomicOwner newValue:1];observer=object;
   host.value=__objc3_some(NativeAtomicStrong,object);
   host.weakValue=__objc3_some(NativeAtomicWeak,object);
  }
  assert(observer && native_atomic_stamp(host.value)==1);
  host.value=__objc3_some(NativeAtomicStrong,[NativeAtomicOwner newValue:2]);
  assert(native_atomic_reentered && !observer && native_atomic_stamp(host.value)==2);
  NativeAtomicWeakMaybe weak=host.weakValue;
  assert(__objc3_has_value(weak));
  int zero=__objc3_match(weak){case __objc3_case_some(let owner)=>owner==(id)0;case __objc3_case_none=>0;};assert(zero);
  host.blockValue=__objc3_some(NativeAtomicBlock,^int{return 23;});
  NativeAtomicBlockMaybe block=host.blockValue;
  int result=__objc3_match(block){case __objc3_case_some(let callback)=>callback();case __objc3_case_none=>-1;};assert(result==23);
  {
   NativeAtomicOwner *owner=[NativeAtomicOwner newValue:3];
   struct NativeAtomicRawRecord value={3,owner,owner};host.rawRecord=value;
  }
  {
   NativeAtomicOwner *owner=[NativeAtomicOwner newValue:4];
   struct NativeAtomicRawRecord value={4,owner,owner};host.rawRecord=value;
  }
  assert((native_atomic_reentered&2)!=0);
  native_atomic_reentry_host=(NativeAtomicHost *)0;
  struct NativeAtomicRecord initial={29,
      __objc3_some(NativeAtomicStrong,[NativeAtomicOwner newValue:29]),__objc3_none(NativeAtomicWeak)};
  host.record=initial;
  struct NativeAtomicThread contexts[4]={{host,1},{host,2},{host,0},{host,0}};pthread_t workers[4];
  for(int i=0;i!=4;++i)assert(pthread_create(&workers[i],0,native_atomic_thread,&contexts[i])==0);
  for(int i=0;i!=4;++i)assert(pthread_join(workers[i],0)==0);
  native_atomic_snapshot_lifetime(host);native_atomic_runtime_lock_suite();host=(NativeAtomicHost *)0;
 }
 assert(native_atomic_created==native_atomic_destroyed);
}
#else
static void atomic_managed_property_suite(void){}
#endif
#endif
