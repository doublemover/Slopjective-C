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
objc3::task<int> chain(unsigned depth) {
  if (!depth)
    co_return 1;
  int value = co_await chain(depth - 1);
  co_return value + 1;
}
objc3::task<int> owned_parent(std::shared_ptr<coroutine_state> state) {
  cleanup guard{state};
  ++state->entered;
  int value = co_await leaf(state);
  ++state->continued;
  co_return value + 42;
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
objc3_current_actor_facts current_destination() {
  objc3_current_actor_facts facts{};
  facts.struct_size = sizeof(facts);
  facts.version = OBJC3_ACTOR_SERVICE_VERSION;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  return facts;
}
objc3::task<int> actor_child(uint64_t actor) {
  assert(current_destination().actor_identity == actor);
  co_await objc3::task_yield;
  assert(current_destination().actor_identity == actor);
  co_return 17;
}
objc3::task<int> actor_hops(objc3_actor_t *first, objc3_actor_t *second,
                            objc3_executor_t *executor,
                            std::shared_ptr<coroutine_state> state,
                            unsigned iterations) {
  cleanup guard{state};
  ++state->entered;
  const auto first_id = objc3_actor_identity(first);
  const auto second_id = objc3_actor_identity(second);
  const auto first_executor = objc3_actor_executor_identity(first);
  std::string live = "preserved while hopping";
  assert(current_destination().actor_identity == first_id);
  co_await objc3::hop_to(first);
  assert(co_await actor_child(first_id) == 17);
  for (unsigned index = 0; index != iterations; ++index) {
    co_await objc3::hop_to(second);
    auto facts = current_destination();
    assert(facts.actor_identity == second_id);
    assert(facts.executor_identity == objc3_actor_executor_identity(second));
    co_await objc3::task_yield;
    assert(current_destination().actor_identity == second_id);
    assert(co_await actor_child(second_id) == 17);
    co_await objc3::hop_to(executor);
    facts = current_destination();
    assert(facts.actor_identity == 0 && facts.executor_identity != 0);
    assert(facts.executor_identity != first_executor);
    co_await objc3::task_yield;
    assert(current_destination().executor_identity == facts.executor_identity);
    co_await objc3::hop_to(first);
    assert(current_destination().actor_identity == first_id);
    assert(live == "preserved while hopping");
    ++state->continued;
  }
  co_return 42;
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
  return chain(depth).start(executor);
}
objc3::task_handle<int> start_owned_child(
    objc3_executor_t *executor, std::shared_ptr<coroutine_state> state) {
  return owned_parent(std::move(state)).start(executor);
}
objc3::task_handle<std::string> start_failure(objc3_executor_t *executor) {
  return failure().start(executor);
}
objc3::task_handle<void> start_foreign_error(objc3_executor_t *executor) {
  return foreign_error().start(executor);
}
objc3::task_handle<int> start_actor_hops(
    objc3_actor_t *first, objc3_actor_t *second, objc3_executor_t *executor,
    std::shared_ptr<coroutine_state> state, unsigned iterations) {
  return actor_hops(first, second, executor, std::move(state), iterations)
      .start_on(first);
}
