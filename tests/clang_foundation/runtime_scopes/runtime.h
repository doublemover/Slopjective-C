#ifndef OBJC3_RUNTIME_SCOPES_H
#define OBJC3_RUNTIME_SCOPES_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#pragma clang assume_nonnull begin
#ifdef __cplusplus
extern "C" {
#endif
id _Nullable class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
id _Nullable object_dispose(__unsafe_unretained id);
#if !__has_feature(objc_arc)
id objc_autorelease(id);
#endif
#ifdef __cplusplus
}
#endif
static int scope_created,scope_destroyed,scope_deferred,scope_lock_evaluations;
static void *(*scope_real_push)(void);
static void (*scope_real_pop)(void *);
static int (*scope_real_enter)(__unsafe_unretained id);
static int (*scope_real_exit)(__unsafe_unretained id);
static void *scope_captured_pool;
static int scope_capture_next_pool,scope_captured_pops;
static int scope_observe_lock,scope_expected_live_destroyed,scope_observed_enters,scope_observed_exits;
static __unsafe_unretained id scope_observed_receiver;
#ifdef __cplusplus
extern "C" {
#endif
__attribute__((noinline,nothrow)) void *objc_autoreleasePoolPush(void) {
  if(!scope_real_push)scope_real_push=(void *(*)(void))dlsym(RTLD_NEXT,"objc_autoreleasePoolPush");
  assert(scope_real_push);void *token=scope_real_push();
  if(scope_capture_next_pool){scope_capture_next_pool=0;scope_captured_pool=token;scope_captured_pops=0;}
  return token;
}
__attribute__((noinline)) void objc_autoreleasePoolPop(void *token) {
  if(!scope_real_pop)scope_real_pop=(void (*)(void *))dlsym(RTLD_NEXT,"objc_autoreleasePoolPop");
  assert(scope_real_pop);if(scope_captured_pool && token==scope_captured_pool)++scope_captured_pops;
  scope_real_pop(token);
}
__attribute__((noinline,nothrow)) int objc_sync_enter(__unsafe_unretained id receiver) {
  if(!scope_real_enter)scope_real_enter=(int (*)(__unsafe_unretained id))dlsym(RTLD_NEXT,"objc_sync_enter");
  assert(scope_real_enter);if(scope_observe_lock && receiver==scope_observed_receiver)++scope_observed_enters;
  return scope_real_enter(receiver);
}
__attribute__((noinline,nothrow)) int objc_sync_exit(__unsafe_unretained id receiver) {
  if(!scope_real_exit)scope_real_exit=(int (*)(__unsafe_unretained id))dlsym(RTLD_NEXT,"objc_sync_exit");
  assert(scope_real_exit);
  if(scope_observe_lock && receiver==scope_observed_receiver){assert(scope_destroyed==scope_expected_live_destroyed);++scope_observed_exits;}
  return scope_real_exit(receiver);
}
#ifdef __cplusplus
}
#endif

__attribute__((objc_root_class)) @interface ScopeProbe {
  Class isa;
@public char payload;
#if !__has_feature(objc_arc)
  int references;
#endif
}
+ (instancetype _Nonnull)new;
#if !__has_feature(objc_arc)
- (id)retain;
- (id)autorelease;
- (oneway void)release;
#endif
@end
@implementation ScopeProbe
+ (instancetype _Nonnull)new {
  ScopeProbe *value=class_createInstance(self,0);assert(value);value->payload='s';
#if !__has_feature(objc_arc)
  value->references=1;
#endif
  ++scope_created;return value;
}
#if !__has_feature(objc_arc)
- (id)retain {++references;return self;}
- (id)autorelease {return objc_autorelease(self);}
- (oneway void)release {if(!--references)[self dealloc];}
#endif
- (void)dealloc {++scope_destroyed;object_dispose(self);}
@end
static ScopeProbe *_Nonnull scope_exception;
static void scope_use(const char *bytes __attribute__((noescape))) {assert(*bytes=='s');}
static void scope_pool_path(int mode) {
  for(int iteration=0;iteration!=2;++iteration) {
    @autoreleasepool {
      int before=scope_destroyed;
      ScopeProbe *_Nonnull owner=[ScopeProbe new];
#if !__has_feature(objc_arc)
      [owner autorelease];
#endif
      const char *view=&owner->payload;
      __objc3_defer {assert(scope_destroyed==before);scope_use(view);++scope_deferred;}
      if(mode==0)return;
      if(mode==1)goto finished;
      if(mode==2)break;
      if(mode==3)continue;
    }
  }
finished:return;
}
static void scope_sync_path(int mode) {
  int before=scope_destroyed;
  ScopeProbe *_Nonnull owner=[ScopeProbe new];
  const char *view=&owner->payload;
  scope_observed_receiver=owner;scope_observe_lock=1;scope_expected_live_destroyed=before;
  @try {
#if !__has_feature(objc_arc)
    withLifetime(owner) {
#endif
      @synchronized((++scope_lock_evaluations,owner)) {
#if __has_feature(objc_arc)
        owner=(id)0;
#else
        [owner release];
#endif
        __objc3_defer {assert(scope_destroyed==before);scope_use(view);++scope_deferred;}
        scope_use(view);
        if(mode==0)return;
        if(mode==1)goto finished;
        if(mode==2)@throw scope_exception;
      }
#if !__has_feature(objc_arc)
    }
#endif
  } @catch(id exception) {assert(mode==2 && exception==scope_exception && scope_destroyed==before+1);}
finished:
  assert(scope_destroyed==before+1);
}
#if !__has_feature(objc_arc)
static void scope_counted_pool(void) {
  int before=scope_destroyed;
  ScopeProbe *_Nonnull owner=[ScopeProbe new];[owner retain];
  const char *view=&owner->payload;
  @autoreleasepool {[owner autorelease];scope_use(view);}
  assert(scope_destroyed==before);scope_use(view);[owner release];
  assert(scope_destroyed==before+1);
}
#endif
static void scope_normal_only_pool_exception(void) {
  int before=scope_destroyed,deferred=scope_deferred;
  void *token=0;scope_capture_next_pool=1;
  @try {
    @autoreleasepool {
      token=scope_captured_pool;assert(token);
      ScopeProbe *_Nonnull owner=[ScopeProbe new];
#if !__has_feature(objc_arc)
      [owner autorelease];
#endif
      const char *view=&owner->payload;
      __objc3_defer {scope_use(view);++scope_deferred;}
      @throw scope_exception;
    }
  } @catch(id exception) {assert(exception==scope_exception);}
  assert(token && scope_captured_pops==0 && scope_deferred==deferred+1);
#if __has_feature(objc_arc)
  assert(scope_destroyed==before+1); // The local strong binding already unwound.
#else
  assert(scope_destroyed==before); // Its explicit autorelease remains queued.
#endif
  // The ordinary native pool intentionally survived exception propagation.
  // Close its recorded token explicitly so this witness retains no leaked pool.
  objc_autoreleasePoolPop(token);
  assert(scope_captured_pops==1 && scope_destroyed==before+1);scope_captured_pool=0;
}
static void runtime_scope_suite(void) {
  int initial_created=scope_created,initial_destroyed=scope_destroyed;
  @autoreleasepool {
    scope_exception=[ScopeProbe new];
    for(int mode=0;mode!=4;++mode) {
      int created=scope_created,destroyed=scope_destroyed,deferred=scope_deferred;
      scope_pool_path(mode);int count=mode==3?2:1;
      assert(scope_created==created+count && scope_destroyed==destroyed+count && scope_deferred==deferred+count);
    }
    for(int mode=0;mode!=4;++mode) {
      int created=scope_created,destroyed=scope_destroyed,deferred=scope_deferred,arguments=scope_lock_evaluations;
      int entered=scope_observed_enters,exited=scope_observed_exits;
      @autoreleasepool {scope_sync_path(mode);}scope_observe_lock=0;
      if(scope_observed_enters!=entered+1 || scope_observed_exits!=exited+1)
        fprintf(stderr,"scope mode %d enter %d/%d exit %d/%d\n",mode,scope_observed_enters,entered,scope_observed_exits,exited);
      assert(scope_observed_enters==entered+1 && scope_observed_exits==exited+1);
      assert(scope_created==created+1 && scope_destroyed==destroyed+1 && scope_deferred==deferred+1);
      assert(scope_lock_evaluations==arguments+1);
    }
    scope_normal_only_pool_exception();
#if !__has_feature(objc_arc)
    scope_counted_pool();[scope_exception release];
#endif
    scope_exception=(id)0;
  }
  assert(scope_created-initial_created==scope_destroyed-initial_destroyed);
}
#pragma clang assume_nonnull end
#endif
