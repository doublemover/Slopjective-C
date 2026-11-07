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
static int group_child_frees,group_child_constructing;
static int group_next,group_fail_constructor,group_fail_destructors,group_allocated,group_freed;
struct ConstructionGroup {
  int index;
  ConstructionGroup():index(++group_next) {allocation_event(index);if(index==group_fail_constructor)throw index;}
  ~ConstructionGroup() noexcept(false) {allocation_event(20+index);if(group_fail_destructors&(1<<index))throw index;}
  static void *operator new[](__SIZE_TYPE__ size) {void *value=malloc(size);assert(value);++group_allocated;return value;}
  static void operator delete[](void *value) noexcept {allocation_event(90);++group_freed;free(value);}
};
static void construction_group_path(ScopeProbe *_Nonnull owner,int heap,int failure,int destruction) {
  int allocated=group_allocated,freed=group_freed,caught=0;
  group_next=0;group_fail_constructor=failure;group_fail_destructors=destruction?(1<<3):0;allocation_trace_count=0;
  withLifetime(owner) {
    const char *view=&owner->payload;
    try {
      __objc3_defer {scope_use(view);allocation_event(91);}
      if(heap)delete[] new ConstructionGroup[3];
      else {ConstructionGroup values[3];(void)values;}
    } catch(int value) {assert(value==(failure?failure:3));++caught;scope_use(view);allocation_event(92);}
  }
  assert(caught==!!(failure || destruction));
  assert(group_allocated==allocated+heap && group_freed==freed+heap);
  int position=0,completed=failure?failure-1:3;
  for(int i=1;i<=(failure?failure:3);++i)assert(allocation_trace[position++]==i);
  for(int i=completed;i;--i)assert(allocation_trace[position++]==20+i);
  if(heap)assert(allocation_trace[position++]==90);
  assert(allocation_trace[position++]==91);
  if(failure || destruction)assert(allocation_trace[position++]==92);
  assert(position==allocation_trace_count);
}
static void construction_group_suite(ScopeProbe *_Nonnull owner) {
  for(int heap=0;heap!=2;++heap) {
    for(int failure=0;failure!=4;++failure)construction_group_path(owner,heap,failure,0);
    construction_group_path(owner,heap,0,1);
  }
  for(int constructing=0;constructing!=2;++constructing) {
    pid_t child=fork();assert(child>=0);
    if(child==0) {
      group_next=0;group_fail_constructor=constructing?3:0;
      group_fail_destructors=(1<<2)|(constructing?0:1<<3);allocation_trace_count=0;
      group_child_frees=group_freed;group_child_constructing=constructing;
      std::set_terminate([]{int constructing=group_child_constructing;
        bool trace=allocation_trace_count==(constructing?4:5) && allocation_trace[0]==1 &&
            allocation_trace[1]==2 && allocation_trace[2]==3 && allocation_trace[3]==(constructing?22:23);
        if(!constructing)trace=trace && allocation_trace[4]==22;
        _Exit(trace && group_freed==group_child_frees?93:94);
      });
      try {if(constructing)delete[] new ConstructionGroup[3];else {ConstructionGroup values[3];(void)values;}}
      catch(...) {_Exit(95);}_Exit(96);
    }
    int status=0;assert(waitpid(child,&status,0)==child);
    assert(WIFEXITED(status) && WEXITSTATUS(status)==93);
  }
  group_fail_constructor=0;group_fail_destructors=0;
}
static int subobject_fail=-1,subobject_mask,subobject_return,subobject_dtor_bodies;
static int subobject_check_local,subobject_expected_destroyed;
static int subobject_stage(int stage) {
  allocation_event(10+stage);if(subobject_fail==stage)throw stage;return stage;
}
struct ScopeSubobjectPart {
  int tag;
  explicit ScopeSubobjectPart(int value) noexcept:tag(value) {}
  ~ScopeSubobjectPart() noexcept(false) {
    if(subobject_check_local)assert(scope_destroyed==subobject_expected_destroyed);
    allocation_event(20+tag);if(subobject_mask&(1<<tag))throw tag;
  }
};
struct ScopeSubobjectBase:ScopeSubobjectPart {
  explicit ScopeSubobjectBase(int tag) noexcept:ScopeSubobjectPart(tag) {}
  ~ScopeSubobjectBase() noexcept(false)=default;
};
template<bool Noexcept> struct ScopeSubobjectWitness:ScopeSubobjectBase {
  ScopeSubobjectPart first,second;
  explicit ScopeSubobjectWitness(ScopeProbe *_Nonnull owner) noexcept(Noexcept)
      :ScopeSubobjectBase(subobject_stage(0)),first(subobject_stage(1)),second(subobject_stage(2)) {
    const char *view=&owner->payload;
    __objc3_defer {scope_use(view);allocation_event(30);}
    if(subobject_fail==3)subobject_stage(3);
    scope_use(view);
  }
  ~ScopeSubobjectWitness() noexcept(Noexcept) {
    ++subobject_dtor_bodies;
    subobject_check_local=1;subobject_expected_destroyed=scope_destroyed+1;
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {
      scope_use(view);allocation_event(31);
#if !__has_feature(objc_arc)
      [local release];
#endif
    }
    if(subobject_fail==4)subobject_stage(4);
    if(subobject_fail==6)delete new ConstructorFailure(1);
    if(subobject_fail==7){auto *value=new ScopeDisposal(true);delete value;}
    try {__objc3_defer {scope_use(view);allocation_event(32);}if(subobject_return)return;}
    catch(...) {allocation_event(99);}
  }
};
static void subobject_path(ScopeProbe *_Nonnull owner,int fail,int mask,int returning) {
  int bodies=subobject_dtor_bodies,caught=0;subobject_fail=fail;subobject_mask=mask;
  subobject_return=returning;subobject_check_local=0;allocation_trace_count=0;
  withLifetime(owner) {
    const char *view=&owner->payload;
    try {__objc3_defer {scope_use(view);allocation_event(40);}ScopeSubobjectWitness<false> value(owner);(void)value;}
    catch(int value) {assert(value==(fail>=0?fail:2));++caught;scope_use(view);allocation_event(41);}
  }
  assert(caught==!!(fail>=0 || mask));
  int position=0,complete=fail>=0 && fail<3?fail:3;
  for(int i=0;i!=(fail>=0 && fail<3?fail+1:3);++i)assert(allocation_trace[position++]==10+i);
  if(fail==3)assert(allocation_trace[position++]==13);
  if(complete==3)assert(allocation_trace[position++]==30);
  if(fail<0 || fail==4) {
    assert(subobject_dtor_bodies==bodies+1);
    if(fail==4)assert(allocation_trace[position++]==14);
    else assert(allocation_trace[position++]==32);
    assert(allocation_trace[position++]==31);
  } else assert(subobject_dtor_bodies==bodies);
  for(int i=complete-1;i>=0;--i)assert(allocation_trace[position++]==20+i);
  assert(allocation_trace[position++]==40);
  if(caught)assert(allocation_trace[position++]==41);
  assert(position==allocation_trace_count);subobject_check_local=0;
}
static void subobject_suite(ScopeProbe *_Nonnull owner) {
  for(int fail=-1;fail!=5;++fail)subobject_path(owner,fail,0,0);
  subobject_path(owner,-1,0,1);subobject_path(owner,-1,1<<2,0);subobject_path(owner,-1,1<<2,1);
  for(int policy=0;policy!=8;++policy) {
    pid_t child=fork();assert(child>=0);
    if(child==0) {
      subobject_fail=policy==0 || policy==5?3:policy==3?4:policy==4?2:policy==6?6:policy==7?7:-1;
      subobject_mask=policy==2?(1<<2):policy>=3?0:((1<<2)|(1<<1));
      subobject_return=1;subobject_check_local=0;allocation_trace_count=0;scope_disposal_count=0;
      group_child_constructing=policy;
      std::set_terminate([]{
        const int policy=group_child_constructing;
        const int expected0[]={10,11,12,13,30,22};
        const int expected1[]={10,11,12,30,32,31,22,21};
        const int expected2[]={10,11,12,30,32,31,22};
        const int expected3[]={10,11,12,30,14};
        const int expected4[]={10,11,12};
        const int expected5[]={10,11,12,13};
        const int expected6[]={10,11,12,30,10,1,11};
        const int expected7[]={10,11,12,30};
        const int *expected=policy==0?expected0:policy==1?expected1:policy==2?expected2:policy==3?expected3:policy==4?expected4:policy==5?expected5:policy==6?expected6:expected7;
        int count=policy==0?6:policy==1?8:policy==2?7:policy==3?5:policy==4?3:policy==5?4:policy==6?7:4;
        bool good=allocation_trace_count==count;
        if(policy==7)good=good && scope_disposal_count==1 && scope_disposal_trace[0]==1;
        for(int i=0;i<count && good;++i)good=allocation_trace[i]==expected[i];
        if(!good) {
          fprintf(stderr,"subobject policy %d native trace:",policy);
          for(int i=0;i<allocation_trace_count;++i)fprintf(stderr," %d",allocation_trace[i]);
          fprintf(stderr," scope disposal:");
          for(int i=0;i<scope_disposal_count;++i)fprintf(stderr," %d",scope_disposal_trace[i]);
          fprintf(stderr,"\n");
        }
        _Exit(good?101:102);
      });
      try {
        if(policy>=2){ScopeSubobjectWitness<true> value(owner);(void)value;}
        else {ScopeSubobjectWitness<false> value(owner);(void)value;}
      } catch(...) {_Exit(103);}_Exit(104);
    }
    int status=0;assert(waitpid(child,&status,0)==child);
    assert(WIFEXITED(status) && WEXITSTATUS(status)==101);
  }
  subobject_fail=-1;subobject_mask=0;subobject_check_local=0;
}
static int delegation_bodies;
struct ScopeDelegatingWitness {
  ScopeSubobjectPart member;
  ScopeDelegatingWitness(ScopeProbe *_Nonnull owner,int):member(subobject_stage(0)) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(50);}
    if(subobject_fail==5)subobject_stage(5);
  }
  explicit ScopeDelegatingWitness(ScopeProbe *_Nonnull owner):ScopeDelegatingWitness(owner,0) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(51);}
    if(subobject_fail==6)subobject_stage(6);
  }
  ~ScopeDelegatingWitness() noexcept(false) {
    ++delegation_bodies;
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(52);
#if !__has_feature(objc_arc)
      [local release];
#endif
    }
  }
};
static void delegation_path(ScopeProbe *_Nonnull owner,int failure) {
  int bodies=delegation_bodies,caught=0;subobject_fail=failure;subobject_mask=0;allocation_trace_count=0;
  withLifetime(owner) {
    const char *view=&owner->payload;
    try {__objc3_defer {scope_use(view);allocation_event(60);}ScopeDelegatingWitness value(owner);(void)value;}
    catch(int value){assert(value==failure);++caught;scope_use(view);allocation_event(61);}
  }
  const int success[]={10,50,51,52,20,60};
  const int initial[]={10,60,61};
  const int target[]={10,15,50,20,60,61};
  const int outer[]={10,50,16,51,52,20,60,61};
  const int *expected=failure<0?success:failure==0?initial:failure==5?target:outer;
  int count=failure<0?6:failure==0?3:failure==5?6:8;
  assert(caught==(failure>=0) && allocation_trace_count==count);
  for(int i=0;i<count;++i)assert(allocation_trace[i]==expected[i]);
  assert(delegation_bodies==bodies+(failure<0 || failure==6));
}
template<bool Noexcept,bool Suppress> struct ScopeFunctionTryWitness:ScopeSubobjectBase {
  ScopeSubobjectPart first,second;
  explicit ScopeFunctionTryWitness(ScopeProbe *_Nonnull owner) noexcept(Noexcept) try
      :ScopeSubobjectBase(subobject_stage(0)),first(subobject_stage(1)),second(subobject_stage(2)) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(70);}
    if(subobject_fail==3)subobject_stage(3);
  } catch(int value) {assert(value==subobject_fail);scope_use(&owner->payload);allocation_event(71);}
  ~ScopeFunctionTryWitness() noexcept(Noexcept) try {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(72);
#if !__has_feature(objc_arc)
      [local release];
#endif
    }
    if(subobject_fail==4)subobject_stage(4);
    if(subobject_return)return;
  } catch(int value) {
    assert(value==(subobject_fail==4?4:2));
    ScopeProbe *_Nonnull handler=[ScopeProbe new];const char *view=&handler->payload;
    __objc3_defer {scope_use(view);allocation_event(74);
#if !__has_feature(objc_arc)
      [handler release];
#endif
    }
    allocation_event(73);scope_use(view);
    if constexpr(Suppress)return;
  }
};
template<bool Noexcept,bool Suppress> static void function_try_path(ScopeProbe *_Nonnull owner,int failure,int mask,int returning) {
  subobject_fail=failure;subobject_mask=mask;subobject_return=returning;subobject_check_local=0;allocation_trace_count=0;
  int caught=0;
  withLifetime(owner) {
    const char *view=&owner->payload;
    try {__objc3_defer {scope_use(view);allocation_event(80);}ScopeFunctionTryWitness<Noexcept,Suppress> value(owner);(void)value;}
    catch(int value){assert(value==(failure>=0?failure:2));++caught;scope_use(view);allocation_event(81);}
  }
  int position=0,complete=failure>=0 && failure<3?failure:3;
  for(int i=0;i!=(failure>=0 && failure<3?failure+1:3);++i)assert(allocation_trace[position++]==10+i);
  if(failure==3)assert(allocation_trace[position++]==13);
  if(complete==3)assert(allocation_trace[position++]==70);
  bool constructor_failed=failure>=0 && failure<=3;
  if(!constructor_failed) {
    if(failure==4)assert(allocation_trace[position++]==14);
    assert(allocation_trace[position++]==72);
  }
  for(int i=complete-1;i>=0;--i)assert(allocation_trace[position++]==20+i);
  if(constructor_failed)assert(allocation_trace[position++]==71);
  else if(failure==4 || mask) {assert(allocation_trace[position++]==73);assert(allocation_trace[position++]==74);}
  assert(allocation_trace[position++]==80);
  bool expected_catch=constructor_failed || (!Suppress && (failure==4 || mask));
  assert(caught==expected_catch);
  if(expected_catch)assert(allocation_trace[position++]==81);
  assert(position==allocation_trace_count);
}
static void delegation_function_try_suite(ScopeProbe *_Nonnull owner) {
  const int failures[]={-1,0,5,6};
  for(int failure:failures)delegation_path(owner,failure);
  for(int failure=-1;failure!=5;++failure)function_try_path<false,false>(owner,failure,0,0);
  function_try_path<false,false>(owner,-1,1<<2,0);function_try_path<false,false>(owner,-1,1<<2,1);
  function_try_path<false,true>(owner,4,0,0);function_try_path<false,true>(owner,-1,1<<2,1);
  function_try_path<true,true>(owner,4,0,0);function_try_path<true,true>(owner,-1,1<<2,1);
  for(int policy=0;policy!=4;++policy) {
    pid_t child=fork();assert(child>=0);
    if(child==0) {
      allocation_trace_count=0;subobject_check_local=0;subobject_return=1;
      subobject_fail=policy==0?6:policy==1?1:policy==3?3:-1;
      subobject_mask=policy==0?1:policy==2?1<<2:policy==3?(1<<2)|(1<<1):0;
      group_child_constructing=policy;
      std::set_terminate([]{
        int policy=group_child_constructing;
        const int e0[]={10,50,16,51,52,20};
        const int e1[]={10,11,20,71};
        const int e2[]={10,11,12,70,72,22,21,20,73,74};
        const int e3[]={10,11,12,13,70,22};
        const int *expected=policy==0?e0:policy==1?e1:policy==2?e2:e3;
        int count=policy==0?6:policy==1?4:policy==2?10:6;
        bool good=allocation_trace_count==count;
        for(int i=0;i<count && good;++i)good=allocation_trace[i]==expected[i];
        if(!good){fprintf(stderr,"function-try policy %d:",policy);for(int i=0;i<allocation_trace_count;++i)fprintf(stderr," %d",allocation_trace[i]);fprintf(stderr,"\n");}
        _Exit(good?111:112);
      });
      try {
        if(policy==0){ScopeDelegatingWitness value(owner);(void)value;}
        else if(policy==1 || policy==2){ScopeFunctionTryWitness<true,false> value(owner);(void)value;}
        else {ScopeFunctionTryWitness<false,false> value(owner);(void)value;}
      } catch(...) {_Exit(113);}_Exit(114);
    }
    int status=0;assert(waitpid(child,&status,0)==child);
    assert(WIFEXITED(status) && WEXITSTATUS(status)==111);
  }
  subobject_fail=-1;subobject_mask=0;subobject_check_local=0;
}

static int virtual_failure=-1,virtual_mask,virtual_returning;
static int virtual_stage(int part) {
  allocation_event(100+part);if(virtual_failure==part)throw part;return part;
}
struct ScopeVirtualPart {
  int tag;
  explicit ScopeVirtualPart(int value) noexcept:tag(value) {}
  ~ScopeVirtualPart() noexcept(false) {allocation_event(120+tag);if(virtual_mask&(1<<tag))throw tag;}
};
struct ScopeVirtualRoot:ScopeVirtualPart {
  explicit ScopeVirtualRoot(int value) noexcept:ScopeVirtualPart(value) {}
  ~ScopeVirtualRoot() noexcept(false)=default;
};
struct ScopeVirtualOwner:virtual ScopeVirtualRoot {
  ScopeVirtualPart member;
  explicit ScopeVirtualOwner(ScopeProbe *_Nonnull owner) try
      :ScopeVirtualRoot(virtual_stage(0)),member(virtual_stage(1)) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(130);}
    if(virtual_returning)goto done;
    if(virtual_failure==2)virtual_stage(2);
  done:scope_use(view);
  } catch(int value) {assert(value==virtual_failure);allocation_event(140);scope_use(&owner->payload);}
  ~ScopeVirtualOwner() noexcept(false) try {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(131);
#if !__has_feature(objc_arc)
      [local release];
#endif
    }
    if(virtual_returning)return;
    if(virtual_failure==4)virtual_stage(4);
  } catch(int value) {assert(value==(virtual_failure==4?4:1));allocation_event(141);}
};
struct ScopeVirtualMost:ScopeVirtualOwner {
  explicit ScopeVirtualMost(ScopeProbe *_Nonnull owner)
      :ScopeVirtualRoot(virtual_stage(3)),ScopeVirtualOwner(owner) {scope_use(&owner->payload);}
  ~ScopeVirtualMost() noexcept(false) {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(132);
#if !__has_feature(objc_arc)
      [local release];
#endif
    }
  }
};
struct ScopeVirtualRight:virtual ScopeVirtualRoot {
  ScopeVirtualRight():ScopeVirtualRoot(virtual_stage(6)) {}
  ~ScopeVirtualRight() noexcept(false) {allocation_event(136);}
};
struct ScopeVirtualDiamond:ScopeVirtualOwner,ScopeVirtualRight {
  explicit ScopeVirtualDiamond(ScopeProbe *_Nonnull owner)
      :ScopeVirtualRoot(virtual_stage(3)),ScopeVirtualOwner(owner),ScopeVirtualRight() {}
  ~ScopeVirtualDiamond() noexcept(false) {allocation_event(137);}
};
struct ScopeVirtualDelegating:virtual ScopeVirtualRoot {
  ScopeVirtualPart member;
  ScopeVirtualDelegating(ScopeProbe *_Nonnull owner,int)
      :ScopeVirtualRoot(virtual_stage(0)),member(virtual_stage(1)) {scope_use(&owner->payload);}
  explicit ScopeVirtualDelegating(ScopeProbe *_Nonnull owner):ScopeVirtualDelegating(owner,0) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(133);}
    virtual_stage(5);
  }
  ~ScopeVirtualDelegating() noexcept(false) {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(134);
#if !__has_feature(objc_arc)
      [local release];
#endif
    }
  }
};
struct ScopeVirtualDelegatedMost:ScopeVirtualDelegating {
  explicit ScopeVirtualDelegatedMost(ScopeProbe *_Nonnull owner)
      :ScopeVirtualRoot(virtual_stage(3)),ScopeVirtualDelegating(owner) {}
  ~ScopeVirtualDelegatedMost() noexcept(false)=default;
};
struct ScopeAnonymousOwner {
  union {ScopeVirtualPart active;int fallback;};
  explicit ScopeAnonymousOwner(ScopeProbe *_Nonnull owner):active(virtual_stage(1)) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(150);}
    if(virtual_failure==2)virtual_stage(2);
  }
  ~ScopeAnonymousOwner() noexcept(false) {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(151);
#if !__has_feature(objc_arc)
      [local release];
#endif
    }
    active.~ScopeVirtualPart();
  }
};
static void virtual_expect(const int *expected,int count,int caught,int exception) {
  assert(caught==exception);
  if(allocation_trace_count!=count){fprintf(stderr,"virtual length %d != %d:",allocation_trace_count,count);for(int i=0;i<allocation_trace_count;++i)fprintf(stderr," %d",allocation_trace[i]);fprintf(stderr,"\n");}
  assert(allocation_trace_count==count);
  for(int i=0;i<count;++i)assert(allocation_trace[i]==expected[i]);
}
static void virtual_object_suite(ScopeProbe *_Nonnull owner) {
  const bool variants[]={false,true};
  for(int kind=0;kind!=3;++kind)for(int returning=0;returning!=2;++returning) {
    virtual_failure=-1;virtual_mask=0;virtual_returning=returning;allocation_trace_count=0;
    if(kind==0){ScopeVirtualOwner value(owner);(void)value;}
    else if(kind==1){ScopeVirtualMost value(owner);(void)value;}
    else {ScopeVirtualDiamond value(owner);(void)value;}
    const int complete[]={100,101,130,131,121,120};
    const int base[]={103,101,130,132,131,121,123};
    const int diamond[]={103,101,130,137,136,131,121,123};
    virtual_expect(kind==0?complete:kind==1?base:diamond,kind==0?6:kind==1?7:8,0,0);
  }
  for(bool base:variants)for(int failure=0;failure!=5;++failure) {
    if(failure==3 || (base && failure==0))continue;
    virtual_failure=failure;virtual_mask=0;virtual_returning=0;allocation_trace_count=0;int caught=0;
    withLifetime(owner) {
      const char *view=&owner->payload;
      try {
        __objc3_defer {scope_use(view);allocation_event(160);}
        if(base){ScopeVirtualMost value(owner);(void)value;}else {ScopeVirtualOwner value(owner);(void)value;}
      } catch(int value){assert(value==failure);scope_use(view);allocation_event(161);++caught;}
    }
    int expected[20],count=0;expected[count++]=base?103:100;
    if(failure!=0)expected[count++]=101;
    if(failure==2)expected[count++]=102;
    if(failure>=2)expected[count++]=130;
    if(failure==4) {
      if(base)expected[count++]=132;
      expected[count++]=104;expected[count++]=131;
    }
    if(failure>=2)expected[count++]=121;
    if(!base && failure!=0)expected[count++]=120;
    expected[count++]=failure==4?141:140;
    if(base)expected[count++]=123;
    expected[count++]=160;expected[count++]=161;
    virtual_expect(expected,count,caught,1);
  }
  for(bool base:variants) {
    virtual_failure=-1;virtual_mask=1<<1;virtual_returning=1;allocation_trace_count=0;int caught=0;
    try {if(base){ScopeVirtualMost value(owner);(void)value;}else {ScopeVirtualOwner value(owner);(void)value;}}
    catch(int value){assert(value==1);++caught;allocation_event(161);}
    const int complete[]={100,101,130,131,121,120,141,161};
    const int most[]={103,101,130,132,131,121,141,123,161};
    virtual_expect(base?most:complete,base?9:8,caught,1);
  }
  for(bool base:variants) {
    virtual_failure=5;virtual_mask=0;virtual_returning=0;allocation_trace_count=0;int caught=0;
    try {if(base){ScopeVirtualDelegatedMost value(owner);(void)value;}else {ScopeVirtualDelegating value(owner);(void)value;}}
    catch(int value){assert(value==5);++caught;}
    const int complete[]={100,101,105,133,134,121,120};
    const int most[]={103,101,105,133,134,121,123};
    virtual_expect(base?most:complete,7,caught,1);
  }
  const int anonymous_failures[]={-1,1,2};
  for(int failure:anonymous_failures) {
    virtual_failure=failure;virtual_mask=0;allocation_trace_count=0;int caught=0;
    try {ScopeAnonymousOwner value(owner);(void)value;}catch(int value){assert(value==failure);++caught;}
    const int normal[]={101,150,121,151};
    const int initial[]={101};const int body[]={101,102,150,121};
    virtual_expect(failure<0?normal:failure==1?initial:body,failure==1?1:4,caught,failure>=0);
  }
  virtual_failure=-1;virtual_mask=0;virtual_returning=0;
}

static void allocation_array_suite(void) {
  ScopeProbe *_Nonnull owner=[ScopeProbe new];
#if !__has_feature(objc_arc)
  __objc3_defer {[owner release];}
#endif
  allocation_failure_path(owner,0);allocation_failure_path(owner,1);
  array_failure_path(owner,0);array_failure_path(owner,1);construction_group_suite(owner);subobject_suite(owner);delegation_function_try_suite(owner);virtual_object_suite(owner);
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

#if __has_feature(objc_arc)
__attribute__((noinline)) static void scope_ordinary_raise(int mode) {
#ifdef __cplusplus
  if(mode)throw 97;
#endif
  @throw scope_exception;
}
static void *_Nullable (*_Nullable scope_real_block_copy)(const void *_Nullable);
static int scope_fail_block_copy;
static int scope_mutation_observe,scope_mutation_events;
static int scope_producer_observe,scope_producer_copies;
#ifdef __cplusplus
extern "C"
#endif
__attribute__((noinline)) void *_Nullable _Block_copy(const void *_Nullable source) {
  if(scope_mutation_observe)scope_mutation_events=scope_mutation_events*10+2;
  if(scope_producer_observe)++scope_producer_copies;
  if(scope_fail_block_copy && --scope_fail_block_copy==0){@throw scope_exception;}
  if(!scope_real_block_copy)scope_real_block_copy=(void *(*)(const void *))dlsym(RTLD_NEXT,"_Block_copy");
  assert(scope_real_block_copy);return scope_real_block_copy(source);
}
static void scope_ordinary_block_unwind(int fail_copy) {
  int before=scope_destroyed;
  @try {
    ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
    const char *view=&local->payload;
    scope_fail_block_copy=fail_copy;
    void (^work)(void)=^{(void)local;scope_use(view);};
    work();scope_ordinary_raise(0);
  } @catch(...) {assert(scope_destroyed==before+1 && scope_fail_block_copy==0);}
  assert(scope_destroyed==before+1);
}

#ifdef __cplusplus
typedef void (^ScopeMemberCallback)(void);
static int scope_member_body_failure,scope_member_destroying_failure;
__attribute__((noinline,nothrow)) static int scope_arm_member_copy(int fault) {
  scope_fail_block_copy=fault==1?1:fault==4?2:0;return 0;
}
struct ScopeBlockMemberOwner {
  int armed;
  ScopeMemberCallback first,second;
  __weak ScopeMemberCallback weak;
  ScopeBlockMemberOwner(ScopeProbe *_Nonnull owner,ScopeMemberCallback source,int fault)
      :armed(scope_arm_member_copy(fault)),first(source),second(source),weak(source) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(170);}
    if(scope_member_body_failure)scope_ordinary_raise(0);
  }
  ~ScopeBlockMemberOwner() noexcept(false) {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(171);}
    if(scope_member_destroying_failure)scope_ordinary_raise(0);
  }
};

__attribute__((noinline,nothrow)) static int scope_arm_array_copy(int copy) {scope_fail_block_copy=copy;return 0;}
__attribute__((noinline,nothrow)) static void scope_check_member_array(ScopeMemberCallback _Nullable const *_Nonnull values __attribute__((noescape)),int occupied) {
  for(int index=0;index!=3;++index)assert((values[index]!=(id)0)==(index<occupied));
}
struct ScopeBlockArrayMemberOwner {
  int armed;
  ScopeMemberCallback _Nullable callbacks[3];
  __weak ScopeMemberCallback _Nullable weak[3];
  ScopeBlockArrayMemberOwner(ScopeProbe *_Nonnull owner,ScopeMemberCallback source,int copy)
      :armed(scope_arm_array_copy(copy)),callbacks{source,source,source},weak{source,source,source} {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(180);}
    scope_check_member_array(callbacks,3);
    if(scope_member_body_failure)scope_ordinary_raise(0);
  }
  ~ScopeBlockArrayMemberOwner() noexcept(false) {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(181);}
    if(scope_member_destroying_failure)scope_ordinary_raise(0);
  }
};
struct ScopeBlockArraySparseOwner {
  ScopeMemberCallback _Nullable callbacks[3];
  ScopeBlockArraySparseOwner(ScopeProbe *_Nonnull owner,ScopeMemberCallback source):callbacks{source} {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);}
    scope_check_member_array(callbacks,1);
  }
  ~ScopeBlockArraySparseOwner() {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);}
  }
};
struct ScopeBlockArrayNullOwner {
  ScopeMemberCallback _Nullable callbacks[3]{};
  explicit ScopeBlockArrayNullOwner(ScopeProbe *_Nonnull owner) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);}
    scope_check_member_array(callbacks,0);
  }
  ~ScopeBlockArrayNullOwner() {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);}
  }
};

static int scope_computed_step_fault,scope_computed_step_mask,scope_computed_callbacks;
__attribute__((noinline)) static int scope_computed_step(int step) {
  scope_computed_step_mask|=1<<step;
  if(scope_computed_step_fault==step)scope_ordinary_raise(0);
  return 0;
}
struct ScopeComputedBlockArrayOwner {
  int armed;
  ScopeMemberCallback _Nullable callbacks[2][2];
  ScopeComputedBlockArrayOwner(ScopeProbe *_Nonnull owner,ScopeMemberCallback source,int flag,int scalar,int copy)
      :armed(scope_arm_array_copy(copy)),
       callbacks{{(scope_computed_step(1),source),flag?source:^{scope_computed_callbacks+=scalar;}},
                 {^{scope_computed_callbacks+=scalar;},(scope_computed_step(4),source)}} {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(190);}
    for(int row=0;row!=2;++row)for(int col=0;col!=2;++col){assert(callbacks[row][col]);callbacks[row][col]();}
    if(scope_member_body_failure)scope_ordinary_raise(0);
  }
  ~ScopeComputedBlockArrayOwner() noexcept(false) {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(191);}
    if(scope_member_destroying_failure)scope_ordinary_raise(0);
  }
};
struct ScopeNestedSparseBlockArrayOwner {
  ScopeMemberCallback _Nullable callbacks[2][2];
  ScopeNestedSparseBlockArrayOwner(ScopeProbe *_Nonnull owner,ScopeMemberCallback source):callbacks{{source},{}} {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);}
    assert(callbacks[0][0]);assert(!callbacks[0][1] && !callbacks[1][0] && !callbacks[1][1]);callbacks[0][0]();
  }
};

static int scope_computed_consumed_calls;
__attribute__((noinline,nothrow)) static void scope_computed_consume(ScopeMemberCallback value __attribute__((ns_consumed))) {
  ++scope_computed_consumed_calls;value();
}
static void scope_computed_block_local_unwind(void) {
  for(int flag=0;flag!=2;++flag)for(int fault=0;fault!=3;++fault) {
    int before=scope_destroyed,caught=0;scope_computed_consumed_calls=0;
    {
      ScopeProbe *_Nonnull owner __attribute__((objc_precise_lifetime))=[ScopeProbe new];
      ScopeMemberCallback source=^{scope_use(&owner->payload);};
      @try {
        ScopeNestedSparseBlockArrayOwner storage(owner,source);
        const char *view=&owner->payload;__objc3_defer {scope_use(view);}
        scope_fail_block_copy=fault;
        ScopeMemberCallback local=(scope_computed_step(1),flag?storage.callbacks[0][0]:source);
        scope_computed_consume(flag?local:storage.callbacks[0][0]);local();
      } @catch(id exception) {assert(exception==scope_exception);++caught;}
      assert(caught==(fault!=0) && scope_fail_block_copy==0);
      assert(scope_computed_consumed_calls==(fault==0));assert(scope_destroyed==before);source();
    }
    assert(scope_destroyed==before+1);
  }
}


static int scope_mutation_fault,scope_mutation_deferred;
__attribute__((noinline)) static int scope_mutation_rhs(void) {
  scope_mutation_events=scope_mutation_events*10+1;
  if(scope_mutation_fault==1)scope_ordinary_raise(0);
  return 0;
}
__attribute__((noinline)) static int scope_mutation_index(void) {
  scope_mutation_events=scope_mutation_events*10+3;
  if(scope_mutation_fault==3)scope_ordinary_raise(0);
  return 0;
}
struct ScopeBlockMutationSlots {
  ScopeMemberCallback _Nullable callbacks[2];
  ScopeBlockMutationSlots():callbacks{} {}
};
static void scope_block_mutation_unwind(void) {
  for(int member=0;member!=2;++member)for(int fault=0;fault!=(member?4:3);++fault) {
    int before=scope_destroyed,caught=0;scope_mutation_fault=fault;scope_mutation_events=0;
    {
      ScopeBlockMutationSlots storage;
      ScopeMemberCallback _Nullable local=(id)0,replacement=(id)0;
      {
        ScopeProbe *_Nonnull oldOwner=[ScopeProbe new],* _Nonnull newOwner=[ScopeProbe new];
        ScopeMemberCallback old=^{scope_use(&oldOwner->payload);};
        ScopeMemberCallback fresh=^{scope_use(&newOwner->payload);};
        if(member)storage.callbacks[0]=old;else local=old;
        replacement=fresh;
      }
      // The literal stack captures have ended; only copied closures hold owners.
      assert(scope_destroyed==before);scope_mutation_deferred=0;
      scope_fail_block_copy=fault==2?1:0;scope_mutation_observe=1;
      @try {
        __objc3_defer {++scope_mutation_deferred;}
        if(member)storage.callbacks[scope_mutation_index()]=(scope_mutation_rhs(),replacement);
        else local=(scope_mutation_rhs(),replacement);
      } @catch(id exception) {assert(exception==scope_exception);++caught;}
      scope_mutation_observe=0;
      assert(caught==(fault!=0) && scope_fail_block_copy==0 && scope_mutation_deferred==1);
      assert(scope_mutation_events==(fault==1?1:fault==2?12:member?123:12));
      assert(scope_destroyed==before+(fault==0));
      if(member)storage.callbacks[0]();else local();
      replacement=(id)0;
      // On failure the old owner survives in the unchanged slot. On success
      // only the new owner survives. A leaked address-evaluation copy breaks
      // this count before the target is cleared.
      assert(scope_destroyed==before+1);
      if(member)storage.callbacks[0]=(ScopeMemberCallback)0;else local=(ScopeMemberCallback)0;
      assert(scope_destroyed==before+2);
    }
    assert(scope_destroyed==before+2);
  }
  scope_mutation_fault=0;scope_mutation_observe=0;
}


static int scope_producer_calls,scope_producer_call_fault,scope_producer_deferred;
__attribute__((noinline,ns_returns_retained)) static ScopeMemberCallback _Nonnull scope_block_producer_owned(int tag) {
  ++scope_producer_calls;
  if(scope_producer_call_fault==scope_producer_calls)scope_ordinary_raise(0);
  ScopeProbe *_Nonnull owner __attribute__((objc_precise_lifetime))=[ScopeProbe new];
  return ^{assert(tag>=0);scope_use(&owner->payload);};
}
__attribute__((noinline)) static ScopeMemberCallback _Nonnull scope_block_producer_shared(int tag) {
  return scope_block_producer_owned(tag);
}
__attribute__((noinline,nothrow)) static int scope_arm_block_producers(int copy,int call) {
  scope_fail_block_copy=copy;scope_producer_call_fault=call;scope_producer_calls=0;scope_producer_copies=0;scope_producer_observe=1;return 0;
}
struct ScopeProducedBlockArrayOwner {
  int armed;
  ScopeMemberCallback _Nullable callbacks[2][2];
  ScopeProducedBlockArrayOwner(ScopeProbe *_Nonnull owner,int copy,int call)
      :armed(scope_arm_block_producers(copy,call)),
       callbacks{{scope_block_producer_owned(1),scope_block_producer_owned(2)},
                 {scope_block_producer_owned(3),scope_block_producer_owned(4)}} {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);allocation_event(210);}
    for(int row=0;row!=2;++row)for(int col=0;col!=2;++col)callbacks[row][col]();
    if(scope_member_body_failure)scope_ordinary_raise(0);
  }
  ~ScopeProducedBlockArrayOwner() noexcept(false) {
    ScopeProbe *_Nonnull local=[ScopeProbe new];const char *view=&local->payload;
    __objc3_defer {scope_use(view);allocation_event(211);}
    if(scope_member_destroying_failure)scope_ordinary_raise(0);
  }
};
struct ScopeProducedBlockDefaultOwner {
  ScopeMemberCallback _Nullable scalar=scope_block_producer_owned(0);
  ScopeMemberCallback _Nullable callbacks[2][2]={{scope_block_producer_owned(1),scope_block_producer_owned(2)},
                                               {scope_block_producer_owned(3),scope_block_producer_owned(4)}};
  ScopeProducedBlockDefaultOwner(ScopeProbe *_Nonnull owner) {
    const char *view=&owner->payload;__objc3_defer {scope_use(view);}scalar();callbacks[1][1]();
  }
};
static void scope_block_producer_subobjects(void) {
  for(int fault=0;fault!=11;++fault) {
    int before=scope_destroyed,caught=0;allocation_trace_count=0;
    int copy=fault>=1 && fault<=4?fault:0,call=fault>=5 && fault<=8?fault-4:0;
    scope_member_body_failure=fault==9;scope_member_destroying_failure=fault==10;
    {
      ScopeProbe *_Nonnull owner __attribute__((objc_precise_lifetime))=[ScopeProbe new];
      @try {ScopeProducedBlockArrayOwner value(owner,copy,call);(void)value;}
      @catch(id exception){assert(exception==scope_exception);++caught;}
      scope_producer_observe=0;
      assert(caught==(fault!=0) && scope_fail_block_copy==0);
      int produced=copy?copy:call?call-1:4;
      assert(scope_producer_calls==(copy?copy:call?call:4));
      assert(scope_producer_copies==produced);
      assert(scope_destroyed==before+produced+(fault==0 || fault==10));
      assert(allocation_trace_count==(copy || call?0:fault==9?1:2));
    }
    assert(scope_destroyed==before+1+(copy?copy:call?call-1:4)+(fault==0 || fault==10));
  }
  scope_member_body_failure=0;scope_member_destroying_failure=0;
  for(int fault=0;fault!=6;++fault) {
    int before=scope_destroyed,caught=0;scope_arm_block_producers(0,fault);
    {
      ScopeProbe *_Nonnull owner __attribute__((objc_precise_lifetime))=[ScopeProbe new];
      @try {ScopeProducedBlockDefaultOwner value(owner);(void)value;}
      @catch(id exception){assert(exception==scope_exception);++caught;}
      scope_producer_observe=0;int produced=fault?fault-1:5;
      assert(caught==(fault!=0));assert(scope_producer_copies==produced);
      assert(scope_destroyed==before+produced);
    }
    assert(scope_destroyed==before+1+(fault?fault-1:5));
  }
  scope_producer_call_fault=0;
}
#define SCOPE_BLOCK_PRODUCER_PATHS(FACTORY,NAME) \
static void NAME(void) { \
  for(int scenario=0;scenario!=8;++scenario) { \
    int before=scope_destroyed,caught=0; \
    int call=scenario>=1 && scenario<=3?scenario:0,copy=scenario>=4 && scenario<=6?scenario-3:0; \
    scope_mutation_fault=scenario==7?3:0;scope_mutation_events=0;scope_producer_deferred=0;scope_computed_consumed_calls=0; \
    @autoreleasepool { \
      ScopeProbe *_Nonnull owner __attribute__((objc_precise_lifetime))=[ScopeProbe new];const char *view=&owner->payload; \
      scope_arm_block_producers(copy,call); \
      @try { \
        __objc3_defer {scope_use(view);++scope_producer_deferred;} \
        ScopeMemberCallback first=FACTORY(1); \
        scope_computed_consume(FACTORY(2)); \
        ScopeBlockMutationSlots storage;storage.callbacks[scope_mutation_index()]=FACTORY(3); \
        first();storage.callbacks[0](); \
      } @catch(id exception){assert(exception==scope_exception);++caught;} \
      scope_producer_observe=0; \
      int produced=call?call-1:copy?copy:3; \
      assert(caught==(scenario!=0) && scope_fail_block_copy==0 && scope_producer_deferred==1); \
      assert(scope_producer_calls==(call?call:copy?copy:3) && scope_producer_copies==produced); \
      assert(scope_computed_consumed_calls==(scope_producer_calls==3)); \
    } \
    assert(scope_destroyed==before+1+(call?call-1:copy?copy:3)); \
  } \
  scope_mutation_fault=0;scope_producer_call_fault=0; \
}
SCOPE_BLOCK_PRODUCER_PATHS(scope_block_producer_owned,scope_block_producer_owned_paths)
SCOPE_BLOCK_PRODUCER_PATHS(scope_block_producer_shared,scope_block_producer_shared_paths)
#undef SCOPE_BLOCK_PRODUCER_PATHS

static void scope_computed_block_array_unwind(void) {
  // Copy failures cover every leaf, including a partial second row. Scalar
  // evaluation faults occur before leaf 1 and leaf 4, independently of copying.
  for(int flag=0;flag!=2;++flag)for(int fault=0;fault!=9;++fault) {
    int before=scope_destroyed,caught=0;allocation_trace_count=0;scope_computed_step_mask=0;scope_computed_callbacks=0;
    scope_computed_step_fault=fault==5?1:fault==6?4:0;
    scope_member_body_failure=fault==7;scope_member_destroying_failure=fault==8;
    {
      ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
      ScopeMemberCallback source=^{scope_use(&local->payload);};
      @try {ScopeComputedBlockArrayOwner value(local,source,flag,3,fault>=1 && fault<=4?fault:0);(void)value;}
      @catch(id exception){assert(exception==scope_exception);++caught;}
      assert(caught==(fault!=0) && scope_fail_block_copy==0);
      assert(scope_destroyed==before+(fault==0 || fault==8));source();
    }
    assert(scope_destroyed==before+1+(fault==0 || fault==8));
    const int reached_body=fault==0 || fault>=7;
    assert(scope_computed_callbacks==(reached_body?(flag?3:6):0));
    assert(scope_computed_step_mask==((fault>=1 && fault<=3) || fault==5?2:18));
    assert(allocation_trace_count==(reached_body?(fault==7?1:2):0));
    if(reached_body)assert(allocation_trace[0]==190);
    if(fault==0 || fault==8)assert(allocation_trace[1]==191);
  }
  scope_computed_step_fault=0;scope_member_body_failure=0;scope_member_destroying_failure=0;
  int before=scope_destroyed;
  {
    ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
    ScopeMemberCallback source=^{scope_use(&local->payload);};
    {ScopeNestedSparseBlockArrayOwner value(local,source);(void)value;}
    assert(scope_destroyed==before);source();
  }
  assert(scope_destroyed==before+1);
}

static void scope_block_array_member_unwind(void) {
  for(int fault=0;fault!=6;++fault) {
    int before=scope_destroyed,caught=0;allocation_trace_count=0;
    scope_member_body_failure=fault==4;scope_member_destroying_failure=fault==5;
    {
      ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
      ScopeMemberCallback source=^{scope_use(&local->payload);};
      @try {ScopeBlockArrayMemberOwner value(local,source,fault>=1 && fault<=3?fault:0);(void)value;}
      @catch(id exception){assert(exception==scope_exception);++caught;}
      assert(caught==(fault!=0) && scope_fail_block_copy==0);
      assert(scope_destroyed==before+(fault==0 || fault==5));source();
    }
    assert(scope_destroyed==before+1+(fault==0 || fault==5));
    assert(allocation_trace_count==(fault>=1 && fault<=3?0:fault==4?1:2));
    if(fault==0 || fault>=4)assert(allocation_trace[0]==180);
    if(fault==0 || fault==5)assert(allocation_trace[1]==181);
  }
  scope_member_body_failure=0;scope_member_destroying_failure=0;
  int before=scope_destroyed;
  {
    ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
    ScopeMemberCallback source=^{scope_use(&local->payload);};
    {ScopeBlockArraySparseOwner value(local,source);(void)value;}
    {ScopeBlockArrayNullOwner value(local);(void)value;}
    assert(scope_destroyed==before+2);source();
  }
  assert(scope_destroyed==before+3);
}

static void scope_block_member_unwind(void) {
  for(int fault=0;fault!=5;++fault) {
    int before=scope_destroyed,caught=0;allocation_trace_count=0;
    scope_member_body_failure=fault==2;scope_member_destroying_failure=fault==3;
    {
      ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
      ScopeMemberCallback source=^{scope_use(&local->payload);};
      @try {ScopeBlockMemberOwner value(local,source,fault);(void)value;}
      @catch(id exception){assert(exception==scope_exception);++caught;}
      assert(caught==(fault!=0) && scope_fail_block_copy==0);
      assert(scope_destroyed==before+(fault==0 || fault==3));
      source();
    }
    assert(scope_destroyed==before+1+(fault==0 || fault==3));
    assert(allocation_trace_count==(fault==1 || fault==4?0:fault==2?1:2));
    if(fault!=1 && fault!=4)assert(allocation_trace[0]==170);
    if(fault==0 || fault==3)assert(allocation_trace[1]==171);
  }
  scope_member_body_failure=0;scope_member_destroying_failure=0;
}
#endif

static void scope_ordinary_record_block(void) {
  int before=scope_destroyed;
  {
    ScopeProbe *_Nullable local=[ScopeProbe new];
    struct CapturedRecord {ScopeProbe *_Nullable owner;};
    struct CapturedRecord record={local};
    void (^first)(void)=^{scope_use(&record.owner->payload);};
    void (^second)(void)=first;
    local=(id)0;record.owner=(id)0;
    second();assert(scope_destroyed==before);
  }
  assert(scope_destroyed==before+1);
}
static void scope_ordinary_owner_unwind(void) {
#ifdef __cplusplus
  scope_block_member_unwind();scope_block_array_member_unwind();scope_computed_block_array_unwind();scope_computed_block_local_unwind();scope_block_mutation_unwind();scope_block_producer_subobjects();scope_block_producer_owned_paths();scope_block_producer_shared_paths();
#endif
  int before=scope_destroyed;
  @try {
    ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
    scope_use(&local->payload);assert(scope_destroyed==before);
    scope_ordinary_raise(0);
  } @catch(...) {assert(scope_destroyed==before+1);}
  assert(scope_destroyed==before+1);
#ifdef __cplusplus
  try {
    ScopeProbe *_Nonnull local __attribute__((objc_precise_lifetime))=[ScopeProbe new];
    scope_use(&local->payload);assert(scope_destroyed==before+1);
    scope_ordinary_raise(1);
  } catch(int value) {assert(value==97 && scope_destroyed==before+2);}
  assert(scope_destroyed==before+2);
#endif
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
    scope_partial_suite();scope_ordinary_owner_unwind();
    scope_ordinary_block_unwind(0);scope_ordinary_block_unwind(1);scope_ordinary_record_block();
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
