#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <cstddef>
#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildExecutableMetadataDebugProjectionSummaryJson(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"source_graph_contract_id\":\""
      << EscapeJsonString(summary.source_graph_contract_id)
      << "\",\"named_metadata_name\":\""
      << EscapeJsonString(summary.named_metadata_name)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"typed_handoff_surface_path\":\""
      << EscapeJsonString(summary.typed_handoff_surface_path)
      << "\",\"source_graph_surface_path\":\""
      << EscapeJsonString(summary.source_graph_surface_path)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataDebugProjectionSummary(summary)
              ? "true"
              : "false")
      << ",\"matrix_published\":"
      << (summary.matrix_published ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"manifest_debug_surface_published\":"
      << (summary.manifest_debug_surface_published ? "true" : "false")
      << ",\"ir_named_metadata_published\":"
      << (summary.ir_named_metadata_published ? "true" : "false")
      << ",\"replay_anchor_deterministic\":"
      << (summary.replay_anchor_deterministic ? "true" : "false")
      << ",\"active_typed_handoff_ready\":"
      << (summary.active_typed_handoff_ready ? "true" : "false")
      << ",\"matrix_row_count\":" << summary.matrix_row_count
      << ",\"rows\":[";
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    const auto &row = summary.rows[index];
    if (index != 0u) {
      out << ",";
    }
    out << "{\"row_key\":\"" << EscapeJsonString(row.row_key)
        << "\",\"artifact_kind\":\"" << EscapeJsonString(row.artifact_kind)
        << "\",\"fixture_path\":\"" << EscapeJsonString(row.fixture_path)
        << "\",\"emit_prefix\":\"" << EscapeJsonString(row.emit_prefix)
        << "\",\"artifact_relative_path\":\""
        << EscapeJsonString(row.artifact_relative_path)
        << "\",\"probe_command\":\"" << EscapeJsonString(row.probe_command)
        << "\",\"inspection_command\":\""
        << EscapeJsonString(row.inspection_command)
        << "\",\"expected_anchor\":\""
        << EscapeJsonString(row.expected_anchor) << "\"}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"active_typed_handoff_replay_key\":\""
      << EscapeJsonString(summary.active_typed_handoff_replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
