#include "runtime/objc3_runtime.h"

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>

namespace {

constexpr std::int64_t kDispatchModulus = 2147483629LL;

struct SelectorSlot {
  std::string spelling_storage;
  objc3_runtime_selector_handle handle{};
};

struct RuntimeState {
  std::mutex mutex;
  std::uint64_t registered_image_count = 0;
  std::uint64_t registered_descriptor_total = 0;
  std::unordered_map<std::string, std::size_t> selector_index_by_name;
  std::deque<SelectorSlot> selector_slots;
};

RuntimeState &State() {
  static RuntimeState state;
  return state;
}

const objc3_runtime_selector_handle *LookupSelectorUnlocked(const char *selector) {
  if (selector == nullptr) {
    return nullptr;
  }

  RuntimeState &state = State();
  const auto found = state.selector_index_by_name.find(selector);
  if (found != state.selector_index_by_name.end()) {
    return &state.selector_slots[found->second].handle;
  }

  SelectorSlot slot;
  slot.spelling_storage = selector;
  state.selector_slots.push_back(std::move(slot));
  SelectorSlot &stored = state.selector_slots.back();
  stored.handle.selector = stored.spelling_storage.c_str();
  stored.handle.stable_id = static_cast<std::uint64_t>(state.selector_slots.size());
  const std::size_t index = state.selector_slots.size() - 1u;
  state.selector_index_by_name.emplace(stored.spelling_storage, index);
  return &stored.handle;
}

std::int64_t ComputeSelectorScore(const char *selector) {
  if (selector == nullptr) {
    return 0;
  }

  std::int64_t selector_score = 0;
  std::int64_t index = 1;
  const unsigned char *cursor =
      reinterpret_cast<const unsigned char *>(selector);
  while (*cursor != 0U) {
    selector_score =
        (selector_score + (static_cast<std::int64_t>(*cursor) * index)) %
        kDispatchModulus;
    ++cursor;
    ++index;
  }
  return selector_score;
}

int ComputeDispatchResult(int receiver, const char *selector, int a0, int a1,
                          int a2, int a3) {
  std::int64_t value = 41;
  value += static_cast<std::int64_t>(receiver) * 97;
  value += static_cast<std::int64_t>(a0) * 7;
  value += static_cast<std::int64_t>(a1) * 11;
  value += static_cast<std::int64_t>(a2) * 13;
  value += static_cast<std::int64_t>(a3) * 17;
  value += ComputeSelectorScore(selector) * 19;
  value %= kDispatchModulus;
  if (value < 0) {
    value += kDispatchModulus;
  }
  return static_cast<int>(value);
}

}  // namespace

int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image) {
  if (image == nullptr || image->module_name == nullptr ||
      image->module_name[0] == '\0') {
    return -1;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ++state.registered_image_count;
  state.registered_descriptor_total += image->class_descriptor_count;
  state.registered_descriptor_total += image->protocol_descriptor_count;
  state.registered_descriptor_total += image->category_descriptor_count;
  state.registered_descriptor_total += image->property_descriptor_count;
  state.registered_descriptor_total += image->ivar_descriptor_count;
  return 0;
}

const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  return LookupSelectorUnlocked(selector);
}

int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3) {
  RuntimeState &state = State();
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    (void)LookupSelectorUnlocked(selector);
  }
  return ComputeDispatchResult(receiver, selector, a0, a1, a2, a3);
}

void objc3_runtime_reset_for_testing(void) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.registered_image_count = 0;
  state.registered_descriptor_total = 0;
  state.selector_index_by_name.clear();
  state.selector_slots.clear();
}
