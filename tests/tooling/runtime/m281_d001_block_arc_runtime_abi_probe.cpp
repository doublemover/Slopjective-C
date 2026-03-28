#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>

namespace {

struct ProbeCaptureState {
  int base = 0;
  int copy_count = 0;
  int dispose_count = 0;
};

struct ProbeBlockStorage {
  int (*invoke)(void *, int, int, int, int) = nullptr;
  void (*copy)(void *) = nullptr;
  void (*dispose)(void *) = nullptr;
  ProbeCaptureState *capture = nullptr;
};

extern "C" int ProbeInvoke(void *storage, int a0, int a1, int a2, int a3) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return -1;
  }
  return block->capture->base + a0 + a1 + a2 + a3;
}

extern "C" void ProbeCopy(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return;
  }
  ++block->capture->copy_count;
}

extern "C" void ProbeDispose(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return;
  }
  ++block->capture->dispose_count;
}

void PrintJsonStringOrNull(const char *value) {
  if (value == nullptr) {
    std::printf("null");
    return;
  }
  std::printf("\"");
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(value);
       *cursor != '\0'; ++cursor) {
    switch (*cursor) {
      case '\\':
      case '\"':
        std::printf("\\%c", *cursor);
        break;
      case '\n':
        std::printf("\\n");
        break;
      case '\r':
        std::printf("\\r");
        break;
      case '\t':
        std::printf("\\t");
        break;
      default:
        std::printf("%c", *cursor);
        break;
    }
  }
  std::printf("\"");
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  objc3_runtime_push_autoreleasepool_scope();
  const int retained = objc3_runtime_retain_i32(77);
  const int autoreleased = objc3_runtime_autorelease_i32(retained);
  const int released = objc3_runtime_release_i32(retained);

  ProbeCaptureState capture{7, 0, 0};
  ProbeBlockStorage block{&ProbeInvoke, &ProbeCopy, &ProbeDispose, &capture};
  const int handle =
      objc3_runtime_promote_block_i32(&block, sizeof(block), 1);
  const int invoke_result =
      handle > 0 ? objc3_runtime_invoke_block_i32(handle, 1, 2, 3, 4) : 0;
  const int retain_handle_result =
      handle > 0 ? objc3_runtime_retain_i32(handle) : 0;
  const int release_handle_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  const int final_release_result =
      handle > 0 ? objc3_runtime_release_i32(handle) : 0;
  objc3_runtime_pop_autoreleasepool_scope();

  objc3_runtime_block_arc_runtime_abi_snapshot abi{};
  objc3_runtime_arc_debug_state_snapshot arc{};
  const int abi_status =
      objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing(&abi);
  const int arc_status = objc3_runtime_copy_arc_debug_state_for_testing(&arc);

  std::printf("{");
  std::printf("\"retained\":%d,", retained);
  std::printf("\"autoreleased\":%d,", autoreleased);
  std::printf("\"released\":%d,", released);
  std::printf("\"handle\":%d,", handle);
  std::printf("\"invoke_result\":%d,", invoke_result);
  std::printf("\"retain_handle_result\":%d,", retain_handle_result);
  std::printf("\"release_handle_result\":%d,", release_handle_result);
  std::printf("\"final_release_result\":%d,", final_release_result);
  std::printf("\"copy_count\":%d,", capture.copy_count);
  std::printf("\"dispose_count\":%d,", capture.dispose_count);
  std::printf("\"abi_status\":%d,", abi_status);
  std::printf("\"arc_status\":%d,", arc_status);
  std::printf("\"private_runtime_abi_ready\":%llu,",
              static_cast<unsigned long long>(abi.private_runtime_abi_ready));
  std::printf("\"public_runtime_header_unchanged\":%llu,",
              static_cast<unsigned long long>(abi.public_runtime_header_unchanged));
  std::printf("\"deterministic\":%llu,",
              static_cast<unsigned long long>(abi.deterministic));
  std::printf("\"live_runtime_block_handle_count\":%llu,",
              static_cast<unsigned long long>(abi.live_runtime_block_handle_count));
  std::printf("\"block_promote_call_count\":%llu,",
              static_cast<unsigned long long>(abi.block_promote_call_count));
  std::printf("\"block_invoke_call_count\":%llu,",
              static_cast<unsigned long long>(abi.block_invoke_call_count));
  std::printf("\"retain_call_count\":%llu,",
              static_cast<unsigned long long>(abi.retain_call_count));
  std::printf("\"release_call_count\":%llu,",
              static_cast<unsigned long long>(abi.release_call_count));
  std::printf("\"autorelease_call_count\":%llu,",
              static_cast<unsigned long long>(abi.autorelease_call_count));
  std::printf("\"autoreleasepool_push_count\":%llu,",
              static_cast<unsigned long long>(abi.autoreleasepool_push_count));
  std::printf("\"autoreleasepool_pop_count\":%llu,",
              static_cast<unsigned long long>(abi.autoreleasepool_pop_count));
  std::printf("\"current_property_read_count\":%llu,",
              static_cast<unsigned long long>(abi.current_property_read_count));
  std::printf("\"current_property_write_count\":%llu,",
              static_cast<unsigned long long>(abi.current_property_write_count));
  std::printf("\"current_property_exchange_count\":%llu,",
              static_cast<unsigned long long>(abi.current_property_exchange_count));
  std::printf("\"weak_current_property_load_count\":%llu,",
              static_cast<unsigned long long>(abi.weak_current_property_load_count));
  std::printf("\"weak_current_property_store_count\":%llu,",
              static_cast<unsigned long long>(abi.weak_current_property_store_count));
  std::printf("\"last_promoted_block_handle\":%d,", abi.last_promoted_block_handle);
  std::printf("\"last_promote_has_pointer_capture_storage\":%d,",
              abi.last_promote_has_pointer_capture_storage);
  std::printf("\"last_invoked_block_handle\":%d,", abi.last_invoked_block_handle);
  std::printf("\"last_block_invoke_result\":%d,", abi.last_block_invoke_result);
  std::printf("\"last_retain_value\":%d,", abi.last_retain_value);
  std::printf("\"last_release_value\":%d,", abi.last_release_value);
  std::printf("\"last_autorelease_value\":%d,", abi.last_autorelease_value);
  std::printf("\"arc_retain_call_count\":%llu,",
              static_cast<unsigned long long>(arc.retain_call_count));
  std::printf("\"arc_release_call_count\":%llu,",
              static_cast<unsigned long long>(arc.release_call_count));
  std::printf("\"arc_autorelease_call_count\":%llu,",
              static_cast<unsigned long long>(arc.autorelease_call_count));
  std::printf("\"arc_autoreleasepool_push_count\":%llu,",
              static_cast<unsigned long long>(arc.autoreleasepool_push_count));
  std::printf("\"arc_autoreleasepool_pop_count\":%llu,",
              static_cast<unsigned long long>(arc.autoreleasepool_pop_count));
  std::printf("\"block_promote_symbol\":");
  PrintJsonStringOrNull(abi.block_promote_symbol);
  std::printf(",\"block_invoke_symbol\":");
  PrintJsonStringOrNull(abi.block_invoke_symbol);
  std::printf(",\"retain_symbol\":");
  PrintJsonStringOrNull(abi.retain_symbol);
  std::printf(",\"release_symbol\":");
  PrintJsonStringOrNull(abi.release_symbol);
  std::printf(",\"autorelease_symbol\":");
  PrintJsonStringOrNull(abi.autorelease_symbol);
  std::printf(",\"autoreleasepool_push_symbol\":");
  PrintJsonStringOrNull(abi.autoreleasepool_push_symbol);
  std::printf(",\"autoreleasepool_pop_symbol\":");
  PrintJsonStringOrNull(abi.autoreleasepool_pop_symbol);
  std::printf(",\"current_property_read_symbol\":");
  PrintJsonStringOrNull(abi.current_property_read_symbol);
  std::printf(",\"current_property_write_symbol\":");
  PrintJsonStringOrNull(abi.current_property_write_symbol);
  std::printf(",\"current_property_exchange_symbol\":");
  PrintJsonStringOrNull(abi.current_property_exchange_symbol);
  std::printf(",\"bind_current_property_context_symbol\":");
  PrintJsonStringOrNull(abi.bind_current_property_context_symbol);
  std::printf(",\"clear_current_property_context_symbol\":");
  PrintJsonStringOrNull(abi.clear_current_property_context_symbol);
  std::printf(",\"weak_current_property_load_symbol\":");
  PrintJsonStringOrNull(abi.weak_current_property_load_symbol);
  std::printf(",\"weak_current_property_store_symbol\":");
  PrintJsonStringOrNull(abi.weak_current_property_store_symbol);
  std::printf(",\"arc_debug_state_snapshot_symbol\":");
  PrintJsonStringOrNull(abi.arc_debug_state_snapshot_symbol);
  std::printf(",\"runtime_abi_boundary_model\":");
  PrintJsonStringOrNull(abi.runtime_abi_boundary_model);
  std::printf(",\"block_runtime_model\":");
  PrintJsonStringOrNull(abi.block_runtime_model);
  std::printf(",\"arc_runtime_model\":");
  PrintJsonStringOrNull(abi.arc_runtime_model);
  std::printf(",\"fail_closed_model\":");
  PrintJsonStringOrNull(abi.fail_closed_model);
  std::printf("}");

  const bool ok = abi_status == OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
                  arc_status == OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
                  retained == 77 && autoreleased == 77 && released == 77 &&
                  handle > 0 && invoke_result == 17 && retain_handle_result == handle &&
                  release_handle_result == handle && final_release_result == handle &&
                  abi.private_runtime_abi_ready == 1 &&
                  abi.public_runtime_header_unchanged == 1 && abi.deterministic == 1 &&
                  abi.live_runtime_block_handle_count == 0 &&
                  abi.block_promote_call_count == 1 && abi.block_invoke_call_count == 1 &&
                  abi.retain_call_count == 2 && abi.release_call_count == 3 &&
                  abi.autorelease_call_count == 1 && abi.autoreleasepool_push_count == 1 &&
                  abi.autoreleasepool_pop_count == 1 && abi.current_property_read_count == 0 &&
                  abi.current_property_write_count == 0 &&
                  abi.current_property_exchange_count == 0 &&
                  abi.weak_current_property_load_count == 0 &&
                  abi.weak_current_property_store_count == 0 &&
                  abi.last_promoted_block_handle == handle &&
                  abi.last_promote_has_pointer_capture_storage == 1 &&
                  abi.last_invoked_block_handle == handle &&
                  abi.last_block_invoke_result == 17 && abi.last_retain_value == handle &&
                  abi.last_release_value == handle && abi.last_autorelease_value == 77 &&
                  arc.retain_call_count == abi.retain_call_count &&
                  arc.release_call_count == abi.release_call_count &&
                  arc.autorelease_call_count == abi.autorelease_call_count &&
                  arc.autoreleasepool_push_count == abi.autoreleasepool_push_count &&
                  arc.autoreleasepool_pop_count == abi.autoreleasepool_pop_count;
  return ok ? 0 : 1;
}
