#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum objc3_runtime_registration_status_code {
  OBJC3_RUNTIME_REGISTRATION_STATUS_OK = 0,
  OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR = -1,
  OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY = -2,
  OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION = -3,
} objc3_runtime_registration_status_code;

typedef struct objc3_runtime_image_descriptor {
  const char *module_name;
  const char *translation_unit_identity_key;
  uint64_t registration_order_ordinal;
  uint64_t class_descriptor_count;
  uint64_t protocol_descriptor_count;
  uint64_t category_descriptor_count;
  uint64_t property_descriptor_count;
  uint64_t ivar_descriptor_count;
} objc3_runtime_image_descriptor;

typedef struct objc3_runtime_selector_handle {
  const char *selector;
  uint64_t stable_id;
} objc3_runtime_selector_handle;

typedef struct objc3_runtime_registration_state_snapshot {
  uint64_t registered_image_count;
  uint64_t registered_descriptor_total;
  uint64_t next_expected_registration_order_ordinal;
  uint64_t last_successful_registration_order_ordinal;
  int last_registration_status;
  const char *last_registered_module_name;
  const char *last_registered_translation_unit_identity_key;
  const char *last_rejected_module_name;
  const char *last_rejected_translation_unit_identity_key;
  uint64_t last_rejected_registration_order_ordinal;
} objc3_runtime_registration_state_snapshot;

// M255-D001 lookup-dispatch-runtime anchor: the canonical runtime-owned lookup
// and dispatch surface remains objc3_runtime_lookup_selector plus
// objc3_runtime_dispatch_i32 over objc3_runtime_selector_handle. Later
// selector-table, method-cache, and protocol/category-aware slow-path work must
// extend this ABI without renaming or silently narrowing it.
// M256-D001 class-realization-runtime anchor: the current runtime-owned class
// realization boundary still fits behind this public lookup/dispatch ABI.
// Class/metaclass graph walking, category attachment, and protocol-aware
// negative runtime checks must preserve these entrypoints rather than widening
// the public ABI prematurely.
// M256-D002 metaclass-graph-root-class anchor: the realized class graph and
// root-class baseline also stay behind this same ABI, with extra proof surface
// exposed only through private testing snapshots.
// M256-D003 category-attachment-protocol-conformance anchor: realized category
// attachment and runtime protocol-conformance queries must continue to consume
// this ABI plus private testing snapshots rather than widening the public
// runtime surface.
// M256-D004 canonical-runnable-object-sample anchor: runtime-owned builtin
// alloc/new/init resolution and inherited class dispatch for canonical object
// samples also stay behind this same ABI rather than adding dedicated public
// allocation or sample-only helper entrypoints.
// M257-D001 property-layout-runtime anchor: runtime-owned property/layout
// consumption and synthesized accessor-backed storage must stay behind this
// same ABI surface.
// M257-D002 instance-allocation-layout-runtime anchor: the same ABI now also
// carries true per-instance allocation and slot-backed synthesized accessors
// without widening the public runtime header.
// M260-D001 runtime-memory-management-api anchor: retain/release/autorelease,
// current-property, and weak helper entrypoints remain private lowering/runtime
// mechanics rather than public runtime ABI. The stable public header must stay
// at registration, lookup, dispatch, and testing snapshots until later runtime
// work makes a deliberate widening decision.
// M262-D001 runtime-arc-helper-api-surface anchor: later ARC-specific runtime
// helper freezing keeps that same rule in place, including autoreleasepool
// hooks. No dedicated public ARC helper ABI is published here.
// M267-D001 error-runtime/bridge-helper anchor: the first runnable Part 6
// error-object storage, bridge normalization, and catch-dispatch helpers also
// remain private bootstrap-internal runtime ABI. No public error helper or
// foreign-exception header widening is published here.
// M268-D001 continuation/runtime-helper anchor: the first Part 7 continuation
// allocation, handoff, and resume helpers follow the same rule. They remain on
// the bootstrap-internal private helper surface and do not widen this public
// runtime header while async lowering is still in the non-suspending slice.
// M268-D002 live continuation/runtime integration anchor: the supported
// non-suspending async slice now executes through that same private helper
// surface, but this public runtime header still does not widen.
// M269-D001 scheduler/executor runtime anchor: the first truthful task
// scheduler/executor helper boundary follows the same rule. Task creation,
// task-group control, cancellation observation, executor hops, and task-state
// snapshots remain bootstrap-internal private runtime ABI and do not widen
// this public runtime header.
// M269-D002 live task runtime anchor: the supported helper-backed task slice
// now executes through that same private runtime surface, but this public
// runtime header still does not widen.
// M254-D001 runtime-bootstrap-api anchor: these exported C ABI type names and
// function signatures are the canonical bootstrap runtime surface. Later image
// walk, realization, and deterministic-reset issues must extend this boundary
// without renaming or silently narrowing these entrypoints.
int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image);
const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector);
int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3);
int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot);
void objc3_runtime_reset_for_testing(void);

#ifdef __cplusplus
}
#endif
