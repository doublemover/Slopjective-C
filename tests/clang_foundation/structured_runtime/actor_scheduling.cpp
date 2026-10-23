#include "objc3_runtime_services.h"
#include "runtime/concurrency/service_internal.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>

// Counter-only hooks keep this executable independent of libobjc. The installed
// native-async fixture separately checks real GNUstep object deallocation.
namespace {
thread_local uintptr_t pool_depth = 0;
thread_local unsigned pool_pushes = 0, pool_pops = 0;
}
extern "C" void *objc_autoreleasePoolPush() {
  ++pool_pushes;
  return reinterpret_cast<void *>(++pool_depth);
}
extern "C" void objc_autoreleasePoolPop(void *token) {
  assert(reinterpret_cast<uintptr_t>(token) == pool_depth);
  --pool_depth;
  ++pool_pops;
}

namespace {
struct Probe {
  objc3_executor_t *executor{};
  objc3_actor_t *actor{};
  std::atomic<unsigned> resumes{0}, destroyed{0}, callbacks{0};
  std::atomic<bool> witnessed{false};
  unsigned previous_pushes = 0;
  bool fairness = false, saturate = false;
};

void count_callback(void *opaque) {
  ++static_cast<Probe *>(opaque)->callbacks;
}
void witness(void *opaque) {
  auto &probe = *static_cast<Probe *>(opaque);
  assert(probe.resumes > 0 && probe.resumes < 256);
  probe.witnessed = true;
}
void resume(objc3_task_t *task, void *opaque) {
  auto &probe = *static_cast<Probe *>(opaque);
  // All pools enclosing the previous resume must have popped before this one.
  assert(pool_depth > 0);
  assert(pool_pops >= probe.previous_pushes);
  probe.previous_pushes = pool_pushes;
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == objc3_actor_identity(probe.actor));
  const unsigned count = ++probe.resumes;
  if (probe.saturate && count == 1) {
    for (unsigned i = 0; i != 1024; ++i)
      assert(objc3_executor_enqueue(probe.executor, count_callback, &probe) ==
             OBJC3_SERVICE_OK);
    assert(objc3_executor_enqueue(probe.executor, count_callback, &probe) ==
           OBJC3_SERVICE_NOT_READY);
  }
  const bool again = probe.saturate ? count == 1
                     : probe.fairness ? !probe.witnessed.load() : count < 5;
  if (again) {
    assert(count < 256); // Fail boundedly if this actor starves its witness.
    assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
    return;
  }
  if (probe.saturate) assert(probe.callbacks == 1024);
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
void destroy(void *opaque) { ++static_cast<Probe *>(opaque)->destroyed; }

struct Gate {
  std::mutex mutex;
  std::condition_variable changed;
  bool entered = false, open = false;
};
void gate_callback(void *opaque) {
  auto &gate = *static_cast<Gate *>(opaque);
  std::unique_lock lock(gate.mutex);
  gate.entered = true;
  gate.changed.notify_all();
  assert(gate.changed.wait_for(lock, std::chrono::seconds(5),
                               [&] { return gate.open; }));
}

void exercise(bool manual, bool actor, bool fairness, bool actor_witness,
              bool saturate = false) {
  Probe probe;
  probe.fairness = fairness;
  probe.saturate = saturate;
  assert(objc3_executor_create(manual, &probe.executor) == OBJC3_SERVICE_OK);
  if (actor)
    assert(objc3_actor_create(probe.executor, &probe.actor) == OBJC3_SERVICE_OK);
  Gate gate;
  if (!manual) {
    assert(objc3_executor_enqueue(probe.executor, gate_callback, &gate) ==
           OBJC3_SERVICE_OK);
    std::unique_lock lock(gate.mutex);
    assert(gate.changed.wait_for(lock, std::chrono::seconds(5),
                                 [&] { return gate.entered; }));
  }
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = actor ? nullptr : probe.executor;
  options.actor = probe.actor;
  options.frame = &probe;
  options.resume = resume;
  options.destroy = destroy;
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  objc3_actor_t *other = nullptr;
  if (fairness) {
    if (actor_witness) {
      assert(objc3_actor_create(probe.executor, &other) == OBJC3_SERVICE_OK);
      objc3_actor_hop_facts facts{};
      facts.struct_size = sizeof(facts);
      facts.version = OBJC3_ACTOR_SERVICE_VERSION;
      assert(objc3_actor_enqueue(other, witness, &probe, nullptr, &facts) ==
             OBJC3_SERVICE_OK);
    } else {
      assert(objc3_executor_enqueue(probe.executor, witness, &probe) ==
             OBJC3_SERVICE_OK);
    }
  }
  if (!manual) {
    std::lock_guard lock(gate.mutex);
    gate.open = true;
    gate.changed.notify_all();
  } else {
    uint32_t executed = 0;
    assert(objc3_executor_pump(probe.executor, 2048, &executed) == OBJC3_SERVICE_OK);
    assert(executed > 0 && executed < 2048);
  }
  assert(objc3_task_wait_quiescent(task) == OBJC3_SERVICE_OK);
  assert(probe.destroyed == 1);
  if (fairness) assert(probe.witnessed && probe.resumes == 2);
  if (probe.actor) {
    assert(objc3_actor_close(probe.actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(probe.actor) == OBJC3_SERVICE_OK);
    objc3_actor_release(probe.actor);
  }
  if (other) {
    assert(objc3_actor_close(other) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(other) == OBJC3_SERVICE_OK);
    objc3_actor_release(other);
  }
  objc3_task_release(task);
  objc3_executor_release(probe.executor);
}

struct CancellationRace {
  Gate registration;
  objc3_task_t *child{};
  std::atomic<unsigned> child_resumes{0}, child_destroyed{0};
  std::atomic<unsigned> parent_destroyed{0};
  std::atomic<bool> inherited{false};
};
void complete_cancelled(objc3_task_t *task) {
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_CANCELLED;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
void race_child_resume(objc3_task_t *task, void *opaque) {
  auto &race = *static_cast<CancellationRace *>(opaque);
  ++race.child_resumes;
  // This check runs on the worker, immediately at the child's first entry.
  race.inherited = objc3_task_is_cancel_requested(task);
  assert(race.inherited);
  complete_cancelled(task);
}
void race_child_destroy(void *opaque) {
  ++static_cast<CancellationRace *>(opaque)->child_destroyed;
}
void race_parent_destroy(void *opaque) {
  ++static_cast<CancellationRace *>(opaque)->parent_destroyed;
}
void race_parent_resume(objc3_task_t *task, void *opaque) {
  auto &race = *static_cast<CancellationRace *>(opaque);
  gate_callback(&race.registration);
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.frame = &race;
  options.resume = race_child_resume;
  options.destroy = race_child_destroy;
  assert(objc3_task_start_child(task, &options, &race.child) == OBJC3_SERVICE_OK);
  complete_cancelled(task);
}
void child_registration_after_worker_cancellation() {
  CancellationRace race;
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(0, &executor) == OBJC3_SERVICE_OK);
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.frame = &race;
  options.resume = race_parent_resume;
  options.destroy = race_parent_destroy;
  objc3_task_t *parent = nullptr;
  assert(objc3_task_start(&options, &parent) == OBJC3_SERVICE_OK);
  {
    std::unique_lock lock(race.registration.mutex);
    assert(race.registration.changed.wait_for(lock, std::chrono::seconds(5),
                                               [&] {
      return race.registration.entered;
    }));
    objc3_task_request_cancel(parent);
    race.registration.open = true;
    race.registration.changed.notify_all();
  }
  assert(objc3_task_wait_quiescent(parent) == OBJC3_SERVICE_OK);
  assert(race.child);
  assert(objc3_task_wait_quiescent(race.child) == OBJC3_SERVICE_OK);
  assert(race.inherited && race.child_resumes == 1);
  assert(race.child_destroyed == 1 && race.parent_destroyed == 1);
  objc3_task_release(race.child);
  objc3_task_release(parent);
  objc3_executor_release(executor);
}

enum class ContinuationLoss { ActorDiscard, ClosedHop, ThrowingResume };
struct LostContinuation {
  objc3_executor_t *other{};
  objc3_actor_t *closed_destination{};
  objc3_task_t *dependency{}, *child{};
  unsigned parent_resumes{0}, child_resumes{0};
  unsigned parent_destroyed{0}, child_destroyed{0};
  ContinuationLoss loss{};
};
void lost_dependency_resume(objc3_task_t *, void *) {}
void lost_child_destroy(void *opaque) {
  ++static_cast<LostContinuation *>(opaque)->child_destroyed;
}
void lost_parent_destroy(void *opaque) {
  ++static_cast<LostContinuation *>(opaque)->parent_destroyed;
}
void lost_child_resume(objc3_task_t *task, void *opaque) {
  auto &state = *static_cast<LostContinuation *>(opaque);
  if (state.child_resumes++ == 0) {
    assert(objc3_task_hop(task, state.other, nullptr) == OBJC3_SERVICE_OK);
  } else if (!objc3_task_is_cancel_requested(task)) {
    assert(objc3_task_await(task, state.dependency) == OBJC3_SERVICE_OK);
  } else {
    complete_cancelled(task);
  }
}
void lost_parent_resume(objc3_task_t *task, void *opaque) {
  auto &state = *static_cast<LostContinuation *>(opaque);
  if (state.parent_resumes++ == 0) {
    objc3_task_start_options options{};
    options.struct_size = sizeof(options);
    options.version = OBJC3_SERVICE_ABI_VERSION;
    options.frame = &state;
    options.resume = lost_child_resume;
    options.destroy = lost_child_destroy;
    assert(objc3_task_start_child(task, &options, &state.child) == OBJC3_SERVICE_OK);
    assert(objc3_task_await(task, state.dependency) == OBJC3_SERVICE_OK);
    return;
  }
  if (state.loss == ContinuationLoss::ThrowingResume) throw 17;
  assert(state.loss == ContinuationLoss::ClosedHop);
  assert(objc3_task_hop(task, nullptr, state.closed_destination) ==
         OBJC3_SERVICE_OK);
}
void lost_continuation_cancels_descendants(ContinuationLoss loss) {
  LostContinuation state;
  state.loss = loss;
  objc3_executor_t *executor = nullptr;
  objc3_actor_t *actor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &state.other) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(executor, &state.closed_destination) ==
         OBJC3_SERVICE_OK);
  assert(objc3_actor_close(state.closed_destination) == OBJC3_SERVICE_OK);
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = executor;
  options.resume = lost_dependency_resume;
  assert(objc3_task_start(&options, &state.dependency) == OBJC3_SERVICE_OK);
  options.executor = nullptr;
  options.actor = actor;
  options.frame = &state;
  options.resume = lost_parent_resume;
  options.destroy = lost_parent_destroy;
  objc3_task_t *parent = nullptr;
  assert(objc3_task_start(&options, &parent) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, 16, &executed) == OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(state.other, 16, &executed) == OBJC3_SERVICE_OK);
  assert(state.parent_resumes == 1 && state.child_resumes == 2);
  assert(objc3_task_schedule_resume(parent) == OBJC3_SERVICE_OK);
  if (loss == ContinuationLoss::ActorDiscard)
    assert(objc3_actor_cancel(actor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(executor, 16, &executed) == OBJC3_SERVICE_OK);
  objc3_completion completion{};
  assert(objc3_task_get_completion(parent, &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
  assert(objc3_task_is_cancel_requested(parent));
  assert(objc3_task_is_cancel_requested(state.child));
  assert(state.parent_destroyed == 1);
  assert(objc3_executor_pump(state.other, 16, &executed) == OBJC3_SERVICE_OK);
  assert(objc3_task_wait_quiescent(state.child) == OBJC3_SERVICE_OK);
  assert(state.child_destroyed == 1 && state.child_resumes == 3);
  completion = {};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  assert(objc3_task_complete(state.dependency, &completion) == OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(executor, 16, &executed) == OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(state.other, 16, &executed) == OBJC3_SERVICE_OK);
  assert(state.parent_destroyed == 1 && state.child_destroyed == 1);
  objc3_task_release(parent);
  objc3_task_release(state.child);
  objc3_task_release(state.dependency);
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(state.closed_destination) == OBJC3_SERVICE_OK);
  objc3_actor_release(state.closed_destination);
  objc3_actor_release(actor);
  objc3_executor_release(state.other);
  objc3_executor_release(executor);
}
} // namespace

void test_actor_scheduling() {
  for (bool manual : {false, true}) {
    for (bool actor : {false, true}) exercise(manual, actor, false, false);
    for (bool actor_witness : {false, true})
      exercise(manual, true, true, actor_witness);
  }
  exercise(true, true, false, false, true);
  child_registration_after_worker_cancellation();
  for (auto loss : {ContinuationLoss::ActorDiscard, ContinuationLoss::ClosedHop,
                    ContinuationLoss::ThrowingResume})
    lost_continuation_cancels_descendants(loss);
  assert(pool_depth == 0 && pool_pushes == pool_pops);
}
