#include "artifacts/objc3_frontend_runtime_descriptor_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummaryJson(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"source_surface_path\":\""
      << EscapeJsonString(summary.source_surface_path)
      << "\",\"registration_descriptor_pragma_name\":\""
      << EscapeJsonString(summary.registration_descriptor_pragma_name)
      << "\",\"image_root_pragma_name\":\""
      << EscapeJsonString(summary.image_root_pragma_name)
      << "\",\"module_identity_source\":\""
      << EscapeJsonString(summary.module_identity_source)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"bootstrap_visible_metadata_ownership_model\":\""
      << EscapeJsonString(summary.bootstrap_visible_metadata_ownership_model)
      << "\",\"module_name\":\"" << EscapeJsonString(summary.module_name)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
              summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"source_surface_frozen\":"
      << (summary.source_surface_frozen ? "true" : "false")
      << ",\"prelude_pragma_contract_published\":"
      << (summary.prelude_pragma_contract_published ? "true" : "false")
      << ",\"registration_descriptor_identifier_resolved\":"
      << (summary.registration_descriptor_identifier_resolved ? "true"
                                                              : "false")
      << ",\"image_root_identifier_resolved\":"
      << (summary.image_root_identifier_resolved ? "true" : "false")
      << ",\"bootstrap_visible_metadata_ownership_published\":"
      << (summary.bootstrap_visible_metadata_ownership_published ? "true"
                                                                 : "false")
      << ",\"ready_for_descriptor_frontend_closure\":"
      << (summary.ready_for_descriptor_frontend_closure ? "true" : "false")
      << ",\"registration_descriptor_pragma_seen\":"
      << (summary.registration_descriptor_pragma_seen ? "true" : "false")
      << ",\"registration_descriptor_pragma_duplicate\":"
      << (summary.registration_descriptor_pragma_duplicate ? "true" : "false")
      << ",\"registration_descriptor_pragma_non_leading\":"
      << (summary.registration_descriptor_pragma_non_leading ? "true"
                                                             : "false")
      << ",\"registration_descriptor_pragma_directive_count\":"
      << summary.registration_descriptor_pragma_directive_count
      << ",\"image_root_pragma_seen\":"
      << (summary.image_root_pragma_seen ? "true" : "false")
      << ",\"image_root_pragma_duplicate\":"
      << (summary.image_root_pragma_duplicate ? "true" : "false")
      << ",\"image_root_pragma_non_leading\":"
      << (summary.image_root_pragma_non_leading ? "true" : "false")
      << ",\"image_root_pragma_directive_count\":"
      << summary.image_root_pragma_directive_count
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeRegistrationDescriptorFrontendClosureSummaryJson(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"source_surface_contract_id\":\""
      << EscapeJsonString(summary.source_surface_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"artifact_relative_path\":\""
      << EscapeJsonString(summary.artifact_relative_path)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"payload_ownership_model\":\""
      << EscapeJsonString(summary.payload_ownership_model)
      << "\",\"registration_descriptor_identifier\":\""
      << EscapeJsonString(summary.registration_descriptor_identifier)
      << "\",\"image_root_identifier\":\""
      << EscapeJsonString(summary.image_root_identifier)
      << "\",\"registration_descriptor_identity_source\":\""
      << EscapeJsonString(summary.registration_descriptor_identity_source)
      << "\",\"image_root_identity_source\":\""
      << EscapeJsonString(summary.image_root_identity_source)
      << "\",\"bootstrap_visible_metadata_ownership_model\":\""
      << EscapeJsonString(summary.bootstrap_visible_metadata_ownership_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_surface_contract_ready\":"
      << (summary.source_surface_contract_ready ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"descriptor_frontend_surface_published\":"
      << (summary.descriptor_frontend_surface_published ? "true" : "false")
      << ",\"descriptor_artifact_template_published\":"
      << (summary.descriptor_artifact_template_published ? "true" : "false")
      << ",\"descriptor_fields_resolved\":"
      << (summary.descriptor_fields_resolved ? "true" : "false")
      << ",\"ready_for_descriptor_artifact_emission\":"
      << (summary.ready_for_descriptor_artifact_emission ? "true" : "false")
      << ",\"ready_for_registration_descriptor_lowering\":"
      << (summary.ready_for_registration_descriptor_lowering ? "true" : "false")
      << ",\"class_descriptor_count\":" << summary.class_descriptor_count
      << ",\"protocol_descriptor_count\":"
      << summary.protocol_descriptor_count
      << ",\"category_descriptor_count\":"
      << summary.category_descriptor_count
      << ",\"property_descriptor_count\":"
      << summary.property_descriptor_count
      << ",\"ivar_descriptor_count\":" << summary.ivar_descriptor_count
      << ",\"total_descriptor_count\":" << summary.total_descriptor_count
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"source_surface_replay_key\":\""
      << EscapeJsonString(summary.source_surface_replay_key)
      << "\",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
