#ifndef OBJC3_RUNTIME_SERVICE_CHECKED_LOCK_H
#define OBJC3_RUNTIME_SERVICE_CHECKED_LOCK_H

#include <condition_variable>
#include <mutex>
#include <utility>

#if defined(__clang__)
#define OBJC3_CAPABILITY(name) __attribute__((capability(name)))
#define OBJC3_SCOPED_CAPABILITY __attribute__((scoped_lockable))
#define OBJC3_GUARDED_BY(lock) __attribute__((guarded_by(lock)))
#define OBJC3_REQUIRES(lock) __attribute__((requires_capability(lock)))
#define OBJC3_EXCLUDES(lock) __attribute__((locks_excluded(lock)))
#define OBJC3_ACQUIRE(lock) __attribute__((acquire_capability(lock)))
#define OBJC3_RELEASE() __attribute__((release_capability()))
#else
#define OBJC3_CAPABILITY(name)
#define OBJC3_SCOPED_CAPABILITY
#define OBJC3_GUARDED_BY(lock)
#define OBJC3_REQUIRES(lock)
#define OBJC3_EXCLUDES(lock)
#define OBJC3_ACQUIRE(lock)
#define OBJC3_RELEASE()
#endif

namespace objc3::services {

// An internal capability for one executor's queue domain. Keep the underlying
// std::mutex and condition_variable protocol; no new scheduling primitive or
// public ABI is introduced. Task, group and pump locks are separate domains.
class OBJC3_CAPABILITY("mutex") ExecutorQueueMutex final {
  std::mutex native_;
  friend class ExecutorQueueLock;
};

class OBJC3_SCOPED_CAPABILITY ExecutorQueueLock final {
  std::unique_lock<std::mutex> native_;

public:
  explicit ExecutorQueueLock(ExecutorQueueMutex &mutex) OBJC3_ACQUIRE(mutex)
      : native_(mutex.native_) {}
  ~ExecutorQueueLock() OBJC3_RELEASE() {}
  ExecutorQueueLock(const ExecutorQueueLock &) = delete;
  ExecutorQueueLock &operator=(const ExecutorQueueLock &) = delete;

  // wait releases/reacquires the very same native mutex. The capability holds
  // at both caller boundaries. A predicate's explicit REQUIRES contract models
  // its invocation under the reacquired lock, including spurious wakeups.
  template <class Predicate>
  void wait(std::condition_variable &condition, Predicate predicate) {
    condition.wait(native_, std::move(predicate));
  }
};

static_assert(sizeof(ExecutorQueueMutex) == sizeof(std::mutex));
static_assert(alignof(ExecutorQueueMutex) == alignof(std::mutex));
static_assert(sizeof(ExecutorQueueLock) == sizeof(std::unique_lock<std::mutex>));

} // namespace objc3::services
#endif
