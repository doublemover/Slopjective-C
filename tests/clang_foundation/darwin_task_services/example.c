#include <objc3_runtime_services.h>
#include <stdio.h>
#include <stdlib.h>

struct observation {
  int resumes;
  int completions;
  int destructions;
  objc3_completion_kind kind;
};

static void require(int condition) {
  if (!condition)
    abort();
}

static void resume(objc3_task_t *task, void *frame) {
  struct observation *entry = frame;
  ++entry->resumes;
  const objc3_completion outcome = {
      .struct_size = sizeof(outcome), .version = OBJC3_SERVICE_ABI_VERSION,
      .kind = objc3_task_is_cancel_requested(task) ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS,
  };
  require(objc3_task_complete(task, &outcome) == OBJC3_SERVICE_OK);
}

static void completion(void *context, const objc3_completion *outcome) {
  struct observation *entry = context;
  ++entry->completions;
  entry->kind = outcome->kind;
}

static void destroy(void *frame) {
  struct observation *entry = frame;
  ++entry->destructions;
}

int main(void) {
  objc3_executor_t *executor = NULL;
  objc3_task_group_t *group = NULL;
  objc3_task_t *tasks[2] = {NULL, NULL};
  struct observation observations[2] = {0};
  require(objc3_executor_get_main(&executor) == OBJC3_SERVICE_OK);
  require(objc3_task_group_create(NULL, &group) == OBJC3_SERVICE_OK);
  for (unsigned i = 0; i != 2; ++i) {
    const objc3_task_start_options options = {
        .struct_size = sizeof(options), .version = OBJC3_SERVICE_ABI_VERSION,
        .executor = executor, .frame = observations + i, .resume = resume,
        .destroy = destroy, .completion = completion, .completion_context = observations + i,
    };
    require(objc3_task_start(&options, tasks + i) == OBJC3_SERVICE_OK);
    require(objc3_task_group_add(group, tasks[i]) == OBJC3_SERVICE_OK);
  }
  require(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
  require(objc3_task_group_join(group) == OBJC3_SERVICE_NOT_READY);
  uint32_t executed = 0;
  require(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK && executed == 1);
  require(objc3_task_group_cancel(group) == OBJC3_SERVICE_OK);
  require(objc3_executor_pump(executor, 1, &executed) == OBJC3_SERVICE_OK && executed == 1);
  require(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  for (unsigned i = 0; i != 2; ++i) {
    const struct observation *entry = observations + i;
    require(entry->resumes == 1 && entry->completions == 1 && entry->destructions == 1);
    require(entry->kind == (i ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS));
    objc3_task_release(tasks[i]);
  }
  objc3_task_group_release(group);
  objc3_executor_release(executor);
  puts("owned Objective-C 3 Darwin task services: PASS");
  return 0;
}
