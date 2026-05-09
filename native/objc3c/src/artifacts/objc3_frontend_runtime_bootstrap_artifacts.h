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

[[nodiscard]] std::string BuildRuntimeBootstrapLegalityFailureContractReplayKey(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary);

[[nodiscard]] Objc3RuntimeBootstrapLegalityFailureContractSummary
BuildRuntimeBootstrapLegalityFailureContractSummary(
    const Objc3BootstrapLegalityFailureContractSummary &semantic_boundary,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics);

[[nodiscard]] std::string BuildRuntimeBootstrapLegalityFailureContractSummaryJson(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary);

[[nodiscard]] std::string BuildRuntimeBootstrapApiReplayKey(
    const Objc3RuntimeBootstrapApiSummary &summary);

[[nodiscard]] Objc3RuntimeBootstrapApiSummary BuildRuntimeBootstrapApiSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring);

[[nodiscard]] std::string BuildRuntimeBootstrapApiSummaryJson(
    const Objc3RuntimeBootstrapApiSummary &summary);

}  // namespace objc3::artifacts::frontend
