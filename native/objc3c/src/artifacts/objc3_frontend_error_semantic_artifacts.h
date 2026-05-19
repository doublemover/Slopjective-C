#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"
#include "lower/contracts/error_handling_result_bridging_contracts.h"
#include "lower/contracts/error_handling_throws_unwind_contracts.h"
#include "sema/objc3_sema_contract_async_error_surfaces.h"
#include "sema/objc3_sema_contract_effects_flow_error_handling.h"
#include "sema/objc3_sema_parity_contract_surface.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] Objc3ThrowsPropagationLoweringContract
BuildThrowsPropagationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3ResultLikeLoweringContract BuildResultLikeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3NSErrorBridgingLoweringContract
BuildNSErrorBridgingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3UnwindCleanupLoweringContract
BuildUnwindCleanupLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] std::string BuildErrorHandlingErrorSemanticModelSummaryJson(
    const Objc3ErrorHandlingErrorSemanticModelSummary &summary);

[[nodiscard]] std::string BuildErrorHandlingTryDoCatchSemanticSummaryJson(
    const Objc3ErrorHandlingTryDoCatchSemanticSummary &summary);

[[nodiscard]] std::string BuildErrorHandlingErrorBridgeLegalitySummaryJson(
    const Objc3ErrorHandlingErrorBridgeLegalitySummary &summary);

}  // namespace objc3::artifacts::frontend
