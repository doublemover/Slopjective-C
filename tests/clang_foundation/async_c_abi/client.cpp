#include "bridge.h"
#include "objc3_task.hpp"
#include <cassert>
#include <stdexcept>
#include <utility>

#ifdef __OBJC__
#define LOGICAL(T) T
#define ASYNC __objc3_async
#define AWAIT __objc3_await
#define RETURN return
#else
#define LOGICAL(T) objc3::task<T>
#define ASYNC
#define AWAIT co_await
#define RETURN co_return
#endif

extern "C" __attribute__((noinline)) void
objc3_async_c_checkpoint(int observed) {
  if (observed != 42)
    __builtin_trap();
}

static LOGICAL(int) cpp_int_value(int value) ASYNC {
  AWAIT objc3::task_yield;
  if (value < 0)
    throw std::runtime_error("native C++ failure");
  RETURN value;
}
static LOGICAL(bridge_pair) cpp_pair_value(bridge_pair value) ASYNC {
  AWAIT objc3::task_yield;
  RETURN value;
}
static LOGICAL(void) cpp_void_value() ASYNC {
  AWAIT objc3::task_yield;
  RETURN;
}
static LOGICAL(int) cpp_await_c(int *destroyed) ASYNC {
  auto frame = bridge_c_int(40, 0, destroyed);
  int value = AWAIT objc3::task<int>::from_native(&frame);
  assert(!frame.context);
  objc3_async_c_checkpoint(value + 2);
  RETURN value + 2;
}
static LOGICAL(int) cpp_await_c_group(objc3_task_group_t *group,
                                      uint64_t expected_actor,
                                      int *destroyed) ASYNC {
  auto frame = bridge_c_group_join(group, 41, expected_actor, destroyed);
  int value = AWAIT objc3::task<int>::from_native(&frame);
  RETURN value + 1;
}
static LOGICAL(int) cpp_await_c_next(objc3_task_group_t *group,
                                     uint64_t expected_actor,
                                     int *destroyed) ASYNC {
  auto frame = bridge_c_group_next(group, expected_actor, destroyed);
  RETURN AWAIT objc3::task<int>::from_native(&frame);
}
static LOGICAL(unsigned) cpp_unsigned_value() ASYNC {
  AWAIT objc3::task_yield;
  RETURN 42u;
}
extern "C" objc3_async_frame bridge_cpp_int(int value) {
  return cpp_int_value(value).release_native();
}
extern "C" objc3_async_frame bridge_cpp_pair(bridge_pair value) {
  return cpp_pair_value(value).release_native();
}
extern "C" objc3_async_frame bridge_cpp_void() {
  return cpp_void_value().release_native();
}

static void pump(objc3_executor_t *executor, uint32_t limit = 10000) {
  uint32_t count = 0;
  assert(objc3_executor_pump(executor, limit, &count) == OBJC3_SERVICE_OK);
}
static int load(const int &value) {
  return __atomic_load_n(&value, __ATOMIC_ACQUIRE);
}

static void group_frame_lifetimes(objc3_executor_t *executor) {
  int destroyed = 0;
  objc3_task_group_t *raw = nullptr;
  assert(objc3_task_group_create(nullptr, &raw) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_close(raw) == OBJC3_SERVICE_OK);
  auto frame = bridge_c_group_join(raw, 42, 0, &destroyed);
  auto abandoned = bridge_c_group_join(raw, 19, 0, &destroyed);
  objc3_task_group_release(raw); // Each lazy C frame already owns its group.
  assert(objc3_async_frame_dispose(&abandoned) == OBJC3_SERVICE_OK);
  assert(objc3_async_frame_dispose(&abandoned) == OBJC3_SERVICE_OK);
  assert(load(destroyed) == 1);
  objc3_task_t *failed = nullptr;
  assert(objc3_async_frame_start(&frame, nullptr, nullptr, &failed) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(frame.context && !failed && load(destroyed) == 1);
  auto lazy = objc3::task<int>::from_native(&frame);
  auto moved = std::move(lazy);
  auto joined = std::move(moved).start(executor);
  pump(executor, 1);
  objc3_completion view{};
  assert(objc3_task_get_completion(joined.native_handle(), &view) ==
         OBJC3_SERVICE_NOT_READY);
  pump(executor);
  assert(joined.result() == 42 && load(destroyed) == 2);

  auto group = objc3::task_group_handle::create();
  auto nested = group.child();
  auto c_frame = bridge_c_int(40, 0, &destroyed);
  auto c_child = objc3::task<int>::from_native(&c_frame).start(executor);
  auto cpp_child = cpp_int_value(2).start(executor);
  nested.add(c_child);
  group.add(cpp_child);
  group.close();
  auto mixed = cpp_await_c_group(group.native_handle(), 0, &destroyed)
                   .start(executor);
  pump(executor);
  assert(mixed.result() == 42 && c_child.result() + cpp_child.result() == 42);
  assert(load(destroyed) == 4);
  group.wait();
  objc3_task_t *next = nullptr;
  assert(objc3_task_group_next(nested.native_handle(), &next) == OBJC3_SERVICE_OK);
  assert(next == c_child.native_handle());
  objc3_task_release(next);
}

static void group_frame_cancellation(objc3_executor_t *executor) {
  for (int mode : {0, 1, 2}) {
    int destroyed = 0;
    auto group = objc3::task_group_handle::create();
    auto child_frame = bridge_c_int(42, 0, &destroyed);
    auto child = objc3::task<int>::from_native(&child_frame).start(executor);
    group.add(child);
    group.close();
    auto parent_frame = bridge_c_group_join(group.native_handle(), 42, 0, &destroyed);
    auto parent = objc3::task<int>::from_native(&parent_frame).start(executor);
    pump(executor, 2); // Child yielded; the C parent registered its group join.
    objc3_completion view{};
    assert(objc3_task_get_completion(parent.native_handle(), &view) ==
           OBJC3_SERVICE_NOT_READY);
    if (mode == 0) {
      parent.cancel();
      assert(!objc3_task_is_cancel_requested(child.native_handle()));
    } else if (mode == 1) {
      parent = {};
      assert(load(destroyed) == 1);
      assert(!objc3_task_is_cancel_requested(child.native_handle()));
    } else {
      group.cancel();
      assert(objc3_task_is_cancel_requested(child.native_handle()));
      assert(!objc3_task_is_cancel_requested(parent.native_handle()));
    }
    pump(executor);
    group.wait();
    assert(load(destroyed) == 2);
    if (mode == 0) {
      bool caught = false;
      try { (void)parent.result(); }
      catch (const objc3::task_cancelled &) { caught = true; }
      assert(caught && child.result() == 42);
    } else if (mode == 1) {
      assert(child.result() == 42);
    } else {
      bool caught = false;
      try { (void)child.result(); }
      catch (const objc3::task_cancelled &) { caught = true; }
      assert(caught && parent.result() == 42);
    }
  }
}

static void group_frame_rejections(objc3_executor_t *executor) {
  for (bool cycle : {false, true}) {
    int destroyed = 0;
    auto group = objc3::task_group_handle::create();
    auto nested = group.child();
    auto frame = bridge_c_group_join(group.native_handle(), 42, 0, &destroyed);
    auto task = objc3::task<int>::from_native(&frame).start(executor);
    if (cycle) {
      nested.add(task);
      group.close();
    }
    pump(executor);
    bool caught = false;
    try { (void)task.result(); }
    catch (const objc3::task_error &error) {
      const auto expected = cycle ? OBJC3_SERVICE_INVALID_ARGUMENT
                                  : OBJC3_SERVICE_NOT_READY;
      const auto *payload = static_cast<const int *>(
          objc3_error_payload(error.native_handle()));
      caught = payload && *payload == expected;
    }
    assert(caught && load(destroyed) == 1);
    if (!cycle) group.close();
    group.wait();
  }
}

static void group_frame_destinations(objc3_executor_t *executor) {
  int destroyed = 0;
  objc3_actor_t *actor = nullptr;
  assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  auto group = objc3::task_group_handle::create();
  auto nested = group.child();
  auto frame = bridge_c_int(42, 0, &destroyed);
  auto child = objc3::task<int>::from_native(&frame).start(executor);
  nested.add(child);
  group.close();
  auto parent = cpp_await_c_group(group.native_handle(),
                                 objc3_actor_identity(actor), &destroyed)
                    .start_on(actor);
  pump(executor);
  assert(parent.result() == 42 && child.result() == 42 && load(destroyed) == 2);
  group.wait();
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  objc3_actor_release(actor);

  objc3_executor_t *worker = nullptr;
  assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
  for (int iteration = 0; iteration != 64; ++iteration) {
    auto work_group = objc3::task_group_handle::create();
    auto settlements = objc3::task_group_handle::create();
    auto c_frame = bridge_c_int(42, 0, &destroyed);
    auto work = objc3::task<int>::from_native(&c_frame).start(worker);
    work_group.add(work);
    work_group.close();
    auto waiter_frame = bridge_c_group_join(work_group.native_handle(), 42, 0,
                                             &destroyed);
    auto waiter = objc3::task<int>::from_native(&waiter_frame).start(worker);
    settlements.add(waiter);
    settlements.close();
    settlements.wait(); // Observe the waiting C frame's own destruction too.
    assert(waiter.result() == 42 && work.result() == 42);
    assert(load(destroyed) == 4 + iteration * 2);
  }
  objc3_executor_release(worker);
}

static void group_frame_result_iteration(objc3_executor_t *executor) {
  int destroyed = 0;
  auto group = objc3::task_group_handle::create();
  auto first_frame = bridge_c_int(10, 0, &destroyed);
  auto first_child = objc3::task<int>::from_native(&first_frame).start(executor);
  auto second_child = cpp_int_value(20).start(executor);
  auto third_frame = bridge_c_int(30, 0, &destroyed);
  auto third_child = objc3::task<int>::from_native(&third_frame).start(executor);
  group.add(first_child);
  group.add(second_child);
  group.add(third_child);
  group.close();
  auto a_frame = bridge_c_group_next(group.native_handle(), 0, &destroyed);
  auto b_frame = bridge_c_group_next(group.native_handle(), 0, &destroyed);
  auto a = objc3::task<int>::from_native(&a_frame).start(executor);
  auto b = objc3::task<int>::from_native(&b_frame).start(executor);
  pump(executor);
  assert(a.result() == 10 && b.result() == 20);
  auto c = cpp_await_c_next(group.native_handle(), 0, &destroyed).start(executor);
  pump(executor);
  assert(c.result() == 30);
  auto drained_frame = bridge_c_group_next(group.native_handle(), 0, &destroyed);
  auto drained = objc3::task<int>::from_native(&drained_frame).start(executor);
  pump(executor, 1);
  objc3_completion view{};
  assert(objc3_task_get_completion(drained.native_handle(), &view) == OBJC3_SERVICE_NOT_READY);
  pump(executor);
  assert(drained.result() == -1 && load(destroyed) == 6);
  group.wait();

  auto typed = objc3::task_group_handle::create();
  auto unsigned_child = cpp_unsigned_value().start(executor);
  typed.add(unsigned_child);
  typed.close();
  auto mismatch_frame = bridge_c_group_next(typed.native_handle(), 0, &destroyed);
  auto mismatch = objc3::task<int>::from_native(&mismatch_frame).start(executor);
  pump(executor);
  bool caught = false;
  try { (void)mismatch.result(); }
  catch (const objc3::task_error &error) {
    caught = *static_cast<const int *>(objc3_error_payload(error.native_handle())) ==
             OBJC3_SERVICE_TYPE_MISMATCH;
  }
  assert(caught);
  objc3_task_t *reserved = nullptr;
  assert(objc3_task_group_next(typed.native_handle(), &reserved) == OBJC3_SERVICE_OK);
  auto owned = objc3::task_handle<unsigned>::adopt_native(&reserved);
  assert(!reserved && owned.result() == 42);
  typed.wait();
}

static void group_frame_result_cancellation(objc3_executor_t *executor) {
  for (int mode : {0, 1, 2}) {
    int destroyed = 0;
    auto group = objc3::task_group_handle::create();
    auto frame = bridge_c_int(42, 0, &destroyed);
    auto child = objc3::task<int>::from_native(&frame).start(executor);
    group.add(child);
    group.close();
    frame = bridge_c_group_next(group.native_handle(), 0, &destroyed);
    auto parent = objc3::task<int>::from_native(&frame).start(executor);
    pump(executor, 2);
    if (mode == 1) {
      pump(executor, 1); // Reserve the child; keep the C consumer queued.
      assert(load(destroyed) == 1);
    }
    if (mode == 2) {
      parent = {};
      assert(load(destroyed) == 1);
    } else {
      parent.cancel();
    }
    pump(executor);
    assert(child.result() == 42 && load(destroyed) == 2);
    if (mode != 2) {
      bool caught = false;
      try { (void)parent.result(); }
      catch (const objc3::task_cancelled &) { caught = true; }
      assert(caught);
    }
    objc3_task_t *polled = nullptr;
    assert(objc3_task_group_next(group.native_handle(), &polled) ==
           (mode == 1 ? OBJC3_SERVICE_CLOSED : OBJC3_SERVICE_OK));
    objc3_task_release(polled);
    group.wait();
  }
}

static void group_frame_result_outcomes(objc3_executor_t *executor) {
  for (int mode : {2, 3}) {
    int destroyed = 0;
    auto group = objc3::task_group_handle::create();
    auto frame = bridge_c_int(73, mode, &destroyed);
    auto child = objc3::task<int>::from_native(&frame).start(executor);
    group.add(child);
    group.close();
    frame = bridge_c_group_next(group.native_handle(), 0, &destroyed);
    auto parent = objc3::task<int>::from_native(&frame).start(executor);
    pump(executor);
    bool caught = false;
    try { (void)parent.result(); }
    catch (const objc3::task_cancelled &) { caught = mode == 2; }
    catch (const objc3::task_error &error) {
      caught = mode == 3 &&
          *static_cast<const int *>(objc3_error_payload(error.native_handle())) == 73;
    }
    assert(caught && load(destroyed) == 2);
    group.wait();
  }
  int destroyed = 0;
  auto group = objc3::task_group_handle::create();
  auto child_frame = bridge_c_int(42, 0, &destroyed);
  auto child = objc3::task<int>::from_native(&child_frame).start(executor);
  group.add(child);
  group.close();
  objc3_actor_t *actor = nullptr;
  assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  auto parent = cpp_await_c_next(group.native_handle(), objc3_actor_identity(actor),
                                &destroyed).start_on(actor);
  pump(executor);
  assert(parent.result() == 42 && load(destroyed) == 2);
  group.wait();
  assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
  objc3_actor_release(actor);
}

template <class T> static objc3::task<T> private_result(T value) {
  co_await objc3::task_yield;
  co_return value;
}

static void private_nominal_types(objc3_executor_t *executor) {
  auto first = [value = 42] { return value; };
  auto second = [value = 96] { return value; };
  using First = decltype(first);
  using Second = decltype(second);
  const auto *a = objc3::task_detail::result_type<First>();
  const auto *b = objc3::task_detail::result_type<Second>();
  assert(a->identity_domain && b->identity_domain &&
         a->identity_domain != b->identity_domain);
  assert(!objc3_async_result_type_equal(a, b));
  auto frame = private_result(first).release_native();
  auto original = frame.context;
  bool rejected = false;
  try {
    (void)objc3::task<Second>::from_native(&frame);
  } catch (const objc3::service_failure &error) {
    rejected = error.status() == OBJC3_SERVICE_TYPE_MISMATCH;
  }
  assert(rejected && frame.context == original);
  auto task = objc3::task<First>::from_native(&frame).start(executor);
  pump(executor);
  assert(task.result()() == 42);
  struct {
    int value;
  } record{42};
  auto record_task = private_result(record).start(executor);
  pump(executor);
  assert(record_task.result().value == 42);
}

static void lifecycle(objc3_executor_t *executor) {
  int destroyed = 0;
  auto frame = bridge_c_int(42, 0, &destroyed);
  auto original = frame;
  objc3_task_t *raw = nullptr;
  assert(objc3_async_frame_start(&frame, nullptr, nullptr, &raw) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(!raw && frame.context == original.context &&
         frame.destroy == original.destroy);
  bool mismatch = false;
  try {
    (void)objc3::task<unsigned>::from_native(&frame);
  } catch (const objc3::service_failure &error) {
    mismatch = error.status() == OBJC3_SERVICE_TYPE_MISMATCH;
  }
  assert(mismatch && frame.context == original.context && load(destroyed) == 0);
  {
    auto lazy = objc3::task<int>::from_native(&frame);
    assert(!frame.context && load(destroyed) == 0);
    auto moved = std::move(lazy);
    auto exported = std::move(moved).release_native();
    assert(exported.context == original.context);
    assert(objc3_async_frame_dispose(&exported) == OBJC3_SERVICE_OK);
    assert(objc3_async_frame_dispose(&exported) == OBJC3_SERVICE_OK);
  }
  assert(load(destroyed) == 1);
  frame = bridge_c_int(42, 0, &destroyed);
  assert(objc3_async_frame_start(&frame, executor, nullptr, &raw) ==
         OBJC3_SERVICE_OK);
  auto owned = raw;
  mismatch = false;
  try {
    (void)objc3::task_handle<unsigned>::adopt_native(&raw);
  } catch (const objc3::service_failure &error) {
    mismatch = error.status() == OBJC3_SERVICE_TYPE_MISMATCH;
  }
  assert(mismatch && raw == owned);
  {
    auto handle = objc3::task_handle<int>::adopt_native(&raw);
    assert(!raw);
    auto copy = handle;
    raw = std::move(handle).release_native();
    assert(raw == owned && copy.native_handle() == owned);
    auto again = objc3::task_handle<int>::adopt_native(&raw);
    pump(executor);
    assert(copy.result() == 42 && again.result() == 42);
  }
  assert(load(destroyed) == 2);
  struct reentrant {
    objc3_async_frame *owner;
    int destroyed;
  } state{};
  frame = bridge_c_int(0, 0, nullptr);
  auto type = frame.result_type;
  assert(objc3_async_frame_dispose(&frame) == OBJC3_SERVICE_OK);
  frame = {sizeof(frame),
           OBJC3_ASYNC_ABI_VERSION,
           &state,
           [](objc3_task_t *, void *) {},
           [](void *context) {
             auto &state = *static_cast<reentrant *>(context);
             assert(!state.owner->context);
             assert(objc3_async_frame_dispose(state.owner) == OBJC3_SERVICE_OK);
             ++state.destroyed;
           },
           type};
  state.owner = &frame;
  assert(objc3_async_frame_dispose(&frame) == OBJC3_SERVICE_OK &&
         state.destroyed == 1);
}

static void typed_results(objc3_executor_t *executor) {
  int destroyed = 0;
  for (int value : {0, 42}) {
    auto frame = bridge_c_int(value, 4, &destroyed);
    auto task = objc3::task<int>::from_native(&frame).start(executor);
    pump(executor);
    assert(task.result() == value);
  }
  assert(load(destroyed) == 2);
  {
    auto frame = bridge_c_pair({40, 2.0}, &destroyed);
    auto task = objc3::task<bridge_pair>::from_native(&frame).start(executor);
    pump(executor);
    auto value = task.result();
    assert(value.left == 40 && value.right == 2.0);
  }
  {
    auto frame = bridge_c_wide(&destroyed);
    auto task = objc3::task<bridge_wide>::from_native(&frame).start(executor);
    pump(executor);
    assert(task.result().bytes[255] == 42);
  }
  {
    bridge_complex value;
    __real__ value = 40;
    __imag__ value = 2;
    auto frame = bridge_c_complex(value, &destroyed);
    auto task =
        objc3::task<bridge_complex>::from_native(&frame).start(executor);
    pump(executor);
    auto result = task.result();
    assert(__real__ result == 40 && __imag__ result == 2);
  }
  {
    auto frame = bridge_c_void(&destroyed);
    auto task = objc3::task<void>::from_native(&frame).start(executor);
    pump(executor);
    task.result();
  }
  {
    auto task = cpp_await_c(&destroyed).start(executor);
    pump(executor);
    assert(task.result() == 42);
  }
  {
    auto child = bridge_cpp_int(41);
    auto parent = bridge_c_chain(&child, &destroyed);
    assert(!child.context);
    auto task = objc3::task<int>::from_native(&parent).start(executor);
    pump(executor);
    assert(task.result() == 42);
  }
  for (int mode : {0, 2, 3}) {
    auto frame = bridge_c_int(42, mode, &destroyed);
    auto task = objc3::task<int>::from_native(&frame).start(executor);
    if (!mode) {
      pump(executor, 1);
      task.cancel();
    }
    pump(executor);
    bool caught = false;
    try {
      (void)task.result();
    } catch (const objc3::task_cancelled &) {
      caught = mode != 3;
    } catch (const objc3::task_error &) {
      caught = mode == 3;
    }
    assert(caught);
  }
  assert(load(destroyed) == 11);
  assert(bridge_c_consume_cpp(executor));
}

static void actor_and_worker(objc3_executor_t *executor) {
  objc3_actor_t *actor = nullptr;
  assert(objc3_actor_create(executor, &actor) == OBJC3_SERVICE_OK);
  int destroyed = 0;
  {
    auto frame = bridge_c_int(42, 0, &destroyed);
    auto task = objc3::task<int>::from_native(&frame).start_on(actor);
    pump(executor);
    assert(task.result() == 42);
  }
  objc3_actor_release(actor);
  assert(load(destroyed) == 1);
  objc3_executor_t *worker = nullptr;
  assert(objc3_executor_create(0, &worker) == OBJC3_SERVICE_OK);
  for (int i = 0; i != 64; ++i) {
    auto frame = bridge_c_int(42, 0, nullptr);
    auto task = objc3::task<int>::from_native(&frame).start(worker);
    if (i & 1)
      task.cancel();
    task.wait();
    try {
      assert(task.result() == 42);
    } catch (const objc3::task_cancelled &) {
      assert(i & 1);
    }
  }
  objc3_executor_release(worker);
}

#ifdef __OBJC__
static LOGICAL(id) cpp_object_value(int value) ASYNC {
  auto frame = bridge_c_object(value, 0, nullptr);
  RETURN AWAIT objc3::task<id>::from_native(&frame);
}
static LOGICAL(bridge_block) cpp_block_value(int value) ASYNC {
  auto frame = bridge_c_block(value, 0, nullptr);
  RETURN AWAIT objc3::task<bridge_block>::from_native(&frame);
}
static LOGICAL(objc3::task_handle<id>) next_object_handle(
    objc3::task_group_handle group) ASYNC {
  RETURN AWAIT group.next<id>();
}
static LOGICAL(objc3::task_handle<bridge_block>) next_block_handle(
    objc3::task_group_handle group) ASYNC {
  RETURN AWAIT group.next<bridge_block>();
}
extern "C" objc3_async_frame bridge_cpp_object(int value) {
  return cpp_object_value(value).release_native();
}
extern "C" objc3_async_frame bridge_cpp_block(int value) {
  return cpp_block_value(value).release_native();
}
static void references(objc3_executor_t *executor) {
  int before = bridge_object_destructions();
  int destroyed = 0;
  @autoreleasepool {
    for (int nil_value : {0, 1}) {
      auto frame = bridge_c_object(42, nil_value, &destroyed);
      auto task = objc3::task<id>::from_native(&frame).start(executor);
      pump(executor);
      id value = task.result();
      assert(nil_value ? value == nullptr
                       : bridge_object_number((__bridge void *)value) == 42);
      frame = bridge_c_block(40, nil_value, &destroyed);
      auto block_task =
          objc3::task<bridge_block>::from_native(&frame).start(executor);
      pump(executor);
      auto block = block_task.result();
      assert(nil_value ? block == nullptr : block(2) == 42);
    }
  }
  assert(load(destroyed) == 4 && bridge_object_destructions() == before + 2);
}
static void group_reference_results(objc3_executor_t *executor) {
  int before = bridge_object_destructions();
  int destroyed = 0;
  @autoreleasepool {
    for (int nil_value : {0, 1}) {
      auto group = objc3::task_group_handle::create();
      auto frame = bridge_c_object(42, nil_value, &destroyed);
      auto child = objc3::task<id>::from_native(&frame).start(executor);
      group.add(child);
      group.close();
      auto waiter = next_object_handle(group).start(executor);
      pump(executor);
      auto selected = waiter.result();
      assert(selected); // A present nil completion still has a task handle.
      auto drained = next_object_handle(group).start(executor);
      pump(executor);
      assert(!drained.result());
      child = {};
      waiter = {};
      drained = {};
      group = {};
      id value = selected.result();
      selected = {};
      assert(nil_value ? value == nullptr
                       : bridge_object_number((__bridge void *)value) == 42);

      group = objc3::task_group_handle::create();
      frame = bridge_c_block(40, nil_value, &destroyed);
      auto block_child = objc3::task<bridge_block>::from_native(&frame).start(executor);
      group.add(block_child);
      group.close();
      auto block_waiter = next_block_handle(group).start(executor);
      pump(executor);
      auto selected_block = block_waiter.result();
      assert(selected_block);
      block_child = {};
      block_waiter = {};
      group = {};
      auto block = selected_block.result();
      selected_block = {};
      assert(nil_value ? block == nullptr : block(2) == 42);
    }
  }
  assert(load(destroyed) == 4 && bridge_object_destructions() == before + 2);
}
#endif

int main() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
#ifdef __OBJC__
  @autoreleasepool {
#endif
    lifecycle(executor);
    private_nominal_types(executor);
    typed_results(executor);
    group_frame_lifetimes(executor);
    group_frame_cancellation(executor);
    group_frame_rejections(executor);
    group_frame_destinations(executor);
    group_frame_result_iteration(executor);
    group_frame_result_cancellation(executor);
    group_frame_result_outcomes(executor);
    actor_and_worker(executor);
#ifdef __OBJC__
    references(executor);
    group_reference_results(executor);
  }
#endif
  objc3_executor_release(executor);
}
