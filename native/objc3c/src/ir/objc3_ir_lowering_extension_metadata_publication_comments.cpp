#include "ir/objc3_ir_lowering_extension_metadata_publication_comments.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRLoweringExtensionCommentPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  if (!metadata.lowering_actor_lowering_metadata_replay_key.empty()) {
    out << "; actor_lowering_metadata_contract = "
        << metadata.lowering_actor_lowering_metadata_replay_key << "\n";
  }
  if (!metadata.lowering_dispatch_dispatch_control_replay_key.empty()) {
    out << "; dispatch_control_lowering_contract = "
        << metadata.lowering_dispatch_dispatch_control_replay_key << "\n";
  }
  if (!metadata.lowering_interop_interop_replay_key.empty()) {
    out << "; interop_interop_lowering_abi_contract = "
        << metadata.lowering_interop_interop_replay_key << "\n";
  }
  if (!metadata.lowering_interop_foreign_call_lifetime_replay_key.empty()) {
    out << "; interop_foreign_call_and_lifetime_lowering = "
        << metadata.lowering_interop_foreign_call_lifetime_replay_key << "\n";
  }
  if (!metadata.lowering_interop_ffi_metadata_interface_preservation_key.empty()) {
    out << "; interop_ffi_metadata_interface_preservation = "
        << metadata.lowering_interop_ffi_metadata_interface_preservation_key
        << "\n";
    out << "; interop_bridge_packaging_toolchain_contract = "
        << Objc3InteropBridgePackagingToolchainSummary() << "\n";
  }
  if (!metadata.lowering_interop_header_module_bridge_generation_key.empty()) {
    out << "; interop_header_module_and_bridge_generation = "
        << metadata.lowering_interop_header_module_bridge_generation_key
        << "\n";
  }
  if (!metadata.lowering_metaprogramming_expansion_replay_key.empty()) {
    out << "; metaprogramming_expansion_lowering_contract = "
        << metadata.lowering_metaprogramming_expansion_replay_key << "\n";
  }
  if (!metadata.lowering_metaprogramming_synthesized_emission_replay_key
           .empty()) {
    out << "; metaprogramming_synthesized_ast_ir_emission = "
        << metadata.lowering_metaprogramming_synthesized_emission_replay_key
        << "\n";
  }
  if (!metadata.lowering_metaprogramming_module_interface_replay_preservation_key
           .empty()) {
    out << "; metaprogramming_module_interface_replay_preservation = "
        << metadata
               .lowering_metaprogramming_module_interface_replay_preservation_key
        << "\n";
  }
  if (!metadata.lowering_metaprogramming_synthesized_emission_replay_key
           .empty() ||
      !metadata.lowering_metaprogramming_module_interface_replay_preservation_key
           .empty()) {
    out << "; metaprogramming_expansion_host_runtime_boundary = "
        << Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary() << "\n";
  }
  if (!metadata.lowering_dispatch_dispatch_metadata_interface_preservation_key
           .empty()) {
    out << "; dispatch_dispatch_metadata_interface_preservation = "
        << metadata.lowering_dispatch_dispatch_metadata_interface_preservation_key
        << "\n";
  }
  if (!metadata.lowering_ownership_system_extension_replay_key.empty()) {
    out << "; system_extension_lowering_contract = "
        << metadata.lowering_ownership_system_extension_replay_key << "\n";
  }
  if (!metadata.ownership_borrowed_retainable_abi_completion_replay_key
           .empty()) {
    out << "; ownership_borrowed_retainable_abi_completion = contract="
        << kObjc3OwnershipBorrowedRetainableAbiCompletionContractId
        << ";surface="
        << kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath
        << ";replay_key="
        << metadata.ownership_borrowed_retainable_abi_completion_replay_key
        << ";returns_borrowed_attribute_sites="
        << metadata
               .ownership_borrowed_retainable_returns_borrowed_attribute_sites
        << ";family_retain_sites="
        << metadata.ownership_borrowed_retainable_family_retain_sites
        << ";family_release_sites="
        << metadata.ownership_borrowed_retainable_family_release_sites
        << ";family_autorelease_sites="
        << metadata.ownership_borrowed_retainable_family_autorelease_sites
        << ";compatibility_returns_retained_sites="
        << metadata
               .ownership_borrowed_retainable_compatibility_returns_retained_sites
        << ";compatibility_returns_not_retained_sites="
        << metadata
               .ownership_borrowed_retainable_compatibility_returns_not_retained_sites
        << ";compatibility_consumed_sites="
        << metadata.ownership_borrowed_retainable_compatibility_consumed_sites
        << ";follow_on_surface=objc3c.ownership.helperruntime.surface.v1\n";
    out << "; ownership_system_helper_runtime_contract = "
        << Objc3OwnershipSystemHelperRuntimeContractSummary() << "\n";
    out << "; ownership_live_cleanup_retainable_runtime_integration = "
        << Objc3OwnershipLiveCleanupRetainableIntegrationSummary() << "\n";
  }
  if (!metadata.lowering_task_runtime_interop_cancellation_replay_key.empty()) {
    out << "; task_runtime_interop_cancellation_lowering = "
        << metadata.lowering_task_runtime_interop_cancellation_replay_key
        << "\n";
  }
  if (!metadata.lowering_concurrency_replay_race_guard_replay_key.empty()) {
    out << "; concurrency_replay_race_guard_lowering = "
        << metadata.lowering_concurrency_replay_race_guard_replay_key << "\n";
  }
  if (!metadata.lowering_unsafe_pointer_extension_replay_key.empty()) {
    out << "; unsafe_pointer_extension_lowering = "
        << metadata.lowering_unsafe_pointer_extension_replay_key << "\n";
  }
  if (!metadata.lowering_inline_asm_intrinsic_governance_replay_key.empty()) {
    out << "; inline_asm_intrinsic_governance_lowering = "
        << metadata.lowering_inline_asm_intrinsic_governance_replay_key << "\n";
  }
}
