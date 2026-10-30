#include "runtime/concurrency/service_internal.h"

#ifndef OBJC3_LOCK_DEFECT
#define OBJC3_LOCK_DEFECT 0
#endif

void requires_queue(objc3_executor_t *executor) OBJC3_REQUIRES(executor->mutex) {
  executor->stopping = true;
}

void outside_queue(objc3_executor_t *executor) OBJC3_EXCLUDES(executor->mutex) {
  (void)executor;
}

void contract(objc3_executor_t *executor) {
#if OBJC3_LOCK_DEFECT == 1
  executor->stopping = true; // missing-lock-write
#elif OBJC3_LOCK_DEFECT == 2
  requires_queue(executor); // missing-required-lock
#elif OBJC3_LOCK_DEFECT == 3
  objc3::services::ExecutorQueueLock lock(executor->mutex);
  outside_queue(executor); // callback-under-lock
#elif OBJC3_LOCK_DEFECT == 4
  objc3::services::ExecutorQueueLock lock(executor->mutex);
  lock.wait(executor->ready, [executor] {
    return executor->stopping; // unmodeled-predicate-lock
  });
#else
  outside_queue(executor);
  {
    objc3::services::ExecutorQueueLock lock(executor->mutex);
    requires_queue(executor);
    executor->queue.clear();
    lock.wait(executor->ready, [executor]() OBJC3_REQUIRES(executor->mutex) {
      return executor->stopping || !executor->queue.empty();
    });
  }
  outside_queue(executor);
#endif
}
