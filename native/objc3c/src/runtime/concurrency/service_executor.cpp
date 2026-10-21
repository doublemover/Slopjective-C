#include "service_internal.h"
#include "service_autorelease_pool.h"

#include <cassert>
#include <new>
#include <utility>

namespace {

constexpr uint32_t kMaximumWorkerExecutors = 8;
constexpr size_t kMaximumQueuedCallbacks = 1024;
std::atomic<uint32_t> live_worker_executors{0};
std::atomic<uint64_t> next_executor_identity{1};
thread_local uint64_t current_executor_logical_identity = 0;
thread_local bool *current_callback_reschedule = nullptr;

struct ExecutorIdentityScope {
  uint64_t previous;
  explicit ExecutorIdentityScope(uint64_t identity) noexcept
      : previous(std::exchange(current_executor_logical_identity, identity)) {}
  ~ExecutorIdentityScope() { current_executor_logical_identity = previous; }
};

struct CallbackRescheduleScope {
  bool *previous;
  explicit CallbackRescheduleScope(bool &reschedule) noexcept
      : previous(std::exchange(current_callback_reschedule, &reschedule)) {}
  ~CallbackRescheduleScope() { current_callback_reschedule = previous; }
};

bool allocate_executor_identity(uint64_t *out_identity) noexcept {
  uint64_t candidate = next_executor_identity.load(std::memory_order_relaxed);
  for (;;) {
    if (candidate == 0 || candidate == UINT64_MAX) return false;
    if (next_executor_identity.compare_exchange_weak(
            candidate, candidate + 1, std::memory_order_relaxed,
            std::memory_order_relaxed)) {
      *out_identity = candidate;
      return true;
    }
  }
}

bool reserve_worker_slot() noexcept {
  uint32_t count = live_worker_executors.load(std::memory_order_relaxed);
  while (count < kMaximumWorkerExecutors) {
    if (live_worker_executors.compare_exchange_weak(
            count, count + 1, std::memory_order_acq_rel,
            std::memory_order_relaxed))
      return true;
  }
  return false;
}

void run_callback(objc3_executor_t *executor,
                  std::list<objc3_executor_t::WorkItem> &active) noexcept {
  bool reschedule = false;
  {
    ExecutorIdentityScope current(executor->identity);
    CallbackRescheduleScope continuation(reschedule);
    objc3::services::CallbackAutoreleasePool pool;
    try {
      active.front().callback(active.front().context);
    } catch (...) {
      // Exceptions cannot escape a C callback or kill the worker.
    }
  }
  if (reschedule) {
    {
      std::lock_guard<std::mutex> lock(executor->mutex);
      // Keep the active node rather than allocating a new callback. A full
      // public queue cannot strand an already-admitted actor continuation.
      executor->queue.splice(executor->queue.end(), active);
    }
    executor->ready.notify_one();
  }
}

void worker_main(objc3_executor_t *executor) noexcept {
  for (;;) {
    std::list<objc3_executor_t::WorkItem> active;
    {
      std::unique_lock<std::mutex> lock(executor->mutex);
      executor->ready.wait(lock, [executor] {
        return executor->stopping || !executor->queue.empty();
      });
      if (executor->queue.empty() && executor->stopping) break;
      active.splice(active.end(), executor->queue, executor->queue.begin());
    }
    run_callback(executor, active);
  }
  objc3_executor_internal_release(executor);
}

} // namespace

uint64_t objc3::services::executor_current_identity() noexcept {
  return current_executor_logical_identity;
}

bool objc3::services::executor_is_current_identity(uint64_t identity) noexcept {
  return identity != 0 && current_executor_logical_identity == identity;
}

void objc3::services::executor_reschedule_current_callback() noexcept {
  assert(current_callback_reschedule);
  *current_callback_reschedule = true;
}

bool objc3_executor_internal_retain(objc3_executor_t *executor) {
  return executor && objc3::services::try_retain(executor->references);
}

void objc3_executor_internal_release(objc3_executor_t *executor) {
  if (executor->references.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    if (executor->worker_slot)
      live_worker_executors.fetch_sub(1, std::memory_order_acq_rel);
    delete executor;
  }
}

extern "C" objc3_service_status objc3_executor_create(
    uint32_t manual, objc3_executor_t **out_executor) {
  if (!out_executor || manual > 1) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_executor = nullptr;
  uint64_t identity = 0;
  if (!allocate_executor_identity(&identity)) return OBJC3_SERVICE_NOT_READY;
  auto *executor = new (std::nothrow) objc3_executor(manual != 0, identity);
  if (!executor) return OBJC3_SERVICE_OUT_OF_MEMORY;
  if (!executor->manual) {
    if (!reserve_worker_slot()) {
      delete executor;
      return OBJC3_SERVICE_NOT_READY;
    }
    executor->worker_slot = true;
    // The worker owns a lifetime reference, independent of public handles.
    if (!objc3_executor_internal_retain(executor)) {
      executor->worker_slot = false;
      live_worker_executors.fetch_sub(1, std::memory_order_acq_rel);
      delete executor;
      return OBJC3_SERVICE_NOT_READY;
    }
    try {
      executor->worker = std::thread(worker_main, executor);
    } catch (...) {
      executor->references.fetch_sub(1, std::memory_order_relaxed);
      executor->worker_slot = false;
      live_worker_executors.fetch_sub(1, std::memory_order_acq_rel);
      delete executor;
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
  }
  *out_executor = executor;
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_executor_t *objc3_executor_retain(objc3_executor_t *executor) {
  if (!executor) return nullptr;
  if (!objc3::services::try_retain(executor->public_references)) return nullptr;
  if (!objc3_executor_internal_retain(executor)) {
    executor->public_references.fetch_sub(1, std::memory_order_relaxed);
    return nullptr;
  }
  return executor;
}

extern "C" void objc3_executor_release(objc3_executor_t *executor) {
  if (!executor) return;
  if (executor->public_references.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    {
      std::lock_guard<std::mutex> lock(executor->mutex);
      executor->stopping = true;
    }
    executor->ready.notify_all();
    if (executor->worker.joinable()) {
      if (executor->worker.get_id() == std::this_thread::get_id())
        executor->worker.detach();
      else
        executor->worker.join();
    }
  }
  objc3_executor_internal_release(executor);
}

extern "C" objc3_service_status objc3_executor_enqueue(
    objc3_executor_t *executor, objc3_executor_callback callback,
    void *context) {
  if (!executor || !callback) return OBJC3_SERVICE_INVALID_ARGUMENT;
  {
    std::lock_guard<std::mutex> lock(executor->mutex);
    if (executor->stopping) return OBJC3_SERVICE_CLOSED;
    if (executor->queue.size() >= kMaximumQueuedCallbacks)
      return OBJC3_SERVICE_NOT_READY;
    try {
      executor->queue.push_back({callback, context});
    } catch (...) {
      return OBJC3_SERVICE_OUT_OF_MEMORY;
    }
  }
  executor->ready.notify_one();
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status objc3_executor_pump(
    objc3_executor_t *executor, uint32_t limit, uint32_t *out_executed) {
  if (!executor || !executor->manual || !out_executed)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_executed = 0;
  std::unique_lock<std::mutex> serial(executor->pump_mutex, std::try_to_lock);
  if (!serial.owns_lock()) return OBJC3_SERVICE_NOT_READY;
  if (!objc3_executor_internal_retain(executor))
    return OBJC3_SERVICE_NOT_READY;
  while (*out_executed < limit) {
    std::list<objc3_executor_t::WorkItem> active;
    {
      std::lock_guard<std::mutex> lock(executor->mutex);
      if (executor->queue.empty()) break;
      active.splice(active.end(), executor->queue, executor->queue.begin());
    }
    run_callback(executor, active);
    ++*out_executed;
  }
  // The final pump reference may destroy the executor when a callback
  // released its last public handle. Drop the member-mutex lock first.
  serial.unlock();
  objc3_executor_internal_release(executor);
  return OBJC3_SERVICE_OK;
}
