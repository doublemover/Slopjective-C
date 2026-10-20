#include "provider.h"
#include <cassert>
#include <thread>
#include <vector>

static void pump(objc3_executor_t *executor, unsigned limit = 100000) {
  uint32_t count = 0;
  assert(objc3_executor_pump(executor, limit, &count) == OBJC3_SERVICE_OK);
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
    assert(objc3_task_group_add(group, value.native_handle()) == OBJC3_SERVICE_OK);
    hopped.push_back(std::move(value));
  }
  // A failed task cancels siblings, so each frame must preserve ownership on
  // success, exception propagation or cancellation during a cross-worker hop.
  assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
  for (const auto &value : hopped) {
    try { assert(value.result() == 42); }
    catch (int error) { assert(error == 42); }
    catch (const objc3::task_cancelled &) {}
  }
  assert(native_object_destructions() == 36);
  hopped.clear();
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
