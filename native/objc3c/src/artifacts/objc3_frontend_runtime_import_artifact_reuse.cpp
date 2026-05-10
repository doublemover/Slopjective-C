#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {

using objc3::io::json::JsonObjectWriter;
using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

std::string BuildSerializedRuntimeMetadataArtifactReuseReplayKey(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_serialized_import_lowering_contract_id
      << ";artifact=" << summary.artifact_relative_path
      << ";reused_module_count=" << summary.reused_module_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";metadata_reference_count=" << summary.metadata_reference_count
      << ";modules=";
  for (std::size_t i = 0; i < summary.reused_module_names_lexicographic.size();
       ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << summary.reused_module_names_lexicographic[i];
  }
  return out.str();
}

Objc3SerializedRuntimeMetadataArtifactReuseSummary
BuildSerializedRuntimeMetadataArtifactReuseSummary(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary
        &serialized_import_lowering,
    const std::string &local_module_name,
    const Objc3RuntimeMetadataSourceRecordSet &reused_runtime_metadata_source_records,
    const std::vector<std::string> &reused_module_names_lexicographic) {
  Objc3SerializedRuntimeMetadataArtifactReuseSummary summary;
  summary.reused_module_names_lexicographic = reused_module_names_lexicographic;
  summary.reused_module_count = reused_module_names_lexicographic.size();
  summary.class_record_count =
      reused_runtime_metadata_source_records.classes_lexicographic.size();
  summary.protocol_record_count =
      reused_runtime_metadata_source_records.protocols_lexicographic.size();
  summary.category_record_count =
      reused_runtime_metadata_source_records.categories_lexicographic.size();
  summary.property_record_count =
      reused_runtime_metadata_source_records.properties_lexicographic.size();
  summary.method_record_count =
      reused_runtime_metadata_source_records.methods_lexicographic.size();
  summary.ivar_record_count =
      reused_runtime_metadata_source_records.ivars_lexicographic.size();
  summary.runtime_owned_declaration_count =
      CountRuntimeMetadataSourceRecordSetDeclarations(
          reused_runtime_metadata_source_records);
  summary.metadata_reference_count =
      CountRuntimeMetadataSourceRecordSetReferences(
          reused_runtime_metadata_source_records);
  summary.fail_closed = true;
  summary.source_serialized_import_lowering_ready =
      IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
          serialized_import_lowering);
  summary.semantic_surface_published = true;
  summary.serialized_metadata_rehydration_landed =
      summary.source_serialized_import_lowering_ready;
  summary.artifact_reuse_landed =
      summary.source_serialized_import_lowering_ready;
  summary.downstream_module_consumption_ready =
      summary.source_serialized_import_lowering_ready;
  if (summary.source_serialized_import_lowering_ready) {
    summary.replay_key =
        BuildSerializedRuntimeMetadataArtifactReuseReplayKey(summary);
  } else {
    summary.failure_reason =
        "serialized runtime metadata artifact reuse summary is incomplete";
  }
  if (!IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "serialized runtime metadata artifact reuse summary is incomplete";
  }
  (void)local_module_name;
  return summary;
}

std::string BuildSerializedRuntimeMetadataArtifactReuseSummaryJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_serialized_import_lowering_contract_id",
                     summary.source_serialized_import_lowering_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("artifact_relative_path", summary.artifact_relative_path);
  object.StringField("payload_member_name", summary.payload_member_name);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.StringArrayField("reused_module_names_lexicographic",
                          summary.reused_module_names_lexicographic);
  object.SizeField("reused_module_count", summary.reused_module_count);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("metadata_reference_count", summary.metadata_reference_count);
  object.BoolField(
      "ready",
      IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_serialized_import_lowering_ready",
                   summary.source_serialized_import_lowering_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("serialized_metadata_rehydration_landed",
                   summary.serialized_metadata_rehydration_landed);
  object.BoolField("artifact_reuse_landed", summary.artifact_reuse_landed);
  object.BoolField("downstream_module_consumption_ready",
                   summary.downstream_module_consumption_ready);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

}  // namespace objc3::artifacts::frontend
