#include "artifacts/objc3_frontend_artifact_interop_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteInteropManifestSurfaces(
    std::ostream &manifest,
    const Objc3InteropInteropSemanticModelSummary
        &interop_interop_semantic_model_summary,
    const Objc3InteropInteropRuntimeParitySummary
        &interop_interop_runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary
        &interop_cpp_interop_interaction_summary,
    const Objc3InteropSwiftInteropIsolationSummary
        &interop_swift_interop_isolation_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &interop_foreign_surface_interface_preservation_summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary
        &interop_header_module_bridge_generation_summary,
    const Objc3InteropInteropLoweringContract &interop_interop_lowering_contract,
    const std::string &interop_interop_lowering_replay_key,
    const Objc3InteropForeignCallLifetimeLoweringContract
        &interop_foreign_call_lifetime_lowering_contract,
    const std::string &interop_foreign_call_lifetime_lowering_replay_key,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &interop_ffi_metadata_interface_preservation_contract,
    const std::string &interop_ffi_metadata_interface_preservation_replay_key) {
  manifest
      << ",\"objc_interop_interop_semantic_model\":"
      << BuildInteropInteropSemanticModelSummaryJson(
             interop_interop_semantic_model_summary)
      << ",\"objc_interop_c_and_objc_runtime_parity_semantics\":"
      << BuildInteropInteropRuntimeParitySummaryJson(
             interop_interop_runtime_parity_summary)
      << ",\"objc_interop_cpp_ownership_throws_and_async_interactions\":"
      << BuildInteropCppInteropInteractionSummaryJson(
             interop_cpp_interop_interaction_summary)
      << ",\"objc_interop_swift_metadata_and_isolation_mapping\":"
      << BuildInteropSwiftInteropIsolationSummaryJson(
             interop_swift_interop_isolation_summary)
      << ",\"objc_interop_foreign_surface_interface_and_module_preservation\":"
      << BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
             interop_foreign_surface_interface_preservation_summary)
      << ",\"objc_interop_header_module_and_bridge_generation\":"
      << BuildInteropHeaderModuleBridgeGenerationSummaryJson(
             interop_header_module_bridge_generation_summary)
      << ",\"objc_interop_interop_lowering_and_abi_contract\":"
      << BuildInteropInteropLoweringContractJson(
             interop_interop_semantic_model_summary,
             interop_interop_runtime_parity_summary,
             interop_cpp_interop_interaction_summary,
             interop_swift_interop_isolation_summary,
             interop_foreign_surface_interface_preservation_summary,
             interop_interop_lowering_contract,
             interop_interop_lowering_replay_key)
      << ",\"objc_interop_foreign_call_and_lifetime_lowering\":"
      << BuildInteropForeignCallLifetimeLoweringContractJson(
             interop_interop_lowering_contract,
             interop_cpp_interop_interaction_summary,
             interop_foreign_surface_interface_preservation_summary,
             interop_foreign_call_lifetime_lowering_contract,
             interop_foreign_call_lifetime_lowering_replay_key)
      << ",\"objc_interop_ffi_metadata_and_interface_preservation\":"
      << BuildInteropFfiMetadataInterfacePreservationContractJson(
             interop_foreign_call_lifetime_lowering_contract,
             interop_foreign_call_lifetime_lowering_replay_key,
             interop_foreign_surface_interface_preservation_summary,
             interop_ffi_metadata_interface_preservation_contract,
             interop_ffi_metadata_interface_preservation_replay_key);
}

}  // namespace objc3::artifacts::frontend
