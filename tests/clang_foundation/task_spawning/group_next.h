#ifndef TASK_SPAWNING_GROUP_NEXT_H
#define TASK_SPAWNING_GROUP_NEXT_H

OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_int, int);
OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_unsigned, unsigned);
#if __has_feature(objc_arc)
OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_object, SpawnOwner *);
OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_block, JoinReader);
#endif
OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_pair, JoinPair);
OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_void, void);
typedef struct __attribute__((aligned(64))) NextWide { int value; } NextWide;
typedef Optional(int) NextMaybeInt;
OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_wide, NextWide);
OBJC_TASK_GROUP_NEXT_DECLARE(next_existing_optional, NextMaybeInt);
OBJC_TASK_GROUP_NEXT_DECLARE_THROWING(next_existing_checked, int, id<Error>);

static int group_int_source(int value) __objc3_async_frame { return value; }
#if __has_feature(objc_arc)
static JoinReader group_nil_block(void) __objc3_async_frame { return (JoinReader)0; }
#endif
static NextWide group_wide_source(void) __objc3_async_frame { return (NextWide){42}; }
static NextMaybeInt group_optional_source(int present) __objc3_async_frame {
  return present?Some(int,9):None(int);
}
static int group_wide_value(objc3_task_group_t *group) __objc3_async_frame {
  Optional(NextWide) item=__objc3_await next_existing_wide(group);
  if let value=item { assert(((__UINTPTR_TYPE__)&value%64)==0);return value.value; }
  return -1;
}
static int group_nested_values(objc3_task_group_t *group) __objc3_async_frame {
  int count=0,sum=0;
  for (;;) {
    Optional(NextMaybeInt) item=__objc3_await next_existing_optional(group);
    if let inner=item { ++count;if let value=inner { sum+=value; } } else { break; }
  }
  return count*100+sum;
}
static int group_sum(objc3_task_group_t *group) __objc3_async_frame {
  int sum=0;
  for (;;) {
    Optional(int) item=__objc3_await next_existing_int(group);
    if let value=item { sum+=value; } else { break; }
  }
  __objc3_await objc3_task_group_join_async(group);
  return sum;
}
static int group_wrong_type(objc3_task_group_t *group) __objc3_async_frame {
  Optional(unsigned) item=__objc3_await next_existing_unsigned(group);
  return hasValue(item);
}
#if __has_feature(objc_arc)
static JoinReader group_owner_block(SpawnOwner *owner) __objc3_async_frame {
  return ^int { return owner->number; };
}
static int group_direct_object(objc3_task_group_t *group) __objc3_async_frame {
  if let value=__objc3_await next_existing_object(group) { return value?value->number:-1; }
  return -2;
}
static int group_assign_object(objc3_task_group_t *group) __objc3_async_frame {
  Optional(SpawnOwner *) item=None(SpawnOwner *);
  item=__objc3_await next_existing_object(group);
  if let value=item { return value?value->number:-1; }
  return -2;
}
static int group_ignore_object(objc3_task_group_t *group) __objc3_async_frame {
  __objc3_await next_existing_object(group);
  __objc3_await objc3_task_group_join_async(group);
  return 42;
}
static int group_object_sum(objc3_task_group_t *group) __objc3_async_frame {
  int sum=0,count=0;
  for (;;) {
    Optional(SpawnOwner *) item=__objc3_await next_existing_object(group);
    if let value=item { ++count; if(value)sum+=value->number; } else { break; }
  }
  return count*100+sum;
}
static int group_block_sum(objc3_task_group_t *group) __objc3_async_frame {
  int sum=0,count=0;
  for (;;) {
    Optional(JoinReader) item=__objc3_await next_existing_block(group);
    if let value=item { ++count; if(value)sum+=value(); } else { break; }
  }
  return count*100+sum;
}
#endif
static int group_pair_sum(objc3_task_group_t *group) __objc3_async_frame {
  Optional(JoinPair) item=__objc3_await next_existing_pair(group);
  if let pair=item { return pair.first+(int)pair.second; }
  return -1;
}
static int group_void_count(objc3_task_group_t *group) __objc3_async_frame {
  int count=0;
  for (;;) {
    Optional(void) item=__objc3_await next_existing_void(group);
    if(!hasValue(item))break;
    ++count;
  }
  return count;
}
static int group_checked_error(objc3_task_group_t *group) __objc3_async_frame {
  __objc3_do {
    Optional(int) item=__objc3_try(__objc3_await next_existing_checked(group));
    return hasValue(item)?0:-1;
  } __objc3_catch(id<Error> error) { return error?79:0; }
}
static int group_actor_sum(objc3_task_group_t *group,uint64_t actor,uint64_t executor,
                           unsigned *entered)
    __objc3_async_frame {
  int sum=0;
  if(entered)__atomic_store_n(entered,1u,__ATOMIC_RELEASE);
  for (;;) {
    Optional(int) item=__objc3_await next_existing_int(group);
    objc3_current_actor_facts facts={0};facts.struct_size=sizeof(facts);
    facts.version=OBJC3_ACTOR_SERVICE_VERSION;
    assert(objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK);
    assert(facts.actor_identity==actor && facts.executor_identity==executor);
    if let value=item { sum+=value; } else { break; }
  }
  return sum;
}
static objc3_task_group_t *group_with(objc3_task_t *task) {
  objc3_task_group_t *group=NULL;
  assert(objc3_task_group_create(NULL,&group)==OBJC3_SERVICE_OK);
  if(task)assert(objc3_task_group_add(group,task)==OBJC3_SERVICE_OK);
  return group;
}
typedef struct GroupNextGate { unsigned ready; } GroupNextGate;
static void group_gate_destroy(void *context) { (void)context; }
static void group_gate_resume(objc3_task_t *task,void *context) {
  GroupNextGate *gate=context;
  if(!__atomic_load_n(&gate->ready,__ATOMIC_ACQUIRE))return;
  objc3_completion completion={0};completion.struct_size=sizeof(completion);
  completion.version=OBJC3_SERVICE_ABI_VERSION;completion.kind=OBJC3_COMPLETION_SUCCESS;
  assert(objc3_task_get_result_type(task,&completion.result_type)==OBJC3_SERVICE_OK);
  assert(objc3_task_complete(task,&completion)==OBJC3_SERVICE_OK);
}
static int group_gated_source(objc3_task_t *gate) __objc3_async_frame {
  __objc3_await join_existing_void(gate);return 42;
}
typedef struct GroupNextBarrier { objc3_executor_t *executor;unsigned passed; } GroupNextBarrier;
static void group_second_barrier(void *context) {
  GroupNextBarrier *barrier=context;
  __atomic_store_n(&barrier->passed,1u,__ATOMIC_RELEASE);
}
static void group_first_barrier(void *context) {
  GroupNextBarrier *barrier=context;
  assert(objc3_executor_enqueue(barrier->executor,group_second_barrier,barrier)==OBJC3_SERVICE_OK);
}
static void group_worker_witness(objc3_executor_t *manual) {
  objc3_task_t *model=join_start(manual,join_void_source());
  (void)join_completion(manual,model);
  const objc3_async_result_type *type=NULL;
  assert(objc3_task_get_result_type(model,&type)==OBJC3_SERVICE_OK);
  GroupNextGate gate={0};objc3_task_start_options options={0};
  options.struct_size=sizeof(options);options.version=OBJC3_SERVICE_ABI_VERSION;
  options.executor=manual;options.frame=&gate;options.resume=group_gate_resume;
  options.destroy=group_gate_destroy;options.result_type=type;
  objc3_task_t *dependency=NULL;assert(objc3_task_start(&options,&dependency)==OBJC3_SERVICE_OK);
  objc3_task_release(model);uint32_t executed=0;
  assert(objc3_executor_pump(manual,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
  objc3_executor_t *worker=NULL;assert(objc3_executor_create(0,&worker)==OBJC3_SERVICE_OK);
  objc3_task_t *source=join_start(worker,group_gated_source(dependency));
  objc3_task_group_t *group=group_with(source);
  objc3_actor_t *actor=NULL;assert(objc3_actor_create(worker,&actor)==OBJC3_SERVICE_OK);
  unsigned entered=0;
  objc3_async_frame frame=group_actor_sum(group,objc3_actor_identity(actor),
                                         objc3_executor_identity(worker),&entered);
  objc3_task_t *client=NULL;assert(objc3_async_frame_start(&frame,NULL,actor,&client)==OBJC3_SERVICE_OK);
  wait_for_destructions(&entered,1);
  // The first barrier follows the active consumer callback; the second follows
  // the adapter it queued. The source gate stays closed until reservation waits.
  GroupNextBarrier barrier={worker,0};
  assert(objc3_executor_enqueue(worker,group_first_barrier,&barrier)==OBJC3_SERVICE_OK);
  wait_for_destructions(&barrier.passed,1);
  __atomic_store_n(&gate.ready,1u,__ATOMIC_RELEASE);
  assert(objc3_task_schedule_resume(dependency)==OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(manual,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
  // Full callback/frame quiescence also completes both raw barrier handoffs
  // before their stack state is destroyed on the caller thread.
  objc3_task_group_t *fence=group_with(client);
  assert(objc3_task_group_close(fence)==OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(fence)==OBJC3_SERVICE_OK);
  objc3_completion completion={0};assert(objc3_task_get_completion(client,&completion)==OBJC3_SERVICE_OK);
  assert(completion.kind==OBJC3_COMPLETION_SUCCESS && *(const int *)completion.payload==42);
  assert(objc3_actor_close(actor)==OBJC3_SERVICE_OK && objc3_actor_join(actor)==OBJC3_SERVICE_OK);
  objc3_task_group_release(fence);objc3_task_group_release(group);
  objc3_task_release(source);objc3_task_release(client);objc3_task_release(dependency);
  objc3_actor_release(actor);objc3_executor_release(worker);
}
static void group_next_witnesses(objc3_executor_t *executor) {
  void *pool=join_pool_push();
  objc3_task_t *first=join_start(executor,group_int_source(17));
  objc3_task_t *second=join_start(executor,group_int_source(25));
  objc3_task_group_t *group=group_with(first);assert(objc3_task_group_add(group,second)==OBJC3_SERVICE_OK);
  join_check_failure(executor,join_start(executor,group_wrong_type(group)),OBJC3_SERVICE_TYPE_MISMATCH);
  join_check_value(executor,join_start(executor,group_sum(group)),42);
  objc3_task_release(first);objc3_task_release(second);objc3_task_group_release(group);
  group=group_with(NULL);join_check_value(executor,join_start(executor,group_sum(group)),0);
  objc3_task_group_release(group);
  join_check_failure(executor,join_start(executor,group_sum(NULL)),OBJC3_SERVICE_INVALID_ARGUMENT);

  unsigned destroyed=0;
  SpawnOwner *owner=[SpawnOwner new];owner->destroyed=&destroyed;owner->number=42;
#if __has_feature(objc_arc)
  first=join_start(executor,join_object_source(owner));
  second=join_start(executor,join_object_source((SpawnOwner *)0));
  group=group_with(first);assert(objc3_task_group_add(group,second)==OBJC3_SERVICE_OK);
  join_check_value(executor,join_start(executor,group_object_sum(group)),242);
  objc3_task_group_release(group);objc3_task_release(first);objc3_task_release(second);

  first=join_start(executor,join_object_source(owner));group=group_with(first);
  join_check_value(executor,join_start(executor,group_ignore_object(group)),42);
  objc3_task_group_release(group);objc3_task_release(first);
  first=join_start(executor,join_object_source(owner));group=group_with(first);
  join_check_value(executor,join_start(executor,group_direct_object(group)),42);
  objc3_task_group_release(group);objc3_task_release(first);
  first=join_start(executor,join_object_source(owner));group=group_with(first);
  join_check_value(executor,join_start(executor,group_assign_object(group)),42);
  objc3_task_group_release(group);objc3_task_release(first);
  first=join_start(executor,group_owner_block(owner));second=join_start(executor,group_nil_block());
  group=group_with(first);assert(objc3_task_group_add(group,second)==OBJC3_SERVICE_OK);
  join_check_value(executor,join_start(executor,group_block_sum(group)),242);
  objc3_task_group_release(group);objc3_task_release(first);objc3_task_release(second);

#endif
  first=join_start(executor,join_pair_source());group=group_with(first);
  join_check_value(executor,join_start(executor,group_pair_sum(group)),42);
  objc3_task_group_release(group);objc3_task_release(first);
  first=join_start(executor,group_wide_source());group=group_with(first);
  join_check_value(executor,join_start(executor,group_wide_value(group)),42);
  objc3_task_group_release(group);objc3_task_release(first);
  first=join_start(executor,group_optional_source(1));second=join_start(executor,group_optional_source(0));
  group=group_with(first);assert(objc3_task_group_add(group,second)==OBJC3_SERVICE_OK);
  join_check_value(executor,join_start(executor,group_nested_values(group)),209);
  objc3_task_group_release(group);objc3_task_release(first);objc3_task_release(second);
  first=join_start(executor,join_void_source());group=group_with(first);
  join_check_value(executor,join_start(executor,group_void_count(group)),1);
  objc3_task_group_release(group);objc3_task_release(first);
  first=join_start(executor,join_error_source(owner));group=group_with(first);
  join_check_value(executor,join_start(executor,group_checked_error(group)),79);
  objc3_task_group_release(group);objc3_task_release(first);
#if !__has_feature(objc_arc)
  release_source((__bridge void *)owner);
#else
  owner=(SpawnOwner *)0;
#endif
  join_pool_pop(pool);
  assert(destroyed==1);
  for(int nested=0;nested!=2;++nested) {
    group=group_with(NULL);objc3_task_group_t *membership=group;
    if(nested)assert(objc3_task_group_create(group,&membership)==OBJC3_SERVICE_OK);
    objc3_task_t *cycle=join_start(executor,group_sum(group));
    assert(objc3_task_group_add(membership,cycle)==OBJC3_SERVICE_OK);
    join_check_failure(executor,cycle,OBJC3_SERVICE_INVALID_ARGUMENT);
    if(nested)objc3_task_group_release(membership);
    objc3_task_group_release(group);
  }
  first=join_start(executor,group_int_source(42));group=group_with(first);
  objc3_actor_t *actor=NULL;assert(objc3_actor_create(executor,&actor)==OBJC3_SERVICE_OK);
  objc3_async_frame frame=group_actor_sum(group,objc3_actor_identity(actor),objc3_executor_identity(executor),NULL);
  objc3_task_t *client=NULL;assert(objc3_async_frame_start(&frame,NULL,actor,&client)==OBJC3_SERVICE_OK);
  join_check_value(executor,client,42);
  objc3_actor_release(actor);objc3_task_release(first);objc3_task_group_release(group);
  group_worker_witness(executor);
}
#endif
