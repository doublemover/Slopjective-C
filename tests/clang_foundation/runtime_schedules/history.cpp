#include "objc3_runtime_services.h"

#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#ifndef SCHEDULE_MUTANT
#define SCHEDULE_MUTANT 0
#endif

namespace {
struct Task {
  objc3_task_t *handle = nullptr;
  unsigned references = 0;
  std::array<std::atomic<unsigned>, 5> counts{};
};
struct Job {
  std::array<std::atomic<unsigned>, 3> counts{};
};
struct Gate {
  std::mutex mutex;
  std::condition_variable changed;
  bool entered = false, open = false, barrier = false;
};
std::array<Task, 8> tasks;
std::array<Job, 8> jobs;
std::atomic<unsigned> active{0}, overlap{0};

void resume(objc3_task_t *task, void *context) {
  auto &state = *static_cast<Task *>(context);
  ++state.counts[0];
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = objc3_task_is_cancel_requested(task)
                        ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}
void destroy_frame(void *context) {
  if constexpr (SCHEDULE_MUTANT != 1)
    ++static_cast<Task *>(context)->counts[1];
}
void complete(void *context, const objc3_completion *completion) {
  auto &state = *static_cast<Task *>(context);
  ++state.counts[2];
  state.counts[4] += completion->kind == OBJC3_COMPLETION_CANCELLED;
}
void waiter(void *context, const objc3_completion *) {
  static_cast<Task *>(context)->counts[3] += SCHEDULE_MUTANT == 2 ? 2 : 1;
}
void actor_job(void *context) {
  if (active.fetch_add(1) != 0) ++overlap;
  ++static_cast<Job *>(context)->counts[0];
  assert(active.fetch_sub(1) == 1);
}
void destroy_job(void *context) {
  ++static_cast<Job *>(context)->counts[1];
}
void gate_callback(void *context) {
  auto &gate = *static_cast<Gate *>(context);
  std::unique_lock lock(gate.mutex);
  gate.entered = true;gate.changed.notify_all();
  assert(gate.changed.wait_for(lock, std::chrono::seconds(10), [&] { return gate.open; }));
}
void barrier_callback(void *context) {
  auto &gate = *static_cast<Gate *>(context);
  std::lock_guard lock(gate.mutex);
  gate.barrier = true;gate.changed.notify_all();
}
void trace(const std::string &operation, unsigned id, int status, int output) {
  std::printf("{\"operation\":\"%s\",\"id\":%u,\"status\":%d,\"output\":%d,\"tasks\":[",
      operation.c_str(), id, status, output);
  for (unsigned i = 0; i != tasks.size(); ++i) {
    std::printf("%s[", i ? "," : "");
    for (unsigned j = 0; j != tasks[i].counts.size(); ++j)
      std::printf("%s%u", j ? "," : "", tasks[i].counts[j].load());
    std::printf("]");
  }
  std::printf("],\"jobs\":[");
  for (unsigned i = 0; i != jobs.size(); ++i) {
    std::printf("%s[", i ? "," : "");
    for (unsigned j = 0; j != jobs[i].counts.size(); ++j)
      std::printf("%s%u", j ? "," : "", jobs[i].counts[j].load());
    std::printf("]");
  }
  std::printf("],\"actor_overlap\":%u}\n",
              overlap.load() + (SCHEDULE_MUTANT == 4 ? 1 : 0));
  std::fflush(stdout);
}
} // namespace

int main(int argc, char **argv) {
  assert(argc == 3);
  const bool manual = std::string(argv[1]) == "manual";
  assert(manual || std::string(argv[1]) == "worker");
  objc3_executor_t *executor = nullptr;
  objc3_actor_t *actor = nullptr;
  objc3_task_group_t *group = nullptr;
  assert(objc3_executor_create(manual, &executor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  Gate gate;
  if (!manual) {
    assert(objc3_executor_enqueue(executor, gate_callback, &gate) == OBJC3_SERVICE_OK);
    std::unique_lock lock(gate.mutex);
    assert(gate.changed.wait_for(lock, std::chrono::seconds(10), [&] { return gate.entered; }));
  }
  const std::array<const char *, 15> operations = {"task", "waiter", "retain-task", "release-task", "add", "close-group",
      "cancel-group", "actor-job", "close-actor", "cancel-actor", "drain", "join-group", "join-actor", "next", "observe"};
  std::ifstream input(argv[2]);assert(input);
  char delimiter = 0;
  input >> delimiter;assert(delimiter == '[');
  unsigned id = 0, count = 0;
  bool drained = false;
  for (;;) {
    input >> delimiter;
    if (delimiter == ']') break;
    assert(delimiter == '[');
    unsigned opcode = 0;input >> opcode >> delimiter;assert(delimiter == ',');
    input >> id >> delimiter;assert(delimiter == ']' && opcode < operations.size());
    const std::string operation = operations[opcode];
    assert(id < 8 && ++count <= 96);
    auto &state = tasks[id];
    int status = OBJC3_SERVICE_OK, output = -1;
    if (operation == "task") {
      assert(!state.handle && !drained);
      objc3_task_start_options options{};
      options.struct_size = sizeof(options);options.version = OBJC3_SERVICE_ABI_VERSION;
      options.executor = executor;options.frame = &state;
      options.resume = resume;options.destroy = destroy_frame;
      options.completion = complete;options.completion_context = &state;
      status = objc3_task_start(&options, &state.handle);
      assert(status == OBJC3_SERVICE_OK);state.references = 1;
    } else if (operation == "waiter") {
      assert(state.handle);status = objc3_task_add_waiter(state.handle, waiter, &state);
    } else if (operation == "retain-task") {
      assert(state.handle && objc3_task_retain(state.handle) == state.handle);++state.references;
    } else if (operation == "release-task") {
      assert(state.references > 1);objc3_task_release(state.handle);--state.references;
    } else if (operation == "add") {
      assert(state.handle);status = objc3_task_group_add(group, state.handle);
    } else if (operation == "close-group") status = objc3_task_group_close(group);
    else if (operation == "cancel-group") status = objc3_task_group_cancel(group);
    else if (operation == "close-actor") status = objc3_actor_close(actor);
    else if (operation == "cancel-actor") status = objc3_actor_cancel(actor);
    else if (operation == "actor-job") {
      objc3_actor_hop_facts facts{};
      facts.struct_size = sizeof(facts);facts.version = OBJC3_ACTOR_SERVICE_VERSION;
      facts.actor_identity = 0xfeed;
      status = objc3_actor_enqueue(actor, actor_job, &jobs[id], destroy_job, &facts);
      if (status != OBJC3_SERVICE_OK) {
        assert(facts.actor_identity == 0xfeed);
        ++jobs[id].counts[2];destroy_job(&jobs[id]);
      }
    } else if (operation == "drain") {
      assert(!drained);drained = true;
      if (manual) {
        unsigned total = 0;
        for (;;) {
          uint32_t executed = 0;
          assert(objc3_executor_pump(executor, 64, &executed) == OBJC3_SERVICE_OK);
          total += executed;assert(total <= 64);
          if (!executed) break;
        }
      } else {
        assert(objc3_executor_enqueue(executor, barrier_callback, &gate) == OBJC3_SERVICE_OK);
        std::unique_lock lock(gate.mutex);gate.open = true;gate.changed.notify_all();
        assert(gate.changed.wait_for(lock, std::chrono::seconds(10), [&] { return gate.barrier; }));
      }
    } else if (operation == "join-group") {
      assert(drained);status = objc3_task_group_join(group);
    } else if (operation == "join-actor") {
      assert(drained);status = objc3_actor_join(actor);
    } else if (operation == "next") {
      assert(drained);
      objc3_task_t *selected = nullptr;
      status = objc3_task_group_next(group, &selected);
      if (selected) {
        for (unsigned i = 0; i != tasks.size(); ++i)
          if (tasks[i].handle == selected) output = i;
        assert(output >= 0);objc3_task_release(selected);
      } else assert(status != OBJC3_SERVICE_OK);
    } else if (operation == "observe") {
      assert(state.handle);
      objc3_completion view{};view.struct_size = sizeof(view);view.version = OBJC3_SERVICE_ABI_VERSION;
      status = objc3_task_get_completion(state.handle, &view);
    } else assert(false && "unmodeled schedule operation");
    if constexpr (SCHEDULE_MUTANT == 3) {
      if (operation == "drain")
        for (auto &task : tasks) task.counts[1] = 0;
    }
    if constexpr (SCHEDULE_MUTANT == 5) {
      static int first_next = -1;
      if (operation == "next" && output >= 0) {
        if (first_next < 0) first_next = output;
        else output = first_next;
      }
    }
    trace(operation, id, status, output);
    input >> delimiter;
    if (delimiter == ']') break;
    assert(delimiter == ',');
  }
  assert(drained && count > 0);
  assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  objc3_task_group_release(group);objc3_actor_release(actor);
  for (auto &state : tasks) {
    if (state.handle) { assert(state.references == 1);objc3_task_release(state.handle); }
  }
  objc3_executor_release(executor);
}
