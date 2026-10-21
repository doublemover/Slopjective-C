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
    actor_and_worker(executor);
#ifdef __OBJC__
    references(executor);
  }
#endif
  objc3_executor_release(executor);
}
