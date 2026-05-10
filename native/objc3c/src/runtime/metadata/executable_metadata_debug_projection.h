#pragma once

#include <array>
#include <cstddef>
#include <string>

struct Objc3ExecutableMetadataDebugProjectionMatrixRow {
  std::string row_key;
  std::string artifact_kind;
  std::string fixture_path;
  std::string emit_prefix = kObjc3ExecutableMetadataDebugProjectionEmitPrefix;
  std::string artifact_relative_path;
  std::string probe_command;
  std::string inspection_command;
  std::string expected_anchor;
};

struct Objc3ExecutableMetadataDebugProjectionSummary {
  std::string contract_id = kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string source_graph_contract_id =
      kObjc3ExecutableMetadataSourceGraphContractId;
  std::string named_metadata_name =
      kObjc3ExecutableMetadataDebugProjectionNamedMetadataName;
  std::string manifest_surface_path =
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath;
  std::string typed_handoff_surface_path =
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath;
  std::string source_graph_surface_path =
      kObjc3ExecutableMetadataSourceGraphManifestSurfacePath;
  bool matrix_published = false;
  bool fail_closed = false;
  bool manifest_debug_surface_published = false;
  bool ir_named_metadata_published = false;
  bool replay_anchor_deterministic = false;
  bool active_typed_handoff_ready = false;
  std::size_t matrix_row_count = 0;
  std::array<Objc3ExecutableMetadataDebugProjectionMatrixRow, 3u> rows = {};
  std::string replay_key;
  std::string active_typed_handoff_replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  if (summary.contract_id.empty() || summary.typed_lowering_handoff_contract_id.empty() ||
      summary.source_graph_contract_id.empty() ||
      summary.named_metadata_name.empty() ||
      summary.manifest_surface_path.empty() ||
      summary.typed_handoff_surface_path.empty() ||
      summary.source_graph_surface_path.empty() || !summary.matrix_published ||
      !summary.fail_closed || !summary.manifest_debug_surface_published ||
      !summary.ir_named_metadata_published ||
      !summary.replay_anchor_deterministic || summary.matrix_row_count != summary.rows.size() ||
      summary.replay_key.empty() || !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &row : summary.rows) {
    if (row.row_key.empty() || row.artifact_kind.empty() || row.fixture_path.empty() ||
        row.emit_prefix.empty() || row.artifact_relative_path.empty() ||
        row.probe_command.empty() || row.inspection_command.empty() ||
        row.expected_anchor.empty()) {
      return false;
    }
  }
  return true;
}
