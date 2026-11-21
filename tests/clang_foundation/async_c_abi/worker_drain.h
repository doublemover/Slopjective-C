#pragma once
#include "objc3_runtime_services.h"
#include <assert.h>
#include <pthread.h>
#include <time.h>

struct bridge_worker_drain {
  pthread_mutex_t mutex;
  pthread_cond_t ready;
  int reached;
};

static void bridge_worker_drain_callback(void *context) {
  struct bridge_worker_drain *state = (struct bridge_worker_drain *)context;
  assert(pthread_mutex_lock(&state->mutex) == 0);
  state->reached = 1;
  assert(pthread_cond_signal(&state->ready) == 0);
  assert(pthread_mutex_unlock(&state->mutex) == 0);
}

static void bridge_finish_serial_worker(objc3_executor_t *worker) {
  struct bridge_worker_drain state = {PTHREAD_MUTEX_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER, 0};
  // Joined task frames can still have the serial dispatch's final task lease.
  // A FIFO barrier retires those leases before the ordinary host last-release
  // closes admission and joins the worker, including its autorelease pool.
  assert(objc3_executor_enqueue(worker, bridge_worker_drain_callback, &state) ==
         OBJC3_SERVICE_OK);
  assert(pthread_mutex_lock(&state.mutex) == 0);
  struct timespec deadline;
  assert(timespec_get(&deadline, TIME_UTC) == TIME_UTC);
  deadline.tv_sec += 10;
  while (!state.reached)
    assert(pthread_cond_timedwait(&state.ready, &state.mutex, &deadline) == 0);
  assert(pthread_mutex_unlock(&state.mutex) == 0);
  objc3_executor_release(worker);
  assert(pthread_cond_destroy(&state.ready) == 0);
  assert(pthread_mutex_destroy(&state.mutex) == 0);
}
