#include "artifacts/objc3_frontend_artifact_ownership_metadata.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/ownership_system_extension_contracts.h"
#include "sema/model/semantic_symbol_ownership_dispatch_summaries.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendOwnershipMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &ownership_system_extension_lowering_replay_key,
    const Objc3OwnershipSystemExtensionLoweringContract
        &ownership_system_extension_lowering_contract,
    const std::string &ownership_borrowed_retainable_abi_completion_replay_key,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
        &ownership_system_extension_source_closure_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_retainable_c_family_source_completion_summary) {
  ir_frontend_metadata.lowering_ownership_system_extension_replay_key =
      ownership_system_extension_lowering_replay_key;
  ir_frontend_metadata.ownership_system_extension_lowering_cleanup_hook_sites =
      ownership_system_extension_lowering_contract.cleanup_hook_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_resource_local_sites =
      ownership_system_extension_lowering_contract.resource_local_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_cleanup_owned_local_sites =
      ownership_system_extension_lowering_contract.cleanup_owned_local_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_resource_move_capture_sites =
      ownership_system_extension_lowering_contract.resource_move_capture_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_borrowed_parameter_sites =
      ownership_system_extension_lowering_contract.borrowed_parameter_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_borrowed_return_callable_sites =
      ownership_system_extension_lowering_contract
          .borrowed_return_callable_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_borrowed_escape_candidate_sites =
      ownership_system_extension_lowering_contract.borrowed_escape_candidate_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_explicit_capture_item_sites =
      ownership_system_extension_lowering_contract.explicit_capture_item_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_retainable_family_callable_sites =
      ownership_system_extension_lowering_contract.retainable_family_callable_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_retainable_family_operation_callable_sites =
      ownership_system_extension_lowering_contract
          .retainable_family_operation_callable_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_retainable_family_alias_callable_sites =
      ownership_system_extension_lowering_contract
          .retainable_family_alias_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_guard_blocked_sites =
      ownership_system_extension_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .ownership_system_extension_lowering_contract_violation_sites =
      ownership_system_extension_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_ownership_system_extension_lowering_handoff =
      ownership_system_extension_lowering_contract.deterministic;

  ir_frontend_metadata.ownership_borrowed_retainable_abi_completion_replay_key =
      ownership_borrowed_retainable_abi_completion_replay_key;
  ir_frontend_metadata
      .ownership_borrowed_retainable_returns_borrowed_attribute_sites =
      ownership_system_extension_source_closure_summary
          .returns_borrowed_attribute_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_retain_sites =
      ownership_retainable_c_family_source_completion_summary.family_retain_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_release_sites =
      ownership_retainable_c_family_source_completion_summary.family_release_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_autorelease_sites =
      ownership_retainable_c_family_source_completion_summary
          .family_autorelease_sites;
  ir_frontend_metadata
      .ownership_borrowed_retainable_compatibility_returns_retained_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_returns_retained_sites;
  ir_frontend_metadata
      .ownership_borrowed_retainable_compatibility_returns_not_retained_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_returns_not_retained_sites;
  ir_frontend_metadata
      .ownership_borrowed_retainable_compatibility_consumed_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_consumed_sites;
  ir_frontend_metadata
      .deterministic_ownership_borrowed_retainable_abi_completion_handoff =
      true;
}

}  // namespace objc3::artifacts::frontend
