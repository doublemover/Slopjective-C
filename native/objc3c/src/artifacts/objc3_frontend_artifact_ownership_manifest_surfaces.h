#pragma once

#include <iosfwd>
#include <string>

#include "lower/contracts/ownership_system_extension_contracts.h"
#include "sema/model/semantic_symbol_ownership_dispatch_summaries.h"
#include "sema/objc3_sema_contract_block_capture_legality.h"
#include "sema/objc3_sema_contract_borrowed_escape.h"
#include "sema/objc3_sema_contract_effects_flow_ownership.h"
#include "sema/objc3_sema_contract_ownership_semantics.h"
#include "sema/objc3_sema_contract_runtime_readiness_keys.h"

struct Objc3CrossModuleSemanticContractsDiagnosticsSummary;
struct Objc3EffectsOwnershipSemanticModelSummary;
struct Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary;
struct Objc3FrontendOwnershipSystemExtensionSourceClosureSummary;
struct Objc3OwnershipBorrowedPointerEscapeAnalysisSummary;
struct Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary;
struct Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary;
struct Objc3OwnershipSystemExtensionLoweringContract;
struct Objc3OwnershipSystemExtensionSemanticModelSummary;

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
    const std::string &ownership_borrowed_retainable_abi_completion_replay_key);

}  // namespace objc3::artifacts::frontend
