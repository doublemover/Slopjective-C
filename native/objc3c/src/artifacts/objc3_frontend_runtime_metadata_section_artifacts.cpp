#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionAbiFreezeSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.object_file_section_inventory_frozen = true;
  summary.symbol_policy_frozen = true;
  summary.visibility_model_frozen = true;
  summary.retention_policy_frozen = true;
  summary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  summary.runtime_export_legality_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  summary.runtime_export_enforcement_ready =
      IsReadyObjc3RuntimeExportEnforcementSummary(runtime_export_enforcement);
  summary.ready_for_section_scaffold =
      summary.runtime_metadata_source_boundary_ready &&
      summary.runtime_export_legality_boundary_ready &&
      summary.runtime_export_enforcement_ready;
  if (!summary.ready_for_section_scaffold) {
    summary.failure_reason =
        "runtime metadata section ABI freeze prerequisites are not ready";
  }
  return summary;
}

Objc3RuntimeMetadataSectionPublicationSummary
BuildRuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionPublicationSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          runtime_export_enforcement)) {
    summary.failure_reason =
        "runtime metadata section publication prerequisites are not ready";
    return summary;
  }

  summary.publication_emitted = true;
  summary.uses_llvm_used = true;
  summary.image_info_emitted = true;
  summary.class_descriptor_count = runtime_export_legality.class_record_count;
  summary.protocol_descriptor_count =
      runtime_export_legality.protocol_record_count;
  summary.category_descriptor_count =
      runtime_export_legality.category_record_count;
  summary.property_descriptor_count =
      runtime_export_legality.property_record_count;
  summary.ivar_descriptor_count = runtime_export_legality.ivar_record_count;
  summary.total_descriptor_count =
      summary.class_descriptor_count + summary.protocol_descriptor_count +
      summary.category_descriptor_count + summary.property_descriptor_count +
      summary.ivar_descriptor_count;
  summary.total_retained_global_count = summary.total_descriptor_count + 6u;
  return summary;
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  Objc3RuntimeMetadataObjectInspectionHarnessSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
          runtime_metadata_section_publication)) {
    summary.failure_reason =
        "runtime metadata object inspection harness prerequisites are not ready";
    return summary;
  }

  summary.matrix_published = true;
  summary.uses_llvm_readobj = true;
  summary.uses_llvm_objdump = true;
  summary.matrix_row_count = 2u;
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixReplayKey(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_graph_contract=" << summary.source_graph_contract_id
      << ";section_abi_contract=" << summary.section_abi_contract_id
      << ";section_publication_contract="
      << summary.section_publication_contract_id
      << ";object_inspection_contract="
      << summary.object_inspection_contract_id
      << ";surface_path=" << summary.manifest_surface_path
      << ";row_ordering=" << summary.row_ordering_model
      << ";row_count=" << summary.matrix_row_count;
  for (const auto &row : summary.rows) {
    out << ";row=" << row.row_key << "|" << row.graph_node_kind << "|"
        << row.emission_mode << "|" << row.logical_section << "|"
        << row.payload_role << "|" << row.descriptor_symbol_family << "|"
        << row.aggregate_symbol << "|" << row.relocation_behavior << "|"
        << row.proof_fixture_path << "|" << row.proof_mode << "|"
        << row.section_inventory_command << "|" << row.symbol_inventory_command;
  }
  return out.str();
}

Objc3RuntimeMetadataSourceToSectionMatrixSummary
BuildRuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection) {
  Objc3RuntimeMetadataSourceToSectionMatrixSummary summary;
  summary.matrix_published = true;
  summary.fail_closed = true;
  summary.source_graph_ready =
      IsReadyObjc3ExecutableMetadataSourceGraph(executable_metadata_source_graph);
  summary.section_abi_ready =
      IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi);
  summary.section_publication_ready =
      IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
          runtime_metadata_section_publication);
  summary.object_inspection_ready =
      IsReadyObjc3RuntimeMetadataObjectInspectionHarnessSummary(
          runtime_metadata_object_inspection);
  summary.supported_node_coverage_complete = true;
  summary.explicit_non_goals_published = true;
  summary.row_ordering_frozen = true;
  summary.rows[0] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey,
      "interface",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted interface payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[1] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionImplementationRowKey,
      "implementation",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted implementation payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[2] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionClassRowKey,
      "class",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalClassDescriptorSection,
      "standalone class descriptor payload",
      "__objc3_meta_class_####",
      kObjc3RuntimeMetadataClassDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[3] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMetaclassRowKey,
      "metaclass",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted metaclass payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[4] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionProtocolRowKey,
      "protocol",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalProtocolDescriptorSection,
      "standalone protocol descriptor payload",
      "__objc3_meta_protocol_####",
      kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[5] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionCategoryRowKey,
      "category",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalCategoryDescriptorSection,
      "standalone category descriptor payload",
      "__objc3_meta_category_####",
      kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[6] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionPropertyRowKey,
      "property",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalPropertyDescriptorSection,
      "standalone property descriptor payload",
      "__objc3_meta_property_####",
      kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[7] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMethodRowKey,
      "method",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted method payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[8] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionIvarRowKey,
      "ivar",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalIvarDescriptorSection,
      "standalone ivar descriptor payload",
      "__objc3_meta_ivar_####",
      kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.matrix_row_count = summary.rows.size();
  summary.replay_key =
      BuildRuntimeMetadataSourceToSectionMatrixReplayKey(summary);
  if (!IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)) {
    summary.failure_reason =
        "runtime metadata source-to-section completeness matrix is incomplete";
  }
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_graph_contract_id\":\""
      << EscapeJsonString(summary.source_graph_contract_id)
      << "\",\"section_abi_contract_id\":\""
      << EscapeJsonString(summary.section_abi_contract_id)
      << "\",\"section_publication_contract_id\":\""
      << EscapeJsonString(summary.section_publication_contract_id)
      << "\",\"object_inspection_contract_id\":\""
      << EscapeJsonString(summary.object_inspection_contract_id)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"row_ordering_model\":\""
      << EscapeJsonString(summary.row_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)
              ? "true"
              : "false")
      << ",\"matrix_published\":"
      << (summary.matrix_published ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_graph_ready\":"
      << (summary.source_graph_ready ? "true" : "false")
      << ",\"section_abi_ready\":"
      << (summary.section_abi_ready ? "true" : "false")
      << ",\"section_publication_ready\":"
      << (summary.section_publication_ready ? "true" : "false")
      << ",\"object_inspection_ready\":"
      << (summary.object_inspection_ready ? "true" : "false")
      << ",\"supported_node_coverage_complete\":"
      << (summary.supported_node_coverage_complete ? "true" : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"row_ordering_frozen\":"
      << (summary.row_ordering_frozen ? "true" : "false")
      << ",\"matrix_row_count\":" << summary.matrix_row_count
      << ",\"rows\":[";
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    const auto &row = summary.rows[index];
    if (index != 0u) {
      out << ",";
    }
    out << "{\"row_key\":\"" << EscapeJsonString(row.row_key)
        << "\",\"graph_node_kind\":\""
        << EscapeJsonString(row.graph_node_kind)
        << "\",\"emission_mode\":\""
        << EscapeJsonString(row.emission_mode)
        << "\",\"logical_section\":\""
        << EscapeJsonString(row.logical_section)
        << "\",\"payload_role\":\""
        << EscapeJsonString(row.payload_role)
        << "\",\"descriptor_symbol_family\":\""
        << EscapeJsonString(row.descriptor_symbol_family)
        << "\",\"aggregate_symbol\":\""
        << EscapeJsonString(row.aggregate_symbol)
        << "\",\"relocation_behavior\":\""
        << EscapeJsonString(row.relocation_behavior)
        << "\",\"proof_fixture_path\":\""
        << EscapeJsonString(row.proof_fixture_path)
        << "\",\"proof_mode\":\""
        << EscapeJsonString(row.proof_mode)
        << "\",\"section_inventory_command\":\""
        << EscapeJsonString(row.section_inventory_command)
        << "\",\"symbol_inventory_command\":\""
        << EscapeJsonString(row.symbol_inventory_command) << "\"}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
