#include "artifacts/objc3_frontend_artifact_runtime_registration_descriptor_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {

using objc3::io::EscapeJsonString;

void AppendObjc3FrontendArtifactRuntimeRegistrationDescriptorManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan) {
  const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
      &runtime_registration_descriptor_image_root_source_surface =
          runtime_registration_plan
              .runtime_registration_descriptor_image_root_source_surface;
  const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
      &runtime_registration_descriptor_frontend_closure =
          runtime_registration_plan
              .runtime_registration_descriptor_frontend_closure;

  manifest
      << ",\"runtime_registration_descriptor_image_root_source_surface_contract_id\":\""
      << EscapeJsonString(runtime_registration_descriptor_image_root_source_surface
                              .contract_id)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_path\":\""
      << EscapeJsonString(runtime_registration_descriptor_image_root_source_surface
                              .source_surface_path)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_name\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_image_root_source_surface
                 .registration_descriptor_pragma_name)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_name\":\""
      << EscapeJsonString(runtime_registration_descriptor_image_root_source_surface
                              .image_root_pragma_name)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_module_identity_source\":\""
      << EscapeJsonString(runtime_registration_descriptor_image_root_source_surface
                              .module_identity_source)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identity_source\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_image_root_source_surface
                 .registration_descriptor_identity_source)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_identity_source\":\""
      << EscapeJsonString(runtime_registration_descriptor_image_root_source_surface
                              .image_root_identity_source)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_bootstrap_visible_metadata_ownership_model\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_image_root_source_surface
                 .bootstrap_visible_metadata_ownership_model)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_module_name\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_image_root_source_surface.module_name)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identifier\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_image_root_source_surface
                 .registration_descriptor_identifier)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_identifier\":\""
      << EscapeJsonString(runtime_registration_descriptor_image_root_source_surface
                              .image_root_identifier)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_manifest_replay_key\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_image_root_source_surface
                 .registration_manifest_replay_key)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_replay_key\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_image_root_source_surface.replay_key)
      << "\",\"runtime_registration_descriptor_image_root_source_surface_fail_closed\":"
      << (runtime_registration_descriptor_image_root_source_surface.fail_closed
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_registration_manifest_contract_ready\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .registration_manifest_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_source_surface_frozen\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .source_surface_frozen
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_prelude_pragma_contract_published\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .prelude_pragma_contract_published
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identifier_resolved\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .registration_descriptor_identifier_resolved
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_identifier_resolved\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .image_root_identifier_resolved
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_bootstrap_visible_metadata_ownership_published\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .bootstrap_visible_metadata_ownership_published
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_ready_for_descriptor_frontend_closure\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .ready_for_descriptor_frontend_closure
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_seen\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .registration_descriptor_pragma_seen
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_duplicate\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .registration_descriptor_pragma_duplicate
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_non_leading\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .registration_descriptor_pragma_non_leading
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_directive_count\":"
      << runtime_registration_descriptor_image_root_source_surface
             .registration_descriptor_pragma_directive_count
      << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_seen\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .image_root_pragma_seen
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_duplicate\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .image_root_pragma_duplicate
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_non_leading\":"
      << (runtime_registration_descriptor_image_root_source_surface
                  .image_root_pragma_non_leading
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_directive_count\":"
      << runtime_registration_descriptor_image_root_source_surface
             .image_root_pragma_directive_count
      << ",\"runtime_registration_descriptor_image_root_source_surface_failure_reason\":\""
      << EscapeJsonString(runtime_registration_descriptor_image_root_source_surface
                              .failure_reason)
      << "\""
      << ",\"runtime_registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_frontend_closure.contract_id)
      << "\",\"runtime_registration_descriptor_frontend_closure_source_surface_contract_id\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .source_surface_contract_id)
      << "\",\"runtime_registration_descriptor_frontend_closure_payload_model\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_frontend_closure.payload_model)
      << "\",\"runtime_registration_descriptor_frontend_closure_artifact_relative_path\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .artifact_relative_path)
      << "\",\"runtime_registration_descriptor_frontend_closure_authority_model\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_frontend_closure.authority_model)
      << "\",\"runtime_registration_descriptor_frontend_closure_translation_unit_identity_model\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .translation_unit_identity_model)
      << "\",\"runtime_registration_descriptor_frontend_closure_payload_ownership_model\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .payload_ownership_model)
      << "\",\"runtime_registration_descriptor_frontend_closure_registration_descriptor_identifier\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .registration_descriptor_identifier)
      << "\",\"runtime_registration_descriptor_frontend_closure_image_root_identifier\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .image_root_identifier)
      << "\",\"runtime_registration_descriptor_frontend_closure_registration_descriptor_identity_source\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .registration_descriptor_identity_source)
      << "\",\"runtime_registration_descriptor_frontend_closure_image_root_identity_source\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .image_root_identity_source)
      << "\",\"runtime_registration_descriptor_frontend_closure_bootstrap_visible_metadata_ownership_model\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .bootstrap_visible_metadata_ownership_model)
      << "\",\"runtime_registration_descriptor_frontend_closure_class_descriptor_count\":"
      << runtime_registration_descriptor_frontend_closure.class_descriptor_count
      << ",\"runtime_registration_descriptor_frontend_closure_protocol_descriptor_count\":"
      << runtime_registration_descriptor_frontend_closure
             .protocol_descriptor_count
      << ",\"runtime_registration_descriptor_frontend_closure_category_descriptor_count\":"
      << runtime_registration_descriptor_frontend_closure.category_descriptor_count
      << ",\"runtime_registration_descriptor_frontend_closure_property_descriptor_count\":"
      << runtime_registration_descriptor_frontend_closure.property_descriptor_count
      << ",\"runtime_registration_descriptor_frontend_closure_ivar_descriptor_count\":"
      << runtime_registration_descriptor_frontend_closure.ivar_descriptor_count
      << ",\"runtime_registration_descriptor_frontend_closure_total_descriptor_count\":"
      << runtime_registration_descriptor_frontend_closure.total_descriptor_count
      << ",\"runtime_registration_descriptor_frontend_closure_translation_unit_registration_order_ordinal\":"
      << runtime_registration_descriptor_frontend_closure
             .translation_unit_registration_order_ordinal
      << ",\"runtime_registration_descriptor_frontend_closure_fail_closed\":"
      << (runtime_registration_descriptor_frontend_closure.fail_closed ? "true"
                                                                      : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_source_surface_contract_ready\":"
      << (runtime_registration_descriptor_frontend_closure
                  .source_surface_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_registration_manifest_contract_ready\":"
      << (runtime_registration_descriptor_frontend_closure
                  .registration_manifest_contract_ready
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_descriptor_frontend_surface_published\":"
      << (runtime_registration_descriptor_frontend_closure
                  .descriptor_frontend_surface_published
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_descriptor_artifact_template_published\":"
      << (runtime_registration_descriptor_frontend_closure
                  .descriptor_artifact_template_published
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_descriptor_fields_resolved\":"
      << (runtime_registration_descriptor_frontend_closure
                  .descriptor_fields_resolved
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_ready_for_descriptor_artifact_emission\":"
      << (runtime_registration_descriptor_frontend_closure
                  .ready_for_descriptor_artifact_emission
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_ready_for_registration_descriptor_lowering\":"
      << (runtime_registration_descriptor_frontend_closure
                  .ready_for_registration_descriptor_lowering
              ? "true"
              : "false")
      << ",\"runtime_registration_descriptor_frontend_closure_source_surface_replay_key\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .source_surface_replay_key)
      << "\",\"runtime_registration_descriptor_frontend_closure_registration_manifest_replay_key\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .registration_manifest_replay_key)
      << "\",\"runtime_registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                              .replay_key)
      << "\",\"runtime_registration_descriptor_frontend_closure_failure_reason\":\""
      << EscapeJsonString(
             runtime_registration_descriptor_frontend_closure.failure_reason)
      << "\"";
}

}  // namespace objc3::artifacts::frontend
