#pragma once

#include <cstdint>
#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] Objc3RuntimeSupportLibraryContractSummary
BuildRuntimeSupportLibraryContractSummary();

[[nodiscard]] Objc3RuntimeSupportLibraryCoreFeatureSummary
BuildRuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library);

[[nodiscard]] Objc3RuntimeSupportLibraryLinkWiringSummary
BuildRuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature);

[[nodiscard]] std::string BuildRuntimeTranslationUnitRegistrationContractReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary);

[[nodiscard]] Objc3RuntimeTranslationUnitRegistrationContractSummary
BuildRuntimeTranslationUnitRegistrationContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        &runtime_ingest_binary_boundary,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring);

[[nodiscard]] std::string BuildRuntimeTranslationUnitRegistrationContractSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary);

[[nodiscard]] std::string BuildRuntimeTranslationUnitRegistrationManifestReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary);

[[nodiscard]] Objc3RuntimeTranslationUnitRegistrationManifestSummary
BuildRuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary
        &registration_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    std::uint64_t translation_unit_registration_order_ordinal);

[[nodiscard]] std::string BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary);

}  // namespace objc3::artifacts::frontend
