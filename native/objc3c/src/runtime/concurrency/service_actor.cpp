#include "service_actor.h"
#include "service_autorelease_pool.h"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <limits>
#include <mutex>
#include <new>
#include <utility>

namespace objc3::services {

struct Actor final {
  struct Entry {
    ActorJob job;
    void *context;
    ActorContextDestroy destroy;
  };

  std::atomic<uint32_t> references{1};
  std::atomic<uint32_t> public_references{1};
  const uint64_t identity;
  objc3_executor_t *executor;
  std::mutex mutex;
  std::condition_variable changed;
  std::deque<Entry> mailbox;
  bool closed{false};
  bool cancelled{false};
  bool drain_scheduled{false};
  bool active{false};
  bool callback_failed{false};

  Actor(uint64_t actor_id, objc3_executor_t *backing_executor)
      : identity(actor_id), executor(objc3_executor_retain(backing_executor)) {}

  ~Actor() { objc3_executor_release(executor); }
};

namespace {

constexpr size_t kMailboxCapacity = 1024;
std::atomic<uint64_t> next_actor_identity{1};
thread_local Actor *current_actor = nullptr;

bool allocate_identity(std::atomic<uint64_t> &source,
                       uint64_t *out_identity) noexcept {
  uint64_t candidate = source.load(std::memory_order_relaxed);
  for (;;) {
    if (candidate == 0 || candidate == UINT64_MAX) return false;
    if (source.compare_exchange_weak(candidate, candidate + 1,
                                     std::memory_order_relaxed,
                                     std::memory_order_relaxed)) {
      *out_identity = candidate;
      return true;
    }
  }
}

bool retain_internal(Actor *actor) noexcept {
  uint32_t count = actor->references.load(std::memory_order_relaxed);
  for (;;) {
    if (count == 0 || count == UINT32_MAX) return false;
    if (actor->references.compare_exchange_weak(
            count, count + 1, std::memory_order_relaxed,
            std::memory_order_relaxed))
      return true;
  }
}

void release_internal(Actor *actor) noexcept {
  if (actor->references.fetch_sub(1, std::memory_order_acq_rel) == 1)
    delete actor;
}

struct ActorExecutionScope {
  Actor *previous_actor;
  explicit ActorExecutionScope(Actor *actor) noexcept
      : previous_actor(std::exchange(current_actor, actor)) {}
  ~ActorExecutionScope() { current_actor = previous_actor; }
};

void invoke_destroy(Actor *actor, const ActorContextDestroy destroy,
                    void *context) noexcept {
  if (!destroy) return;
  ActorExecutionScope current(actor);
  try {
    destroy(context);
  } catch (...) {
    std::lock_guard<std::mutex> lock(actor->mutex);
    actor->callback_failed = true;
  }
}

void drain_mailbox(void *opaque) noexcept {
  auto *actor = static_cast<Actor *>(opaque);
  Actor::Entry entry{};
  bool execute_job = false;
  {
    std::lock_guard<std::mutex> lock(actor->mutex);
    if (!actor->mailbox.empty()) {
      entry = actor->mailbox.front();
      actor->mailbox.pop_front();
      actor->active = true;
      execute_job = !actor->cancelled;
    }
  }
  if (entry.job) {
    ActorExecutionScope current(actor);
    CallbackAutoreleasePool pool;
    if (execute_job) {
      try {
        entry.job(entry.context);
      } catch (...) {
        std::lock_guard<std::mutex> lock(actor->mutex);
        actor->callback_failed = true;
      }
    }
    invoke_destroy(actor, entry.destroy, entry.context);
    // Drain this job's pool while its actor identity and active flag still
    // hold, before another resume can observe its autoreleased temporaries.
  }
  {
    std::lock_guard<std::mutex> lock(actor->mutex);
    actor->active = false;
    if (!actor->mailbox.empty()) {
      executor_reschedule_current_callback();
      // Transfer the existing drain reference to the rescheduled callback.
      actor->changed.notify_all();
      return;
    }
    actor->drain_scheduled = false;
    actor->changed.notify_all();
  }
  release_internal(actor);
}

} // namespace

objc3_service_status actor_create(objc3_executor_t *executor,
                                  Actor **out_actor) {
  if (!executor || !out_actor) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_actor = nullptr;
  uint64_t identity = 0;
  if (!allocate_identity(next_actor_identity, &identity))
    return OBJC3_SERVICE_NOT_READY;
  auto *actor = new (std::nothrow) Actor(identity, executor);
  if (!actor) return OBJC3_SERVICE_OUT_OF_MEMORY;
  if (!actor->executor) {
    delete actor;
    return OBJC3_SERVICE_NOT_READY;
  }
  *out_actor = actor;
  return OBJC3_SERVICE_OK;
}

Actor *actor_retain(Actor *actor) {
  if (!actor) return nullptr;
  uint32_t count = actor->public_references.load(std::memory_order_relaxed);
  for (;;) {
    if (count == 0 || count == UINT32_MAX) return nullptr;
    if (actor->public_references.compare_exchange_weak(
            count, count + 1, std::memory_order_relaxed,
            std::memory_order_relaxed))
      break;
  }
  if (!retain_internal(actor)) {
    actor->public_references.fetch_sub(1, std::memory_order_relaxed);
    return nullptr;
  }
  return actor;
}

void actor_release(Actor *actor) {
  if (!actor) return;
  const uint32_t previous = actor->public_references.fetch_sub(
      1, std::memory_order_acq_rel);
  if (previous == 0) {
    actor->public_references.fetch_add(1, std::memory_order_relaxed);
    return;
  }
  if (previous == 1) (void)actor_cancel(actor);
  release_internal(actor);
}

uint64_t actor_identity(const Actor *actor) noexcept {
  return actor ? actor->identity : 0;
}

uint64_t actor_executor_identity(const Actor *actor) noexcept {
  return actor ? actor->executor->identity : 0;
}

objc3_executor_t *actor_executor(Actor *actor) noexcept {
  return actor ? actor->executor : nullptr;
}

objc3_service_status actor_enqueue(Actor *actor, ActorJob job, void *context,
                                  ActorContextDestroy destroy,
                                  ActorHopFacts *out_facts) {
  if (!actor || !job || !out_facts) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (out_facts->struct_size < sizeof(ActorHopFacts) ||
      out_facts->version != kActorServiceVersion)
    return out_facts->version != kActorServiceVersion
               ? OBJC3_SERVICE_UNSUPPORTED_VERSION
               : OBJC3_SERVICE_INVALID_ARGUMENT;

  ActorHopFacts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = kActorServiceVersion;
  facts.actor_identity = actor->identity;
  facts.current_actor_identity = current_actor ? current_actor->identity : 0;
  facts.target_executor_identity = actor->executor->identity;
  facts.current_executor_identity = executor_current_identity();
  facts.actor_hop = facts.current_actor_identity == facts.actor_identity ? 0u : 1u;
  facts.executor_hop = facts.current_executor_identity == facts.target_executor_identity ? 0u : 1u;

  std::unique_lock<std::mutex> lock(actor->mutex);
  if (actor->closed) return OBJC3_SERVICE_CLOSED;
  if (actor->mailbox.size() >= kMailboxCapacity)
    return OBJC3_SERVICE_NOT_READY;
  try {
    actor->mailbox.push_back({job, context, destroy});
  } catch (...) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  if (!actor->drain_scheduled) {
    if (!retain_internal(actor)) {
      actor->mailbox.pop_back();
      return OBJC3_SERVICE_NOT_READY;
    }
    actor->drain_scheduled = true;
    const objc3_service_status queued =
        objc3_executor_enqueue(actor->executor, drain_mailbox, actor);
    if (queued != OBJC3_SERVICE_OK) {
      actor->drain_scheduled = false;
      actor->mailbox.pop_back();
      lock.unlock();
      release_internal(actor);
      return queued;
    }
  }
  lock.unlock();
  *out_facts = facts;
  return OBJC3_SERVICE_OK;
}

objc3_service_status actor_close(Actor *actor) {
  if (!actor) return OBJC3_SERVICE_INVALID_ARGUMENT;
  std::lock_guard<std::mutex> lock(actor->mutex);
  actor->closed = true;
  actor->changed.notify_all();
  return OBJC3_SERVICE_OK;
}

objc3_service_status actor_cancel(Actor *actor) {
  if (!actor) return OBJC3_SERVICE_INVALID_ARGUMENT;
  {
    std::lock_guard<std::mutex> lock(actor->mutex);
    actor->closed = true;
    actor->cancelled = true;
  }
  actor->changed.notify_all();
  return OBJC3_SERVICE_OK;
}

objc3_service_status actor_join(Actor *actor) {
  if (!actor) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (current_actor == actor) return OBJC3_SERVICE_NOT_READY;
  std::unique_lock<std::mutex> lock(actor->mutex);
  if (!actor->closed) return OBJC3_SERVICE_NOT_READY;
  const bool pending = !actor->mailbox.empty() || actor->active ||
                       actor->drain_scheduled;
  if (pending && (actor->executor->manual ||
                  executor_is_current_identity(actor->executor->identity)))
      return OBJC3_SERVICE_NOT_READY;
  actor->changed.wait(lock, [actor] {
    return actor->mailbox.empty() && !actor->active && !actor->drain_scheduled;
  });
  return actor->callback_failed ? OBJC3_SERVICE_INVALID_ARGUMENT
                                : OBJC3_SERVICE_OK;
}

objc3_service_status actor_current_facts(CurrentActorFacts *out_facts) noexcept {
  if (!out_facts) return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (out_facts->struct_size < sizeof(CurrentActorFacts) ||
      out_facts->version != kActorServiceVersion)
    return out_facts->version != kActorServiceVersion
               ? OBJC3_SERVICE_UNSUPPORTED_VERSION
               : OBJC3_SERVICE_INVALID_ARGUMENT;
  out_facts->actor_identity = current_actor ? current_actor->identity : 0;
  out_facts->executor_identity = executor_current_identity();
  return OBJC3_SERVICE_OK;
}

} // namespace objc3::services
