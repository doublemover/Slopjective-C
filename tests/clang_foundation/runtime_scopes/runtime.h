#ifndef OBJC3_RUNTIME_SCOPES_H
#define OBJC3_RUNTIME_SCOPES_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __cplusplus
#include <exception>
#include <sys/wait.h>
#include <unistd.h>
#endif
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

static int scope_finally_trace[32],scope_finally_trace_count;
static void scope_finally_event(int value) {
  assert(scope_finally_trace_count<32);scope_finally_trace[scope_finally_trace_count++]=value;
}
static int scope_finally_transfer(int mode) {
  ScopeProbe *_Nonnull owner=[ScopeProbe new];
#if !__has_feature(objc_arc)
  __objc3_defer {[owner release];}
#endif
  const char *view=&owner->payload;
  @try {
    scope_finally_event(1);
    __objc3_defer {scope_use(view);scope_finally_event(2);}
    if(mode==0)return 17;
    if(mode==1)goto finished;
    if(mode==2)@throw scope_exception;
  } @catch(id exception) {assert(exception==scope_exception);scope_finally_event(3);}
    @finally {
      scope_use(view);scope_finally_event(4);
      __objc3_defer {scope_use(view);scope_finally_event(5);}
    }
  scope_finally_event(6);
finished:scope_finally_event(7);return 19;
}
static int scope_finally_replaces_return(ScopeProbe *_Nonnull owner) {
  const char *view=&owner->payload;
  @try {scope_use(view);return 11;} @finally {scope_use(view);return 23;}
}
static void scope_finally_replaces_exception(ScopeProbe *_Nonnull owner) {
  const char *view=&owner->payload;int caught=0;
  @try {
    @try {@throw scope_exception;}
    @finally {scope_use(view);@throw owner;}
  } @catch(id exception) {assert(exception==owner);scope_use(view);++caught;}
  assert(caught==1);
}
static int scope_finally_suppresses_exception(ScopeProbe *_Nonnull owner) {
  const char *view=&owner->payload;
  @try {@throw scope_exception;} @finally {scope_use(view);return 29;}
}
static void scope_finally_outward_jump(ScopeProbe *_Nonnull owner) {
  const char *view=&owner->payload;int visits=0;
repeat:
  if(visits){assert(visits==1);scope_use(view);return;}
  @try {scope_use(view);return;}
  @finally {++visits;scope_use(view);goto repeat;}
}
static void scope_finally_loop(ScopeProbe *_Nonnull owner) {
  const char *view=&owner->payload;int finalized=0,iterations=0;
  for(int i=0;i!=4;++i) {
    ++iterations;
    @try {scope_use(view);if(i==0)continue;if(i==2)break;}
    @finally {scope_use(view);++finalized;}
  }
  assert(iterations==3 && finalized==3);
}
static int scope_finally_replaces_return_with_loop(ScopeProbe *_Nonnull owner) {
  const char *view=&owner->payload;int finalized=0;
  for(int i=0;i!=3;++i) {
    @try {scope_use(view);return 11;}
    @finally {scope_use(view);++finalized;if(i==0)continue;break;}
  }
  return finalized;
}
static void scope_finally_suite(void) {
  for(int mode=0;mode!=4;++mode) {
    int before=scope_destroyed;scope_finally_trace_count=0;
    assert(scope_finally_transfer(mode)==(mode==0?17:19));
    assert(scope_destroyed==before+1);
    const int expected[4][7]={{1,2,4,5,0,0,0},{1,2,4,5,7,0,0},
                            {1,2,3,4,5,6,7},{1,2,4,5,6,7,0}};
    const int counts[4]={4,5,7,6};assert(scope_finally_trace_count==counts[mode]);
    for(int i=0;i!=counts[mode];++i)assert(scope_finally_trace[i]==expected[mode][i]);
  }
  ScopeProbe *_Nonnull owner=[ScopeProbe new];
#if !__has_feature(objc_arc)
  __objc3_defer {[owner release];}
#endif
  assert(scope_finally_replaces_return(owner)==23);
  assert(scope_finally_replaces_return_with_loop(owner)==2);
  assert(scope_finally_suppresses_exception(owner)==29);
  scope_finally_replaces_exception(owner);scope_finally_outward_jump(owner);scope_finally_loop(owner);
#ifdef __cplusplus
  const char *view=&owner->payload;int caught=0;
  try {@try {throw 17;} @finally {scope_use(view);}}
  catch(int value){assert(value==17);scope_use(view);++caught;}
  assert(caught==1);
#endif
}


#if __has_feature(objc_arc)
static int scope_partial_initializer(ScopeProbe *_Nullable __strong *slot,
                                     const char *view __attribute__((noescape)),int fail) {
  *slot=(id)0;scope_use(view);
  if(fail)@throw scope_exception;
  return 31;
}
static void scope_partial_record(int fail) {
  int before=scope_destroyed,caught=0;
  ScopeProbe *_Nonnull owner=[ScopeProbe new];const char *view=&owner->payload;
  @try {
    struct Pair {ScopeProbe *_Nullable first;int tag;};
    struct Pair pair={owner,scope_partial_initializer(&owner,view,fail)};
    __objc3_defer {assert(scope_destroyed==before);scope_use(view);}
    scope_use(view);assert(pair.first && pair.tag==31 && !owner);
  } @catch(id exception) {assert(fail && exception==scope_exception);++caught;}
  assert(caught==fail && scope_destroyed==before+1);
}
static void scope_partial_array(int fail) {
  int before=scope_destroyed,caught=0;
  ScopeProbe *_Nonnull owner=[ScopeProbe new];const char *view=&owner->payload;
  @try {
    ScopeProbe *_Nullable values[2]={owner,(scope_partial_initializer(&owner,view,fail),(id)0)};
    __objc3_defer {assert(scope_destroyed==before);scope_use(view);}
    scope_use(view);assert(values[0] && !values[1] && !owner);
  } @catch(id exception) {assert(fail && exception==scope_exception);++caught;}
  assert(caught==fail && scope_destroyed==before+1);
}
static void scope_partial_nested(int fail) {
  int before=scope_destroyed,caught=0;
  ScopeProbe *_Nonnull owner=[ScopeProbe new];const char *view=&owner->payload;
  @try {
    struct Inner {ScopeProbe *_Nullable owner;int tag;};
    struct Outer {struct Inner values[2];};
    struct Outer outer={{{owner,11},{owner,scope_partial_initializer(&owner,view,fail)}}};
    __objc3_defer {assert(scope_destroyed==before);scope_use(view);}
    scope_use(view);assert(outer.values[0].owner && outer.values[1].owner && !owner);
  } @catch(id exception) {assert(fail && exception==scope_exception);++caught;}
  assert(caught==fail && scope_destroyed==before+1);
}
#ifdef __cplusplus
static void scope_partial_lambda(int fail) {
  int before=scope_destroyed,caught=0;
  ScopeProbe *_Nonnull owner=[ScopeProbe new];const char *view=&owner->payload;
  @try {
    auto work=[first=owner,tag=scope_partial_initializer(&owner,view,fail),view] {
      assert(first && tag==31);scope_use(view);
    };
    work();assert(!owner && scope_destroyed==before);
  } @catch(id exception) {assert(fail && exception==scope_exception);++caught;}
  assert(caught==fail && scope_destroyed==before+1);
}
#endif
static void scope_partial_suite(void) {
  for(int fail=0;fail!=2;++fail) {
    scope_partial_record(fail);scope_partial_array(fail);scope_partial_nested(fail);
#ifdef __cplusplus
    scope_partial_lambda(fail);
#endif
  }
}
#endif


#ifdef __cplusplus
static int scope_disposal_trace[8],scope_disposal_count,scope_disposal_allocations,scope_disposal_frees;
static int scope_disposal_deallocation_throws,scope_disposal_expected_child_frees,scope_disposal_operands;
[[noreturn]] static void scope_disposal_fault(void) {throw 79;}
static void scope_disposal_event(int event) {
  assert(scope_disposal_count<8);scope_disposal_trace[scope_disposal_count++]=event;
}
struct ScopeDisposal {
  int failure;
  explicit ScopeDisposal(int fail) noexcept:failure(fail) {}
  static void *operator new(__SIZE_TYPE__ size) {
    void *value=malloc(size);assert(value);++scope_disposal_allocations;return value;
  }
  ~ScopeDisposal() noexcept(false) {scope_disposal_event(1);if(failure)throw 73;}
  static void operator delete(void *value) noexcept {
    scope_disposal_event(2);++scope_disposal_frees;free(value);
    if(scope_disposal_deallocation_throws)scope_disposal_fault();
  }
};
static ScopeDisposal *scope_disposal_operand(ScopeDisposal *value) noexcept {
  ++scope_disposal_operands;return value;
}
struct ScopeNeverDisposal {
  __attribute__((noreturn)) ~ScopeNeverDisposal() noexcept {abort();}
  static void operator delete(void *value) noexcept {free(value);}
};
static void scope_disposal_path(ScopeProbe *_Nonnull owner,int fail,int deallocation_throws) {
  int allocations=scope_disposal_allocations,frees=scope_disposal_frees,caught=0,operands=scope_disposal_operands;
  scope_disposal_count=0;scope_disposal_deallocation_throws=deallocation_throws;
  withLifetime(owner) {
    const char *view=&owner->payload;
    try {
      __objc3_defer {scope_use(view);scope_disposal_event(3);}
      delete scope_disposal_operand(new ScopeDisposal(fail));
    } catch(int value) {
      assert(value==(deallocation_throws?79:73));++caught;scope_use(view);scope_disposal_event(4);
    }
    scope_use(view);
  }
  assert(caught==(fail || deallocation_throws));
  assert(scope_disposal_allocations==allocations+1 && scope_disposal_frees==frees+1 && scope_disposal_operands==operands+1);
  assert(scope_disposal_count==(caught?4:3));
  for(int i=0;i!=scope_disposal_count;++i)assert(scope_disposal_trace[i]==i+1);
}
static void scope_disposal_suite(void) {
  ScopeProbe *_Nonnull owner=[ScopeProbe new];
#if !__has_feature(objc_arc)
  __objc3_defer {[owner release];}
#endif
  scope_disposal_path(owner,0,0);scope_disposal_path(owner,1,0);
  scope_disposal_count=0;
  const char *view=&owner->payload;
  {__objc3_defer {scope_use(view);scope_disposal_event(3);}
   delete static_cast<ScopeNeverDisposal *>(nullptr);scope_use(view);}
  assert(scope_disposal_count==1 && scope_disposal_trace[0]==3);
  // A second exception from deallocation during destructor unwind terminates.
  pid_t child=fork();assert(child>=0);
  if(child==0) {
    scope_disposal_expected_child_frees=scope_disposal_frees+1;
    std::set_terminate([]{
      _Exit(scope_disposal_frees==scope_disposal_expected_child_frees && scope_disposal_count==2 &&
            scope_disposal_trace[0]==1 && scope_disposal_trace[1]==2?83:82);
    });scope_disposal_count=0;
    scope_disposal_deallocation_throws=1;
    // Supply a primary handler so C++ performs phase-two unwinding.
    try {delete new ScopeDisposal(1);}catch(...) {_Exit(81);}
    _Exit(84);
  }
  int status=0;assert(waitpid(child,&status,0)==child);
  assert(WIFEXITED(status) && WEXITSTATUS(status)==83);
  scope_disposal_deallocation_throws=0;
}
#endif


#ifdef __cplusplus
static int allocation_trace[32],allocation_trace_count,allocation_created,allocation_freed;
static void allocation_event(int event) {assert(allocation_trace_count<32);allocation_trace[allocation_trace_count++]=event;}
struct AllocationMember {
  AllocationMember() noexcept {allocation_event(10);}
  ~AllocationMember() noexcept {allocation_event(11);}
};
struct ConstructorFailure {
  AllocationMember member;
  explicit ConstructorFailure(int fail) {allocation_event(1);if(fail)throw 73;}
  ~ConstructorFailure() noexcept {allocation_event(5);}
  static void *operator new(__SIZE_TYPE__ size) {void *value=malloc(size);assert(value);++allocation_created;return value;}
  static void operator delete(void *value) noexcept {allocation_event(2);++allocation_freed;free(value);}
};
static void allocation_failure_path(ScopeProbe *_Nonnull owner,int fail) {
  int created=allocation_created,freed=allocation_freed,caught=0;allocation_trace_count=0;
  withLifetime(owner) {
    const char *view=&owner->payload;
    try {__objc3_defer {scope_use(view);allocation_event(3);}delete new ConstructorFailure(fail);}
    catch(int value) {assert(value==73);++caught;scope_use(view);allocation_event(4);}
  }
  assert(caught==fail && allocation_created==created+1 && allocation_freed==freed+1);
  const int expected[2][6]={{10,1,5,11,2,3},{10,1,11,2,3,4}};
  assert(allocation_trace_count==6);
  for(int i=0;i!=6;++i)assert(allocation_trace[i]==expected[fail][i]);
}
static int array_next,array_failure_mask,array_created,array_freed,array_expected_child_frees;
struct ArrayFailure {
  int index;
  ArrayFailure() noexcept:index(++array_next) {}
  ~ArrayFailure() noexcept(false) {allocation_event(index);if(array_failure_mask&(1<<index))throw index;}
  static void *operator new[](__SIZE_TYPE__ size) {void *value=malloc(size);assert(value);++array_created;return value;}
  static void operator delete[](void *value) noexcept {allocation_event(100);++array_freed;free(value);}
};
static void array_failure_path(ScopeProbe *_Nonnull owner,int fail) {
  int created=array_created,freed=array_freed,caught=0;
  array_next=0;array_failure_mask=fail?(1<<2):0;allocation_trace_count=0;
  withLifetime(owner) {
    const char *view=&owner->payload;
    try {__objc3_defer {scope_use(view);allocation_event(103);}delete[] new ArrayFailure[3];}
    catch(int value) {assert(value==2);++caught;scope_use(view);allocation_event(104);}
  }
  assert(caught==fail && array_created==created+1 && array_freed==freed+1);
  const int expected[6]={3,2,1,100,103,104};assert(allocation_trace_count==(fail?6:5));
  for(int i=0;i!=allocation_trace_count;++i)assert(allocation_trace[i]==expected[i]);
}
static void allocation_array_suite(void) {
  ScopeProbe *_Nonnull owner=[ScopeProbe new];
#if !__has_feature(objc_arc)
  __objc3_defer {[owner release];}
#endif
  allocation_failure_path(owner,0);allocation_failure_path(owner,1);
  array_failure_path(owner,0);array_failure_path(owner,1);
  const char *view=&owner->payload;delete[] new ScopeNeverDisposal[0];scope_use(view);
  pid_t child=fork();assert(child>=0);
  if(child==0) {
    array_next=0;array_failure_mask=(1<<2)|(1<<1);allocation_trace_count=0;
    array_expected_child_frees=array_freed;
    std::set_terminate([]{
      _Exit(array_freed==array_expected_child_frees && allocation_trace_count==3 &&
            allocation_trace[0]==3 && allocation_trace[1]==2 && allocation_trace[2]==1?87:86);
    });
    try {delete[] new ArrayFailure[3];}catch(...) {_Exit(85);}_Exit(84);
  }
  int status=0;assert(waitpid(child,&status,0)==child);
  assert(WIFEXITED(status) && WEXITSTATUS(status)==87);array_failure_mask=0;
}
#endif

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
    scope_normal_only_pool_exception();scope_finally_suite();
#ifdef __cplusplus
    scope_disposal_suite();allocation_array_suite();
#endif
#if __has_feature(objc_arc)
    scope_partial_suite();
#endif
#if !__has_feature(objc_arc)
    scope_counted_pool();[scope_exception release];
#endif
    scope_exception=(id)0;
  }
  assert(scope_created-initial_created==scope_destroyed-initial_destroyed);
}
#pragma clang assume_nonnull end
#endif
