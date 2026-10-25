#include "service_group.h"

#include <exception>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <string>

namespace {
// The canonical Itanium type-name key for void is fixed by the shared async ABI.
constexpr char void_key[] = "objc3.async.v1:_ZTSv";
const objc3_async_result_type void_type{
    sizeof(objc3_async_result_type), OBJC3_ASYNC_ABI_VERSION, 0, 1,
    OBJC3_ASYNC_VOID, 0, void_key, sizeof(void_key) - 1, nullptr};

void publish(objc3_task_t *owner, const objc3_completion &completion) noexcept;
void fail(objc3_task_t *owner, objc3_service_status status) noexcept {
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  if (status == OBJC3_SERVICE_CANCELLED) {
    completion.kind = OBJC3_COMPLETION_CANCELLED;
  } else {
    completion.kind = OBJC3_COMPLETION_ERROR;
    if (objc3_async_error_create_service(status, &completion.error) !=
            OBJC3_SERVICE_OK || !completion.error)
      std::terminate();
  }
  const auto accepted = objc3_task_complete(owner, &completion);
  objc3_error_release(completion.error);
  if (accepted != OBJC3_SERVICE_OK && accepted != OBJC3_SERVICE_ALREADY_COMPLETED)
    std::terminate();
}
void publish(objc3_task_t *owner, const objc3_completion &completion) noexcept {
  const auto status = objc3_task_complete(owner, &completion);
  if (status != OBJC3_SERVICE_OK && status != OBJC3_SERVICE_ALREADY_COMPLETED)
    fail(owner, status);
}

struct TaskJoin {
  objc3_task_t *dependency;
  bool waiting = false;
  ~TaskJoin() { objc3_task_release(dependency); }
};
void destroy_task_join(void *context) noexcept {
  delete static_cast<TaskJoin *>(context);
}
void resume_task_join(objc3_task_t *owner, void *context) noexcept {
  auto &frame = *static_cast<TaskJoin *>(context);
  if (objc3_task_is_cancel_requested(owner)) {
    fail(owner, OBJC3_SERVICE_CANCELLED);
    return;
  }
  if (!frame.waiting) {
    // A compiler await starts this adapter as the caller's structured child.
    // Waiting for that caller would hide a self-cycle behind the extra frame.
    if (owner->await_target == frame.dependency->await_target ||
        owner->await_target->structured_parent == frame.dependency->await_target) {
      fail(owner, OBJC3_SERVICE_INVALID_ARGUMENT);
      return;
    }
    frame.waiting = true;
    const auto status = objc3_task_await(owner, frame.dependency);
    if (status != OBJC3_SERVICE_OK) fail(owner, status);
    return;
  }
  objc3_completion completion{};
  const auto status = objc3_task_get_completion(frame.dependency, &completion);
  if (status != OBJC3_SERVICE_OK) {
    fail(owner, status);
    return;
  }
  // Publication copies/retains the original result or error while the independent
  // dependency lease still owns the borrowed completion view. A failed payload
  // copy preserves its service status instead of manufacturing cancellation.
  publish(owner, completion);
}

struct GroupJoin {
  objc3_task_group_t *group;
  bool waiting = false;
  objc3_service_status status = OBJC3_SERVICE_NOT_READY;
  ~GroupJoin() { objc3_task_group_release(group); }
};
void destroy_group_join(void *context) noexcept {
  delete static_cast<GroupJoin *>(context);
}
void resume_group_join(objc3_task_t *owner, void *context) noexcept {
  auto &frame = *static_cast<GroupJoin *>(context);
  if (objc3_task_is_cancel_requested(owner)) {
    fail(owner, OBJC3_SERVICE_CANCELLED);
    return;
  }
  if (!frame.waiting) {
    auto status = objc3_task_group_close(frame.group);
    if (status == OBJC3_SERVICE_OK &&
        (objc3::services::task_group_contains_target(
             reinterpret_cast<objc3::services::TaskGroup *>(frame.group),
             owner->await_target) ||
         objc3::services::task_group_contains_target(
             reinterpret_cast<objc3::services::TaskGroup *>(frame.group),
             owner->await_target->structured_parent)))
      status = OBJC3_SERVICE_INVALID_ARGUMENT;
    if (status == OBJC3_SERVICE_OK) {
      frame.waiting = true;
      status = objc3_task_group_await(frame.group, owner, &frame.status);
    }
    if (status != OBJC3_SERVICE_OK) fail(owner, status);
    return;
  }
  if (frame.status != OBJC3_SERVICE_OK) {
    fail(owner, frame.status);
    return;
  }
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.result_type = &void_type;
  publish(owner, completion);
}

objc3_service_status validate_next_plan(const objc3_async_group_next_plan *plan) {
  if (!plan || plan->struct_size < sizeof(*plan) || plan->reserved ||
      !plan->element_type || !plan->result_type || !plan->result_ops)
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  if (plan->version != 1) return OBJC3_SERVICE_UNSUPPORTED_VERSION;
  for (auto *type : {plan->element_type, plan->result_type}) {
    const auto status = objc3_async_result_type_validate(type);
    if (status != OBJC3_SERVICE_OK) return status;
  }
  const auto status = objc3_value_ops_validate(plan->result_ops);
  if (status != OBJC3_SERVICE_OK) return status;
  const auto &result = *plan->result_type;
  const auto &element = *plan->element_type;
  if (result.kind != OBJC3_ASYNC_VALUE || plan->tag_size != 1 ||
      plan->tag_offset >= result.storage_size ||
      result.storage_size != plan->result_ops->payload_size ||
      result.storage_alignment != plan->result_ops->payload_alignment ||
      plan->payload_offset > result.storage_size ||
      element.storage_size > result.storage_size - plan->payload_offset ||
      result.storage_alignment < element.storage_alignment ||
      plan->payload_offset % element.storage_alignment ||
      (element.storage_size && plan->tag_offset >= plan->payload_offset &&
       plan->tag_offset - plan->payload_offset < element.storage_size))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  return OBJC3_SERVICE_OK;
}

struct GroupNext {
  objc3_task_group_t *group = nullptr;
  objc3_task_t *selected = nullptr;
  objc3_service_status status = OBJC3_SERVICE_NOT_READY;
  bool waiting = false, value_live = false;
  objc3_async_result_type element{}, result{};
  std::string element_identity, result_identity;
  objc3_value_ops ops{};
  uint64_t payload_offset = 0, tag_offset = 0;
  void *value = nullptr;
  std::size_t alignment = 0;
  ~GroupNext() {
    if (value_live) ops.destroy(value);
    if (value) ::operator delete(value, std::align_val_t(alignment));
    objc3_task_release(selected);
    objc3_task_group_release(group);
  }
};
void destroy_group_next(void *context) noexcept {
  delete static_cast<GroupNext *>(context);
}
void resume_group_next(objc3_task_t *owner, void *context) noexcept {
  auto &frame = *static_cast<GroupNext *>(context);
  if (objc3_task_is_cancel_requested(owner)) {
    fail(owner, OBJC3_SERVICE_CANCELLED);
    return;
  }
  if (!frame.waiting) {
    auto status = objc3_task_group_close(frame.group);
    auto *group = reinterpret_cast<objc3::services::TaskGroup *>(frame.group);
    if (status == OBJC3_SERVICE_OK &&
        (objc3::services::task_group_contains_target(group, owner->await_target) ||
         objc3::services::task_group_contains_target(
             group, owner->await_target->structured_parent)))
      status = OBJC3_SERVICE_INVALID_ARGUMENT;
    if (status == OBJC3_SERVICE_OK) {
      frame.waiting = true;
      status = objc3_task_group_await_next(frame.group, owner, &frame.element,
                                           &frame.status, &frame.selected);
    }
    if (status != OBJC3_SERVICE_OK) fail(owner, status);
    return;
  }
  if (frame.status != OBJC3_SERVICE_OK && frame.status != OBJC3_SERVICE_CLOSED) {
    fail(owner, frame.status);
    return;
  }
  objc3_completion child{};
  if (frame.status == OBJC3_SERVICE_OK) {
    const auto status = objc3_task_get_completion(frame.selected, &child);
    if (status != OBJC3_SERVICE_OK) { fail(owner, status); return; }
    if (child.kind != OBJC3_COMPLETION_SUCCESS) { publish(owner, child); return; }
    if (!child.result_type ||
        !objc3_async_result_type_equal(&frame.element, child.result_type) ||
        (frame.element.kind == OBJC3_ASYNC_VOID ? child.has_payload
                                               : !child.has_payload)) {
      fail(owner, OBJC3_SERVICE_TYPE_MISMATCH);
      return;
    }
  }
  frame.alignment = std::max<std::size_t>(alignof(std::max_align_t),
                                         frame.result.storage_alignment);
  frame.value = ::operator new(frame.result.storage_size,
                               std::align_val_t(frame.alignment), std::nothrow);
  if (!frame.value) { fail(owner, OBJC3_SERVICE_OUT_OF_MEMORY); return; }
  // Only the tag owns an active arm. Padding and inactive payload bytes are
  // initialized for determinism, but are never interpreted as a live value.
  std::memset(frame.value, 0, frame.result.storage_size);
  frame.value_live = true;
  if (frame.status == OBJC3_SERVICE_OK) {
    if (frame.element.kind != OBJC3_ASYNC_VOID) {
      const auto *ops = child.payload_ops;
      if (!ops || objc3_value_ops_validate(ops) != OBJC3_SERVICE_OK ||
          ops->payload_size != frame.element.storage_size ||
          ops->payload_alignment != frame.element.storage_alignment) {
        fail(owner, OBJC3_SERVICE_TYPE_MISMATCH);
        return;
      }
      const auto status = ops->copy_init(
          static_cast<char *>(frame.value) + frame.payload_offset, child.payload);
      if (status != OBJC3_SERVICE_OK) { fail(owner, status); return; }
    }
    static_cast<unsigned char *>(frame.value)[frame.tag_offset] = 1;
  }
  objc3_completion completion{};
  completion.struct_size = sizeof(completion);
  completion.version = OBJC3_SERVICE_ABI_VERSION;
  completion.kind = OBJC3_COMPLETION_SUCCESS;
  completion.has_payload = 1;
  completion.payload = frame.value;
  completion.payload_ops = &frame.ops;
  completion.result_type = &frame.result;
  publish(owner, completion);
}

struct Failure { objc3_service_status status; };
const Failure failures[] = {
    {OBJC3_SERVICE_INVALID_ARGUMENT}, {OBJC3_SERVICE_UNSUPPORTED_VERSION},
    {OBJC3_SERVICE_OUT_OF_MEMORY}, {OBJC3_SERVICE_ALREADY_COMPLETED},
    {OBJC3_SERVICE_NOT_READY}, {OBJC3_SERVICE_CLOSED},
    {OBJC3_SERVICE_CANCELLED}, {OBJC3_SERVICE_FAILED},
    {OBJC3_SERVICE_TYPE_MISMATCH}};
void destroy_failure(void *) noexcept {}
void resume_failure(objc3_task_t *owner, void *context) noexcept {
  fail(owner, objc3_task_is_cancel_requested(owner)
                  ? OBJC3_SERVICE_CANCELLED
                  : static_cast<const Failure *>(context)->status);
}
objc3_async_frame failure_frame(objc3_service_status status) noexcept {
  unsigned index = static_cast<unsigned>(status);
  if (!index || index > sizeof(failures) / sizeof(failures[0])) index = 1;
  // Static, immutable context makes factory failure allocation independent.
  return {sizeof(objc3_async_frame), OBJC3_ASYNC_ABI_VERSION,
          const_cast<Failure *>(&failures[index - 1]), resume_failure,
          destroy_failure, &void_type};
}
} // namespace

extern "C" objc3_service_status
objc3_async_frame_from_task(objc3_task_t *dependency,
                            objc3_async_frame *out_frame) {
  if (!dependency || !out_frame) return OBJC3_SERVICE_INVALID_ARGUMENT;
  auto *owned = objc3_task_retain(dependency);
  if (!owned) return OBJC3_SERVICE_NOT_READY;
  const objc3_async_result_type *type = nullptr;
  const auto status = objc3_task_get_result_type(owned, &type);
  if (status != OBJC3_SERVICE_OK) {
    objc3_task_release(owned);
    return status;
  }
  auto *frame = new (std::nothrow) TaskJoin{owned};
  if (!frame) {
    objc3_task_release(owned);
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  *out_frame = {sizeof(objc3_async_frame), OBJC3_ASYNC_ABI_VERSION, frame,
                resume_task_join, destroy_task_join, type};
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status
objc3_async_frame_from_group(objc3_task_group_t *group,
                             objc3_async_frame *out_frame) {
  if (!group || !out_frame) return OBJC3_SERVICE_INVALID_ARGUMENT;
  auto *owned = objc3_task_group_retain(group);
  if (!owned) return OBJC3_SERVICE_NOT_READY;
  auto *frame = new (std::nothrow) GroupJoin{owned};
  if (!frame) {
    objc3_task_group_release(owned);
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  }
  *out_frame = {sizeof(objc3_async_frame), OBJC3_ASYNC_ABI_VERSION, frame,
                resume_group_join, destroy_group_join, &void_type};
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_service_status
objc3_async_frame_from_group_next(objc3_task_group_t *group,
                                  const objc3_async_group_next_plan *plan,
                                  objc3_async_frame *out_frame) {
  if (!group || !out_frame) return OBJC3_SERVICE_INVALID_ARGUMENT;
  const auto status = validate_next_plan(plan);
  if (status != OBJC3_SERVICE_OK) return status;
  std::unique_ptr<GroupNext> frame(new (std::nothrow) GroupNext);
  if (!frame) return OBJC3_SERVICE_OUT_OF_MEMORY;
  frame->group = objc3_task_group_retain(group);
  if (!frame->group) return OBJC3_SERVICE_NOT_READY;
  try {
    frame->element = *plan->element_type;
    frame->result = *plan->result_type;
    frame->ops = *plan->result_ops;
    frame->element_identity.assign(frame->element.identity,
                                    frame->element.identity_size);
    frame->result_identity.assign(frame->result.identity,
                                   frame->result.identity_size);
  } catch (...) { return OBJC3_SERVICE_OUT_OF_MEMORY; }
  frame->element.identity = frame->element_identity.c_str();
  frame->result.identity = frame->result_identity.c_str();
  frame->payload_offset = plan->payload_offset;
  frame->tag_offset = plan->tag_offset;
  *out_frame = {sizeof(objc3_async_frame), OBJC3_ASYNC_ABI_VERSION, frame.get(),
                resume_group_next, destroy_group_next, &frame->result};
  frame.release();
  return OBJC3_SERVICE_OK;
}

extern "C" objc3_async_frame
__objc3_task_group_next_frame(objc3_task_group_t *group,
                              const objc3_async_group_next_plan *plan) {
  objc3_async_frame frame{};
  const auto status = objc3_async_frame_from_group_next(group, plan, &frame);
  return status == OBJC3_SERVICE_OK ? frame : failure_frame(status);
}

// Reserved physical entries for the standard C header's logical async
// declarations. Error frames are inspected before typed result extraction by
// native C await lowering, preserving factory failures for every result type.
extern "C" objc3_async_frame __objc3_task_join_frame(objc3_task_t *dependency) {
  objc3_async_frame frame{};
  const auto status = objc3_async_frame_from_task(dependency, &frame);
  return status == OBJC3_SERVICE_OK ? frame : failure_frame(status);
}
extern "C" objc3_async_frame
__objc3_task_group_join_frame(objc3_task_group_t *group) {
  objc3_async_frame frame{};
  const auto status = objc3_async_frame_from_group(group, &frame);
  return status == OBJC3_SERVICE_OK ? frame : failure_frame(status);
}
