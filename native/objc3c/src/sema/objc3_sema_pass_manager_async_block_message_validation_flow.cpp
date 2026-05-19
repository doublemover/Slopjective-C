#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaAsyncBlockMessageParityValidationReadinessRecord
BuildObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_await_lowering_suspension_state_lowering_handoff,
    bool deterministic_block_literal_capture_semantics_handoff,
    bool deterministic_block_abi_invoke_trampoline_handoff,
    bool deterministic_block_storage_escape_handoff,
    bool deterministic_block_copy_dispose_handoff,
    bool deterministic_block_determinism_perf_baseline_handoff,
    bool deterministic_message_send_selector_lowering_handoff) {
  Objc3SemaAsyncBlockMessageParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
#include "sema/objc3_sema_pass_manager_async_block_message_validation_await.inc"
#include "sema/objc3_sema_pass_manager_async_block_message_validation_block_capture.inc"
#include "sema/objc3_sema_pass_manager_async_block_message_validation_block_abi.inc"
#include "sema/objc3_sema_pass_manager_async_block_message_validation_block_storage.inc"
#include "sema/objc3_sema_pass_manager_async_block_message_validation_block_copy_dispose.inc"
#include "sema/objc3_sema_pass_manager_async_block_message_validation_block_determinism.inc"
#include "sema/objc3_sema_pass_manager_async_block_message_validation_message_send.inc"
#include "sema/objc3_sema_pass_manager_async_block_message_validation_finalization.inc"
  return record;
}
