#include "objc3_runtime_services.h"
#include "runtime/concurrency/service_internal.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

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
  unsigned entrants = 0;
};
void gate_callback(void *opaque) {
  auto &gate = *static_cast<Gate *>(opaque);
  std::unique_lock lock(gate.mutex);
  gate.entered = true;
  ++gate.entrants;
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
  objc3_task_t *dependency{}, *parent_dependency{}, *child{};
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
    assert(objc3_task_await(task, state.parent_dependency) == OBJC3_SERVICE_OK);
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
  assert(objc3_task_start(&options, &state.parent_dependency) == OBJC3_SERVICE_OK);
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
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  assert(objc3_task_complete(state.parent_dependency, &completion) == OBJC3_SERVICE_OK);
  if (loss == ContinuationLoss::ActorDiscard)
    assert(objc3_actor_cancel(actor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(executor, 16, &executed) == OBJC3_SERVICE_OK);
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
  objc3_task_release(state.parent_dependency);
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(state.closed_destination) == OBJC3_SERVICE_OK);
  objc3_actor_release(state.closed_destination);
  objc3_actor_release(actor);
  objc3_executor_release(state.other);
  objc3_executor_release(executor);
}

struct DefaultExecutorProbe {
  std::mutex mutex;
  std::condition_variable changed;
  std::thread::id main_thread;
  uint64_t expected_identity{};
  objc3_executor_t *executor{};
  bool expect_main{false}, ran{false};
};
void default_executor_callback(void *opaque) {
  auto &probe = *static_cast<DefaultExecutorProbe *>(opaque);
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == 0 && facts.executor_identity == probe.expected_identity);
  assert(objc3_executor_identity(probe.executor) == probe.expected_identity);
  assert(objc3_executor_is_current(probe.executor));
  assert((std::this_thread::get_id() == probe.main_thread) == probe.expect_main);
  std::lock_guard lock(probe.mutex);
  probe.ran = true;
  probe.changed.notify_all();
}
void canonical_executor_defaults() {
  assert(objc3_executor_get_main(nullptr) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_executor_get_global(nullptr) == OBJC3_SERVICE_INVALID_ARGUMENT);
  objc3_executor_t *main = nullptr;
  // First access may occur on a worker, but that cannot redefine main affinity.
  std::thread initialize([&] {
    assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  });
  initialize.join();
  objc3_executor_t *again = nullptr;
  assert(objc3_executor_get_main(&again) == OBJC3_SERVICE_OK && again == main);
  DefaultExecutorProbe main_probe;
  main_probe.main_thread = std::this_thread::get_id();
  main_probe.expected_identity = main->identity;
  main_probe.executor = main;
  main_probe.expect_main = true;
  assert(objc3_executor_enqueue(main, default_executor_callback, &main_probe) ==
         OBJC3_SERVICE_OK);
  std::thread wrong_thread([&] {
    uint32_t executed = 19;
    assert(objc3_executor_pump(main, 1, &executed) == OBJC3_SERVICE_NOT_READY);
    assert(executed == 0);
  });
  wrong_thread.join();
  assert(!main_probe.ran);
  uint32_t executed = 0;
  assert(objc3_executor_pump(main, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && main_probe.ran);
  const auto main_identity = main->identity;
  objc3_executor_release(main);
  objc3_executor_release(again);
  assert(objc3_executor_get_main(&main) == OBJC3_SERVICE_OK);
  assert(main->identity == main_identity);
  objc3_executor_release(main);

  objc3_executor_t *global = nullptr;
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  assert(objc3_executor_get_global(&again) == OBJC3_SERVICE_OK && again == global);
  assert(global->identity != main_identity);
  Gate gate;
  // All four workers can enter independent callbacks concurrently. Keep all
  // of them occupied while the queued ownership witness releases its clients.
  for (unsigned index = 0; index != 4; ++index)
    assert(objc3_executor_enqueue(global, gate_callback, &gate) == OBJC3_SERVICE_OK);
  {
    std::unique_lock lock(gate.mutex);
    assert(gate.changed.wait_for(lock, std::chrono::seconds(5),
                                 [&] { return gate.entrants == 4; }));
  }
  DefaultExecutorProbe global_probe;
  global_probe.main_thread = std::this_thread::get_id();
  global_probe.expected_identity = global->identity;
  global_probe.executor = global;
  assert(objc3_executor_enqueue(global, default_executor_callback, &global_probe) ==
         OBJC3_SERVICE_OK);
  objc3_executor_release(global);
  objc3_executor_release(again);
  // The canonical owner keeps pending work alive when all clients release.
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  assert(global->identity == global_probe.expected_identity);
  {
    std::lock_guard lock(gate.mutex);
    gate.open = true;
    gate.changed.notify_all();
  }
  {
    std::unique_lock lock(global_probe.mutex);
    assert(global_probe.changed.wait_for(lock, std::chrono::seconds(5),
                                         [&] { return global_probe.ran; }));
  }
  objc3_executor_release(global);
}

struct ActorJoinProbe {
  objc3_actor_t *actor{};
  objc3_service_status status{OBJC3_SERVICE_CLOSED};
};
void cross_executor_actor_join(void *opaque) {
  auto &probe = *static_cast<ActorJoinProbe *>(opaque);
  probe.status = objc3_actor_join(probe.actor);
}
void empty_actor_job(void *) {}
void actor_join_does_not_block_another_executor() {
  objc3_executor_t *worker = nullptr, *manual = nullptr;
  assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &manual) == OBJC3_SERVICE_OK);
  Gate gate;
  assert(objc3_executor_enqueue(worker, gate_callback, &gate) == OBJC3_SERVICE_OK);
  {
    std::unique_lock lock(gate.mutex);
    assert(gate.changed.wait_for(lock, std::chrono::seconds(5),
                                 [&] { return gate.entered; }));
  }
  ActorJoinProbe probe;
  assert(objc3_actor_create(worker, &probe.actor) == OBJC3_SERVICE_OK);
  objc3_actor_hop_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_enqueue(probe.actor, empty_actor_job, nullptr, nullptr,
                              &facts) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(probe.actor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_enqueue(manual, cross_executor_actor_join, &probe) ==
         OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(manual, 1, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && probe.status == OBJC3_SERVICE_NOT_READY);
  {
    std::lock_guard lock(gate.mutex);
    gate.open = true;
    gate.changed.notify_all();
  }
  assert(objc3_actor_join(probe.actor) == OBJC3_SERVICE_OK);
  objc3_actor_release(probe.actor);
  objc3_executor_release(manual);
  objc3_executor_release(worker);
}

struct GlobalActorBarrier {
  std::mutex mutex;
  std::condition_variable changed;
  unsigned entered{0};
};
struct GlobalActorProbe {
  GlobalActorBarrier *barrier{};
  objc3_actor_t *actor{};
  std::atomic<unsigned> active{0}, ran{0}, cleaned{0};
};
void global_actor_job(void *opaque) {
  auto &probe = *static_cast<GlobalActorProbe *>(opaque);
  assert(probe.active.fetch_add(1) == 0);
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == objc3_actor_identity(probe.actor));
  if (probe.ran++ == 0) {
    std::unique_lock lock(probe.barrier->mutex);
    ++probe.barrier->entered;
    probe.barrier->changed.notify_all();
    // Two distinct actors can make progress simultaneously on global, while
    // repeated jobs belonging to either actor must still exclude each other.
    assert(probe.barrier->changed.wait_for(lock, std::chrono::seconds(5), [&] {
      return probe.barrier->entered == 2;
    }));
  }
  std::this_thread::yield();
  assert(probe.active.fetch_sub(1) == 1);
}
void global_actor_cleanup(void *opaque) {
  auto &probe = *static_cast<GlobalActorProbe *>(opaque);
  assert(probe.active == 0);
  ++probe.cleaned;
}
struct GlobalTaskProbe {
  std::atomic<unsigned> active{0}, resumes{0}, destroyed{0};
};
void global_task_resume(objc3_task_t *task, void *opaque) {
  auto &probe = *static_cast<GlobalTaskProbe *>(opaque);
  assert(probe.active.fetch_add(1) == 0);
  if (++probe.resumes != 64) {
    assert(objc3_task_schedule_resume(task) == OBJC3_SERVICE_OK);
    for (unsigned index = 0; index != 32; ++index) std::this_thread::yield();
  } else {
    objc3_completion completion{};
    completion.struct_size = sizeof(completion);
    completion.version = OBJC3_SERVICE_ABI_VERSION;
    completion.kind = OBJC3_COMPLETION_SUCCESS;
    assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
    assert(probe.destroyed == 0);
  }
  assert(probe.active.fetch_sub(1) == 1);
}
void global_task_destroy(void *opaque) {
  auto &probe = *static_cast<GlobalTaskProbe *>(opaque);
  assert(probe.active == 0);
  ++probe.destroyed;
}
void global_pool_preserves_actor_and_task_exclusion() {
  objc3_executor_t *global = nullptr;
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  GlobalActorBarrier barrier;
  GlobalActorProbe actors[2];
  for (auto &probe : actors) {
    probe.barrier = &barrier;
    assert(objc3_actor_create(global, &probe.actor) == OBJC3_SERVICE_OK);
    objc3_actor_hop_facts facts{};
    facts.struct_size = sizeof(facts);
    facts.version = OBJC3_ACTOR_SERVICE_VERSION;
    for (unsigned index = 0; index != 64; ++index)
      assert(objc3_actor_enqueue(probe.actor, global_actor_job, &probe,
                                  global_actor_cleanup, &facts) == OBJC3_SERVICE_OK);
    assert(objc3_actor_close(probe.actor) == OBJC3_SERVICE_OK);
  }
  GlobalTaskProbe task_probe;
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = global;
  options.frame = &task_probe;
  options.resume = global_task_resume;
  options.destroy = global_task_destroy;
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  assert(objc3_task_wait_quiescent(task) == OBJC3_SERVICE_OK);
  assert(task_probe.resumes == 64 && task_probe.destroyed == 1);
  for (auto &probe : actors) {
    assert(objc3_actor_join(probe.actor) == OBJC3_SERVICE_OK);
    assert(probe.ran == 64 && probe.cleaned == 64 && probe.active == 0);
    objc3_actor_release(probe.actor);
  }
  objc3_task_release(task);
  objc3_executor_release(global);
}

void named_executor_identity_and_registration_races() {
  objc3_executor_t *first = nullptr, *second = nullptr, *found = nullptr;
  assert(objc3_executor_create(1, &first) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(1, &second) == OBJC3_SERVICE_OK);
  assert(objc3_executor_register_named(nullptr, first) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_executor_register_named("", first) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_executor_get_named("fixture.not.registered", &found) ==
         OBJC3_SERVICE_NOT_READY && !found);
  char name[] = "fixture.named.copy";
  assert(objc3_executor_register_named(name, first) == OBJC3_SERVICE_OK);
  name[0] = 'X';
  assert(objc3_executor_get_named("fixture.named.copy", &found) == OBJC3_SERVICE_OK);
  assert(found == first);
  objc3_executor_release(found);
  assert(objc3_executor_get_named(name, &found) == OBJC3_SERVICE_NOT_READY && !found);
  assert(objc3_executor_register_named("fixture.named.copy", first) == OBJC3_SERVICE_OK);
  assert(objc3_executor_register_named("fixture.named.copy", second) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_executor_get_named("Fixture.named.copy", &found) ==
         OBJC3_SERVICE_NOT_READY && !found);
  const auto identity = objc3_executor_identity(first);
  objc3_executor_release(first); // Registry alone now owns this destination.
  assert(objc3_executor_get_named("fixture.named.copy", &found) == OBJC3_SERVICE_OK);
  assert(objc3_executor_identity(found) == identity);
  assert(objc3_executor_unregister_named("fixture.named.copy") == OBJC3_SERVICE_OK);
  assert(objc3_executor_unregister_named("fixture.named.copy") == OBJC3_SERVICE_OK);
  assert(objc3_executor_register_named("fixture.named.copy", second) == OBJC3_SERVICE_CLOSED);
  assert(objc3_executor_identity(found) == identity); // Existing client survives.
  objc3_executor_release(found);

  objc3_executor_t *candidates[8]{};
  objc3_service_status status[8]{};
  std::thread registrations[8];
  for (unsigned index = 0; index != 8; ++index) {
    assert(objc3_executor_create(1, &candidates[index]) == OBJC3_SERVICE_OK);
    registrations[index] = std::thread([&, index] {
      status[index] = objc3_executor_register_named("fixture.named.race", candidates[index]);
    });
  }
  unsigned winners = 0;
  objc3_executor_t *winner = nullptr;
  for (unsigned index = 0; index != 8; ++index) {
    registrations[index].join();
    if (status[index] == OBJC3_SERVICE_OK) {
      ++winners;
      winner = candidates[index];
    } else {
      assert(status[index] == OBJC3_SERVICE_INVALID_ARGUMENT);
    }
  }
  assert(winners == 1);
  assert(objc3_executor_get_named("fixture.named.race", &found) == OBJC3_SERVICE_OK);
  assert(found == winner);
  assert(objc3_executor_unregister_named("fixture.named.race") == OBJC3_SERVICE_OK);
  objc3_executor_release(found);
  for (auto *candidate : candidates) objc3_executor_release(candidate);
  objc3_executor_release(second);
}
struct NamedRetirementProbe {
  std::atomic<bool> queried{false};
};
void query_retired_executor(void *opaque) {
  auto &probe = *static_cast<NamedRetirementProbe *>(opaque);
  objc3_executor_t *found = nullptr;
  assert(objc3_executor_get_named("fixture.named.retirement", &found) ==
         OBJC3_SERVICE_CLOSED && !found);
  probe.queried = true;
}
void named_retirement_releases_executor_outside_registry_lock() {
  objc3_executor_t *worker = nullptr;
  assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
  assert(objc3_executor_register_named("fixture.named.retirement", worker) == OBJC3_SERVICE_OK);
  Gate gate;
  assert(objc3_executor_enqueue(worker, gate_callback, &gate) == OBJC3_SERVICE_OK);
  {
    std::unique_lock lock(gate.mutex);
    assert(gate.changed.wait_for(lock, std::chrono::seconds(5), [&] { return gate.entered; }));
  }
  NamedRetirementProbe probe;
  assert(objc3_executor_enqueue(worker, query_retired_executor, &probe) == OBJC3_SERVICE_OK);
  objc3_executor_release(worker); // The registry holds the last public lease.
  objc3_service_status retired = OBJC3_SERVICE_NOT_READY;
  std::thread retire([&] {
    retired = objc3_executor_unregister_named("fixture.named.retirement");
  });
  // The blocked worker itself keeps this internal record alive. Observe stop
  // publication without retaining a public handle which would alter shutdown.
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  for (;;) {
    bool stopping;
    {
      std::lock_guard lock(worker->mutex);
      stopping = worker->stopping;
    }
    if (stopping) break;
    assert(std::chrono::steady_clock::now() < deadline);
    std::this_thread::yield();
  }
  {
    std::lock_guard lock(gate.mutex);
    gate.open = true;
    gate.changed.notify_all();
  }
  // Retirement joins the draining worker. Its pending callback must be able to
  // reacquire the registry lock before that join can return.
  retire.join();
  assert(retired == OBJC3_SERVICE_OK && probe.queried);
}

void retained_actor_executor_preserves_exact_backing_identity() {
  objc3_executor_t *backing = nullptr, *found = nullptr;
  objc3_actor_t *first = nullptr, *second = nullptr;
  assert(objc3_executor_create(1, &backing) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(backing, &first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(backing, &second) == OBJC3_SERVICE_OK);
  assert(objc3_actor_get_executor(nullptr, &found) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(!found);
  assert(objc3_actor_get_executor(first, nullptr) == OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_actor_identity(first) != objc3_actor_identity(second));
  const auto identity = objc3_executor_identity(backing);
  std::thread queries[8];
  for (unsigned index = 0; index != 8; ++index) {
    queries[index] = std::thread([&, index] {
      auto *actor = index % 2 ? first : second;
      for (unsigned turn = 0; turn != 64; ++turn) {
        objc3_executor_t *lease = nullptr;
        assert(objc3_actor_get_executor(actor, &lease) == OBJC3_SERVICE_OK);
        assert(lease == backing && objc3_executor_identity(lease) == identity);
        assert(objc3_actor_executor_identity(actor) == identity);
        objc3_executor_release(lease);
      }
    });
  }
  for (auto &query : queries) query.join();
  assert(objc3_actor_close(first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_get_executor(first, &found) == OBJC3_SERVICE_OK);
  objc3_executor_release(backing);
  objc3_actor_release(first);
  objc3_actor_release(second);
  // The query's independent lease keeps the destination live after both
  // mailbox lifetimes finish, without merging their distinct actor identities.
  assert(objc3_executor_identity(found) == identity);
  std::atomic<unsigned> callbacks{0};
  assert(objc3_executor_enqueue(found, [](void *opaque) {
    ++*static_cast<std::atomic<unsigned> *>(opaque);
  }, &callbacks) == OBJC3_SERVICE_OK);
  uint32_t executed = 0;
  assert(objc3_executor_pump(found, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  assert(executed == 1 && callbacks == 1);
  objc3_executor_release(found);
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
  canonical_executor_defaults();
  actor_join_does_not_block_another_executor();
  global_pool_preserves_actor_and_task_exclusion();
  named_executor_identity_and_registration_races();
  named_retirement_releases_executor_outside_registry_lock();
  retained_actor_executor_preserves_exact_backing_identity();
  assert(pool_depth == 0 && pool_pushes == pool_pops);
}
