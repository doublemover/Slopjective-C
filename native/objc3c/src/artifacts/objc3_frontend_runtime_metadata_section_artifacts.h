#pragma once

#include <cstddef>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "runtime/metadata/runtime_metadata_model.h"

namespace objc3::artifacts::frontend {

struct Objc3AccessorStorageLoweringMetadataSummary {
  std::size_t synthesized_accessor_owner_entries = 0;
  std::size_t synthesized_getter_entries = 0;
  std::size_t synthesized_setter_entries = 0;
  std::size_t current_property_read_entries = 0;
  std::size_t current_property_write_entries = 0;
  std::size_t current_property_exchange_entries = 0;
  std::size_t weak_current_property_load_entries = 0;
  std::size_t weak_current_property_store_entries = 0;
  bool deterministic = false;
};

struct Objc3ExecutableAccessorLayoutLoweringSummary {
  std::size_t property_metadata_entries = 0;
  std::size_t ivar_metadata_entries = 0;
  std::size_t property_attribute_profile_entries = 0;
  std::size_t accessor_ownership_profile_entries = 0;
  std::size_t synthesized_binding_entries = 0;
  std::size_t implementation_owned_property_entries = 0;
  std::size_t synthesized_getter_entries = 0;
  std::size_t synthesized_setter_entries = 0;
  std::size_t synthesized_accessor_entries = 0;
  std::size_t ivar_layout_entries = 0;
  std::size_t ivar_layout_owner_entries = 0;
  bool deterministic = false;
};

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

[[nodiscard]] std::string BuildExecutableMetadataDebugProjectionRowDescriptor(
    const Objc3ExecutableMetadataDebugProjectionMatrixRow &row);

[[nodiscard]] std::string BuildExecutableMetadataDebugProjectionReplayKey(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary);

[[nodiscard]] Objc3ExecutableMetadataDebugProjectionSummary
BuildExecutableMetadataDebugProjectionSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff);

[[nodiscard]] std::string BuildExecutableMetadataDebugProjectionSummaryJson(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary);

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

[[nodiscard]] Objc3AccessorStorageLoweringMetadataSummary
BuildAccessorStorageLoweringMetadataSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records);

[[nodiscard]] Objc3ExecutableAccessorLayoutLoweringSummary
BuildExecutableAccessorLayoutLoweringSummary(
    const Objc3ExecutableMetadataSourceGraph &source_graph);

}  // namespace objc3::artifacts::frontend
