#include "provider.h"
#include <cassert>
#include <thread>
#include <vector>

static void pump(objc3_executor_t *executor, unsigned limit = 100000) {
  uint32_t count = 0;
  assert(objc3_executor_pump(executor, limit, &count) == OBJC3_SERVICE_OK);
}

#ifdef __OBJC__
static void owned_results(objc3_executor_t *executor) {
  static_assert(std::is_same_v<objc3::task_handle<NativeAsyncResult *>,
                               objc3::task_handle<NativeAsyncResult *__unsafe_unretained>>);
  static_assert(std::is_same_v<objc3::task<NativeAsyncResult *>,
                               objc3::task<NativeAsyncResult *__unsafe_unretained>>);
  static_assert(std::is_same_v<decltype(native_result_object(1)),
                               objc3::task<NativeAsyncResult *>>);
  static_assert(std::is_same_v<decltype(native_result_block(1)),
                               objc3::task<native_result_reader>>);
  static_assert(std::is_same_v<decltype(native_result_protocol(1)),
                               objc3::task<id<NativeAsyncResultProtocol>>>);
  const auto before = native_result_destructions();
  {
    auto result = native_result_object(42).start(executor);
    pump(executor);
    assert(native_result_destructions() == before);
    @autoreleasepool {
      auto copied = result;
      NativeAsyncResult *value = result.result();
      result = {};
      assert(native_result_number(value) == 42);
      assert(native_result_number(copied.result()) == 42);
      copied = {};
      // Extracted +0 results outlive every handle until this pool is drained.
      assert(native_result_number(value) == 42);
    }
    assert(native_result_destructions() == before + 1);
  }
  {
    auto result = native_result_block(40).start(executor);
    pump(executor);
    assert(native_result_destructions() == before + 1);
    @autoreleasepool {
      native_result_reader value = result.result();
      result = {};
      assert(value(2) == 42);
    }
    assert(native_result_destructions() == before + 2);
  }
  {
    auto result = native_result_protocol(42).start(executor);
    auto relay = native_result_relay(nullptr, 73).start(executor);
    auto nil = native_result_nil().start(executor);
    auto nil_block = native_result_nil_block().start(executor);
    auto null_value = native_identity(nullptr).start(executor);
    auto metaclass = native_result_class().start(executor);
    pump(executor);
    @autoreleasepool {
      id<NativeAsyncResultProtocol> value = result.result();
      NativeAsyncResult *relayed = relay.result();
      result = {};
      relay = {};
      assert([value number] == 42 && native_result_number(relayed) == 73);
      assert(nil.result() == (id)0 && nil_block.result() == 0);
      assert(null_value.result() == nullptr && metaclass.result() != (Class)0);
    }
    assert(native_result_destructions() == before + 4);
  }
  {
    auto source = native_result_object(42).start(executor);
    auto first = native_result_shared(source).start(executor);
    auto second = native_result_shared(source).start(executor);
    auto lazy = native_result_lazy(native_result_object(73)).start(executor);
    pump(executor);
    source = {};
    @autoreleasepool {
      NativeAsyncResult *left = first.result(), *right = second.result();
      NativeAsyncResult *moved = lazy.result();
      first = {};
      second = {};
      lazy = {};
      assert(left == right && native_result_number(left) == 42);
      assert(native_result_number(moved) == 73);
    }
    assert(native_result_destructions() == before + 6);
  }
  {
    auto failed = native_result_object(96, true).start(executor);
    auto cancelled = native_result_object(1).start(executor);
    auto abandoned = native_result_object(2).start(executor);
    pump(executor, 3);
    cancelled.cancel();
    abandoned = {};
    pump(executor);
    bool caught = false;
    try {
      (void)failed.result();
    } catch (int value) {
      caught = value == 96;
    }
    assert(caught);
    caught = false;
    try {
      (void)cancelled.result();
    } catch (const objc3::task_cancelled &) {
      caught = true;
    }
    assert(caught && native_result_destructions() == before + 9);
  }
}
#endif

static void checked_results(objc3_executor_t *executor) {
  auto state = std::make_shared<async_state>();
  const auto before = native_checked_destructions();
  {
    auto success = native_checked_recover(state, false).start(executor);
    auto recovered = native_checked_recover(state, true).start(executor);
    auto some = native_checked_optional(state, false).start(executor);
    auto none = native_checked_optional(state, true).start(executor);
    auto forced = native_checked_force(state).start(executor);
    auto sync = native_checked_sync(state, true).start(executor);
    auto boundaries = native_checked_native_handlers(state).start(executor);
    pump(executor);
    assert(success.result() == 42 && recovered.result() == 96);
    assert(some.result() == 42 && none.result() == 0 && forced.result() == 42);
    assert(sync.result() == 96 && boundaries.result() == 96);
  }
  assert(native_checked_destructions() == before + 7);
  {
    // Lazy creation does not run or synchronously throw. The completion owns
    // its error after both private coroutine frames have been reclaimed.
    auto failed = native_checked_parent(state, true).start(executor);
    pump(executor);
    assert(native_checked_destructions() == before + 7);
    auto copied = failed;
    bool caught = false;
    try { (void)failed.result(); }
    catch (const objc3::task_error &error) {
      assert(objc3::task_detail::__objc3_task_error_object(error.native_handle()));
      caught = true;
    }
    assert(caught);
    failed = {};
    assert(native_checked_destructions() == before + 7);
    copied = {};
    assert(native_checked_destructions() == before + 8);
  }
  {
    // Native exceptions bypass typed handlers and retain their native identity.
    auto cpp = native_checked_recover(state, true, 1).start(executor);
    auto objc = native_checked_recover(state, true, 2).start(executor);
    auto cleanup = native_checked_cleanup(state).start(executor);
    auto recovered_cleanup = native_checked_cleanup_caught(state).start(executor);
    pump(executor);
    bool caught = false;
    try { (void)cpp.result(); } catch (int value) { caught = value == 73; }
    assert(caught);
    caught = false;
    @try { (void)objc.result(); } @catch (id error) { caught = error != (id)0; }
    assert(caught);
    caught = false;
    try { (void)cleanup.result(); } catch (int value) { caught = value == 91; }
    assert(caught && recovered_cleanup.result() == 42);
  }
  assert(native_checked_destructions() == before + 12);
  {
    auto cancelled = native_checked_recover(state, true).start(executor);
    auto abandoned = native_checked_recover(state, true).start(executor);
    pump(executor, 6);
    cancelled.cancel();
    abandoned = {};
    pump(executor);
    bool caught = false;
    try { (void)cancelled.result(); }
    catch (const objc3::task_cancelled &) { caught = true; }
    assert(caught);
  }
  assert(state->entered == state->destroyed);
  assert(state->parents == state->deferred);
  const auto before_kinds = native_checked_destructions();
  const auto before_objects = native_result_destructions();
  {
    std::vector<objc3::task_handle<int>> tasks;
    for (unsigned kind = 0; kind != 4; ++kind)
      tasks.push_back(native_checked_result_kinds(state, kind).start(executor));
    auto untyped = native_checked_untyped_recover(state).start(executor);
    auto indirect = native_checked_indirect(state).start(executor);
    auto inlined = native_checked_inline(state).start(executor);
    pump(executor);
    assert(tasks[0].result() == 42);
    for (unsigned kind = 1; kind != 4; ++kind) assert(tasks[kind].result() == 96);
    assert(untyped.result() == 96 && indirect.result() == 96 && inlined.result() == 42);
  }
  assert(native_checked_destructions() == before_kinds + 12);
  assert(native_result_destructions() == before_objects + 5);
  assert(state->entered == state->destroyed);
  assert(state->parents == state->deferred);
}

int main() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  auto state = std::make_shared<async_state>();
  auto synchronous =
      +[](std::shared_ptr<async_state>, int value) { return value; };
  assert(native_overload(synchronous) == 1);
  assert(native_overload(native_leaf) == 2);
  assert(callback_owner().choose(native_leaf) == 2);
  assert(native_error_kind(static_cast<first_error_callback>(nullptr)) == 1);
  assert(native_error_kind(static_cast<second_error_callback>(nullptr)) == 2);
  assert(native_error_kind(static_cast<untyped_error_callback>(nullptr)) == 3);
  assert(native_error_kind(static_cast<void (*)()>(nullptr)) == 4);

  {
    auto lazy = native_parent(state, 42);
    assert(state->entered == 0);
    auto parent = std::move(lazy).start(executor);
    pump(executor, 1);
    assert(state->entered == 1 && state->resumed == 0);
    pump(executor, 1);
    assert(state->entered == 2 && state->resumed == 0);
    pump(executor);
    assert(parent.result() == 43 && state->resumed == 1);
    assert(state->entered == state->destroyed);
  }
  {
    auto parent = native_parent(state, -73).start(executor);
    pump(executor);
    bool caught = false;
    try {
      (void)parent.result();
    } catch (int value) {
      caught = value == -73;
    }
    assert(caught && state->entered == state->destroyed);
  }
  // Cancellation wakes the suspended parent and cancels its private child.
  {
    auto parent = native_parent(state, 7).start(executor);
    pump(executor, 1);
    parent.cancel();
    pump(executor);
    bool caught = false;
    try {
      (void)parent.result();
    } catch (const objc3::task_cancelled &) {
      caught = true;
    }
    assert(caught && state->entered == state->destroyed);
  }
  {
    auto indirect = native_indirect(state, 0).start(executor);
    auto inlined = native_inline(state, 9).start(executor);
    auto chain = native_chain(2048).start(executor);
    static_assert(std::is_same_v<decltype(native_const_text()),
                                 objc3::task<const std::string>>);
    auto text = native_text("owned string").start(executor);
    auto qualified = native_const_text().start(executor);
    auto aligned = native_aligned(89).start(executor);
    auto zero = native_identity(0).start(executor);
    auto falsity = native_identity(false).start(executor);
    auto generic =
        native_identity(std::string("generic result")).start(executor);
    auto nothing = native_void(state).start(executor);
    auto empty = native_empty().start(executor);
    pump(executor);
    assert(indirect.result() == 0 && inlined.result() == 9);
    assert(chain.result() == 2048 && text.result() == "owned string resumed");
    assert(qualified.result() == "qualified result");
    assert(aligned.result().value == 96 && zero.result() == 0);
    assert(!falsity.result() && generic.result() == "generic result");
    nothing.result();
    empty.result();
    assert(state->entered == state->destroyed);
  }
  {
    // Destroying a lazy native call releases copied parameters without running
    // its body. The weak reference observes the frame's final ownership.
    auto owned = std::make_shared<async_state>();
    std::weak_ptr<async_state> weak = owned;
    {
      auto abandoned = native_parent(owned, 3);
      owned.reset();
      assert(!weak.expired());
    }
    assert(weak.expired());
  }
#ifdef __OBJC__
  {
    auto owned = native_owned(false).start(executor);
    pump(executor, 1);
    assert(native_object_destructions() == 0);
    pump(executor);
    assert(owned.result() == 96 && native_object_destructions() == 1);
    auto failed = native_owned(true).start(executor);
    pump(executor);
    bool caught = false;
    try {
      (void)failed.result();
    } catch (int value) {
      caught = value == 96;
    }
    assert(caught && native_object_destructions() == 2);
    auto cancelled = native_owned(false).start(executor);
    pump(executor, 1);
    cancelled.cancel();
    pump(executor);
    caught = false;
    try {
      (void)cancelled.result();
    } catch (const objc3::task_cancelled &) {
      caught = true;
    }
    assert(caught && native_object_destructions() == 3);
    auto objective_error = native_object_error().start(executor);
    pump(executor);
    assert(native_object_destructions() == 3);
    caught = false;
    @try {
      (void)objective_error.result();
    } @catch (id value) {
      caught = native_exception_number(value) == 73;
    }
    assert(caught && native_object_destructions() == 3);
    objective_error = {};
    assert(native_object_destructions() == 4);
  }
#endif
  {
    auto abandoned = native_parent(state, 5).start(executor);
    pump(executor, 2);
    abandoned = {};
    pump(executor);
    assert(state->entered == state->destroyed);
  }
  assert(state->parents == state->deferred);
#ifdef __OBJC__
  owned_results(executor);
  checked_results(executor);
#endif
  objc3_executor_release(executor);
  // Worker execution exercises nested frame creation and publication races.
  assert(objc3_executor_create(0, &executor) == OBJC3_SERVICE_OK);
  {
    std::vector<objc3::task_handle<int>> tasks;
    for (int i = 0; i < 256; ++i)
      tasks.push_back(native_parent(state, i).start(executor));
    for (int i = 0; i < 256; ++i) {
      tasks[i].wait();
      assert(tasks[i].result() == i + 1);
    }
  }
  objc3_executor_release(executor);
  assert(state->entered == state->destroyed);
  assert(state->parents == state->deferred);
#ifdef __OBJC__
  objc3_executor_t *other = nullptr;
  assert(objc3_executor_create(0, &other) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(0, &executor) == OBJC3_SERVICE_OK);
  objc3_actor_t *first = nullptr, *second = nullptr;
  assert(objc3_actor_create(executor, &first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_create(other, &second) == OBJC3_SERVICE_OK);
  objc3_task_group_t *group = nullptr;
  assert(objc3_task_group_create(nullptr, &group) == OBJC3_SERVICE_OK);
  std::vector<objc3::task_handle<int>> hopped;
  for (unsigned index = 0; index != 32; ++index) {
    auto value = native_actor_owned(first, second, index % 2).start_on(first);
    assert(objc3_task_group_add(group, value.native_handle()) ==
           OBJC3_SERVICE_OK);
    hopped.push_back(std::move(value));
  }
  // A failed task cancels siblings, so each frame must preserve ownership on
  // success, exception propagation or cancellation during a cross-worker hop.
  assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  for (const auto &value : hopped) {
    try {
      assert(value.result() == 42);
    } catch (int error) {
      assert(error == 42);
    } catch (const objc3::task_cancelled &) {
    }
  }
  assert(native_object_destructions() == 36);
  hopped.clear();
  const auto before_results = native_result_destructions();
  std::vector<objc3::task_handle<NativeAsyncResult *>> objects;
  std::vector<objc3::task_handle<native_result_reader>> readers;
  for (int index = 0; index != 64; ++index) {
    objects.push_back(native_result_relay(second, index).start_on(first));
    readers.push_back(native_result_block(index).start_on(second));
  }
  for (int index = 0; index != 64; ++index) {
    objects[index].wait();
    readers[index].wait();
    @autoreleasepool {
      NativeAsyncResult *value = objects[index].result();
      auto reader = readers[index].result();
      objects[index] = {};
      readers[index] = {};
      assert(native_result_number(value) == index && reader(1) == index + 1);
    }
  }
  const auto before_checked_workers = native_checked_destructions();
  std::vector<objc3::task_handle<int>> checked_workers;
  for (unsigned index = 0; index != 64; ++index)
    checked_workers.push_back(native_checked_hop(first, second, state).start_on(first));
  for (auto &task : checked_workers) { task.wait(); assert(task.result() == 96); }
  checked_workers.clear();
  // Terminal completion can precede the final callback's frame destruction
  // and autorelease-pool drain. Observe both worker queues before counting.
  std::atomic<unsigned> drained{0};
  auto fence = +[](void *context) {
    auto &count = *static_cast<std::atomic<unsigned> *>(context);
    count.fetch_add(1, std::memory_order_release);
    count.notify_all();
  };
  assert(objc3_executor_enqueue(executor, fence, &drained) == OBJC3_SERVICE_OK);
  assert(objc3_executor_enqueue(other, fence, &drained) == OBJC3_SERVICE_OK);
  for (auto count = drained.load(std::memory_order_acquire); count != 2;
       count = drained.load(std::memory_order_acquire))
    drained.wait(count, std::memory_order_acquire);
  assert(native_result_destructions() == before_results + 128);
  assert(native_checked_destructions() == before_checked_workers + 64);
  objc3_task_group_release(group);
  assert(objc3_actor_close(first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(second) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(first) == OBJC3_SERVICE_OK);
  assert(objc3_actor_join(second) == OBJC3_SERVICE_OK);
  objc3_actor_release(second);
  objc3_actor_release(first);
  objc3_executor_release(other);
  objc3_executor_release(executor);
#endif
}
