#ifndef OBJC3_NATIVE_BYREF_C_STORAGE_H
#define OBJC3_NATIVE_BYREF_C_STORAGE_H
#include <pthread.h>
#ifdef __cplusplus
extern "C" {
#endif
void _Block_object_assign(void *,const void *,int);
void _Block_object_dispose(const void *,int);
#ifdef __cplusplus
}
#endif
struct ByrefRaceCell {
 void *isa;struct ByrefRaceCell *forwarding;int flags,size;
 void (*keep)(void *,void *);void (*dispose)(void *);int value;
} __attribute__((aligned(64)));
static pthread_mutex_t byref_race_mutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t byref_race_condition=PTHREAD_COND_INITIALIZER;
static unsigned byref_race_entered;
static int byref_race_live,byref_race_created,byref_race_destroyed;
static void byref_race_keep(void *destination,void *source) {
 assert((__UINTPTR_TYPE__)destination%64==0);
 assert(((struct ByrefRaceCell *)destination)->value==((struct ByrefRaceCell *)source)->value);
 __atomic_fetch_add(&byref_race_live,1,__ATOMIC_SEQ_CST);
 __atomic_fetch_add(&byref_race_created,1,__ATOMIC_SEQ_CST);
 pthread_mutex_lock(&byref_race_mutex);
 if(++byref_race_entered==2)pthread_cond_broadcast(&byref_race_condition);
 while(byref_race_entered<2)pthread_cond_wait(&byref_race_condition,&byref_race_mutex);
 pthread_mutex_unlock(&byref_race_mutex);
}
static void byref_race_dispose(void *value) {
 assert((__UINTPTR_TYPE__)value%64==0);
 assert(__atomic_fetch_sub(&byref_race_live,1,__ATOMIC_SEQ_CST)>0);
 __atomic_fetch_add(&byref_race_destroyed,1,__ATOMIC_SEQ_CST);
}
struct ByrefRaceThread {struct ByrefRaceCell *source,*result;};
static void *byref_race_thread(void *context) {
 struct ByrefRaceThread *thread=(struct ByrefRaceThread *)context;
 _Block_object_assign(&thread->result,thread->source,8);return 0;
}
#ifdef __cplusplus
static bool byref_keep_throw;
static int byref_keep_live,byref_keep_created,byref_keep_destroyed;
static void byref_keep_failure(void *destination,void *source) {
 (void)destination;(void)source;
 if(byref_keep_throw)throw 752;
 ++byref_keep_live;++byref_keep_created;
}
static void byref_keep_dispose(void *value) {
 (void)value;assert(byref_keep_live>0);--byref_keep_live;++byref_keep_destroyed;
}
static void byref_keep_failure_suite() {
 struct ByrefRaceCell source={(void *)64,0,(1<<25)|(1<<26),sizeof(struct ByrefRaceCell),byref_keep_failure,byref_keep_dispose,137};
 source.forwarding=&source;struct ByrefRaceCell *result=0;
 byref_keep_throw=true;bool caught=false;
 try{_Block_object_assign(&result,&source,8);}catch(int error){assert(error==752);caught=true;}
 assert(caught && !result && source.forwarding==&source && !byref_keep_live);
 byref_keep_throw=false;_Block_object_assign(&result,&source,8);
 assert(result && result==source.forwarding && byref_keep_live==1);
 _Block_object_dispose(result,8);_Block_object_dispose(&source,8);
 assert(!byref_keep_live && byref_keep_created==byref_keep_destroyed);
}
#else
static void byref_keep_failure_suite(void){}
#endif
static void byref_c_storage_suite(void) {
 {
  __block int value __attribute__((aligned(64)))=117;
  __unsafe_unretained AlignedCBlockProbe literal=^int {
   assert((__UINTPTR_TYPE__)&value%64==0);return ++value;
  };
#if __has_feature(objc_arc)
  AlignedCBlockProbe copied=(__bridge_transfer AlignedCBlockProbe)_Block_copy((__bridge const void *)literal);
#else
  AlignedCBlockProbe copied=(AlignedCBlockProbe)_Block_copy((const void *)literal);
#endif
  assert(copied()==118 && value==118 && literal()==119);
#if __has_feature(objc_arc)
  copied=(AlignedCBlockProbe)0;
#else
  _Block_release((const void *)copied);
#endif
 }
 for(int round=0;round!=4;++round) {
  byref_race_entered=0;
  struct ByrefRaceCell source={(void *)64,0,(1<<25)|(1<<26),sizeof(struct ByrefRaceCell),byref_race_keep,byref_race_dispose,121};
  source.forwarding=&source;
  struct ByrefRaceThread threads[2]={{&source,0},{&source,0}};pthread_t workers[2];
  for(int i=0;i!=2;++i)assert(pthread_create(&workers[i],0,byref_race_thread,&threads[i])==0);
  for(int i=0;i!=2;++i)assert(pthread_join(workers[i],0)==0);
  assert(threads[0].result && threads[0].result==threads[1].result);
  assert((__atomic_load_n(&threads[0].result->flags,__ATOMIC_SEQ_CST)&0xffffff)==3);
  assert(byref_race_live==1 && byref_race_created==byref_race_destroyed+1);
  _Block_object_dispose(threads[0].result,8);_Block_object_dispose(threads[1].result,8);
  assert(byref_race_live==1);_Block_object_dispose(&source,8);
  assert(!byref_race_live && byref_race_created==byref_race_destroyed);
 }
 byref_keep_failure_suite();
}
#endif
