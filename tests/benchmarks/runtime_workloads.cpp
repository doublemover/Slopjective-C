// Sustained public runtime API workloads. Counters observe completed semantic
// work; timers include allocation, quiescence and teardown, but exclude output.
#include <objc3c/runtime/generic_collection_service.h>
#include <objc3c/runtime/objc3_runtime_services.h>
#include <objc3c/runtime/objc3_text.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <source_location>
#include <thread>
#include <vector>

struct Ledger {
  unsigned long long checksum = 0, operations = 0, copies = 0, destroys = 0;
  unsigned long long resumes = 0, callbacks = 0, live = 0;
};
static Ledger ledger;
static void require(bool condition, const std::source_location site = std::source_location::current()) {
  if (!condition) {
    std::fprintf(stderr, "runtime workload invariant failed at %u\n", site.line());std::exit(4);
  }
}
static void checked(int status, const std::source_location site = std::source_location::current()) {
  if (status) {
    std::fprintf(stderr, "runtime workload service status %d at %u\n", status, site.line());std::exit(3);
  }
}

static void text_work(unsigned size) {
  objc3_text_builder_v2_t builder = nullptr;
  checked(objc3_runtime_stdlib_text_builder_v2_create(&builder));
  ++ledger.operations;
  for (unsigned i = 0; i < size; ++i) {
    checked(objc3_runtime_stdlib_text_builder_v2_append_utf8(builder, "a\xc3\xa9", 3));
    ++ledger.operations;
  }
  objc3_text_v2_t text = nullptr, slice = nullptr, joined = nullptr;
  checked(objc3_runtime_stdlib_text_builder_v2_finish(builder, &text));
  checked(objc3_runtime_stdlib_text_builder_v2_release(builder));
  unsigned long long expected_bytes = 3ull * size;
  uint64_t bytes = 0, scalars = 0;
  checked(objc3_runtime_stdlib_text_v2_byte_count_checked(text, &bytes));
  checked(objc3_runtime_stdlib_text_v2_scalar_count_checked(text, &scalars));
  require(bytes == expected_bytes && scalars == 2ull * size);
  ledger.operations += 4;
  for (unsigned i = 0; i < size; ++i) {
    for (unsigned offset = 0; offset < 3; ++offset) {
      uint8_t byte = 0;
      checked(objc3_runtime_stdlib_text_v2_byte_at_checked(text, 3ull * i + offset, &byte));
      require(byte == static_cast<unsigned char>("a\xc3\xa9"[offset]));
      ledger.checksum += byte;
      ++ledger.operations;
    }
  }
  checked(objc3_runtime_stdlib_text_v2_slice_scalars(text, 2 * (size / 2), 2, &slice));
  checked(objc3_runtime_stdlib_text_v2_concat(text, slice, &joined));
  checked(objc3_runtime_stdlib_text_v2_byte_count_checked(joined, &bytes));
  require(bytes == expected_bytes + 3);
  int found = 0;uint64_t position = 0;
  checked(objc3_runtime_stdlib_text_v2_find_scalars(text, "\xc3\xa9", 2, 0, &found, &position));
  require(found == 1 && position == 1);
  checked(objc3_runtime_stdlib_text_v2_release_checked(joined));
  checked(objc3_runtime_stdlib_text_v2_release_checked(slice));
  checked(objc3_runtime_stdlib_text_v2_release_checked(text));
  ledger.operations += 7;
}

struct Payload { uint64_t value; };
static objc3_service_status copy_payload(void *destination, const void *source) {
  new (destination) Payload{static_cast<const Payload *>(source)->value};
  ++ledger.copies;++ledger.live;
  return OBJC3_SERVICE_OK;
}
static void destroy_payload(void *value) {
  static_cast<Payload *>(value)->~Payload();
  require(ledger.live != 0);--ledger.live;++ledger.destroys;
}
static objc3_service_status move_payload(void *destination, void *source) {
  checked(copy_payload(destination, source));destroy_payload(source);
  return OBJC3_SERVICE_OK;
}
static const objc3_value_ops payload_ops = {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION,
  sizeof(Payload), alignof(Payload), copy_payload, move_payload, destroy_payload};

static void array_work(unsigned size, unsigned seed) {
  std::vector<Payload> input(size);
  for (unsigned i = 0; i < size; ++i) input[i].value = i + seed;
  objc3_generic_array_t array = nullptr, retained = nullptr;
  checked(objc3_generic_array_create(&payload_ops, input.data(), size, &array));
  checked(objc3_generic_array_retain(array, &retained));
  ledger.operations += 2;
  for (unsigned i = 0; i < size; ++i) {
    const Payload replacement{2ull * i + seed};
    checked(objc3_generic_array_set(array, i, &replacement));
    ++ledger.operations;
  }
  for (unsigned i = 0; i < size; ++i) {
    alignas(Payload) unsigned char storage[sizeof(Payload)];
    checked(objc3_generic_array_get(retained, i, storage));
    auto *value = std::launder(reinterpret_cast<Payload *>(storage));
    require(value->value == 2ull * i + seed);
    ledger.checksum += value->value;
    destroy_payload(value);++ledger.operations;
  }
  uint64_t count = 0;
  checked(objc3_generic_array_count(array, &count));require(count == size);
  checked(objc3_generic_array_release(retained));
  checked(objc3_generic_array_release(array));
  ledger.operations += 3;
  require(ledger.live == 0 && ledger.copies == ledger.destroys);
}

struct Frame { unsigned value; unsigned phase = 0; uint64_t actor; };
static void resume_task(objc3_task_t *task, void *opaque) {
  auto *frame = static_cast<Frame *>(opaque);
  objc3_current_actor_facts facts{sizeof(objc3_current_actor_facts), OBJC3_ACTOR_SERVICE_VERSION, 0, 0};
  checked(objc3_actor_current_facts(&facts));require(facts.actor_identity == frame->actor);
  ++ledger.resumes;++ledger.operations;
  if (frame->phase++ == 0) {
    checked(objc3_task_schedule_resume(task));return;
  }
  require(frame->phase == 2);
  const objc3_completion completion{sizeof(objc3_completion), OBJC3_SERVICE_ABI_VERSION,
    OBJC3_COMPLETION_SUCCESS, 0, nullptr, nullptr, nullptr, nullptr};
  checked(objc3_task_complete(task, &completion));
}
static void completed_task(void *opaque, const objc3_completion *completion) {
  auto *frame = static_cast<Frame *>(opaque);
  require(completion->kind == OBJC3_COMPLETION_SUCCESS && frame->phase == 2);
  ledger.checksum += frame->value;++ledger.callbacks;++ledger.operations;
}
static void destroy_frame(void *opaque) {
  require(ledger.live != 0);--ledger.live;++ledger.destroys;++ledger.operations;
  delete static_cast<Frame *>(opaque);
}

static void task_work(unsigned size, unsigned seed) {
  objc3_executor_t *executor = nullptr;objc3_actor_t *actor = nullptr;
  objc3_task_group_t *group = nullptr;
  checked(objc3_executor_create(1, &executor));
  checked(objc3_actor_create(executor, &actor));
  checked(objc3_task_group_create(nullptr, &group));
  for (unsigned i = 0; i < size; ++i) {
    auto *frame = new Frame{i + seed, 0, objc3_actor_identity(actor)};
    ++ledger.live;
    const objc3_task_start_options options{sizeof(objc3_task_start_options), OBJC3_SERVICE_ABI_VERSION,
      nullptr, frame, resume_task, destroy_frame, completed_task, frame, actor, nullptr};
    objc3_task_t *task = nullptr;
    checked(objc3_task_start(&options, &task));
    checked(objc3_task_group_add(group, task));objc3_task_release(task);
    // Admission is deliberately bounded below the actor mailbox capacity. All
    // scales still perform the same total work and retain results until join.
    if (i % 256 == 255) {
      unsigned pumps = 0;
      while (ledger.live) {
        require(++pumps <= 256 * 4 + 32);
        uint32_t executed = 0;checked(objc3_executor_pump(executor, 64, &executed));
        require(executed != 0);
      }
    }
  }
  checked(objc3_task_group_close(group));
  unsigned pumps = 0;
  while (objc3_task_group_join(group) == OBJC3_SERVICE_NOT_READY) {
    require(++pumps <= size * 4 + 32);
    uint32_t executed = 0;checked(objc3_executor_pump(executor, 64, &executed));
    require(executed != 0);
  }
  checked(objc3_task_group_join(group));require(ledger.live == 0);
  unsigned consumed = 0;
  while (true) {
    objc3_task_t *task = nullptr;
    const auto status = objc3_task_group_next(group, &task);
    if (status == OBJC3_SERVICE_CLOSED) break;
    checked(status);require(task != nullptr);
    objc3_completion completion{};checked(objc3_task_get_completion(task, &completion));
    require(completion.kind == OBJC3_COMPLETION_SUCCESS);
    objc3_task_release(task);++consumed;
  }
  require(consumed == size);objc3_task_group_release(group);
  checked(objc3_actor_close(actor));checked(objc3_actor_join(actor));
  objc3_actor_release(actor);objc3_executor_release(executor);
}

static unsigned argument(const char *text, unsigned minimum, unsigned maximum) {
  char *end = nullptr;const auto value = std::strtoul(text, &end, 10);
  if (!*text || *end || value < minimum || value > maximum) std::exit(2);
  return static_cast<unsigned>(value);
}
int main(int argc, char **argv) {
  if (argc != 5 && argc != 6) return 2;
  const unsigned size = argument(argv[2], 1, 2048), rounds = argument(argv[3], 1, 128);
  const unsigned seed = argument(argv[4], 0, 1024);
  const unsigned delay = argc == 6 ? argument(argv[5], 0, 1) : 0;
  const auto started = std::chrono::steady_clock::now();
  for (unsigned round = 0; round < rounds; ++round) {
    if (!std::strcmp(argv[1], "text")) text_work(size);
    else if (!std::strcmp(argv[1], "generic-array")) array_work(size, seed);
    else if (!std::strcmp(argv[1], "task-group-actor")) task_work(size, seed);
    else return 2;
    // Bounded measurement control. The same completed semantic work and
    // lifetime oracles must hold with and without the extra timer cost.
    if (delay) std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now() - started).count();
  std::printf("{\"checksum\":%llu,\"completed_operations\":%llu,\"nanoseconds\":%lld,"
    "\"payload_copies\":%llu,\"destructions\":%llu,\"resumes\":%llu,\"callbacks\":%llu,\"live_values\":%llu}\n",
    ledger.checksum, ledger.operations, static_cast<long long>(nanos), ledger.copies,
    ledger.destroys, ledger.resumes, ledger.callbacks, ledger.live);
}
