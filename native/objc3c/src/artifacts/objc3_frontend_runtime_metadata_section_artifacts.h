#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildExecutableMetadataSourceGraphJson(
    const Objc3ExecutableMetadataSourceGraph &graph);

[[nodiscard]] std::string BuildExecutableMetadataSemanticConsistencyBoundaryJson(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary);

[[nodiscard]] std::string BuildExecutableMetadataSemanticValidationSurfaceJson(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface);

[[nodiscard]] std::string BuildExecutableMetadataLoweringHandoffSurfaceJson(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface);

[[nodiscard]] std::string BuildExecutableMetadataTypedLoweringHandoffJson(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface);

[[nodiscard]] Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement);

[[nodiscard]] Objc3RuntimeMetadataSectionPublicationSummary
BuildRuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement);

[[nodiscard]] Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

[[nodiscard]] std::string BuildRuntimeMetadataSourceToSectionMatrixReplayKey(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary);

[[nodiscard]] Objc3RuntimeMetadataSourceToSectionMatrixSummary
BuildRuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection);

[[nodiscard]] std::string BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary);

}  // namespace objc3::artifacts::frontend
