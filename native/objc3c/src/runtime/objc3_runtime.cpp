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
  std::uint64_t next_expected_registration_order_ordinal = 1;
  std::uint64_t last_successful_registration_order_ordinal = 0;
  int last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  std::string last_registered_module_name;
  std::string last_registered_translation_unit_identity_key;
  std::string last_rejected_module_name;
  std::string last_rejected_translation_unit_identity_key;
  std::uint64_t last_rejected_registration_order_ordinal = 0;
  std::unordered_map<std::string, std::uint64_t>
      registration_order_by_identity_key;
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

std::uint64_t DescriptorTotal(
    const objc3_runtime_image_descriptor *image) {
  return image->class_descriptor_count + image->protocol_descriptor_count +
         image->category_descriptor_count + image->property_descriptor_count +
         image->ivar_descriptor_count;
}

const char *StableCString(const std::string &text) {
  return text.empty() ? nullptr : text.c_str();
}

void MarkRejectedRegistrationUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    int status) {
  state.last_registration_status = status;
  state.last_rejected_module_name =
      image != nullptr && image->module_name != nullptr ? image->module_name : "";
  state.last_rejected_translation_unit_identity_key =
      image != nullptr && image->translation_unit_identity_key != nullptr
          ? image->translation_unit_identity_key
          : "";
  state.last_rejected_registration_order_ordinal =
      image != nullptr ? image->registration_order_ordinal : 0;
}

void ClearRejectedRegistrationUnlocked(RuntimeState &state) {
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
}

}  // namespace

// M254-D001 runtime-bootstrap-api anchor: registration, selector lookup,
// dispatch, snapshot, and reset remain the frozen bootstrap runtime boundary.
// D002/D003 may extend image walk and reset behavior, but they must preserve
// this surface and its fail-closed status/result contract.
int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (image == nullptr || image->module_name == nullptr ||
      image->module_name[0] == '\0' ||
      image->translation_unit_identity_key == nullptr ||
      image->translation_unit_identity_key[0] == '\0' ||
      image->registration_order_ordinal == 0) {
    MarkRejectedRegistrationUnlocked(
        state, image, OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  if (state.registration_order_by_identity_key.find(
          image->translation_unit_identity_key) !=
      state.registration_order_by_identity_key.end()) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY;
  }

  if (image->registration_order_ordinal !=
      state.next_expected_registration_order_ordinal) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION;
  }

  ++state.registered_image_count;
  state.registered_descriptor_total += DescriptorTotal(image);
  state.next_expected_registration_order_ordinal =
      image->registration_order_ordinal + 1;
  state.last_successful_registration_order_ordinal =
      image->registration_order_ordinal;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_registered_module_name = image->module_name;
  state.last_registered_translation_unit_identity_key =
      image->translation_unit_identity_key;
  state.registration_order_by_identity_key.emplace(
      image->translation_unit_identity_key, image->registration_order_ordinal);
  ClearRejectedRegistrationUnlocked(state);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
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

int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->registered_image_count = state.registered_image_count;
  snapshot->registered_descriptor_total = state.registered_descriptor_total;
  snapshot->next_expected_registration_order_ordinal =
      state.next_expected_registration_order_ordinal;
  snapshot->last_successful_registration_order_ordinal =
      state.last_successful_registration_order_ordinal;
  snapshot->last_registration_status = state.last_registration_status;
  snapshot->last_registered_module_name =
      StableCString(state.last_registered_module_name);
  snapshot->last_registered_translation_unit_identity_key =
      StableCString(state.last_registered_translation_unit_identity_key);
  snapshot->last_rejected_module_name =
      StableCString(state.last_rejected_module_name);
  snapshot->last_rejected_translation_unit_identity_key =
      StableCString(state.last_rejected_translation_unit_identity_key);
  snapshot->last_rejected_registration_order_ordinal =
      state.last_rejected_registration_order_ordinal;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_msgsend_i32(int receiver, const char *selector, int a0,
                                 int a1, int a2, int a3) {
  // D003 compatibility bridge: existing emitted objects still default to the
  // legacy lowering symbol while the canonical runtime API remains
  // objc3_runtime_dispatch_i32.
  return objc3_runtime_dispatch_i32(receiver, selector, a0, a1, a2, a3);
}

void objc3_runtime_reset_for_testing(void) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.registered_image_count = 0;
  state.registered_descriptor_total = 0;
  state.next_expected_registration_order_ordinal = 1;
  state.last_successful_registration_order_ordinal = 0;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_registered_module_name.clear();
  state.last_registered_translation_unit_identity_key.clear();
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
  state.registration_order_by_identity_key.clear();
  state.selector_index_by_name.clear();
  state.selector_slots.clear();
}
