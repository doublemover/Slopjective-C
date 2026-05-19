#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>

#include "io/json/json_writer.h"

namespace objc3::artifacts::frontend {

using objc3::io::json::JsonObjectWriter;

std::string BuildSerializedRuntimeMetadataImportLoweringReplayKey(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_imported_semantic_rules_contract_id
      << ";input_model=" << summary.input_model
      << ";imported_input_path_count=" << summary.imported_input_path_count
      << ";imported_module_count=" << summary.imported_module_count
      << ";imported_surface_ingest_landed="
      << (summary.imported_surface_ingest_landed ? "true" : "false")
      << ";serialized_metadata_rehydration_landed="
      << (summary.serialized_metadata_rehydration_landed ? "true" : "false")
      << ";incremental_reuse_landed="
      << (summary.incremental_reuse_landed ? "true" : "false")
      << ";imported_metadata_ir_lowering_landed="
      << (summary.imported_metadata_ir_lowering_landed ? "true" : "false")
      << ";public_live_imported_payload_abi_landed="
      << (summary.public_live_imported_payload_abi_landed ? "true" : "false");
  return out.str();
}

Objc3SerializedRuntimeMetadataImportLoweringSummary
BuildSerializedRuntimeMetadataImportLoweringSummary(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules) {
  Objc3SerializedRuntimeMetadataImportLoweringSummary summary;
  summary.imported_input_path_count =
      imported_runtime_metadata_semantic_rules.imported_input_path_count;
  summary.imported_module_count =
      imported_runtime_metadata_semantic_rules.imported_module_count;
  summary.fail_closed = true;
  summary.source_imported_semantic_rules_ready =
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          imported_runtime_metadata_semantic_rules);
  summary.semantic_surface_published = true;
  summary.imported_surface_ingest_landed =
      imported_runtime_metadata_semantic_rules
          .ready_for_imported_metadata_semantic_rules;
  if (summary.source_imported_semantic_rules_ready) {
    summary.replay_key =
        BuildSerializedRuntimeMetadataImportLoweringReplayKey(summary);
  } else {
    summary.failure_reason =
        "serialized runtime metadata import/lowering summary is incomplete";
  }
  if (!IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "serialized runtime metadata import/lowering summary is incomplete";
  }
  return summary;
}

std::string BuildSerializedRuntimeMetadataImportLoweringSummaryJson(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_imported_semantic_rules_contract_id",
                     summary.source_imported_semantic_rules_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("source_artifact_relative_path",
                     summary.source_artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.SizeField("imported_input_path_count",
                   summary.imported_input_path_count);
  object.SizeField("imported_module_count", summary.imported_module_count);
  object.BoolField(
      "ready",
      IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_imported_semantic_rules_ready",
                   summary.source_imported_semantic_rules_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_surface_ingest_landed",
                   summary.imported_surface_ingest_landed);
  object.BoolField("serialized_metadata_rehydration_landed",
                   summary.serialized_metadata_rehydration_landed);
  object.BoolField("incremental_reuse_landed",
                   summary.incremental_reuse_landed);
  object.BoolField("imported_metadata_ir_lowering_landed",
                   summary.imported_metadata_ir_lowering_landed);
  object.BoolField("public_live_imported_payload_abi_landed",
                   summary.public_live_imported_payload_abi_landed);
  object.BoolField("ready_for_serialized_metadata_lowering_impl",
                   summary.ready_for_serialized_metadata_lowering_impl);
  object.BoolField("ready_for_incremental_reuse_impl",
                   summary.ready_for_incremental_reuse_impl);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

}  // namespace objc3::artifacts::frontend
