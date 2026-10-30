// Bounded per-pass IR witness using the original native_leaf/native_parent
// implementations. The full client.cpp remains the broad source/runtime suite.
#include "provider.h"
#include <cassert>

static void pump(objc3_executor_t *executor, unsigned limit = 100000) {
  uint32_t count = 0;
  assert(objc3_executor_pump(executor, limit, &count) == OBJC3_SERVICE_OK);
}

int main() {
  objc3_executor_t *executor = nullptr;
  assert(objc3_executor_create(1, &executor) == OBJC3_SERVICE_OK);
  auto state = std::make_shared<async_state>();
  {
    auto unstarted = native_parent(state, 0);
    assert(state->entered == 0 && state->deferred == 0);
    assert(state.use_count() > 1);
  }
  assert(state.use_count() == 1 && state->destroyed == 0);
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
    assert(state->entered == state->destroyed && state->deferred == 1);
  }
  {
    auto parent = native_parent(state, -73).start(executor);
    pump(executor);
    bool caught = false;
    try { (void)parent.result(); } catch (int value) { caught = value == -73; }
    assert(caught && state->entered == state->destroyed && state->deferred == 2);
  }
  {
    auto parent = native_parent(state, 7).start(executor);
    pump(executor, 1);
    parent.cancel();
    pump(executor);
    bool caught = false;
    try { (void)parent.result(); }
    catch (const objc3::task_cancelled &) { caught = true; }
    assert(caught && state->entered == state->destroyed && state->deferred == 3);
  }
  assert(state->parents == 3 && state->resumed == 1);
  objc3_executor_release(executor);
}
