#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <cstddef>
#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

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
        << "\",\"payload_role\":\"" << EscapeJsonString(row.payload_role)
        << "\",\"descriptor_symbol_family\":\""
        << EscapeJsonString(row.descriptor_symbol_family)
        << "\",\"aggregate_symbol\":\""
        << EscapeJsonString(row.aggregate_symbol)
        << "\",\"relocation_behavior\":\""
        << EscapeJsonString(row.relocation_behavior)
        << "\",\"proof_fixture_path\":\""
        << EscapeJsonString(row.proof_fixture_path)
        << "\",\"proof_mode\":\"" << EscapeJsonString(row.proof_mode)
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
