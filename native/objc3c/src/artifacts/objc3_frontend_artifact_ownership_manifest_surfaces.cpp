#include "artifacts/objc3_frontend_artifact_ownership_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_module_semantic_artifacts.h"
#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteOwnershipManifestSurfaces(
    std::ostream &manifest,
    const Objc3OwnershipSystemExtensionSemanticModelSummary
        &ownership_system_extension_semantic_model_summary,
    const Objc3EffectsOwnershipSemanticModelSummary
        &effects_ownership_semantic_model_summary,
    const Objc3CrossModuleSemanticContractsDiagnosticsSummary
        &cross_module_semantic_contracts_diagnostics_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
        &ownership_resource_move_use_after_move_semantics_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary
        &ownership_borrowed_pointer_escape_analysis_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &ownership_capture_list_retainable_family_legality_completion_summary,
    const Objc3OwnershipSystemExtensionLoweringContract
        &ownership_system_extension_lowering_contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
        &ownership_system_extension_source_closure_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_retainable_c_family_source_completion_summary,
    const std::string &ownership_system_extension_lowering_replay_key,
    const std::string &ownership_borrowed_retainable_abi_completion_replay_key) {
  manifest
      << ",\"objc_ownership_system_extension_semantic_model\":"
      << BuildOwnershipSystemExtensionSemanticModelSummaryJson(
             ownership_system_extension_semantic_model_summary)
      << ",\"objc_effects_ownership_semantic_model\":"
      << BuildEffectsOwnershipSemanticModelSummaryJson(
             effects_ownership_semantic_model_summary)
      << ",\"objc_cross_module_semantic_contracts_and_diagnostics\":"
      << BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
             cross_module_semantic_contracts_diagnostics_summary)
      << ",\"objc_ownership_resource_move_and_use_after_move_semantics\":"
      << BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
             ownership_resource_move_use_after_move_semantics_summary)
      << ",\"objc_ownership_borrowed_pointer_escape_analysis\":"
      << BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
             ownership_borrowed_pointer_escape_analysis_summary)
      << ",\"objc_ownership_capture_list_and_retainable_family_legality_completion\":"
      << BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
             ownership_capture_list_retainable_family_legality_completion_summary)
      << ",\"objc_ownership_system_extension_lowering_contract\":"
      << BuildOwnershipSystemExtensionLoweringContractJson(
             ownership_system_extension_semantic_model_summary,
             ownership_resource_move_use_after_move_semantics_summary,
             ownership_borrowed_pointer_escape_analysis_summary,
             ownership_capture_list_retainable_family_legality_completion_summary,
             ownership_system_extension_lowering_contract,
             ownership_system_extension_lowering_replay_key)
      << ",\"objc_ownership_borrowed_pointer_and_retainable_family_abi_completion\":"
      << BuildOwnershipBorrowedRetainableAbiCompletionJson(
             ownership_system_extension_lowering_contract,
             ownership_system_extension_source_closure_summary,
             ownership_retainable_c_family_source_completion_summary,
             ownership_system_extension_lowering_replay_key,
             ownership_borrowed_retainable_abi_completion_replay_key);
}

}  // namespace objc3::artifacts::frontend
