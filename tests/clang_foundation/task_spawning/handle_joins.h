#ifndef TASK_SPAWNING_HANDLE_JOINS_H
#define TASK_SPAWNING_HANDLE_JOINS_H

OBJC_TASK_JOIN_DECLARE(join_existing_int, int);
OBJC_TASK_JOIN_DECLARE(join_existing_unsigned, unsigned);
OBJC_TASK_JOIN_DECLARE(join_existing_void, void);
OBJC_TASK_JOIN_DECLARE(join_existing_object, SpawnOwner *);
typedef int (^JoinReader)(void);
OBJC_TASK_JOIN_DECLARE(join_existing_block, JoinReader);
OBJC_TASK_JOIN_DECLARE_THROWING(join_existing_checked, int, id<Error>);
typedef struct JoinPair { int first; double second; } JoinPair;
OBJC_TASK_JOIN_DECLARE(join_existing_pair, JoinPair);

static int join_int_source(void) __objc3_async_frame { return 42; }
static void join_void_source(void) __objc3_async_frame {}
static JoinPair join_pair_source(void) __objc3_async_frame { return (JoinPair){17,25.0}; }
static SpawnOwner *join_object_source(SpawnOwner *object) __objc3_async_frame { return object; }
static JoinReader join_block_source(int value) __objc3_async_frame {
  JoinReader reader = ^int { return value; };
  return reader;
}
static int join_error_source(SpawnOwner *error) __objc3_async_frame __objc3_throws(id<Error>) {
  __objc3_throw((id<Error>)error);
}
static int join_int_client(objc3_task_t *task) __objc3_async_frame {
  return __objc3_await join_existing_int(task);
}
static int join_self_client(objc3_task_t **task) __objc3_async_frame {
  return __objc3_await join_existing_int(*task);
}
static unsigned join_mismatch_client(objc3_task_t *task) __objc3_async_frame {
  return __objc3_await join_existing_unsigned(task);
}
static int join_void_client(objc3_task_t *task) __objc3_async_frame {
  __objc3_await join_existing_void(task); return 42;
}
static int join_pair_client(objc3_task_t *task) __objc3_async_frame {
  JoinPair pair = __objc3_await join_existing_pair(task);
  return pair.first + (int)pair.second;
}
static int join_object_client(objc3_task_t *task) __objc3_async_frame {
  SpawnOwner *value = __objc3_await join_existing_object(task);
  return value->number;
}
static int join_block_client(objc3_task_t *task) __objc3_async_frame {
  JoinReader reader = __objc3_await join_existing_block(task);
  return reader();
}
static int join_checked_client(objc3_task_t *task) __objc3_async_frame {
  __objc3_do {
    return __objc3_try(__objc3_await join_existing_checked(task));
  } __objc3_catch(id<Error> error) { return error ? 79 : 0; }
}
static int join_group_client(objc3_task_group_t *group) __objc3_async_frame {
  __objc3_await objc3_task_group_join_async(group); return 42;
}
static objc3_task_t *join_start(objc3_executor_t *executor, objc3_async_frame frame) {
  objc3_task_t *task = NULL;
  assert(objc3_async_frame_start(&frame,executor,NULL,&task)==OBJC3_SERVICE_OK);
  return task;
}
static objc3_completion join_completion(objc3_executor_t *executor, objc3_task_t *task) {
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
  objc3_completion completion = {0};
  assert(objc3_task_get_completion(task,&completion)==OBJC3_SERVICE_OK);
  return completion;
}
static void join_check_value(objc3_executor_t *executor, objc3_task_t *task, int expected) {
  objc3_completion completion = join_completion(executor,task);
  assert(completion.kind==OBJC3_COMPLETION_SUCCESS && completion.has_payload);
  assert(*(const int *)completion.payload==expected);
  objc3_task_release(task);
}
static void join_check_failure(objc3_executor_t *executor, objc3_task_t *task,
                                objc3_service_status expected) {
  objc3_completion completion = join_completion(executor,task);
  assert(completion.kind==OBJC3_COMPLETION_ERROR);
  objc3_service_status actual = OBJC3_SERVICE_OK;
  assert(objc3_async_error_get_service(completion.error,&actual)==OBJC3_SERVICE_OK && actual==expected);
  objc3_task_release(task);
}
static void handle_join_witnesses(objc3_executor_t *executor) {
  objc3_task_t *integer = join_start(executor,join_int_source());
  join_check_value(executor,join_start(executor,join_int_client(integer)),42);
  join_check_failure(executor,join_start(executor,join_mismatch_client(integer)),OBJC3_SERVICE_TYPE_MISMATCH);
  join_check_failure(executor,join_start(executor,join_int_client(NULL)),OBJC3_SERVICE_INVALID_ARGUMENT);
  objc3_task_t *nothing = join_start(executor,join_void_source());
  join_check_value(executor,join_start(executor,join_void_client(nothing)),42);
  objc3_task_t *pair = join_start(executor,join_pair_source());
  join_check_value(executor,join_start(executor,join_pair_client(pair)),42);
  objc3_task_t *block = join_start(executor,join_block_source(42));
  join_check_value(executor,join_start(executor,join_block_client(block)),42);

  unsigned destroyed = 0;
  SpawnOwner *owner = [SpawnOwner new]; owner->destroyed=&destroyed; owner->number=42;
  objc3_task_t *object = join_start(executor,join_object_source(owner));
  join_check_value(executor,join_start(executor,join_object_client(object)),42);
  objc3_task_t *error = join_start(executor,join_error_source(owner));
  join_check_value(executor,join_start(executor,join_checked_client(error)),79);
  objc3_task_group_t *group = NULL;
  assert(objc3_task_group_create(NULL,&group)==OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group,integer)==OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group,block)==OBJC3_SERVICE_OK);
  join_check_value(executor,join_start(executor,join_group_client(group)),42);
  join_check_failure(executor,join_start(executor,join_group_client(NULL)),OBJC3_SERVICE_INVALID_ARGUMENT);
  objc3_task_group_release(group);
  objc3_task_t *self = NULL;
  self = join_start(executor,join_self_client(&self));
  join_check_failure(executor,self,OBJC3_SERVICE_INVALID_ARGUMENT);
  for (int nested=0; nested!=2; ++nested) {
    assert(objc3_task_group_create(NULL,&group)==OBJC3_SERVICE_OK);
    objc3_task_group_t *membership=group;
    if (nested) assert(objc3_task_group_create(group,&membership)==OBJC3_SERVICE_OK);
    objc3_task_t *cycle = join_start(executor,join_group_client(group));
    assert(objc3_task_group_add(membership,cycle)==OBJC3_SERVICE_OK);
    join_check_failure(executor,cycle,OBJC3_SERVICE_INVALID_ARGUMENT);
    if (nested) objc3_task_group_release(membership);
    objc3_task_group_release(group);
  }
  objc3_task_release(integer); objc3_task_release(nothing); objc3_task_release(pair);
  objc3_task_release(block); objc3_task_release(object); objc3_task_release(error);
#if !__has_feature(objc_arc)
  release_source((__bridge void *)owner);
#else
  owner = (SpawnOwner *)0;
#endif
  assert(destroyed==1);
}
#endif
