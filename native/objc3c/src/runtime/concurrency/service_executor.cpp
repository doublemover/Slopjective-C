#include "service_internal.h"

#include <new>

namespace {

constexpr uint32_t kMaximumWorkerExecutors = 8;
constexpr size_t kMaximumQueuedCallbacks = 1024;
std::atomic<uint32_t> live_worker_executors{0};
std::atomic<uint64_t> next_executor_identity{1};
thread_local uint64_t current_executor_logical_identity = 0;

struct ExecutorIdentityScope {
  uint64_t previous;
  explicit ExecutorIdentityScope(uint64_t identity) noexcept
      : previous(std::exchange(current_executor_logical_identity, identity)) {}
  ~ExecutorIdentityScope() { current_executor_logical_identity = previous; }
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
                  const objc3_executor_t::WorkItem &item) noexcept {
  ExecutorIdentityScope current(executor->identity);
  try {
    item.callback(item.context);
  } catch (...) {
    // Exceptions are not permitted to escape a C callback or kill the worker.
  }
}

void worker_main(objc3_executor_t *executor) noexcept {
  for (;;) {
    objc3_executor_t::WorkItem item{};
    {
      std::unique_lock<std::mutex> lock(executor->mutex);
      executor->ready.wait(lock, [executor] {
        return executor->stopping || !executor->queue.empty();
      });
      if (executor->queue.empty() && executor->stopping) break;
      item = executor->queue.front();
      executor->queue.pop_front();
    }
    run_callback(executor, item);
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

void objc3_executor_internal_retain(objc3_executor_t *executor) {
  executor->references.fetch_add(1, std::memory_order_relaxed);
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
    executor->references.fetch_add(1, std::memory_order_relaxed);
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
  executor->public_references.fetch_add(1, std::memory_order_relaxed);
  objc3_executor_internal_retain(executor);
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
  objc3_executor_internal_retain(executor);
  while (*out_executed < limit) {
    objc3_executor_t::WorkItem item{};
    {
      std::lock_guard<std::mutex> lock(executor->mutex);
      if (executor->queue.empty()) break;
      item = executor->queue.front();
      executor->queue.pop_front();
    }
    run_callback(executor, item);
    ++*out_executed;
  }
  // The final pump reference may destroy the executor when a callback
  // released its last public handle. Drop the member-mutex lock first.
  serial.unlock();
  objc3_executor_internal_release(executor);
  return OBJC3_SERVICE_OK;
}
