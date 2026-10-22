#include "objc3_task.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace {
void pump(objc3_executor_t *executor, uint32_t limit = UINT32_MAX) {
  uint32_t executed = 0;
  assert(objc3_executor_pump(executor, limit, &executed) == OBJC3_SERVICE_OK);
}

objc3_executor_t *executor(bool manual = true) {
  objc3_executor_t *result = nullptr;
  assert(objc3_executor_create(manual, &result) == OBJC3_SERVICE_OK);
  return result;
}

void pending(objc3_task_t *task) {
  objc3_completion completion{};
  completion.version = 999;
  assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_NOT_READY);
  assert(completion.version == 999);
}

void complete(objc3_task_t *task,
              objc3_completion_kind kind = OBJC3_COMPLETION_SUCCESS) {
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = kind;
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}

struct Gate {
  std::mutex mutex;
  std::condition_variable changed;
  bool entered = false;
  bool released = false;
  void arrive() {
    std::unique_lock<std::mutex> lock(mutex);
    entered = true;
    changed.notify_all();
    assert(changed.wait_for(lock, std::chrono::seconds(5),
                            [this] { return released; }));
  }
  void wait() {
    std::unique_lock<std::mutex> lock(mutex);
    assert(changed.wait_for(lock, std::chrono::seconds(5),
                            [this] { return entered; }));
  }
  void release() {
    std::lock_guard<std::mutex> lock(mutex);
    released = true;
    changed.notify_all();
  }
};

struct HeldFrame {
  std::atomic<unsigned> resumes{0}, destroyed{0};
  Gate *destruction_gate = nullptr;
};
void held_resume(objc3_task_t *, void *opaque) {
  ++static_cast<HeldFrame *>(opaque)->resumes;
}
void held_destroy(void *opaque) {
  auto &frame = *static_cast<HeldFrame *>(opaque);
  if (frame.destruction_gate) frame.destruction_gate->arrive();
  ++frame.destroyed;
}
objc3_task_t *start_held(
    objc3_executor_t *destination, HeldFrame &frame,
    const objc3_async_result_type *type = nullptr) {
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = destination;
  options.frame = &frame;
  options.resume = held_resume;
  options.destroy = held_destroy;
  options.result_type = type;
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  return task;
}

struct JoinFrame {
  objc3_task_group_t *group;
  objc3_actor_t *actor = nullptr;
  objc3_task_t *dependency = nullptr;
  std::atomic<unsigned> resumes{0}, destroyed{0};
  bool reject_cycle = false;
  objc3_service_status status = OBJC3_SERVICE_UNSUPPORTED_VERSION;
};
void join_resume(objc3_task_t *task, void *opaque) {
  auto &frame = *static_cast<JoinFrame *>(opaque);
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == objc3_actor_identity(frame.actor));
  if (frame.resumes++ == 0) {
    assert(objc3_task_group_await(nullptr, task, &frame.status) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    const auto status = objc3_task_group_await(frame.group, task, &frame.status);
    if (frame.reject_cycle) {
      assert(status == OBJC3_SERVICE_INVALID_ARGUMENT);
      assert(frame.status == OBJC3_SERVICE_UNSUPPORTED_VERSION);
      // Rejection must not consume the parent's one suspension slot.
      assert(objc3_task_await(task, frame.dependency) == OBJC3_SERVICE_OK);
    } else {
      assert(status == OBJC3_SERVICE_OK);
      assert(objc3_task_group_await(frame.group, task, &frame.status) ==
             OBJC3_SERVICE_INVALID_ARGUMENT);
    }
    return;
  }
  if (objc3_task_is_cancel_requested(task)) {
    assert(frame.status == OBJC3_SERVICE_CANCELLED);
    complete(task, OBJC3_COMPLETION_CANCELLED);
    return;
  }
  if (!frame.reject_cycle) {
    assert(frame.status == OBJC3_SERVICE_OK);
    assert(objc3_task_group_join(frame.group) == OBJC3_SERVICE_OK);
  }
  complete(task);
}
void join_destroy(void *opaque) {
  ++static_cast<JoinFrame *>(opaque)->destroyed;
}
objc3_task_t *start_join(objc3_executor_t *destination, JoinFrame &frame) {
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = frame.actor ? nullptr : destination;
  options.actor = frame.actor;
  options.frame = &frame;
  options.resume = join_resume;
  options.destroy = join_destroy;
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  return task;
}

void raw_quiescence_and_nested_waiters() {
  auto *first = executor();
  auto *second = executor();
  auto root = objc3::task_group_handle::create();
  auto child_group = root.child();
  HeldFrame child_frame;
  auto *child = start_held(first, child_frame);
  assert(objc3_task_group_add(child_group.native_handle(), child) == OBJC3_SERVICE_OK);
  pump(first);

  Gate callback_gate, destruction_gate;
  child_frame.destruction_gate = &destruction_gate;
  assert(objc3_task_add_waiter(child, [](void *opaque, const objc3_completion *) {
    static_cast<Gate *>(opaque)->arrive();
  }, &callback_gate) == OBJC3_SERVICE_OK);
  root.close();
  JoinFrame left{root.native_handle()}, right{root.native_handle()};
  auto *a = start_join(first, left);
  auto *b = start_join(second, right);
  pump(first);
  pump(second);
  assert(left.resumes == 1 && right.resumes == 1);
  assert(objc3_task_group_await(root.native_handle(), a, &left.status) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);

  std::thread finisher([&] { complete(child); });
  callback_gate.wait();
  pump(first);
  pump(second);
  pending(a);
  pending(b);
  callback_gate.release();
  destruction_gate.wait();
  pump(first);
  pump(second);
  pending(a);
  pending(b);
  assert(child_frame.destroyed == 0);
  destruction_gate.release();
  finisher.join();
  pump(first);
  pump(second);
  assert(left.resumes == 2 && right.resumes == 2);
  assert(left.destroyed == 1 && right.destroyed == 1 && child_frame.destroyed == 1);
  root.wait();
  objc3_task_t *next = nullptr;
  assert(objc3_task_group_next(child_group.native_handle(), &next) == OBJC3_SERVICE_OK);
  assert(next == child); // Awaiting a group does not consume completion order.
  objc3_task_release(next);
  objc3_task_release(a);
  objc3_task_release(b);
  objc3_task_release(child);
  objc3_executor_release(first);
  objc3_executor_release(second);
}

void cycle_rejection() {
  auto *destination = executor();
  auto root = objc3::task_group_handle::create();
  auto nested = root.child();
  HeldFrame dependency_frame;
  auto *dependency = start_held(destination, dependency_frame);
  JoinFrame frame{root.native_handle()};
  frame.reject_cycle = true;
  frame.dependency = dependency;
  auto *parent = start_join(destination, frame);
  assert(objc3_task_group_add(nested.native_handle(), parent) == OBJC3_SERVICE_OK);
  root.close();
  pump(destination);
  assert(frame.resumes == 1);
  complete(dependency);
  pump(destination);
  assert(frame.resumes == 2 && frame.destroyed == 1);
  root.wait();
  objc3_task_release(parent);
  objc3_task_release(dependency);
  objc3_executor_release(destination);
}

void cancellation_and_abandonment() {
  auto *destination = executor();
  auto group = objc3::task_group_handle::create();
  HeldFrame child_frame;
  auto *child = start_held(destination, child_frame);
  assert(objc3_task_group_add(group.native_handle(), child) == OBJC3_SERVICE_OK);
  group.close();
  JoinFrame cancelled{group.native_handle()}, abandoned{group.native_handle()};
  auto *a = start_join(destination, cancelled);
  auto *b = start_join(destination, abandoned);
  pump(destination);
  objc3_task_request_cancel(a);
  assert(cancelled.status == OBJC3_SERVICE_CANCELLED);
  objc3_task_release(b);
  assert(abandoned.destroyed == 1);
  pump(destination);
  assert(cancelled.resumes == 2 && cancelled.destroyed == 1);
  assert(!objc3_task_is_cancel_requested(child));
  pending(child);
  complete(child);
  pump(destination);
  assert(cancelled.resumes == 2 && abandoned.resumes == 1);
  assert(abandoned.status == OBJC3_SERVICE_UNSUPPORTED_VERSION);
  group.wait();
  objc3_task_release(a);
  objc3_task_release(child);
  objc3_executor_release(destination);
}

objc3::task<int> join_group(objc3::task_group_handle group,
                           std::atomic<unsigned> &turns) {
  ++turns;
  co_await group;
  ++turns;
  co_return 41;
}
objc3::task<void> value() { co_return; }

template <class T>
objc3::task<objc3::task_handle<T>> next_task(objc3::task_group_handle group,
                                           uint64_t expected_actor = 0) {
  objc3_current_actor_facts before{};
  before.struct_size = sizeof(before);
  before.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&before) == OBJC3_SERVICE_OK);
  assert(before.actor_identity == expected_actor);
  auto result = co_await group.next<T>();
  objc3_current_actor_facts after{};
  after.struct_size = sizeof(after);
  after.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&after) == OBJC3_SERVICE_OK);
  assert(after.actor_identity == expected_actor &&
         after.executor_identity == before.executor_identity);
  co_return result;
}

template <class T> void complete_value(objc3_task_t *task, const T &value) {
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.has_payload = 1;
  completion.payload = &value;
  completion.payload_ops = &objc3::task_detail::value_ops<T>::descriptor;
  completion.result_type = objc3::task_detail::result_type<T>();
  assert(objc3_task_complete(task, &completion) == OBJC3_SERVICE_OK);
}

void next_completion_order_and_type_reservation() {
  auto *destination = executor();
  auto group = objc3::task_group_handle::create();
  HeldFrame frames[3];
  objc3_task_t *tasks[3];
  for (unsigned index = 0; index != 3; ++index) {
    tasks[index] = start_held(destination, frames[index],
                              objc3::task_detail::result_type<int>());
    assert(objc3_task_group_add(group.native_handle(), tasks[index]) == OBJC3_SERVICE_OK);
  }
  group.close();
  auto wrong = next_task<unsigned>(group).start(destination);
  auto first = next_task<int>(group).start(destination);
  auto second = next_task<int>(group).start(destination);
  pump(destination);
  pending(first.native_handle());
  pending(second.native_handle());
  complete_value(tasks[1], 20);
  objc3_task_t *polled = nullptr;
  assert(objc3_task_group_next(group.native_handle(), &polled) == OBJC3_SERVICE_NOT_READY);
  assert(!polled); // A poll cannot steal an async waiter's reserved task.
  complete_value(tasks[0], 10);
  pump(destination);
  bool mismatch = false;
  try { (void)wrong.result(); }
  catch (const objc3::service_failure &failure) {
    mismatch = failure.status() == OBJC3_SERVICE_TYPE_MISMATCH;
  }
  assert(mismatch);
  assert(first.result().native_handle() == tasks[1] && first.result().result() == 20);
  assert(second.result().native_handle() == tasks[0] && second.result().result() == 10);
  complete_value(tasks[2], 30);
  auto third = next_task<int>(group).start(destination);
  pump(destination, 1);
  pending(third.native_handle()); // Ready iteration still queues a continuation.
  pump(destination);
  assert(third.result().native_handle() == tasks[2] && third.result().result() == 30);
  auto drained = next_task<int>(group).start(destination);
  pump(destination, 1);
  pending(drained.native_handle());
  pump(destination);
  assert(!drained.result());
  group.wait();
  for (auto *task : tasks) objc3_task_release(task);
  objc3_executor_release(destination);
}

void next_cancellation_ownership() {
  auto *destination = executor();
  for (unsigned mode = 0; mode != 4; ++mode) {
    auto group = objc3::task_group_handle::create();
    HeldFrame frame;
    auto *child = start_held(destination, frame,
                             objc3::task_detail::result_type<std::shared_ptr<int>>());
    assert(objc3_task_group_add(group.native_handle(), child) == OBJC3_SERVICE_OK);
    group.close();
    auto parent = next_task<std::shared_ptr<int>>(group).start(destination);
    pump(destination);
    auto value = std::make_shared<int>(42);
    std::weak_ptr<int> lifetime = value;
    if (mode == 0) parent.cancel(); // Cancellation precedes reservation.
    if (mode == 3) parent = {}; // Abandonment invalidates both output slots.
    complete_value(child, value);
    value.reset();
    if (mode == 1) parent.cancel(); // The retained task is already in the frame.
    if (mode == 2) {
      // A queued consumer may finish without any caller handle. Its native
      // frame and completion payload must release the reserved task owner.
      parent = {};
    }
    pump(destination);
    if (mode < 2) {
      bool caught = false;
      try { (void)parent.result(); }
      catch (const objc3::task_cancelled &) { caught = true; }
      assert(caught);
    }
    objc3_task_t *polled = nullptr;
    const auto status = objc3_task_group_next(group.native_handle(), &polled);
    assert(status == (mode == 0 || mode == 3 ? OBJC3_SERVICE_OK
                                            : OBJC3_SERVICE_CLOSED));
    if (polled) objc3_task_release(polled);
    objc3_task_release(child);
    group = {};
    parent = {};
    assert(lifetime.expired());
  }
  objc3_executor_release(destination);
}

void next_dispatch_failure_cleanup() {
  auto *destination = executor();
  auto group = objc3::task_group_handle::create();
  HeldFrame frame;
  auto *child = start_held(destination, frame,
                           objc3::task_detail::result_type<std::shared_ptr<int>>());
  assert(objc3_task_group_add(group.native_handle(), child) == OBJC3_SERVICE_OK);
  group.close();
  auto parent = next_task<std::shared_ptr<int>>(group).start(destination);
  pump(destination);
  for (unsigned index = 0; index != 1024; ++index)
    assert(objc3_executor_enqueue(destination, [](void *) {}, nullptr) == OBJC3_SERVICE_OK);
  auto value = std::make_shared<int>(42);
  std::weak_ptr<int> lifetime = value;
  complete_value(child, value);
  value.reset();
  objc3_completion completion{};
  assert(objc3_task_get_completion(parent.native_handle(), &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
  objc3_task_t *polled = nullptr;
  assert(objc3_task_group_next(group.native_handle(), &polled) == OBJC3_SERVICE_CLOSED);
  objc3_task_release(child);
  group = {};
  assert(lifetime.expired());
  pump(destination);
  objc3_executor_release(destination);
}

struct NextFrame {
  objc3_task_group_t *group;
  objc3_async_result_type expected{};
  std::string identity;
  objc3_service_status status = OBJC3_SERVICE_UNSUPPORTED_VERSION;
  objc3_task_t *result = nullptr;
  unsigned resumes = 0, destroyed = 0;
  int observed = 0;
};
void next_resume(objc3_task_t *task, void *opaque) {
  auto &frame = *static_cast<NextFrame *>(opaque);
  if (frame.resumes++ == 0) {
    objc3_task_t *occupied = task;
    auto rejected_status = OBJC3_SERVICE_UNSUPPORTED_VERSION;
    assert(objc3_task_group_await_next(frame.group, task, &frame.expected,
                                       &rejected_status, &occupied) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    assert(occupied == task && rejected_status == OBJC3_SERVICE_UNSUPPORTED_VERSION);
    assert(objc3_task_group_await_next(frame.group, task, &frame.expected,
                                       &frame.status, &frame.result) == OBJC3_SERVICE_OK);
    assert(objc3_task_group_await_next(frame.group, task, &frame.expected,
                                       &frame.status, &frame.result) ==
           OBJC3_SERVICE_INVALID_ARGUMENT);
    return;
  }
  if (objc3_task_is_cancel_requested(task)) {
    assert(frame.status == OBJC3_SERVICE_CANCELLED && !frame.result);
    complete(task, OBJC3_COMPLETION_CANCELLED);
  } else {
    assert(frame.status == OBJC3_SERVICE_OK && frame.result);
    assert(objc3_task_copy_result(frame.result, objc3::task_detail::result_type<int>(),
                                  &objc3::task_detail::value_ops<int>::descriptor,
                                  &frame.observed) == OBJC3_SERVICE_OK);
    complete(task);
  }
}
objc3_task_t *start_next(objc3_executor_t *destination, NextFrame &frame) {
  frame.expected = *objc3::task_detail::result_type<int>();
  frame.identity = frame.expected.identity;
  frame.expected.identity = frame.identity.c_str();
  objc3_task_start_options options{};
  options.struct_size = sizeof(options);
  options.version = OBJC3_SERVICE_ABI_VERSION;
  options.executor = destination;
  options.frame = &frame;
  options.resume = next_resume;
  options.destroy = [](void *opaque) {
    auto &frame = *static_cast<NextFrame *>(opaque);
    objc3_task_release(std::exchange(frame.result, nullptr));
    ++frame.destroyed;
  };
  objc3_task_t *task = nullptr;
  assert(objc3_task_start(&options, &task) == OBJC3_SERVICE_OK);
  return task;
}
void next_c_slots_and_descriptor_copy() {
  auto *destination = executor();
  for (bool precancelled : {false, true}) {
    auto group = objc3::task_group_handle::create();
    HeldFrame child_frame;
    auto *child = start_held(destination, child_frame,
                             objc3::task_detail::result_type<int>());
    assert(objc3_task_group_add(group.native_handle(), child) == OBJC3_SERVICE_OK);
    group.close();
    NextFrame frame{group.native_handle()};
    auto *parent = start_next(destination, frame);
    if (precancelled) objc3_task_request_cancel(parent);
    pump(destination);
    if (!precancelled) {
      assert(frame.resumes == 1 && !frame.result);
      frame.identity[0] = '!';
      ++frame.expected.storage_size;
    }
    complete_value(child, 42);
    pump(destination);
    assert(frame.resumes == 2 && frame.destroyed == 1 && !frame.result);
    assert(frame.observed == (precancelled ? 0 : 42));
    objc3_task_t *polled = nullptr;
    assert(objc3_task_group_next(group.native_handle(), &polled) ==
           (precancelled ? OBJC3_SERVICE_OK : OBJC3_SERVICE_CLOSED));
    objc3_task_release(polled);
    group.wait();
    objc3_task_release(parent);
    objc3_task_release(child);
  }
  objc3_executor_release(destination);
}

void next_nested_and_actor_destinations() {
  auto *destination = executor();
  auto group = objc3::task_group_handle::create();
  auto nested = group.child();
  HeldFrame child_frame;
  auto *child = start_held(destination, child_frame,
                           objc3::task_detail::result_type<int>());
  assert(objc3_task_group_add(nested.native_handle(), child) == OBJC3_SERVICE_OK);
  group.close();
  auto root_next = next_task<int>(group).start(destination);
  pump(destination);
  assert(!root_next.result()); // Nested children are not direct iteration results.
  objc3_actor_t *actor = nullptr;
  assert(objc3_actor_create(destination, &actor) == OBJC3_SERVICE_OK);
  auto actor_next = next_task<int>(nested, objc3_actor_identity(actor)).start_on(actor);
  pump(destination);
  pending(actor_next.native_handle());
  complete_value(child, 42);
  pump(destination);
  assert(actor_next.result().result() == 42);
  group.wait();
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  objc3_actor_release(actor);
  objc3_task_release(child);
  objc3_executor_release(destination);
}

void next_worker_registration_races() {
  auto *producer = executor(false);
  auto *consumer = executor(false);
  for (unsigned iteration = 0; iteration != 64; ++iteration) {
    auto group = objc3::task_group_handle::create();
    auto child = value().start(producer);
    if (iteration % 2) child.wait();
    group.add(child);
    group.close();
    auto next = next_task<void>(group).start(consumer);
    next.wait();
    auto result = next.result();
    assert(result.native_handle() == child.native_handle());
    result.result();
    auto closed = next_task<void>(group).start(consumer);
    closed.wait();
    assert(!closed.result());
    group.wait();
  }
  objc3_executor_release(producer);
  objc3_executor_release(consumer);
}

void next_failed_and_cancelled_results() {
  auto *destination = executor();
  auto group = objc3::task_group_handle::create();
  HeldFrame failed_frame, cancelled_frame;
  auto *failed = start_held(destination, failed_frame,
                            objc3::task_detail::result_type<int>());
  auto *cancelled = start_held(destination, cancelled_frame,
                               objc3::task_detail::result_type<int>());
  assert(objc3_task_group_add(group.native_handle(), failed) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group.native_handle(), cancelled) == OBJC3_SERVICE_OK);
  group.close();
  auto first = next_task<int>(group).start(destination);
  pump(destination);
  int payload = 73;
  objc3_error_t *error = nullptr;
  assert(objc3_error_create(&payload, &objc3::task_detail::value_ops<int>::descriptor,
                             &error) == OBJC3_SERVICE_OK);
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_ERROR;
  completion.error = error;
  assert(objc3_task_complete(failed, &completion) == OBJC3_SERVICE_OK);
  objc3_error_release(error);
  assert(objc3_task_is_cancel_requested(cancelled));
  complete(cancelled, OBJC3_COMPLETION_CANCELLED);
  pump(destination);
  auto failed_result = first.result();
  bool caught = false;
  try { (void)failed_result.result(); }
  catch (const objc3::task_error &failure) {
    caught = *static_cast<const int *>(objc3_error_payload(failure.native_handle())) == 73;
  }
  assert(caught);
  auto second = next_task<int>(group).start(destination);
  pump(destination);
  auto cancelled_result = second.result();
  caught = false;
  try { (void)cancelled_result.result(); }
  catch (const objc3::task_cancelled &) { caught = true; }
  assert(caught && cancelled_result.native_handle() == cancelled);
  group.wait();
  objc3_task_release(failed);
  objc3_task_release(cancelled);
  objc3_executor_release(destination);
}

void many_group_observation_lifetimes() {
  auto *destination = executor();
  HeldFrame frame;
  auto *child = start_held(destination, frame);
  std::vector<objc3::task_group_handle> groups;
  groups.reserve(4096);
  for (unsigned index = 0; index != 4096; ++index) {
    auto group = objc3::task_group_handle::create();
    assert(objc3_task_group_add(group.native_handle(), child) == OBJC3_SERVICE_OK);
    group.close();
    groups.push_back(std::move(group));
  }
  pump(destination);
  complete(child);
  for (const auto &group : groups) group.wait();
  groups.clear();
  objc3_task_release(child); // Final release must have bounded native stack use.
  assert(frame.destroyed == 1);
  objc3_executor_release(destination);
}

void late_callback_snapshot() {
  auto *destination = executor();
  auto group = objc3::task_group_handle::create();
  HeldFrame frame;
  auto *child = start_held(destination, frame);
  assert(objc3_task_group_add(group.native_handle(), child) == OBJC3_SERVICE_OK);
  group.close();
  pump(destination);
  complete(child);
  group.wait();

  Gate before, after;
  auto block = [&](Gate &gate) {
    assert(objc3_task_add_waiter(child, [](void *opaque, const objc3_completion *) {
      static_cast<Gate *>(opaque)->arrive();
    }, &gate) == OBJC3_SERVICE_OK);
  };
  std::thread first([&] { block(before); });
  before.wait();
  std::atomic<unsigned> turns{0};
  auto joined = join_group(group, turns).start(destination);
  pump(destination);
  pending(joined.native_handle());
  assert(turns == 1); // A late callback invalidates the previous quiescent view.
  before.release();
  first.join(); // The first quiescent observation now queues the group join.
  std::thread second([&] { block(after); });
  after.wait();
  pump(destination);
  // A callback registered after that observation cannot invalidate its result.
  assert(joined.result() == 41 && turns == 2);
  after.release();
  second.join();
  group.wait();
  objc3_task_release(child);
  objc3_executor_release(destination);
}

void cpp_handle_contract() {
  auto *destination = executor();
  auto empty = objc3::task_group_handle::create();
  std::atomic<unsigned> turns{0};
  // An open group rejects suspension without silently closing membership.
  auto rejected = join_group(empty, turns).start(destination);
  pump(destination);
  bool caught = false;
  try { (void)rejected.result(); }
  catch (const objc3::service_failure &failure) {
    caught = failure.status() == OBJC3_SERVICE_NOT_READY;
  }
  assert(caught && turns == 1);
  empty.close();
  auto joined = join_group(empty, turns).start(destination);
  pump(destination, 1);
  assert(turns == 2);
  pending(joined.native_handle()); // Empty joins still create a queued turn.
  pump(destination, 1);
  assert(turns == 3 && joined.result() == 41);

  auto group = objc3::task_group_handle::create();
  auto nested = group.child();
  auto work = value().start(destination);
  nested.add(work);
  group.close();
  auto copied = group;
  auto moved = std::move(copied);
  assert(!copied && moved);
  auto waiting = join_group(std::move(moved), turns).start(destination);
  group = {}; // The lazy frame owns the group before its first resume.
  nested = {};
  pump(destination);
  assert(waiting.result() == 41 && turns == 5);
  objc3_executor_release(destination);
}

void actor_destination_and_dispatch_failure() {
  auto *destination = executor();
  objc3_actor_t *actor = nullptr;
  assert(objc3_actor_create(destination, &actor) == OBJC3_SERVICE_OK);
  auto group = objc3::task_group_handle::create();
  HeldFrame child_frame;
  auto *child = start_held(destination, child_frame);
  assert(objc3_task_group_add(group.native_handle(), child) == OBJC3_SERVICE_OK);
  group.close();
  JoinFrame actor_frame{group.native_handle()};
  actor_frame.actor = actor;
  auto *parent = start_join(destination, actor_frame);
  pump(destination);
  complete(child);
  pump(destination);
  assert(actor_frame.resumes == 2 && actor_frame.destroyed == 1);
  objc3_task_release(parent);
  objc3_task_release(child);
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  objc3_actor_release(actor);

  auto pressured = objc3::task_group_handle::create();
  HeldFrame pending_frame;
  child = start_held(destination, pending_frame);
  assert(objc3_task_group_add(pressured.native_handle(), child) == OBJC3_SERVICE_OK);
  pressured.close();
  JoinFrame pressure_frame{pressured.native_handle()};
  parent = start_join(destination, pressure_frame);
  pump(destination);
  for (unsigned i = 0; i != 1024; ++i)
    assert(objc3_executor_enqueue(destination, [](void *) {}, nullptr) == OBJC3_SERVICE_OK);
  complete(child);
  objc3_completion completion{};
  assert(objc3_task_get_completion(parent, &completion) == OBJC3_SERVICE_OK);
  assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
  assert(pressure_frame.resumes == 1 && pressure_frame.destroyed == 1);
  pump(destination);
  pressured.wait();
  objc3_task_release(parent);
  objc3_task_release(child);
  objc3_executor_release(destination);
}

void worker_registration_races() {
  auto *producer = executor(false);
  auto *consumer = executor(false);
  for (unsigned iteration = 0; iteration != 128; ++iteration) {
    auto group = objc3::task_group_handle::create();
    auto nested = group.child();
    auto work = value().start(producer);
    // Exercise registration both before and after terminal publication.
    if (iteration % 2) work.wait();
    nested.add(work);
    group.close();
    std::atomic<unsigned> turns{0};
    auto joined = join_group(group, turns).start(consumer);
    joined.wait();
    assert(joined.result() == 41 && turns == 2);
    group.wait();
  }
  objc3_executor_release(producer);
  objc3_executor_release(consumer);
}
} // namespace

int main() {
  raw_quiescence_and_nested_waiters();
  cycle_rejection();
  cancellation_and_abandonment();
  cpp_handle_contract();
  late_callback_snapshot();
  actor_destination_and_dispatch_failure();
  worker_registration_races();
  next_completion_order_and_type_reservation();
  next_cancellation_ownership();
  next_dispatch_failure_cleanup();
  next_c_slots_and_descriptor_copy();
  next_nested_and_actor_destinations();
  next_worker_registration_races();
  next_failed_and_cancelled_results();
  many_group_observation_lifetimes();
}
