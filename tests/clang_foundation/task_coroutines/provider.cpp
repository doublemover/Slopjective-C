#include "provider.h"
#include <cassert>
#include <stdexcept>

extern "C" __attribute__((noinline)) void
objc3_coroutine_debug_checkpoint(int value, const char *text) {
  asm volatile("" : : "r"(value), "r"(text) : "memory");
}

namespace {
struct cleanup {
  std::shared_ptr<coroutine_state> state;
  ~cleanup() { ++state->destroyed; }
};
objc3::task<int> leaf(std::shared_ptr<coroutine_state> state) {
  cleanup guard{state};
  ++state->entered;
  while (!state->ready.load())
    co_await objc3::task_yield;
  co_return 0;
}
objc3::task<int> parent(objc3::task_handle<int> dependency,
                        std::shared_ptr<coroutine_state> state) {
  cleanup guard{state};
  ++state->entered;
  std::string live = "preserved across suspension";
  objc3_current_actor_facts before{};
  before.struct_size = sizeof(before);
  before.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&before) == OBJC3_SERVICE_OK);
  int value = co_await dependency;
  objc3_coroutine_debug_checkpoint(value + 42, live.c_str());
  objc3_current_actor_facts after{};
  after.struct_size = sizeof(after);
  after.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&after) == OBJC3_SERVICE_OK);
  assert(before.executor_identity != 0 &&
         before.executor_identity == after.executor_identity);
  assert(live == "preserved across suspension");
  ++state->continued;
  co_return value + 42;
}
objc3::task<int> chain(objc3_executor_t *executor, unsigned depth) {
  if (!depth)
    co_return 1;
  int value = co_await chain(executor, depth - 1).start(executor);
  co_return value + 1;
}
objc3::task<std::string> failure() {
  co_await objc3::task_yield;
  throw std::runtime_error("native coroutine error");
  co_return "unreachable";
}
objc3::task<void> foreign_error() {
  int payload = 73;
  objc3_error_t *error = nullptr;
  assert(objc3_error_create(&payload,
                            &objc3::task_detail::value_ops<int>::descriptor,
                            &error) == OBJC3_SERVICE_OK);
  objc3::task_error owned(error);
  objc3_error_release(error);
  throw owned;
  co_return;
}
} // namespace
objc3::task_handle<int> start_leaf(objc3_executor_t *executor,
                                   std::shared_ptr<coroutine_state> state) {
  return leaf(std::move(state)).start(executor);
}
objc3::task_handle<int> start_parent(objc3_executor_t *executor,
                                     objc3::task_handle<int> dependency,
                                     std::shared_ptr<coroutine_state> state) {
  return parent(std::move(dependency), std::move(state)).start(executor);
}
objc3::task_handle<int> start_chain(objc3_executor_t *executor,
                                    unsigned depth) {
  return chain(executor, depth).start(executor);
}
objc3::task_handle<std::string> start_failure(objc3_executor_t *executor) {
  return failure().start(executor);
}
objc3::task_handle<void> start_foreign_error(objc3_executor_t *executor) {
  return foreign_error().start(executor);
}
