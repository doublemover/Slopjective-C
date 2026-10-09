#ifndef OBJC3_RUNTIME_SERVICE_INTERNAL_H
#define OBJC3_RUNTIME_SERVICE_INTERNAL_H

#include "../public/objc3_runtime_services.h"
#include "service_ref_count.h"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

namespace objc3::services {

// These are logical scheduler facts. They never expose or infer physical
// thread identity.
uint64_t executor_current_identity() noexcept;
bool executor_is_current_identity(uint64_t identity) noexcept;

} // namespace objc3::services

struct objc3_executor final {
  struct WorkItem {
    objc3_executor_callback callback;
    void *context;
  };

  std::atomic<uint32_t> references{1};
  std::atomic<uint32_t> public_references{1};
  const uint64_t identity;
  const bool manual;
  bool worker_slot{false};
  std::mutex mutex;
  // Separate from the queue lock so callbacks remain reentrant-safe. A pump
  // attempted while another pump is active returns NOT_READY.
  std::mutex pump_mutex;
  std::condition_variable ready;
  std::deque<WorkItem> queue;
  bool stopping{false};
  std::thread worker;

  explicit objc3_executor(bool is_manual, uint64_t logical_identity)
      : identity(logical_identity), manual(is_manual) {}
};

struct objc3_task final {
  struct Waiter {
    objc3_completion_callback callback;
    void *context;
  };

  std::atomic<uint32_t> references{1};
  std::atomic<bool> cancel_requested{false};
  mutable std::mutex mutex;
  std::condition_variable completed;
  objc3_executor_t *executor{nullptr};
  void *frame{nullptr};
  objc3_task_resume resume{nullptr};
  objc3_frame_destroy destroy{nullptr};
  bool terminal{false};
  bool completion_claimed{false};
  bool resume_queued{false};
  bool resume_active{false};
  uint32_t callbacks_active{0};
  bool frame_destroying{false};
  bool frame_destroyed{true};
  objc3_completion_kind kind{OBJC3_COMPLETION_CANCELLED};
  bool has_payload{false};
  void *payload{nullptr};
  objc3_value_ops payload_ops{};
  bool payload_ops_set{false};
  objc3_error_t *error{nullptr};
  std::vector<Waiter> waiters;

  ~objc3_task();
};

bool objc3_executor_internal_retain(objc3_executor_t *executor);
void objc3_executor_internal_release(objc3_executor_t *executor);
bool objc3_task_internal_retain(objc3_task_t *task);
void objc3_task_internal_release(objc3_task_t *task);
void objc3_service_run_task_resume(void *context);
void objc3_task_destroy_frame(void *frame, objc3_frame_destroy destroy);
objc3_task_t *objc3_task_current_callback() noexcept;
objc3_service_status objc3_task_wait_quiescent(objc3_task_t *task);

#endif
