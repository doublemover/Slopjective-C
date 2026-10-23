#include "objc3_runtime_services.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct Trace {
  int destroyed;
  int completed;
  int failed;
  int cancelled;
} Trace;
typedef struct Frame {
  Trace *trace;
  int fail;
} Frame;
static objc3_service_status copy_int(void *destination, const void *source) {
  memcpy(destination, source, sizeof(int));
  return OBJC3_SERVICE_OK;
}
static void destroy_int(void *value) { (void)value; }
static objc3_service_status move_int(void *destination, void *source) {
  return copy_int(destination, source);
}
#ifdef __cplusplus
#define INT_ALIGNMENT alignof(int)
#define ZERO_INIT                                                              \
  {                                                                            \
  }
#else
#define INT_ALIGNMENT _Alignof(int)
#define ZERO_INIT {0}
#endif
static const objc3_value_ops int_ops = {sizeof(objc3_value_ops),
                                        OBJC3_VALUE_OPS_VERSION,
                                        sizeof(int),
                                        INT_ALIGNMENT,
                                        copy_int,
                                        move_int,
                                        destroy_int};
static void completed(void *context, const objc3_completion *completion) {
  Trace *trace = (Trace *)context;
  ++trace->completed;
  trace->failed += completion->kind == OBJC3_COMPLETION_ERROR;
  trace->cancelled += completion->kind == OBJC3_COMPLETION_CANCELLED;
}
static void inspect_failed(void *context, const objc3_completion *completion) {
  int *observed = (int *)context;
  assert(completion->kind == OBJC3_COMPLETION_ERROR && completion->error);
  assert(*(const int *)objc3_error_payload(completion->error) == 42);
  ++*observed;
}
static void destroy_frame(void *context) {
  Frame *frame = (Frame *)context;
  ++frame->trace->destroyed;
  free(frame);
}
static void resume(objc3_task_t *task, void *context) {
  Frame *frame = (Frame *)context;
  objc3_completion completion = ZERO_INIT;
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  objc3_error_t *error = NULL;
  if (objc3_task_is_cancel_requested(task)) {
    completion.kind = OBJC3_COMPLETION_CANCELLED;
  } else if (frame->fail) {
    int payload = 42;
    assert(objc3_error_create(&payload, &int_ops, &error) == OBJC3_SERVICE_OK);
    completion.kind = OBJC3_COMPLETION_ERROR;
    completion.error = error;
  } else {
    completion.kind = OBJC3_COMPLETION_SUCCESS;
  }
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
  objc3_error_release(error);
}
static objc3_task_t *start(objc3_executor_t *executor, Trace *trace, int fail) {
  Frame *frame = (Frame *)malloc(sizeof(Frame));
  assert(frame);
  frame->trace = trace;
  frame->fail = fail;
  objc3_task_start_options options = ZERO_INIT;
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = frame;
  options.resume = resume;
  options.destroy = destroy_frame;
  options.completion = completed;
  options.completion_context = trace;
  objc3_task_t *task = NULL;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  return task;
}

static void groups(void) {
  Trace trace = ZERO_INIT;
  objc3_executor_t *executor = NULL;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  objc3_task_group_t *parent = NULL, *child = NULL;
  assert(objc3_task_group_create(NULL, &parent) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_create(parent, &child) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_retain(parent) == parent);
  objc3_task_group_release(parent);
  objc3_task_t *failed = start(executor, &trace, 1);
  objc3_task_t *sibling = start(executor, &trace, 0);
  objc3_task_t *ancestor_sibling = start(executor, &trace, 0);
  assert(objc3_task_group_add(child, failed) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(child, failed) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_task_group_add(child, sibling) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(parent, ancestor_sibling) == OBJC3_SERVICE_OK);
  objc3_task_t *next = failed;
  assert(objc3_task_group_next(NULL, &next) == OBJC3_SERVICE_INVALID_ARGUMENT &&
         !next);
  assert(objc3_task_group_next(child, &next) == OBJC3_SERVICE_NOT_READY &&
         !next);
  assert(objc3_task_group_join(parent) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_task_group_close(parent) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(parent, failed) == OBJC3_SERVICE_CLOSED);
  assert(objc3_task_group_join(parent) == OBJC3_SERVICE_NOT_READY);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK &&
         executed == 1);
  assert(objc3_task_is_cancel_requested(sibling));
  assert(objc3_task_is_cancel_requested(ancestor_sibling));
  assert(objc3_task_group_next(child, &next) == OBJC3_SERVICE_OK &&
         next == failed);
  int observed = 0;
  assert(objc3_task_add_waiter(next, inspect_failed, &observed) ==
         OBJC3_SERVICE_OK);
  assert(observed == 1);
  objc3_task_release(next);
  assert(objc3_task_group_next(child, &next) == OBJC3_SERVICE_NOT_READY &&
         !next);
  assert(objc3_executor_pump(executor, 8, &executed) == OBJC3_SERVICE_OK &&
         executed == 2);
  assert(objc3_task_group_join(parent) == OBJC3_SERVICE_OK);
  assert(trace.completed == 3 && trace.destroyed == 3 && trace.failed == 1 &&
         trace.cancelled == 2);
  assert(objc3_task_group_next(child, &next) == OBJC3_SERVICE_OK &&
         next == sibling);
  objc3_task_release(next);
  assert(objc3_task_group_next(child, &next) == OBJC3_SERVICE_CLOSED && !next);
  assert(objc3_task_group_next(parent, &next) == OBJC3_SERVICE_OK &&
         next == ancestor_sibling);
  objc3_task_release(next);
  assert(objc3_task_group_next(parent, &next) == OBJC3_SERVICE_CLOSED && !next);
  assert(objc3_task_group_cancel(parent) == OBJC3_SERVICE_OK);
  objc3_task_release(failed);
  objc3_task_release(sibling);
  objc3_task_release(ancestor_sibling);
  objc3_task_group_release(child);
  objc3_task_group_release(parent);
  objc3_executor_release(executor);
}

typedef struct ActorTrace {
  objc3_actor_t *first;
  objc3_actor_t *second;
  int ran;
  int destroyed;
  int sequence[3];
} ActorTrace;
typedef struct Job {
  ActorTrace *trace;
  objc3_actor_t *actor;
  int value;
} Job;
static void actor_job(void *context);
static void destroy_job(void *context) {
  Job *job = (Job *)context;
  objc3_current_actor_facts current = ZERO_INIT;
  current.struct_size = sizeof(current);
  current.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&current) == OBJC3_SERVICE_OK);
  assert(current.actor_identity == objc3_actor_identity(job->actor));
  assert(objc3_actor_join(job->actor) == OBJC3_SERVICE_NOT_READY);
  ++job->trace->destroyed;
  free(job);
}
static Job *job_for(ActorTrace *trace, objc3_actor_t *actor, int value) {
  Job *job = (Job *)malloc(sizeof(Job));
  assert(job);
  job->trace = trace;
  job->actor = actor;
  job->value = value;
  return job;
}
static void actor_job(void *context) {
  Job *job = (Job *)context;
  ActorTrace *trace = job->trace;
  objc3_current_actor_facts current = ZERO_INIT;
  current.struct_size = sizeof(current);
  current.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&current) == OBJC3_SERVICE_OK);
  assert(current.actor_identity == objc3_actor_identity(job->actor));
  assert(current.executor_identity ==
         objc3_actor_executor_identity(job->actor));
  assert(trace->ran < 3);
  trace->sequence[trace->ran++] = job->value;
  if (job->value == 1) {
    objc3_actor_hop_facts facts = ZERO_INIT;
    facts.struct_size = sizeof(facts);
    facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    assert(objc3_actor_enqueue(trace->second, actor_job,
                               job_for(trace, trace->second, 2), destroy_job,
                               &facts) == OBJC3_SERVICE_OK);
    assert(facts.actor_hop == 1 && facts.executor_hop == 0);
    assert(facts.current_actor_identity == current.actor_identity);
    assert(objc3_actor_enqueue(trace->first, actor_job,
                               job_for(trace, trace->first, 3), destroy_job,
                               &facts) == OBJC3_SERVICE_OK);
    assert(facts.actor_hop == 0 && facts.executor_hop == 0);
  }
}
static void actors(void) {
  objc3_executor_t *executor = NULL;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  ActorTrace trace = ZERO_INIT;
  assert(objc3_actor_create(executor, &trace.first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(executor, &trace.second) == OBJC3_SERVICE_OK);
  assert(objc3_actor_identity(trace.first) !=
         objc3_actor_identity(trace.second));
  assert(objc3_actor_executor_identity(trace.first) ==
         objc3_actor_executor_identity(trace.second));
  assert(objc3_actor_retain(trace.first) == trace.first);
  objc3_actor_release(trace.first);
  objc3_actor_hop_facts facts = ZERO_INIT;
  facts.struct_size = sizeof(facts);
  facts.version = 99;
  Job *job = job_for(&trace, trace.first, 1);
  objc3_actor_hop_facts before = facts;
  assert(objc3_actor_enqueue(trace.first, actor_job, job, destroy_job,
                             &facts) == OBJC3_SERVICE_UNSUPPORTED_VERSION);
  assert(memcmp(&facts, &before, sizeof(facts)) == 0 && trace.destroyed == 0);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_enqueue(trace.first, actor_job, job, destroy_job,
                             &facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_hop == 1 && facts.executor_hop == 1);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 8, &executed) == OBJC3_SERVICE_OK);
  assert(trace.ran == 3 && trace.destroyed == 3);
  assert(trace.sequence[0] == 1 && trace.sequence[1] == 2 &&
         trace.sequence[2] == 3);
  assert(objc3_actor_close(trace.first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(trace.second) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(trace.first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(trace.second) == OBJC3_SERVICE_OK);
  objc3_actor_t *cancelled = NULL;
  assert(objc3_actor_create(executor, &cancelled) == OBJC3_SERVICE_OK);
  assert(objc3_actor_enqueue(cancelled, actor_job,
                             job_for(&trace, cancelled, 9), destroy_job,
                             &facts) == OBJC3_SERVICE_OK);
  assert(objc3_actor_cancel(cancelled) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(cancelled) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_executor_pump(executor, 8, &executed) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(cancelled) == OBJC3_SERVICE_OK &&
         trace.destroyed == 4 && trace.ran == 3);
  objc3_actor_release(cancelled);
  objc3_actor_release(trace.second);
  objc3_actor_release(trace.first);
  objc3_executor_release(executor);
  objc3_current_actor_facts current = ZERO_INIT;
  current.struct_size = sizeof(current);
  current.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&current) == OBJC3_SERVICE_OK &&
         current.actor_identity == 0 && current.executor_identity == 0);
}
typedef struct HopFrame {
  objc3_actor_t *first;
  objc3_actor_t *second;
  objc3_executor_t *other;
  objc3_task_t *child;
  unsigned step;
  int destroyed;
  objc3_task_group_t *group;
  objc3_task_t *task;
} HopFrame;
static void pending_wait_from_other_executor(void *opaque) {
  HopFrame *frame = (HopFrame *)opaque;
  // The pending task can hop here after the check. Waiting would prevent its
  // future continuation from running, even though its current actor is elsewhere.
  assert(objc3_task_wait(frame->task) == OBJC3_SERVICE_NOT_READY);
  assert(objc3_task_group_join(frame->group) == OBJC3_SERVICE_NOT_READY);
}
static objc3_current_actor_facts current_destination(void) {
  objc3_current_actor_facts facts = ZERO_INIT;
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  return facts;
}
static void child_resume(objc3_task_t *task, void *opaque) {
  HopFrame *frame = (HopFrame *)opaque;
  assert(current_destination().actor_identity ==
         objc3_actor_identity(frame->second));
  objc3_completion completion = ZERO_INIT;
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
static void hop_resume(objc3_task_t *task, void *opaque) {
  HopFrame *frame = (HopFrame *)opaque;
  objc3_current_actor_facts current = current_destination();
  assert(objc3_task_hop(task, NULL, NULL) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_task_hop(task, frame->other, frame->first) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  switch (frame->step++) {
  case 0:
    assert(current.actor_identity == objc3_actor_identity(frame->first));
    assert(objc3_task_hop(task, NULL, frame->second) == OBJC3_SERVICE_OK);
    break;
  case 1:
    assert(current.actor_identity == objc3_actor_identity(frame->second));
    assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
    break;
  case 2: {
    assert(current.actor_identity == objc3_actor_identity(frame->second));
    objc3_task_start_options options = ZERO_INIT;
    options.struct_size = sizeof(options);
    options.version = OBJC3_SERVICE_ABI_VERSION;
    options.resume = child_resume;
    options.frame = frame;
    options.destroy = destroy_int;
    options.executor = frame->other;
    assert(objc3_task_start_child(task, &options, &frame->child) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    options.executor = NULL;
    options.actor = frame->second;
    assert(objc3_task_start_child_on(task, &options, &frame->child) ==
           OBJC3_SERVICE_OK);
    assert(objc3_task_await(task, frame->child) == OBJC3_SERVICE_OK);
    assert(objc3_task_hop(task, frame->other, NULL) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    break;
  }
  case 3:
    assert(current.actor_identity == objc3_actor_identity(frame->second));
    objc3_task_release(frame->child);
    frame->child = NULL;
    assert(objc3_task_hop(task, frame->other, NULL) == OBJC3_SERVICE_OK);
    break;
  case 4:
    assert(current.actor_identity == 0 && current.executor_identity != 0);
    assert(current.executor_identity !=
           objc3_actor_executor_identity(frame->first));
    assert(objc3_task_hop(task, NULL, frame->first) == OBJC3_SERVICE_OK);
    break;
  case 5: {
    assert(current.actor_identity == objc3_actor_identity(frame->first));
    objc3_completion completion = ZERO_INIT;
    completion.struct_size = sizeof(completion);
    completion.version = OBJC3_SERVICE_ABI_VERSION;
    assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
    break;
  }
  default:
    assert(0);
  }
}
static void hop_destroy(void *opaque) {
  ++((HopFrame *)opaque)->destroyed;
}
static void task_hops(void) {
  HopFrame frame = ZERO_INIT;
  objc3_executor_t *executor = NULL;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &frame.other) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(executor, &frame.first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(executor, &frame.second) == OBJC3_SERVICE_OK);
  objc3_task_start_options options = ZERO_INIT;
  options.struct_size = sizeof(options);
  options.version = 2; // The previous start layout is an unsupported ABI.
  options.actor = frame.first;
  options.resume = hop_resume;
  options.destroy = hop_destroy;
  options.frame = &frame;
  objc3_task_t *task = NULL;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_UNSUPPORTED_VERSION);
  assert(!task && !frame.destroyed);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(!task && !frame.destroyed);
  options.executor = NULL;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  frame.task = task;
  assert(objc3_task_group_create(NULL, &frame.group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(frame.group, task) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(frame.group) == OBJC3_SERVICE_OK);
  assert(objc3_executor_enqueue(frame.other, pending_wait_from_other_executor,
                                 &frame) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(frame.other, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && frame.step == 0);
  assert(objc3_task_hop(task, executor, NULL) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_executor_pump(executor, 100, &executed) == OBJC3_SERVICE_OK);
  assert(frame.step == 4 && !frame.destroyed);
  assert(objc3_executor_pump(frame.other, 100, &executed) == OBJC3_SERVICE_OK);
  assert(frame.step == 5 && !frame.destroyed);
  assert(objc3_executor_pump(executor, 100, &executed) == OBJC3_SERVICE_OK);
  assert(frame.step == 6 && frame.destroyed == 1);
  assert(objc3_task_group_join(frame.group) == OBJC3_SERVICE_OK);
  objc3_task_group_release(frame.group);
  objc3_task_release(task);
  objc3_actor_release(frame.second);
  objc3_actor_release(frame.first);
  objc3_executor_release(frame.other);
  objc3_executor_release(executor);
}

/* Native C frames exercise the same child entry used by compiler-generated
 * async frames. The independent dependency keeps each live task suspended, so
 * cancellation must publish a wake rather than merely set a query bit. */
typedef struct CancellationTree CancellationTree;
typedef struct CancellationFrame {
  CancellationTree *tree;
  unsigned index;
  unsigned step;
} CancellationFrame;
struct CancellationTree {
  objc3_executor_t *executor;
  objc3_task_t *dependency;
  objc3_task_t *tasks[5];
  unsigned destroyed[5];
  unsigned resumes[5];
  int complete_middle;
};
static void tree_resume(objc3_task_t *task, void *opaque);
static void tree_destroy(void *opaque) {
  CancellationFrame *frame = (CancellationFrame *)opaque;
  ++frame->tree->destroyed[frame->index];
  free(frame);
}
static objc3_task_t *tree_start(CancellationTree *tree, unsigned index,
                                objc3_task_t *parent) {
  CancellationFrame *frame = (CancellationFrame *)calloc(1, sizeof(*frame));
  assert(frame);
  frame->tree = tree;
  frame->index = index;
  objc3_task_start_options options = ZERO_INIT;
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = parent ? NULL : tree->executor;
  options.resume = tree_resume;
  options.destroy = tree_destroy;
  options.frame = frame;
  objc3_task_t *task = NULL;
  assert((parent ? objc3_task_start_child(parent, &options, &task)
                 : objc3_task_start(&options, &task)) == OBJC3_SERVICE_OK);
  tree->tasks[index] = task;
  return task;
}
static void tree_complete(objc3_task_t *task, objc3_completion_kind kind) {
  objc3_completion completion = ZERO_INIT;
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = kind;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
static void tree_resume(objc3_task_t *task, void *opaque) {
  CancellationFrame *frame = (CancellationFrame *)opaque;
  CancellationTree *tree = frame->tree;
  ++tree->resumes[frame->index];
  if (frame->step++ == 0) {
    if (frame->index == 0) {
      tree_start(tree, 1, task);
      tree_start(tree, 2, task);
      tree_start(tree, 4, NULL); /* detached from the active parent */
    } else if (frame->index == 1) {
      tree_start(tree, 3, task);
      if (tree->complete_middle) {
        tree_complete(task, OBJC3_COMPLETION_SUCCESS);
        return;
      }
    }
    if (!objc3_task_is_cancel_requested(task)) {
      assert(objc3_task_await(task, tree->dependency) == OBJC3_SERVICE_OK);
      return;
    }
  }
  tree_complete(task, objc3_task_is_cancel_requested(task)
                          ? OBJC3_COMPLETION_CANCELLED
                          : OBJC3_COMPLETION_SUCCESS);
}
static void remain_suspended(objc3_task_t *task, void *frame) {
  (void)task;
  (void)frame;
}
static void tree_pump(CancellationTree *tree) {
  uint32_t executed = 0;
  assert(objc3_executor_pump(tree->executor, 64, &executed) == OBJC3_SERVICE_OK);
  assert(executed < 64);
}
static void tree_assert_kind(objc3_task_t *task, objc3_completion_kind kind) {
  objc3_completion completion = ZERO_INIT;
  assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == kind);
}
static void child_cancellation(int cancel_before_spawn, int cancel_child_only,
                                int release_completed_middle,
                                int abandon_parent) {
  CancellationTree tree = ZERO_INIT;
  tree.complete_middle = release_completed_middle;
  assert(objc3_executor_create(1, &tree.executor) == OBJC3_SERVICE_OK);
  objc3_task_start_options dependency_options = ZERO_INIT;
  dependency_options.struct_size = sizeof(dependency_options);
  dependency_options.version = OBJC3_SERVICE_ABI_VERSION;
  dependency_options.executor = tree.executor;
  dependency_options.resume = remain_suspended;
  assert(objc3_task_start(&dependency_options, &tree.dependency) ==
         OBJC3_SERVICE_OK);
  tree_start(&tree, 0, NULL);
  if (cancel_before_spawn) objc3_task_request_cancel(tree.tasks[0]);
  tree_pump(&tree);
  if (release_completed_middle) {
    tree_assert_kind(tree.tasks[1], OBJC3_COMPLETION_SUCCESS);
    assert(tree.destroyed[1] == 1);
    objc3_task_release(tree.tasks[1]);
    tree.tasks[1] = NULL;
  }
  if (abandon_parent) {
    objc3_task_release(tree.tasks[0]);
    tree.tasks[0] = NULL;
    assert(tree.destroyed[0] == 1);
  } else if (!cancel_before_spawn)
    objc3_task_request_cancel(tree.tasks[cancel_child_only ? 1 : 0]);
  /* Cancellation is visible throughout the tree before any queued continuation
   * runs. Child cancellation never travels upward or sideways. */
  if (tree.tasks[0])
    assert(objc3_task_is_cancel_requested(tree.tasks[0]) == !cancel_child_only);
  assert(objc3_task_is_cancel_requested(tree.tasks[2]) == !cancel_child_only);
  assert(objc3_task_is_cancel_requested(tree.tasks[3]));
  assert(!objc3_task_is_cancel_requested(tree.tasks[4]));
  assert(!objc3_task_is_cancel_requested(tree.dependency));
  tree_pump(&tree);
  tree_assert_kind(tree.tasks[3], OBJC3_COMPLETION_CANCELLED);
  if (!cancel_child_only && tree.tasks[0])
    tree_assert_kind(tree.tasks[0], OBJC3_COMPLETION_CANCELLED);
  objc3_task_request_cancel(tree.tasks[0]); /* idempotent and cancels siblings */
  tree_pump(&tree);
  tree_complete(tree.dependency, OBJC3_COMPLETION_SUCCESS);
  tree_pump(&tree);
  tree_assert_kind(tree.tasks[4], OBJC3_COMPLETION_SUCCESS);
  for (unsigned index = 0; index != 5; ++index) {
    assert(tree.destroyed[index] == 1);
    if (tree.tasks[index]) objc3_task_release(tree.tasks[index]);
  }
  objc3_task_release(tree.dependency);
  objc3_executor_release(tree.executor);
}

typedef struct DeepCancellation {
  objc3_executor_t *executor;
  objc3_task_t *dependency;
  objc3_task_t **tasks;
  unsigned count;
  unsigned destroyed;
} DeepCancellation;
typedef struct DeepCancellationFrame {
  DeepCancellation *state;
  unsigned index;
  unsigned step;
} DeepCancellationFrame;
static void deep_cancellation_resume(objc3_task_t *task, void *opaque);
static void deep_cancellation_destroy(void *opaque) {
  DeepCancellationFrame *frame = (DeepCancellationFrame *)opaque;
  ++frame->state->destroyed;
  free(frame);
}
static void deep_cancellation_start(DeepCancellation *state, unsigned index,
                                     objc3_task_t *parent) {
  DeepCancellationFrame *frame =
      (DeepCancellationFrame *)calloc(1, sizeof(*frame));
  assert(frame);
  frame->state = state;
  frame->index = index;
  objc3_task_start_options options = ZERO_INIT;
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = parent ? NULL : state->executor;
  options.frame = frame;
  options.resume = deep_cancellation_resume;
  options.destroy = deep_cancellation_destroy;
  assert((parent ? objc3_task_start_child(parent, &options, &state->tasks[index])
                 : objc3_task_start(&options, &state->tasks[index])) == OBJC3_SERVICE_OK);
}
static void deep_cancellation_resume(objc3_task_t *task, void *opaque) {
  DeepCancellationFrame *frame = (DeepCancellationFrame *)opaque;
  if (frame->step++ == 0) {
    if (frame->index + 1 != frame->state->count)
      deep_cancellation_start(frame->state, frame->index + 1, task);
    assert(objc3_task_await(task, frame->state->dependency) == OBJC3_SERVICE_OK);
  } else {
    assert(objc3_task_is_cancel_requested(task));
    tree_complete(task, OBJC3_COMPLETION_CANCELLED);
  }
}
static void deep_child_cancellation_and_ancestry_cleanup(void) {
  DeepCancellation state = ZERO_INIT;
  state.count = 1536; /* Deliberately exceeds executor continuation admission. */
  state.tasks = (objc3_task_t **)calloc(state.count, sizeof(*state.tasks));
  assert(state.tasks);
  assert(objc3_executor_create(1, &state.executor) == OBJC3_SERVICE_OK);
  objc3_task_start_options options = ZERO_INIT;
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = state.executor;
  options.resume = remain_suspended;
  assert(objc3_task_start(&options, &state.dependency) == OBJC3_SERVICE_OK);
  deep_cancellation_start(&state, 0, NULL);
  uint32_t executed = 0;
  assert(objc3_executor_pump(state.executor, state.count * 2, &executed) == OBJC3_SERVICE_OK);
  assert(executed == state.count + 1 && state.destroyed == 0);
  objc3_task_request_cancel(state.tasks[0]);
  for (unsigned index = 0; index != state.count; ++index)
    assert(objc3_task_is_cancel_requested(state.tasks[index]));
  assert(objc3_executor_pump(state.executor, state.count * 2, &executed) == OBJC3_SERVICE_OK);
  assert(state.destroyed == state.count);
  for (unsigned index = 0; index != state.count; ++index) {
    tree_assert_kind(state.tasks[index], OBJC3_COMPLETION_CANCELLED);
    objc3_task_release(state.tasks[index]);
  }
  /* Late dependency notifications own only ancestry tokens. Their final
   * release reclaims the deep chain without recursing through native frames. */
  tree_complete(state.dependency, OBJC3_COMPLETION_SUCCESS);
  objc3_task_release(state.dependency);
  objc3_executor_release(state.executor);
  free(state.tasks);
}
typedef struct CleanupJoinC {
  objc3_task_group_t *group;
  objc3_service_status status;
  unsigned resumes;
  unsigned destroyed;
} CleanupJoinC;
static void cleanup_join_c_resume(objc3_task_t *task, void *opaque) {
  CleanupJoinC *frame = (CleanupJoinC *)opaque;
  if (++frame->resumes == 1) {
    objc3_task_request_cancel(task);
    assert(objc3_task_group_await_cleanup(frame->group, task, &frame->status) ==
           OBJC3_SERVICE_OK);
    return;
  }
  assert(frame->status == OBJC3_SERVICE_OK && objc3_task_is_cancel_requested(task));
  assert(objc3_task_group_join(frame->group) == OBJC3_SERVICE_OK);
  tree_complete(task, OBJC3_COMPLETION_CANCELLED);
}
static void cleanup_join_c_destroy(void *opaque) {
  ++((CleanupJoinC *)opaque)->destroyed;
}
static void cancelled_cleanup_join_c(void) {
  CleanupJoinC frame = ZERO_INIT;
  frame.status = OBJC3_SERVICE_UNSUPPORTED_VERSION;
  objc3_executor_t *executor = NULL;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_create(NULL, &frame.group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(frame.group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_cancel(frame.group) == OBJC3_SERVICE_OK);
  objc3_task_start_options options = ZERO_INIT;
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = &frame;
  options.resume = cleanup_join_c_resume;
  options.destroy = cleanup_join_c_destroy;
  objc3_task_t *task = NULL;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(frame.resumes == 1 && frame.destroyed == 0);
  objc3_task_group_release(frame.group);
  assert(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK);
  assert(frame.resumes == 2 && frame.destroyed == 1);
  objc3_task_release(task);
  objc3_executor_release(executor);
}
static const objc3_async_result_type spawn_void_type = {
    sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION,
    0, 1, OBJC3_ASYNC_VOID, 0, "fixture.spawn.void", 18, NULL};
typedef struct SpawnProbe {
  objc3_actor_t *expected_actor;
  objc3_executor_t *expected_executor;
  int cancelled;
  unsigned ran, destroyed;
} SpawnProbe;
static void spawn_probe_destroy(void *opaque) {
  ++((SpawnProbe *)opaque)->destroyed;
}
static void spawn_probe_resume(objc3_task_t *task, void *opaque) {
  SpawnProbe *probe = (SpawnProbe *)opaque;
  objc3_current_actor_facts current = current_destination();
  assert(current.actor_identity == objc3_actor_identity(probe->expected_actor));
  assert(current.executor_identity == objc3_executor_identity(probe->expected_executor));
  assert(!!objc3_task_is_cancel_requested(task) == probe->cancelled);
  ++probe->ran;
  objc3_completion completion = ZERO_INIT;
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = probe->cancelled ? OBJC3_COMPLETION_CANCELLED
                                     : OBJC3_COMPLETION_SUCCESS;
  if (!probe->cancelled) completion.result_type = &spawn_void_type;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
static objc3_async_frame spawn_frame(SpawnProbe *probe) {
  objc3_async_frame frame = {sizeof(objc3_async_frame), OBJC3_ASYNC_ABI_VERSION,
                              probe, spawn_probe_resume, spawn_probe_destroy,
                              &spawn_void_type};
  return frame;
}
typedef struct SpawnParent {
  SpawnProbe child, detached;
  objc3_task_group_t *group;
  objc3_task_t *child_task, *detached_task;
  unsigned destroyed;
} SpawnParent;
static void spawn_parent_destroy(void *opaque) {
  ++((SpawnParent *)opaque)->destroyed;
}
static void spawn_parent_resume(objc3_task_t *parent, void *opaque) {
  SpawnParent *state = (SpawnParent *)opaque;
  objc3_task_request_cancel(parent);
  objc3_async_frame child = spawn_frame(&state->child);
  objc3_async_frame detached = spawn_frame(&state->detached);
  assert(objc3_async_frame_spawn(&child, &state->child_task) == OBJC3_SERVICE_OK);
  assert(!child.context && objc3_task_is_cancel_requested(state->child_task));
  assert(objc3_async_frame_detach(&detached, &state->detached_task) == OBJC3_SERVICE_OK);
  assert(!detached.context && !objc3_task_is_cancel_requested(state->detached_task));
  assert(objc3_task_group_add(state->group, state->child_task) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(state->group, state->detached_task) == OBJC3_SERVICE_OK);
  objc3_current_actor_facts current = current_destination();
  assert(current.actor_identity == objc3_actor_identity(state->child.expected_actor));
  objc3_completion completion = ZERO_INIT;
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(parent, &completion) == OBJC3_SERVICE_OK);
}
static void ambient_child_and_detached_start(void) {
  objc3_executor_t *source = NULL, *global = NULL;
  objc3_actor_t *actor = NULL;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(source, &actor) == OBJC3_SERVICE_OK);
  SpawnProbe outside = ZERO_INIT;
  objc3_async_frame unused = spawn_frame(&outside);
  objc3_task_t *missing = NULL;
  assert(objc3_async_frame_spawn(&unused, &missing) == OBJC3_SERVICE_NOT_READY);
  assert(unused.context == &outside && !missing && outside.destroyed == 0);
  assert(objc3_async_frame_dispose(&unused) == OBJC3_SERVICE_OK);
  assert(outside.destroyed == 1);

  SpawnParent state = ZERO_INIT;
  state.child.expected_actor = actor;
  state.child.expected_executor = source;
  state.child.cancelled = 1;
  state.detached.expected_executor = global;
  assert(objc3_task_group_create(NULL, &state.group) == OBJC3_SERVICE_OK);
  objc3_task_start_options options = ZERO_INIT;
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.actor = actor;
  options.frame = &state;
  options.resume = spawn_parent_resume;
  options.destroy = spawn_parent_destroy;
  objc3_task_t *parent = NULL;
  assert(objc3_task_start(&options, &parent) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(state.group, parent) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(state.group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(state.group) == OBJC3_SERVICE_OK);
  assert(state.child.ran == 1 && state.child.destroyed == 1);
  assert(state.detached.ran == 1 && state.detached.destroyed == 1);
  assert(state.destroyed == 1);
  objc3_task_release(state.child_task);
  objc3_task_release(state.detached_task);
  objc3_task_release(parent);
  objc3_task_group_release(state.group);
  objc3_actor_release(actor);
  objc3_executor_release(global);
  objc3_executor_release(source);
}

int main(void) {
  groups();
  actors();
  task_hops();
  child_cancellation(1, 0, 0, 0);
  child_cancellation(0, 0, 0, 0);
  child_cancellation(0, 1, 0, 0);
  child_cancellation(0, 0, 1, 0);
  child_cancellation(0, 0, 0, 1);
  deep_child_cancellation_and_ancestry_cleanup();
  cancelled_cleanup_join_c();
  ambient_child_and_detached_start();
  objc3_executor_t *main_executor = NULL, *global_executor = NULL;
  objc3_error_t *service_error = NULL;
  objc3_service_status service_status = OBJC3_SERVICE_OK;
  assert(objc3_async_error_create_service(OBJC3_SERVICE_OUT_OF_MEMORY,
                                          &service_error) == OBJC3_SERVICE_OK);
  assert(objc3_async_error_get_service(service_error, &service_status) ==
         OBJC3_SERVICE_OK && service_status == OBJC3_SERVICE_OUT_OF_MEMORY);
  objc3_error_release(service_error);
  objc3_executor_t *main_again = NULL, *global_again = NULL;
  assert(objc3_executor_get_main(&main_executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_global(&global_executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_main(&main_again) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_global(&global_again) == OBJC3_SERVICE_OK);
  assert(main_executor == main_again && global_executor == global_again);
  assert(main_executor != global_executor);
  assert(objc3_executor_identity(main_executor) != 0);
  assert(objc3_executor_identity(global_executor) !=
         objc3_executor_identity(main_executor));
  assert(objc3_executor_identity(NULL) == 0 && !objc3_executor_is_current(NULL));
  assert(!objc3_executor_is_current(main_executor));
  assert(objc3_executor_register_named("fixture.c.owner", main_executor) ==
         OBJC3_SERVICE_OK);
  objc3_executor_t *named = NULL;
  assert(objc3_executor_get_named("fixture.c.owner", &named) == OBJC3_SERVICE_OK);
  assert(named == main_executor);
  assert(objc3_executor_unregister_named("fixture.c.owner") == OBJC3_SERVICE_OK);
  objc3_executor_t *retired = named;
  assert(objc3_executor_get_named("fixture.c.owner", &retired) ==
         OBJC3_SERVICE_CLOSED && !retired);
  assert(objc3_executor_register_named("fixture.c.owner", main_executor) ==
         OBJC3_SERVICE_CLOSED);
  objc3_executor_release(named);
  objc3_executor_release(main_again);
  objc3_executor_release(global_again);
  objc3_executor_release(main_executor);
  objc3_executor_release(global_executor);
  return 0;
}
