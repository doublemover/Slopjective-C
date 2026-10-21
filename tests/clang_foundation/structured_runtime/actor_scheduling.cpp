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
} // namespace

void test_actor_scheduling() {
  for (bool manual : {false, true}) {
    for (bool actor : {false, true}) exercise(manual, actor, false, false);
    for (bool actor_witness : {false, true})
      exercise(manual, true, true, actor_witness);
  }
  exercise(true, true, false, false, true);
  assert(pool_depth == 0 && pool_pushes == pool_pops);
}
