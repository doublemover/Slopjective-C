#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <sstream>

namespace objc3::artifacts::frontend {

std::string BuildExecutableMetadataDebugProjectionRowDescriptor(
    const Objc3ExecutableMetadataDebugProjectionMatrixRow &row) {
  std::ostringstream out;
  out << row.row_key << "|" << row.artifact_kind << "|" << row.fixture_path << "|"
      << row.emit_prefix << "|" << row.artifact_relative_path << "|"
      << row.probe_command << "|" << row.inspection_command << "|"
      << row.expected_anchor;
  return out.str();
}

std::string BuildExecutableMetadataDebugProjectionReplayKey(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";typed_handoff_contract_id="
      << summary.typed_lowering_handoff_contract_id
      << ";source_graph_contract_id=" << summary.source_graph_contract_id
      << ";named_metadata_name=" << summary.named_metadata_name
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";typed_handoff_surface_path=" << summary.typed_handoff_surface_path
      << ";source_graph_surface_path=" << summary.source_graph_surface_path;
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    out << ";row[" << index << "]="
        << BuildExecutableMetadataDebugProjectionRowDescriptor(
               summary.rows[index]);
  }
  return out.str();
}

Objc3ExecutableMetadataDebugProjectionSummary
BuildExecutableMetadataDebugProjectionSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff) {
  Objc3ExecutableMetadataDebugProjectionSummary summary;
  summary.fail_closed = true;
  summary.matrix_published = true;
  summary.manifest_debug_surface_published = true;
  summary.ir_named_metadata_published = true;
  summary.active_typed_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          executable_metadata_typed_lowering_handoff);
  if (summary.active_typed_handoff_ready) {
    summary.active_typed_handoff_replay_key =
        executable_metadata_typed_lowering_handoff.replay_key;
  }
  summary.rows[0] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionClassManifestRowKey,
      "manifest",
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionManifestRelativePath,
      kObjc3ExecutableMetadataDebugProjectionClassProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath};
  summary.rows[1] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionCategoryManifestRowKey,
      "manifest",
      kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionManifestRelativePath,
      kObjc3ExecutableMetadataDebugProjectionCategoryProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand,
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath};
  summary.rows[2] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionIrNamedMetadataRowKey,
      "llvm-ir",
      kObjc3ExecutableMetadataDebugProjectionIrFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionIrRelativePath,
      kObjc3ExecutableMetadataDebugProjectionIrProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionIrInspectionCommand,
      kObjc3ExecutableMetadataDebugProjectionNamedMetadataName};
  summary.matrix_row_count = summary.rows.size();
  summary.replay_key =
      BuildExecutableMetadataDebugProjectionReplayKey(summary);
  summary.replay_anchor_deterministic = !summary.replay_key.empty();
  if (!IsReadyObjc3ExecutableMetadataDebugProjectionSummary(summary)) {
    summary.failure_reason =
        "executable metadata debug projection matrix contract is incomplete";
  }
  return summary;
}

}  // namespace objc3::artifacts::frontend
