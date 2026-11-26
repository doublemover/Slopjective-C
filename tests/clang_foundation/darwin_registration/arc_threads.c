#include <pthread.h>
#include <stddef.h>

extern void *objc3_darwin_allocate_v1(const void *cls);
extern void *objc3_darwin_arc_autorelease_v1(void *object);
extern void *objc3_darwin_arc_pool_push_v1(void);
extern void objc3_darwin_arc_pool_pop_v1(void *pool);
extern void *objc3_darwin_arc_init_weak_v1(void **slot, void *object);
extern void *objc3_darwin_arc_load_weak_retained_v1(void **slot);
extern void objc3_darwin_arc_move_weak_v1(void **destination, void **source);
extern void objc3_darwin_arc_destroy_weak_v1(void **slot);
extern void objc3_darwin_arc_release_v1(void *object);

struct worker {
  void **observer;
  pthread_mutex_t mutex;
  pthread_cond_t changed;
  int retained;
  int may_release;
  int alive;
};

static void *hold_weak_on_another_thread(void *argument) {
  struct worker *worker = argument;
  void *pool = objc3_darwin_arc_pool_push_v1();
  void *held = objc3_darwin_arc_load_weak_retained_v1(worker->observer);
  pthread_mutex_lock(&worker->mutex);
  worker->alive = held != NULL;
  worker->retained = 1;
  pthread_cond_signal(&worker->changed);
  while (!worker->may_release)
    pthread_cond_wait(&worker->changed, &worker->mutex);
  pthread_mutex_unlock(&worker->mutex);
  objc3_darwin_arc_autorelease_v1(held);
  objc3_darwin_arc_pool_pop_v1(pool);
  return NULL;
}

int objc3_darwin_check_arc_threads(const void *cls) {
  void *pool = objc3_darwin_arc_pool_push_v1();
  void *owned = objc3_darwin_allocate_v1(cls);
  void *main_observer;
  void *moved_observer;
  objc3_darwin_arc_init_weak_v1(&main_observer, owned);
  objc3_darwin_arc_move_weak_v1(&moved_observer, &main_observer);
  struct worker worker = {.observer = &moved_observer,
                          .mutex = PTHREAD_MUTEX_INITIALIZER,
                          .changed = PTHREAD_COND_INITIALIZER};
  pthread_t thread;
  if (pthread_create(&thread, NULL, hold_weak_on_another_thread, &worker) != 0) {
    objc3_darwin_arc_release_v1(owned);
    objc3_darwin_arc_destroy_weak_v1(&moved_observer);
    objc3_darwin_arc_destroy_weak_v1(&main_observer);
    objc3_darwin_arc_pool_pop_v1(pool);
    pthread_mutex_destroy(&worker.mutex);
    pthread_cond_destroy(&worker.changed);
    return 0;
  }

  pthread_mutex_lock(&worker.mutex);
  while (!worker.retained)
    pthread_cond_wait(&worker.changed, &worker.mutex);
  // The worker's active pool must not intercept an autorelease on this thread.
  void *independent = objc3_darwin_allocate_v1(cls);
  void *pool_observer;
  objc3_darwin_arc_init_weak_v1(&pool_observer, independent);
  objc3_darwin_arc_autorelease_v1(independent);
  objc3_darwin_arc_release_v1(owned);
  void *alive_in_worker = objc3_darwin_arc_load_weak_retained_v1(&moved_observer);
  int alive = alive_in_worker != NULL;
  objc3_darwin_arc_release_v1(alive_in_worker);
  worker.may_release = 1;
  pthread_cond_signal(&worker.changed);
  pthread_mutex_unlock(&worker.mutex);
  pthread_join(thread, NULL);
  void *after_worker_pop = objc3_darwin_arc_load_weak_retained_v1(&moved_observer);
  void *before_main_pop = objc3_darwin_arc_load_weak_retained_v1(&pool_observer);
  int passed = worker.alive && alive && after_worker_pop == NULL && before_main_pop != NULL;
  objc3_darwin_arc_release_v1(before_main_pop);
  objc3_darwin_arc_release_v1(after_worker_pop);
  objc3_darwin_arc_destroy_weak_v1(&moved_observer);
  objc3_darwin_arc_destroy_weak_v1(&main_observer);
  objc3_darwin_arc_pool_pop_v1(pool);
  void *after_main_pop = objc3_darwin_arc_load_weak_retained_v1(&pool_observer);
  passed &= after_main_pop == NULL;
  objc3_darwin_arc_release_v1(after_main_pop);
  objc3_darwin_arc_destroy_weak_v1(&pool_observer);
  pthread_mutex_destroy(&worker.mutex);
  pthread_cond_destroy(&worker.changed);
  return passed;
}
