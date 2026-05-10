#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/objc3_json.h"
#include "runtime/metadata/selector_metadata.h"
#include "sema/model/semantic_type.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << "\"" << EscapeJsonString(values[i]) << "\"";
  }
  out << "]";
  return out.str();
}

}  // namespace

std::string BuildCrossModuleBuildRuntimeOrchestrationReplayKey(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary
             .source_serialized_runtime_metadata_artifact_reuse_contract_id
      << ";local_manifest_contract="
      << summary.source_local_registration_manifest_contract_id
      << ";imported_runtime_semantic_rules_contract="
      << summary.source_imported_runtime_metadata_semantic_rules_contract_id
      << ";module_image_count=" << summary.module_image_count
      << ";direct_import_input_count=" << summary.direct_import_input_count
      << ";local_total_descriptor_count="
      << summary.local_total_descriptor_count
      << ";transitive_runtime_owned_declaration_count="
      << summary.transitive_runtime_owned_declaration_count
      << ";transitive_metadata_reference_count="
      << summary.transitive_metadata_reference_count
      << ";imported_optional_send_site_count="
      << summary.imported_optional_send_site_count
      << ";imported_typed_keypath_literal_site_count="
      << summary.imported_typed_keypath_literal_site_count << ";modules=";
  for (std::size_t i = 0; i < summary.module_names_lexicographic.size(); ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << summary.module_names_lexicographic[i];
  }
  return out.str();
}

Objc3CrossModuleBuildRuntimeOrchestrationSummary
BuildCrossModuleBuildRuntimeOrchestrationSummary(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &local_runtime_registration_manifest,
    std::size_t direct_import_input_count) {
  Objc3CrossModuleBuildRuntimeOrchestrationSummary summary;
  summary.module_names_lexicographic =
      serialized_runtime_metadata_artifact_reuse
          .reused_module_names_lexicographic;
  summary.module_image_count = summary.module_names_lexicographic.size();
  summary.direct_import_input_count = direct_import_input_count;
  summary.local_class_descriptor_count =
      local_runtime_registration_manifest.class_descriptor_count;
  summary.local_protocol_descriptor_count =
      local_runtime_registration_manifest.protocol_descriptor_count;
  summary.local_category_descriptor_count =
      local_runtime_registration_manifest.category_descriptor_count;
  summary.local_property_descriptor_count =
      local_runtime_registration_manifest.property_descriptor_count;
  summary.local_ivar_descriptor_count =
      local_runtime_registration_manifest.ivar_descriptor_count;
  summary.local_total_descriptor_count =
      local_runtime_registration_manifest.total_descriptor_count;
  summary.transitive_runtime_owned_declaration_count =
      serialized_runtime_metadata_artifact_reuse
          .runtime_owned_declaration_count;
  summary.transitive_metadata_reference_count =
      serialized_runtime_metadata_artifact_reuse.metadata_reference_count;
  summary.imported_optional_send_site_count =
      imported_runtime_metadata_semantic_rules.optional_send_site_count;
  summary.imported_typed_keypath_literal_site_count =
      imported_runtime_metadata_semantic_rules.typed_keypath_literal_site_count;
  summary.imported_live_optional_lowering_site_count =
      imported_runtime_metadata_semantic_rules.live_optional_lowering_site_count;
  summary.imported_live_typed_keypath_artifact_site_count =
      imported_runtime_metadata_semantic_rules
          .live_typed_keypath_artifact_site_count;
  summary.fail_closed = true;
  summary.source_serialized_runtime_metadata_artifact_reuse_ready =
      IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
          serialized_runtime_metadata_artifact_reuse);
  summary.source_imported_runtime_metadata_semantic_rules_ready =
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          imported_runtime_metadata_semantic_rules);
  summary.source_local_registration_manifest_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          local_runtime_registration_manifest);
  summary.semantic_surface_published = true;
  summary.local_registration_manifest_emitted =
      local_runtime_registration_manifest
          .runtime_registration_artifact_emitted_by_driver;
  summary.imported_type_system_type_surface_landed =
      imported_runtime_metadata_semantic_rules.imported_type_system_type_surface_landed;
  summary.imported_optional_runtime_semantics_landed =
      imported_runtime_metadata_semantic_rules
          .imported_optional_runtime_semantics_landed;
  summary.imported_typed_keypath_runtime_semantics_landed =
      imported_runtime_metadata_semantic_rules
          .imported_typed_keypath_runtime_semantics_landed;
  summary.source_serialized_runtime_metadata_replay_key =
      serialized_runtime_metadata_artifact_reuse.replay_key;
  summary.source_local_registration_manifest_replay_key =
      local_runtime_registration_manifest.replay_key;
  summary.source_imported_runtime_metadata_semantic_rules_replay_key =
      imported_runtime_metadata_semantic_rules.replay_key;
  if (summary.source_serialized_runtime_metadata_artifact_reuse_ready &&
      summary.source_imported_runtime_metadata_semantic_rules_ready &&
      summary.source_local_registration_manifest_ready) {
    summary.replay_key =
        BuildCrossModuleBuildRuntimeOrchestrationReplayKey(summary);
  } else {
    summary.failure_reason =
        "cross-module build/runtime orchestration contract is incomplete";
  }
  if (!IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "cross-module build/runtime orchestration contract is incomplete";
  }
  return summary;
}

std::string BuildCrossModuleBuildRuntimeOrchestrationSummaryJson(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_serialized_runtime_metadata_artifact_reuse_contract_id\":\""
      << EscapeJsonString(
             summary
                 .source_serialized_runtime_metadata_artifact_reuse_contract_id)
      << "\",\"source_local_registration_manifest_contract_id\":\""
      << EscapeJsonString(
             summary.source_local_registration_manifest_contract_id)
      << "\",\"source_imported_runtime_metadata_semantic_rules_contract_id\":\""
      << EscapeJsonString(
             summary.source_imported_runtime_metadata_semantic_rules_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"import_artifact_relative_path\":\""
      << EscapeJsonString(summary.import_artifact_relative_path)
      << "\",\"local_registration_manifest_artifact_relative_path\":\""
      << EscapeJsonString(
             summary.local_registration_manifest_artifact_relative_path)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"input_model\":\"" << EscapeJsonString(summary.input_model)
      << "\",\"registration_scope_model\":\""
      << EscapeJsonString(summary.registration_scope_model)
      << "\",\"packaging_model\":\""
      << EscapeJsonString(summary.packaging_model)
      << "\",\"module_names_lexicographic\":"
      << BuildStringArrayJson(summary.module_names_lexicographic)
      << ",\"module_image_count\":" << summary.module_image_count
      << ",\"direct_import_input_count\":" << summary.direct_import_input_count
      << ",\"local_class_descriptor_count\":"
      << summary.local_class_descriptor_count
      << ",\"local_protocol_descriptor_count\":"
      << summary.local_protocol_descriptor_count
      << ",\"local_category_descriptor_count\":"
      << summary.local_category_descriptor_count
      << ",\"local_property_descriptor_count\":"
      << summary.local_property_descriptor_count
      << ",\"local_ivar_descriptor_count\":"
      << summary.local_ivar_descriptor_count
      << ",\"local_total_descriptor_count\":"
      << summary.local_total_descriptor_count
      << ",\"transitive_runtime_owned_declaration_count\":"
      << summary.transitive_runtime_owned_declaration_count
      << ",\"transitive_metadata_reference_count\":"
      << summary.transitive_metadata_reference_count
      << ",\"imported_optional_send_site_count\":"
      << summary.imported_optional_send_site_count
      << ",\"imported_typed_keypath_literal_site_count\":"
      << summary.imported_typed_keypath_literal_site_count
      << ",\"imported_live_optional_lowering_site_count\":"
      << summary.imported_live_optional_lowering_site_count
      << ",\"imported_live_typed_keypath_artifact_site_count\":"
      << summary.imported_live_typed_keypath_artifact_site_count
      << ",\"ready\":"
      << (IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_serialized_runtime_metadata_artifact_reuse_ready\":"
      << (summary.source_serialized_runtime_metadata_artifact_reuse_ready
              ? "true"
              : "false")
      << ",\"source_local_registration_manifest_ready\":"
      << (summary.source_local_registration_manifest_ready ? "true" : "false")
      << ",\"source_imported_runtime_metadata_semantic_rules_ready\":"
      << (summary.source_imported_runtime_metadata_semantic_rules_ready ? "true"
                                                                        : "false")
      << ",\"semantic_surface_published\":"
      << (summary.semantic_surface_published ? "true" : "false")
      << ",\"local_registration_manifest_emitted\":"
      << (summary.local_registration_manifest_emitted ? "true" : "false")
      << ",\"imported_type_system_type_surface_landed\":"
      << (summary.imported_type_system_type_surface_landed ? "true" : "false")
      << ",\"imported_optional_runtime_semantics_landed\":"
      << (summary.imported_optional_runtime_semantics_landed ? "true"
                                                             : "false")
      << ",\"imported_typed_keypath_runtime_semantics_landed\":"
      << (summary.imported_typed_keypath_runtime_semantics_landed ? "true"
                                                                  : "false")
      << ",\"cross_module_link_plan_artifact_landed\":"
      << (summary.cross_module_link_plan_artifact_landed ? "true" : "false")
      << ",\"imported_registration_manifest_loading_landed\":"
      << (summary.imported_registration_manifest_loading_landed ? "true"
                                                                : "false")
      << ",\"runtime_archive_aggregation_landed\":"
      << (summary.runtime_archive_aggregation_landed ? "true" : "false")
      << ",\"cross_module_runtime_registration_landed\":"
      << (summary.cross_module_runtime_registration_landed ? "true" : "false")
      << ",\"cross_module_launch_orchestration_landed\":"
      << (summary.cross_module_launch_orchestration_landed ? "true"
                                                           : "false")
      << ",\"public_cross_module_orchestration_abi_landed\":"
      << (summary.public_cross_module_orchestration_abi_landed ? "true"
                                                               : "false")
      << ",\"ready_for_packaging_and_runtime_registration_impl\":"
      << (summary.ready_for_packaging_and_runtime_registration_impl ? "true"
                                                                   : "false")
      << ",\"source_serialized_runtime_metadata_replay_key\":\""
      << EscapeJsonString(
             summary.source_serialized_runtime_metadata_replay_key)
      << "\",\"source_local_registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.source_local_registration_manifest_replay_key)
      << "\",\"source_imported_runtime_metadata_semantic_rules_replay_key\":\""
      << EscapeJsonString(
             summary.source_imported_runtime_metadata_semantic_rules_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
