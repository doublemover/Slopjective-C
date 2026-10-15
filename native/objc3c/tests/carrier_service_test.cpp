#include "runtime/services/carrier_service.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>

namespace {
struct Counts {
  int copies = 0;
  int moves = 0;
  int destroys = 0;
  bool fail_copy = false;
  bool fail_move = false;
} counts;

template <typename T>
objc3_service_status Copy(void *destination, const void *source) {
  if (counts.fail_copy) return OBJC3_SERVICE_CLOSED;
  std::memcpy(destination, source, sizeof(T));
  ++counts.copies;
  return OBJC3_SERVICE_OK;
}
template <typename T>
objc3_service_status Move(void *destination, void *source) {
  if (counts.fail_move) return OBJC3_SERVICE_CLOSED;
  std::memcpy(destination, source, sizeof(T));
  std::memset(source, 0, sizeof(T));
  ++counts.moves;
  return OBJC3_SERVICE_OK;
}
void Destroy(void *) { ++counts.destroys; }
objc3_service_status CopyVoid(void *, const void *) {
  ++counts.copies;
  return OBJC3_SERVICE_OK;
}
objc3_service_status MoveVoid(void *, void *) {
  ++counts.moves;
  return OBJC3_SERVICE_OK;
}

template <typename T>
objc3_value_ops Ops() {
  return {sizeof(objc3_value_ops), OBJC3_VALUE_OPS_VERSION, sizeof(T),
          alignof(T), &Copy<T>, &Move<T>, &Destroy};
}
void ResetCounts() { counts = {}; }
void CheckView(const objc3_carrier_payload_view &view, bool has_payload) {
  assert(view.struct_size == sizeof(view));
  assert(view.version == OBJC3_VALUE_OPS_VERSION);
  assert(view.has_payload == (has_payload ? 1u : 0u));
  assert((view.payload != nullptr) == has_payload);
  assert((view.payload_ops != nullptr) == has_payload);
}
}  // namespace

int main() {
  const auto int_ops = Ops<int>();
  const auto int64_ops = Ops<int64_t>();
  const auto bool_ops = Ops<bool>();
  const objc3_value_ops void_ops{sizeof(objc3_value_ops),
                                OBJC3_VALUE_OPS_VERSION, 0, 1,
                                &CopyVoid, &MoveVoid, &Destroy};

  // None is a real initialized tag and never destroys its uninitialized bytes.
  ResetCounts();
  objc3_optional_value_t *optional = nullptr;
  assert(objc3_optional_value_create(&optional) == OBJC3_SERVICE_OK);
  assert(objc3_optional_value_get_tag(optional) == OBJC3_OPTIONAL_UNINITIALIZED);
  assert(objc3_optional_none_init(optional) == OBJC3_SERVICE_OK);
  assert(objc3_optional_value_get_tag(optional) == OBJC3_OPTIONAL_NONE);
  objc3_carrier_payload_view view{sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_optional_value_get_payload(optional, &view) == OBJC3_SERVICE_OK);
  CheckView(view, false);
  objc3_optional_value_destroy(optional);
  assert(counts.destroys == 0);

  // Present zero, false, and a full-width i64 are independent of presence.
  ResetCounts();
  assert(objc3_optional_value_create(&optional) == OBJC3_SERVICE_OK);
  int zero = 0;
  assert(objc3_optional_some_copy_init(optional, &zero, &int_ops) ==
         OBJC3_SERVICE_OK);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_optional_value_get_payload(optional, &view) == OBJC3_SERVICE_OK);
  CheckView(view, true);
  assert(*static_cast<const int *>(view.payload) == 0);
  assert(view.payload_ops->payload_size == sizeof(int));
  objc3_optional_value_destroy(optional);
  assert(counts.destroys == 1);

  ResetCounts();
  bool no = false;
  assert(objc3_optional_value_create(&optional) == OBJC3_SERVICE_OK);
  assert(objc3_optional_some_copy_init(optional, &no, &bool_ops) ==
         OBJC3_SERVICE_OK);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_optional_value_get_payload(optional, &view) == OBJC3_SERVICE_OK);
  assert(view.has_payload == 1 && !*static_cast<const bool *>(view.payload));
  objc3_optional_value_destroy(optional);
  assert(counts.destroys == 1);

  ResetCounts();
  int64_t wide = INT64_C(0x1020304050607080);
  assert(objc3_optional_value_create(&optional) == OBJC3_SERVICE_OK);
  assert(objc3_optional_some_copy_init(optional, &wide, &int64_ops) ==
         OBJC3_SERVICE_OK);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_optional_value_get_payload(optional, &view) == OBJC3_SERVICE_OK);
  assert(*static_cast<const int64_t *>(view.payload) == wide);
  assert(view.payload_ops->payload_size == sizeof(int64_t));
  objc3_optional_value_destroy(optional);

  // A void payload is present through its zero-sized descriptor.
  assert(objc3_optional_value_create(&optional) == OBJC3_SERVICE_OK);
  assert(objc3_optional_some_copy_init(optional, nullptr, &void_ops) ==
         OBJC3_SERVICE_OK);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_optional_value_get_payload(optional, &view) == OBJC3_SERVICE_OK);
  CheckView(view, true);
  assert(view.payload_ops->payload_size == 0);
  assert(objc3_optional_value_get_tag(optional) == OBJC3_OPTIONAL_SOME);
  objc3_optional_value_destroy(optional);

  // Failed initialization leaves a blank destination blank, with no destroy.
  ResetCounts();
  assert(objc3_optional_value_create(&optional) == OBJC3_SERVICE_OK);
  counts.fail_copy = true;
  assert(objc3_optional_some_copy_init(optional, &zero, &int_ops) ==
         OBJC3_SERVICE_CLOSED);
  assert(objc3_optional_value_get_tag(optional) == OBJC3_OPTIONAL_UNINITIALIZED);
  assert(counts.destroys == 0);
  counts.fail_copy = false;
  assert(objc3_optional_some_copy_init(optional, &zero, &int_ops) ==
         OBJC3_SERVICE_OK);
  assert(objc3_optional_value_get_tag(optional) == OBJC3_OPTIONAL_SOME);
  objc3_optional_value_destroy(optional);
  assert(counts.destroys == 1);

  // Copy assignment replaces exactly once, tolerates self-copy, and preserves
  // the old initialized value when construction of the replacement fails.
  ResetCounts();
  objc3_optional_value_t *left = nullptr, *right = nullptr;
  assert(objc3_optional_value_create(&left) == OBJC3_SERVICE_OK);
  assert(objc3_optional_value_create(&right) == OBJC3_SERVICE_OK);
  int one = 1;
  assert(objc3_optional_some_copy_init(left, &one, &int_ops) == OBJC3_SERVICE_OK);
  assert(objc3_optional_some_copy_init(right, &zero, &int_ops) == OBJC3_SERVICE_OK);
  assert(objc3_optional_copy_assign(left, right) == OBJC3_SERVICE_OK);
  assert(counts.destroys == 1);
  assert(objc3_optional_copy_assign(left, left) == OBJC3_SERVICE_OK);
  assert(counts.copies == 3);
  counts.fail_copy = true;
  assert(objc3_optional_copy_assign(left, right) == OBJC3_SERVICE_CLOSED);
  assert(objc3_optional_value_get_tag(left) == OBJC3_OPTIONAL_SOME);
  counts.fail_copy = false;
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_optional_value_get_payload(left, &view) == OBJC3_SERVICE_OK);
  assert(*static_cast<const int *>(view.payload) == 0);
  objc3_optional_value_destroy(left);
  objc3_optional_value_destroy(right);
  assert(counts.destroys == 3);

  // Move assignment calls move_init once, empties the source without destroying
  // its moved-from payload, and destroys the replacement once at final release.
  ResetCounts();
  assert(objc3_optional_value_create(&left) == OBJC3_SERVICE_OK);
  assert(objc3_optional_value_create(&right) == OBJC3_SERVICE_OK);
  assert(objc3_optional_some_copy_init(left, &one, &int_ops) == OBJC3_SERVICE_OK);
  int moved_value = 73;
  assert(objc3_optional_some_copy_init(right, &moved_value, &int_ops) ==
         OBJC3_SERVICE_OK);
  assert(objc3_optional_move_assign(left, right) == OBJC3_SERVICE_OK);
  assert(counts.moves == 1 && counts.destroys == 1);
  assert(objc3_optional_value_get_tag(right) == OBJC3_OPTIONAL_UNINITIALIZED);
  assert(objc3_optional_move_assign(left, left) == OBJC3_SERVICE_OK);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_optional_value_get_payload(left, &view) == OBJC3_SERVICE_OK);
  assert(*static_cast<const int *>(view.payload) == moved_value);
  objc3_optional_value_destroy(left);
  objc3_optional_value_destroy(right);
  assert(counts.destroys == 2);

  // Result's success/error tag selects the payload even when both are zero.
  ResetCounts();
  objc3_result_value_t *result = nullptr;
  assert(objc3_result_value_create(&result) == OBJC3_SERVICE_OK);
  assert(objc3_result_success_copy_init(result, &zero, &int_ops) ==
         OBJC3_SERVICE_OK);
  assert(objc3_result_value_get_tag(result) == OBJC3_RESULT_SUCCESS);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_result_value_get_payload(result, &view) == OBJC3_SERVICE_OK);
  CheckView(view, true);
  assert(*static_cast<const int *>(view.payload) == 0);
  assert(objc3_result_error_copy_init(result, &zero, &int_ops) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  objc3_result_value_destroy(result);
  assert(counts.destroys == 1);

  assert(objc3_result_value_create(&result) == OBJC3_SERVICE_OK);
  assert(objc3_result_error_copy_init(result, &zero, &int_ops) ==
         OBJC3_SERVICE_OK);
  assert(objc3_result_value_get_tag(result) == OBJC3_RESULT_ERROR);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_result_value_get_payload(result, &view) == OBJC3_SERVICE_OK);
  assert(view.has_payload == 1 && *static_cast<const int *>(view.payload) == 0);
  objc3_result_value_destroy(result);
  assert(counts.destroys == 2);

  // Zero-sized success/error payloads retain valid typed descriptors.
  assert(objc3_result_value_create(&result) == OBJC3_SERVICE_OK);
  assert(objc3_result_success_copy_init(result, nullptr, &void_ops) ==
         OBJC3_SERVICE_OK);
  view = {sizeof(view), 0, 0, 0, nullptr, nullptr};
  assert(objc3_result_value_get_payload(result, &view) == OBJC3_SERVICE_OK);
  CheckView(view, true);
  assert(view.payload_ops->payload_size == 0);
  objc3_result_value_destroy(result);
  assert(objc3_result_value_create(&result) == OBJC3_SERVICE_OK);
  assert(objc3_result_error_copy_init(result, nullptr, &void_ops) ==
         OBJC3_SERVICE_OK);
  objc3_result_value_destroy(result);
  assert(counts.destroys == 4);

  // Descriptor version/alignment rejection is explicit.
  assert(objc3_optional_value_create(&optional) == OBJC3_SERVICE_OK);
  auto bad = int_ops;
  bad.version += 1;
  assert(objc3_optional_some_copy_init(optional, &zero, &bad) ==
         OBJC3_SERVICE_UNSUPPORTED_VERSION);
  bad = int_ops;
  bad.payload_alignment = 3;
  assert(objc3_optional_some_copy_init(optional, &zero, &bad) ==
         OBJC3_SERVICE_INVALID_ARGUMENT);
  assert(objc3_optional_value_get_tag(optional) == OBJC3_OPTIONAL_UNINITIALIZED);
  objc3_optional_value_destroy(optional);
  return 0;
}
