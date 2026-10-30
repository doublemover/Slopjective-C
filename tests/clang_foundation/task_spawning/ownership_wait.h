#ifndef SPAWN_OWNERSHIP_WAIT_H
#define SPAWN_OWNERSHIP_WAIT_H
#include <assert.h>
#include <sched.h>
#include <time.h>

/* Joining waits for frame/callback cleanup. The scheduler may still hold its
 * final task lease, so payload destruction after releasing public owners is
 * asynchronous. Synchronize with the destructor's release store. */
static inline void wait_for_destructions(const unsigned *count, unsigned expected) {
  struct timespec start, now;
  assert(timespec_get(&start, TIME_UTC) == TIME_UTC);
  while (__atomic_load_n(count, __ATOMIC_ACQUIRE) < expected) {
    assert(timespec_get(&now, TIME_UTC) == TIME_UTC);
    assert(now.tv_sec - start.tv_sec < 5);
    sched_yield();
  }
  assert(__atomic_load_n(count, __ATOMIC_ACQUIRE) == expected);
}
#endif
