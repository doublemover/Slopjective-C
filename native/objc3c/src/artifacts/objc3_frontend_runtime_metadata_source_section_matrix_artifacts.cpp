#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <cstddef>
#include <sstream>

#include "ast/objc3_ast_contracts.h"

namespace objc3::artifacts::frontend {

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

}  // namespace objc3::artifacts::frontend
