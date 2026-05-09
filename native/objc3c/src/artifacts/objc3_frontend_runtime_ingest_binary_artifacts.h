#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildExecutableMetadataRuntimeIngestPackagingReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary &summary);

[[nodiscard]] Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection);

[[nodiscard]] std::string
BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary &summary);

[[nodiscard]] std::string BuildExecutableMetadataRuntimeIngestBinaryEnvelopePayload(
    const std::string &packaging_json,
    const std::string &typed_handoff_json,
    const std::string &debug_projection_json);

[[nodiscard]] std::string BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary);

[[nodiscard]] std::string BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary);

}  // namespace objc3::artifacts::frontend
