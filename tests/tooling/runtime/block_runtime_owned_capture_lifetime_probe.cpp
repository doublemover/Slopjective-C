#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/blocks/block_descriptor.h"
#include "support/typed_dispatch_helpers.h"

#include <cstdio>

namespace {

struct ProbeBlockStorage {
  const objc3c::runtime::RuntimeBlockDescriptor *descriptor = nullptr;
  void (*copy)(void *) = nullptr;
  void (*dispose)(void *) = nullptr;
  int *captured_object = nullptr;
};

struct InstanceObservation {
  int found = 0;
  int retain_count = 0;
};

int g_copy_count = 0;
int g_dispose_count = 0;
int g_owned_capture_retain_count = 0;
int g_owned_capture_release_count = 0;
int g_last_retained_owned_capture = 0;
int g_last_released_owned_capture = 0;
int g_post_release_callback_count = 0;
bool g_allow_invoke_storage_access = true;

InstanceObservation ObserveInstance(int receiver) {
  objc3_runtime_instance_entry_snapshot snapshot{};
  (void)objc3_runtime_copy_instance_entry_for_testing(receiver, &snapshot);
  InstanceObservation observation;
  observation.found = snapshot.found;
  observation.retain_count = static_cast<int>(snapshot.retain_count);
  return observation;
}

int AllocateBoxInstance() {
  objc3_runtime_realized_class_entry_snapshot entry{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Box", &entry);
  if (entry.found == 0 || entry.class_receiver_identity == 0u) {
    return 0;
  }
  return objc3c::runtime::probe::DispatchTypedObjectReference(
      static_cast<int>(entry.class_receiver_identity), "alloc");
}

extern "C" int ProbeInvoke(void *storage, int a0, int a1, int a2, int a3) {
  if (!g_allow_invoke_storage_access) {
    ++g_post_release_callback_count;
    return -777;
  }
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_object == nullptr) {
    return -1;
  }
  const int captured = *block->captured_object;
  const InstanceObservation observation = ObserveInstance(captured);
  if (observation.found == 0) {
    return -2;
  }
  return 100 + observation.retain_count + a0 + a1 + a2 + a3;
}

extern "C" void ProbeCopy(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_object == nullptr) {
    return;
  }
  ++g_copy_count;
  const int captured = *block->captured_object;
  g_last_retained_owned_capture = captured;
  ++g_owned_capture_retain_count;
  *block->captured_object = objc3_runtime_retain_i32(captured);
}

extern "C" void ProbeDispose(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_object == nullptr) {
    return;
  }
  ++g_dispose_count;
  const int captured = *block->captured_object;
  g_last_released_owned_capture = captured;
  ++g_owned_capture_release_count;
  (void)objc3_runtime_release_i32(captured);
}

const objc3c::runtime::RuntimeBlockDescriptor &ProbeDescriptor() {
  static const objc3c::runtime::RuntimeBlockDescriptor descriptor{
      sizeof(ProbeBlockStorage),
      1,
      4,
      objc3c::runtime::kRuntimeBlockDescriptorPointerCaptureStorageFlag |
          objc3c::runtime::kRuntimeBlockDescriptorCopyHelperFlag |
          objc3c::runtime::kRuntimeBlockDescriptorDisposeHelperFlag,
      0,
      &ProbeInvoke};
  return descriptor;
}

void PrintInstanceObservation(const char *prefix,
                              const InstanceObservation &observation) {
  std::printf("\"%s_found\":%d,", prefix, observation.found);
  std::printf("\"%s_retain_count\":%d,", prefix, observation.retain_count);
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();
  g_copy_count = 0;
  g_dispose_count = 0;
  g_owned_capture_retain_count = 0;
  g_owned_capture_release_count = 0;
  g_last_retained_owned_capture = 0;
  g_last_released_owned_capture = 0;
  g_post_release_callback_count = 0;
  g_allow_invoke_storage_access = true;

  int captured_object = AllocateBoxInstance();
  const InstanceObservation initial_capture = ObserveInstance(captured_object);
  ProbeBlockStorage block{&ProbeDescriptor(), &ProbeCopy, &ProbeDispose,
                          &captured_object};

  const int handle =
      objc3_runtime_promote_block_i32(&block, sizeof(block), 1);
  const int copy_count_after_promotion = g_copy_count;
  const int owned_capture_retain_count_after_promotion =
      g_owned_capture_retain_count;
  const InstanceObservation after_promotion = ObserveInstance(captured_object);
  const int release_original_owner_result =
      captured_object > 0 ? objc3_runtime_release_i32(captured_object) : 0;
  const InstanceObservation after_original_release =
      ObserveInstance(captured_object);
  const int invoke_result =
      handle > 0 ? objc3_runtime_invoke_block_i32(handle, 4, 5, 0, 0) : 0;
  const int retain_block_result =
      handle > 0 ? objc3_runtime_retain_i32(handle) : 0;
  const int release_retained_block_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  const int dispose_count_before_final_release = g_dispose_count;
  const int owned_capture_release_count_before_final_release =
      g_owned_capture_release_count;
  const int final_release_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  const int dispose_count_after_final_release = g_dispose_count;
  const int owned_capture_release_count_after_final_release =
      g_owned_capture_release_count;
  const InstanceObservation after_final_release =
      ObserveInstance(captured_object);
  g_allow_invoke_storage_access = false;
  const int invoke_after_release_result =
      handle > 0 ? objc3_runtime_invoke_block_i32(handle, 9, 0, 0, 0) : 0;

  std::printf("{");
  std::printf("\"handle\":%d,", handle);
  std::printf("\"owned_capture_value\":%d,", captured_object);
  PrintInstanceObservation("initial_capture", initial_capture);
  std::printf("\"copy_count_after_promotion\":%d,",
              copy_count_after_promotion);
  std::printf("\"owned_capture_retain_count_after_promotion\":%d,",
              owned_capture_retain_count_after_promotion);
  std::printf("\"last_retained_owned_capture\":%d,",
              g_last_retained_owned_capture);
  PrintInstanceObservation("after_promotion_capture", after_promotion);
  std::printf("\"release_original_owner_result\":%d,",
              release_original_owner_result);
  PrintInstanceObservation("after_original_release_capture",
                           after_original_release);
  std::printf("\"invoke_result\":%d,", invoke_result);
  std::printf("\"retain_block_result\":%d,", retain_block_result);
  std::printf("\"release_retained_block_result\":%d,",
              release_retained_block_result);
  std::printf("\"dispose_count_before_final_release\":%d,",
              dispose_count_before_final_release);
  std::printf("\"owned_capture_release_count_before_final_release\":%d,",
              owned_capture_release_count_before_final_release);
  std::printf("\"final_release_result\":%d,", final_release_result);
  std::printf("\"dispose_count_after_final_release\":%d,",
              dispose_count_after_final_release);
  std::printf("\"owned_capture_release_count_after_final_release\":%d,",
              owned_capture_release_count_after_final_release);
  std::printf("\"last_released_owned_capture\":%d,",
              g_last_released_owned_capture);
  PrintInstanceObservation("after_final_release_capture",
                           after_final_release);
  std::printf("\"post_release_callback_count\":%d,",
              g_post_release_callback_count);
  std::printf("\"invoke_after_release_result\":%d",
              invoke_after_release_result);
  std::printf("}");
  return handle > 0 && captured_object > 0 ? 0 : 1;
}
