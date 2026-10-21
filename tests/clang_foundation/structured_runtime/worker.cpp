#define main deterministic_contract_main
#include "contract.c"
#undef main

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace {
struct Gate {
  std::mutex mutex;
  std::condition_variable changed;
  unsigned started = 0;
  bool released = false;
};
struct ActorState {
  objc3_actor_t *actor = nullptr;
  Gate *gate = nullptr;
  std::atomic<unsigned> active{0};
  std::atomic<unsigned> destroyed{0};
  unsigned executed = 0;
};
struct WorkerJob {
  ActorState *state;
  unsigned sequence;
};
void worker_job(void *opaque) {
  auto *job = static_cast<WorkerJob *>(opaque);
  auto &state = *job->state;
  assert(state.active.fetch_add(1) == 0);
  assert(job->sequence == state.executed++);
  objc3_current_actor_facts current{};
  current.struct_size = sizeof(current);
  current.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&current) == OBJC3_SERVICE_OK);
  assert(current.actor_identity == objc3_actor_identity(state.actor));
  assert(current.executor_identity ==
         objc3_actor_executor_identity(state.actor));
  assert(objc3_actor_join(state.actor) == OBJC3_SERVICE_NOT_READY);
  if (job->sequence == 0) {
    std::unique_lock lock(state.gate->mutex);
    ++state.gate->started;
    state.gate->changed.notify_all();
    assert(state.gate->changed.wait_for(lock, std::chrono::seconds(5),
                                        [&] { return state.gate->released; }));
  }
  assert(state.active.fetch_sub(1) == 1);
}
void destroy_worker_job(void *opaque) {
  auto *job = static_cast<WorkerJob *>(opaque);
  assert(job->state->active.load() == 0);
  ++job->state->destroyed;
  delete job;
}
void worker_actors(objc3_executor_t *first, objc3_executor_t *second) {
  Gate gate;
  ActorState states[2];
  for (unsigned index = 0; index != 2; ++index) {
    states[index].gate = &gate;
    assert(objc3_actor_create(index == 0 ? first : second,
                              &states[index].actor) == OBJC3_SERVICE_OK);
    for (unsigned sequence = 0; sequence != 65; ++sequence) {
      objc3_actor_hop_facts facts{};
      facts.struct_size = sizeof(facts);
      facts.version = OBJC3_ACTOR_SERVICE_VERSION;
      auto *job = new WorkerJob{&states[index], sequence};
      assert(objc3_actor_enqueue(states[index].actor, worker_job, job,
                                 destroy_worker_job,
                                 &facts) == OBJC3_SERVICE_OK);
      assert(facts.actor_hop && facts.executor_hop);
    }
  }
  {
    std::unique_lock lock(gate.mutex);
    assert(gate.changed.wait_for(lock, std::chrono::seconds(5),
                                 [&] { return gate.started == 2; }));
    assert(states[0].active == 1 && states[1].active == 1);
    gate.released = true;
    gate.changed.notify_all();
  }
  for (auto &state : states) {
    assert(objc3_actor_close(state.actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(state.actor) == OBJC3_SERVICE_OK);
    assert(state.executed == 65 && state.destroyed == 65 && state.active == 0);
    objc3_actor_release(state.actor);
  }
}
struct TaskState {
  std::atomic<unsigned> completed{0};
  std::atomic<unsigned> destroyed{0};
};
struct WorkerFrame {
  TaskState *state;
};
void worker_completed(void *opaque, const objc3_completion *completion) {
  assert(completion->kind == OBJC3_COMPLETION_SUCCESS &&
         !completion->has_payload);
  ++static_cast<TaskState *>(opaque)->completed;
}
void worker_resume(objc3_task_t *task, void *) {
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
void destroy_worker_frame(void *opaque) {
  auto *frame = static_cast<WorkerFrame *>(opaque);
  ++frame->state->destroyed;
  delete frame;
}
void worker_groups(objc3_executor_t *first, objc3_executor_t *second) {
  TaskState state;
  objc3_task_group_t *group = nullptr;
  assert(objc3_task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::vector<objc3_task_t *> tasks;
  for (unsigned index = 0; index != 128; ++index) {
    objc3_task_start_options options{};
    options.struct_size = sizeof(options);
    options.version = OBJC3_SERVICE_ABI_VERSION;
    options.executor = index % 2 ? first : second;
    options.frame = new WorkerFrame{&state};
    options.resume = worker_resume;
    options.destroy = destroy_worker_frame;
    options.completion = worker_completed;
    options.completion_context = &state;
    objc3_task_t *task = nullptr;
    assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_add(group, task) == OBJC3_SERVICE_OK);
    tasks.push_back(task);
  }
  assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  assert(state.completed == 128 && state.destroyed == 128);
  unsigned drained = 0;
  objc3_task_t *next = nullptr;
  objc3_service_status result;
  while ((result = objc3_task_group_next(group, &next)) == OBJC3_SERVICE_OK) {
    ++drained;
    objc3_task_release(next);
  }
  assert(result == OBJC3_SERVICE_CLOSED && !next && drained == 128);
  for (auto *task : tasks)
    objc3_task_release(task);
  objc3_task_group_release(group);
}
} // namespace
int main() {
  assert(deterministic_contract_main() == 0);
  objc3_executor_t *first = nullptr, *second = nullptr;
  assert(objc3_executor_create(0, &first) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(0, &second) == OBJC3_SERVICE_OK);
  worker_actors(first, second);
  worker_groups(first, second);
  objc3_executor_release(first);
  objc3_executor_release(second);
}
