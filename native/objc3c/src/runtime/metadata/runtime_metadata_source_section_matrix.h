#pragma once

#include <array>
#include <cstddef>
#include <string>

struct Objc3RuntimeMetadataSourceToSectionMatrixRow {
  std::string row_key;
  std::string graph_node_kind;
  std::string emission_mode;
  std::string logical_section;
  std::string payload_role;
  std::string descriptor_symbol_family;
  std::string aggregate_symbol;
  std::string relocation_behavior;
  std::string proof_fixture_path;
  std::string proof_mode;
  std::string section_inventory_command;
  std::string symbol_inventory_command;
};

struct Objc3RuntimeMetadataSourceToSectionMatrixSummary {
  std::string contract_id = kObjc3RuntimeMetadataSourceToSectionMatrixContractId;
  std::string source_graph_contract_id =
      kObjc3ExecutableMetadataSourceGraphContractId;
  std::string section_abi_contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  std::string section_publication_contract_id =
      kObjc3RuntimeMetadataSectionPublicationContractId;
  std::string object_inspection_contract_id =
      kObjc3RuntimeMetadataObjectInspectionContractId;
  std::string manifest_surface_path =
      kObjc3RuntimeMetadataSourceToSectionMatrixSurfacePath;
  std::string row_ordering_model =
      kObjc3RuntimeMetadataSourceToSectionMatrixOrderingModel;
  bool matrix_published = false;
  bool fail_closed = false;
  bool source_graph_ready = false;
  bool section_abi_ready = false;
  bool section_publication_ready = false;
  bool object_inspection_ready = false;
  bool supported_node_coverage_complete = false;
  bool explicit_non_goals_published = false;
  bool row_ordering_frozen = false;
  std::size_t matrix_row_count = 0;
  std::array<Objc3RuntimeMetadataSourceToSectionMatrixRow, 9u> rows = {};
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  if (summary.contract_id.empty() || summary.source_graph_contract_id.empty() ||
      summary.section_abi_contract_id.empty() ||
      summary.section_publication_contract_id.empty() ||
      summary.object_inspection_contract_id.empty() ||
      summary.manifest_surface_path.empty() ||
      summary.row_ordering_model.empty() || !summary.matrix_published ||
      !summary.fail_closed || !summary.source_graph_ready ||
      !summary.section_abi_ready || !summary.section_publication_ready ||
      !summary.object_inspection_ready ||
      !summary.supported_node_coverage_complete ||
      !summary.explicit_non_goals_published || !summary.row_ordering_frozen ||
      summary.matrix_row_count != summary.rows.size() || summary.replay_key.empty() ||
      !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &row : summary.rows) {
    if (row.row_key.empty() || row.graph_node_kind.empty() ||
        row.emission_mode.empty() || row.logical_section.empty() ||
        row.payload_role.empty() || row.descriptor_symbol_family.empty() ||
        row.aggregate_symbol.empty() || row.relocation_behavior.empty() ||
        row.proof_fixture_path.empty() || row.proof_mode.empty() ||
        row.section_inventory_command.empty() ||
        row.symbol_inventory_command.empty()) {
      return false;
    }
  }
  return true;
}
