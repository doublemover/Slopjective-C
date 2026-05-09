#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildRuntimeStartupBootstrapInvariantReplayKey(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary);

[[nodiscard]] Objc3RuntimeStartupBootstrapInvariantSummary
BuildRuntimeStartupBootstrapInvariantSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest);

[[nodiscard]] std::string BuildRuntimeStartupBootstrapInvariantSummaryJson(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary);

[[nodiscard]] std::string BuildRuntimeBootstrapSemanticsReplayKey(
    const Objc3RuntimeBootstrapSemanticsSummary &summary);

[[nodiscard]] Objc3RuntimeBootstrapSemanticsSummary
BuildRuntimeBootstrapSemanticsSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &bootstrap_invariants,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest);

[[nodiscard]] std::string BuildRuntimeBootstrapSemanticsSummaryJson(
    const Objc3RuntimeBootstrapSemanticsSummary &summary);

[[nodiscard]] std::string BuildRuntimeBootstrapLoweringReplayKey(
    const Objc3RuntimeBootstrapLoweringSummary &summary);

[[nodiscard]] Objc3RuntimeBootstrapLoweringSummary BuildRuntimeBootstrapLoweringSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure);

[[nodiscard]] std::string BuildRuntimeBootstrapLoweringSummaryJson(
    const Objc3RuntimeBootstrapLoweringSummary &summary);

}  // namespace objc3::artifacts::frontend
