// Compiler-generated coroutine frames through the production typed task bridge.
// Move-only parameter leases observe frame destruction after final suspension.
#include <objc3c/runtime/objc3_task.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <utility>
#include <vector>

struct Ledger {
  unsigned long long checksum = 0, operations = 0, created = 0, destroys = 0;
  unsigned long long resumes = 0, completed = 0, cancelled = 0, live = 0;
};
static Ledger ledger;
static void require(bool condition) { if (!condition) std::exit(4); }
static void checked(objc3_service_status status) { require(status == OBJC3_SERVICE_OK); }
struct FrameLease {
  bool owns = true;
  FrameLease() { ++ledger.created;++ledger.live;++ledger.operations; }
  FrameLease(const FrameLease &) = delete;
  FrameLease(FrameLease &&other) noexcept : owns(std::exchange(other.owns, false)) {}
  ~FrameLease() {
    if (owns) { require(ledger.live != 0);--ledger.live;++ledger.destroys;++ledger.operations; }
  }
};
static objc3::task<unsigned> generated(unsigned value, FrameLease lease) {
  require(lease.owns);++ledger.resumes;++ledger.operations;
  ledger.checksum += value;
  try {
    co_await objc3::task_yield;
    ++ledger.resumes;++ledger.operations;
  } catch (const objc3::task_cancelled &) {
    ++ledger.resumes;++ledger.operations;++ledger.cancelled;
    throw;
  }
  co_return value;
}
static void work(unsigned size, unsigned seed, bool cancel) {
  objc3_executor_t *executor = nullptr;checked(objc3_executor_create(1, &executor));
  for (unsigned start = 0; start < size; start += 256) {
    const unsigned count = size-start < 256 ? size-start : 256;
    objc3_task_group_t *group = nullptr;checked(objc3_task_group_create(nullptr, &group));
    std::vector<objc3::task_handle<unsigned>> children;
    children.reserve(count);
    const auto before = ledger.resumes;
    for (unsigned i = 0; i < count; ++i) {
      auto child = generated(start+i+seed, FrameLease{}).start(executor);
      checked(objc3_task_group_add(group, child.native_handle()));
      children.push_back(std::move(child));
    }
    if (cancel) {
      // Exactly one FIFO dispatch per newly admitted frame puts every child at
      // its first yield. No continuation can overtake the remaining first jobs.
      for (unsigned i = 0; i < count; ++i) {
        uint32_t executed = 0;checked(objc3_executor_pump(executor, 1, &executed));require(executed == 1);
      }
      require(ledger.resumes == before+count && ledger.live == count);
    }
    checked(objc3_task_group_close(group));
    if (cancel) checked(objc3_task_group_cancel(group));
    unsigned pumps = 0;
    while (objc3_task_group_join(group) == OBJC3_SERVICE_NOT_READY) {
      require(++pumps <= count*4+32);
      uint32_t executed = 0;checked(objc3_executor_pump(executor, 64, &executed));require(executed != 0);
    }
    checked(objc3_task_group_join(group));require(ledger.live == 0);
    for (unsigned i = 0; i < count; ++i) {
      try {
        const auto result = children[i].result();
        require(!cancel && result == start+i+seed);ledger.checksum += result;
      } catch (const objc3::task_cancelled &) { require(cancel); }
      ++ledger.operations;
    }
    unsigned consumed = 0;
    while (true) {
      objc3_task_t *task = nullptr;const auto status = objc3_task_group_next(group, &task);
      if (status == OBJC3_SERVICE_CLOSED) break;
      checked(status);require(task != nullptr);
      objc3_completion completion{};checked(objc3_task_get_completion(task, &completion));
      require(completion.kind == (cancel ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS));
      objc3_task_release(task);++ledger.completed;++ledger.operations;++consumed;
    }
    require(consumed == count);objc3_task_group_release(group);
  }
  objc3_executor_release(executor);
}
static unsigned argument(const char *text, unsigned minimum, unsigned maximum) {
  char *end = nullptr;const auto value = std::strtoul(text, &end, 10);
  if (!*text || *end || value < minimum || value > maximum) std::exit(2);
  return static_cast<unsigned>(value);
}
int main(int argc, char **argv) {
  if (argc != 5 && argc != 6) return 2;
  const bool cancel = !std::strcmp(argv[1], "task-coroutine-cancel");
  if (!cancel && std::strcmp(argv[1], "task-coroutine")) return 2;
  const unsigned size = argument(argv[2], 1, 2048), rounds = argument(argv[3], 1, 128);
  const unsigned seed = argument(argv[4], 0, 1024), delay = argc == 6 ? argument(argv[5], 0, 1) : 0;
  const auto started = std::chrono::steady_clock::now();
  for (unsigned round = 0; round < rounds; ++round) {
    work(size, seed, cancel);
    if (delay) std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now()-started).count();
  std::printf("{\"checksum\":%llu,\"completed_operations\":%llu,\"nanoseconds\":%lld,"
    "\"payload_copies\":0,\"destructions\":%llu,\"resumes\":%llu,\"callbacks\":0,\"live_values\":%llu,"
    "\"frame_leases_created\":%llu,\"completed_tasks\":%llu,\"cancelled\":%llu}\n",
    ledger.checksum, ledger.operations, static_cast<long long>(nanos), ledger.destroys,
    ledger.resumes, ledger.live, ledger.created, ledger.completed, ledger.cancelled);
}
