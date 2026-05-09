#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildExecutableMetadataRuntimeIngestBinaryEnvelopePayload(
    const std::string &packaging_json,
    const std::string &typed_handoff_json,
    const std::string &debug_projection_json);

[[nodiscard]] std::string BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary);

[[nodiscard]] std::string BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary);

}  // namespace objc3::artifacts::frontend
