#ifndef OBJC3_RUNTIME_CARRIER_SERVICE_H
#define OBJC3_RUNTIME_CARRIER_SERVICE_H

#include "runtime/public/objc3_runtime_services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_optional_value objc3_optional_value_t;
typedef struct objc3_result_value objc3_result_value_t;

typedef enum objc3_optional_tag {
  OBJC3_OPTIONAL_UNINITIALIZED = 0,
  OBJC3_OPTIONAL_NONE = 1,
  OBJC3_OPTIONAL_SOME = 2
} objc3_optional_tag;

typedef enum objc3_result_tag {
  OBJC3_RESULT_UNINITIALIZED = 0,
  OBJC3_RESULT_SUCCESS = 1,
  OBJC3_RESULT_ERROR = 2
} objc3_result_tag;

/* Views borrow the active payload until the carrier is mutated or destroyed.
 * has_payload is separate from the tag so Some<void>/success<void> differ from
 * absence. A zero-valued payload is still present and has_payload is one. */
typedef struct objc3_carrier_payload_view {
  uint32_t struct_size;
  uint32_t version;
  uint32_t has_payload;
  uint32_t reserved;
  const void *payload;
  const objc3_value_ops *payload_ops;
} objc3_carrier_payload_view;

objc3_service_status objc3_optional_value_create(
    objc3_optional_value_t **out_value);
void objc3_optional_value_destroy(objc3_optional_value_t *value);
objc3_optional_tag objc3_optional_value_get_tag(
    const objc3_optional_value_t *value);
objc3_service_status objc3_optional_value_get_payload(
    const objc3_optional_value_t *value, objc3_carrier_payload_view *out_view);

objc3_service_status objc3_optional_none_init(objc3_optional_value_t *value);
objc3_service_status objc3_optional_some_copy_init(
    objc3_optional_value_t *value, const void *payload,
    const objc3_value_ops *ops);
objc3_service_status objc3_optional_some_move_init(
    objc3_optional_value_t *value, void *payload,
    const objc3_value_ops *ops);
objc3_service_status objc3_optional_copy_init(
    objc3_optional_value_t *destination, const objc3_optional_value_t *source);
objc3_service_status objc3_optional_move_init(
    objc3_optional_value_t *destination, objc3_optional_value_t *source);
/* Assignment is transactional: a failed payload init preserves destination.
 * Self-copy and self-move are successful no-ops. */
objc3_service_status objc3_optional_copy_assign(
    objc3_optional_value_t *destination, const objc3_optional_value_t *source);
objc3_service_status objc3_optional_move_assign(
    objc3_optional_value_t *destination, objc3_optional_value_t *source);

objc3_service_status objc3_result_value_create(objc3_result_value_t **out_value);
void objc3_result_value_destroy(objc3_result_value_t *value);
objc3_result_tag objc3_result_value_get_tag(const objc3_result_value_t *value);
objc3_service_status objc3_result_value_get_payload(
    const objc3_result_value_t *value, objc3_carrier_payload_view *out_view);

objc3_service_status objc3_result_success_copy_init(
    objc3_result_value_t *value, const void *payload,
    const objc3_value_ops *ops);
objc3_service_status objc3_result_success_move_init(
    objc3_result_value_t *value, void *payload,
    const objc3_value_ops *ops);
objc3_service_status objc3_result_error_copy_init(
    objc3_result_value_t *value, const void *payload,
    const objc3_value_ops *ops);
objc3_service_status objc3_result_error_move_init(
    objc3_result_value_t *value, void *payload,
    const objc3_value_ops *ops);
objc3_service_status objc3_result_copy_init(
    objc3_result_value_t *destination, const objc3_result_value_t *source);
objc3_service_status objc3_result_move_init(
    objc3_result_value_t *destination, objc3_result_value_t *source);
objc3_service_status objc3_result_copy_assign(
    objc3_result_value_t *destination, const objc3_result_value_t *source);
objc3_service_status objc3_result_move_assign(
    objc3_result_value_t *destination, objc3_result_value_t *source);

#ifdef __cplusplus
}
#endif
#endif
