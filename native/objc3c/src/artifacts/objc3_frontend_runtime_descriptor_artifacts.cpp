#include "artifacts/objc3_frontend_runtime_descriptor_artifacts.h"

#include <sstream>
#include <string>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"
#include "support/objc3_identifier_safe_suffix.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceReplayKey(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";source_surface_path=" << summary.source_surface_path
      << ";registration_descriptor_pragma_name="
      << summary.registration_descriptor_pragma_name
      << ";image_root_pragma_name=" << summary.image_root_pragma_name
      << ";module_identity_source=" << summary.module_identity_source
      << ";module_name=" << summary.module_name
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";image_root_identity_source=" << summary.image_root_identity_source
      << ";bootstrap_visible_metadata_ownership_model="
      << summary.bootstrap_visible_metadata_ownership_model
      << ";registration_descriptor_pragma_seen="
      << (summary.registration_descriptor_pragma_seen ? "true" : "false")
      << ";registration_descriptor_pragma_duplicate="
      << (summary.registration_descriptor_pragma_duplicate ? "true" : "false")
      << ";registration_descriptor_pragma_non_leading="
      << (summary.registration_descriptor_pragma_non_leading ? "true"
                                                             : "false")
      << ";registration_descriptor_pragma_directive_count="
      << summary.registration_descriptor_pragma_directive_count
      << ";image_root_pragma_seen="
      << (summary.image_root_pragma_seen ? "true" : "false")
      << ";image_root_pragma_duplicate="
      << (summary.image_root_pragma_duplicate ? "true" : "false")
      << ";image_root_pragma_non_leading="
      << (summary.image_root_pragma_non_leading ? "true" : "false")
      << ";image_root_pragma_directive_count="
      << summary.image_root_pragma_directive_count
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
    const Objc3Program &program,
    const Objc3FrontendBootstrapRegistrationSourcePragmaContract
        &bootstrap_registration_source_pragma_contract,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.source_surface_frozen = true;
  summary.prelude_pragma_contract_published = true;
  summary.bootstrap_visible_metadata_ownership_published = true;
  summary.module_name =
      program.module_name.empty() ? "objc3_module" : program.module_name;
  summary.registration_descriptor_pragma_seen =
      bootstrap_registration_source_pragma_contract.registration_descriptor.seen;
  summary.registration_descriptor_pragma_duplicate =
      bootstrap_registration_source_pragma_contract.registration_descriptor
          .duplicate;
  summary.registration_descriptor_pragma_non_leading =
      bootstrap_registration_source_pragma_contract.registration_descriptor
          .non_leading;
  summary.registration_descriptor_pragma_directive_count =
      bootstrap_registration_source_pragma_contract.registration_descriptor
          .directive_count;
  summary.image_root_pragma_seen =
      bootstrap_registration_source_pragma_contract.image_root.seen;
  summary.image_root_pragma_duplicate =
      bootstrap_registration_source_pragma_contract.image_root.duplicate;
  summary.image_root_pragma_non_leading =
      bootstrap_registration_source_pragma_contract.image_root.non_leading;
  summary.image_root_pragma_directive_count =
      bootstrap_registration_source_pragma_contract.image_root.directive_count;

  const std::string safe_module_name =
      objc3c::support::MakeIdentifierSafeSuffix(summary.module_name,
                                                "objc3_module");
  if (summary.registration_descriptor_pragma_seen &&
      !bootstrap_registration_source_pragma_contract.registration_descriptor
           .identifier.empty()) {
    summary.registration_descriptor_identifier =
        bootstrap_registration_source_pragma_contract.registration_descriptor
            .identifier;
    summary.registration_descriptor_identity_source =
        kObjc3RuntimeBootstrapDerivedIdentitySourcePragma;
  } else {
    summary.registration_descriptor_identifier =
        safe_module_name +
        kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  }
  if (summary.image_root_pragma_seen &&
      !bootstrap_registration_source_pragma_contract.image_root.identifier
           .empty()) {
    summary.image_root_identifier =
        bootstrap_registration_source_pragma_contract.image_root.identifier;
    summary.image_root_identity_source =
        kObjc3RuntimeBootstrapDerivedIdentitySourcePragma;
  } else {
    summary.image_root_identifier =
        safe_module_name + kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  }

  summary.registration_descriptor_identifier_resolved =
      !summary.registration_descriptor_identifier.empty();
  summary.image_root_identifier_resolved =
      !summary.image_root_identifier.empty();
  summary.ready_for_descriptor_frontend_closure =
      summary.registration_manifest_contract_ready &&
      summary.registration_descriptor_identifier_resolved &&
      summary.image_root_identifier_resolved;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_descriptor_frontend_closure) {
    summary.replay_key =
        BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceReplayKey(
            summary);
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
          summary)) {
    summary.failure_reason =
        "registration descriptor/image-root source surface summary is incomplete";
  }
  return summary;
}

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

std::string BuildRuntimeRegistrationDescriptorFrontendClosureReplayKey(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_surface_contract_id=" << summary.source_surface_contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";payload_model=" << summary.payload_model
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";authority_model=" << summary.authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";payload_ownership_model=" << summary.payload_ownership_model
      << ";registration_descriptor_identifier="
      << summary.registration_descriptor_identifier
      << ";image_root_identifier=" << summary.image_root_identifier
      << ";registration_descriptor_identity_source="
      << summary.registration_descriptor_identity_source
      << ";image_root_identity_source="
      << summary.image_root_identity_source
      << ";bootstrap_visible_metadata_ownership_model="
      << summary.bootstrap_visible_metadata_ownership_model
      << ";class_descriptor_count=" << summary.class_descriptor_count
      << ";protocol_descriptor_count=" << summary.protocol_descriptor_count
      << ";category_descriptor_count=" << summary.category_descriptor_count
      << ";property_descriptor_count=" << summary.property_descriptor_count
      << ";ivar_descriptor_count=" << summary.ivar_descriptor_count
      << ";total_descriptor_count=" << summary.total_descriptor_count
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";source_surface_replay_key=" << summary.source_surface_replay_key
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
BuildRuntimeRegistrationDescriptorFrontendClosureSummary(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &source_surface_summary,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest_summary) {
  Objc3RuntimeRegistrationDescriptorFrontendClosureSummary summary;
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
          source_surface_summary);
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest_summary);
  summary.descriptor_frontend_surface_published = true;
  summary.descriptor_artifact_template_published = true;
  summary.registration_descriptor_identifier =
      source_surface_summary.registration_descriptor_identifier;
  summary.image_root_identifier = source_surface_summary.image_root_identifier;
  summary.registration_descriptor_identity_source =
      source_surface_summary.registration_descriptor_identity_source;
  summary.image_root_identity_source =
      source_surface_summary.image_root_identity_source;
  summary.bootstrap_visible_metadata_ownership_model =
      source_surface_summary.bootstrap_visible_metadata_ownership_model;
  summary.class_descriptor_count =
      registration_manifest_summary.class_descriptor_count;
  summary.protocol_descriptor_count =
      registration_manifest_summary.protocol_descriptor_count;
  summary.category_descriptor_count =
      registration_manifest_summary.category_descriptor_count;
  summary.property_descriptor_count =
      registration_manifest_summary.property_descriptor_count;
  summary.ivar_descriptor_count =
      registration_manifest_summary.ivar_descriptor_count;
  summary.total_descriptor_count =
      registration_manifest_summary.total_descriptor_count;
  summary.translation_unit_registration_order_ordinal =
      registration_manifest_summary.translation_unit_registration_order_ordinal;
  summary.descriptor_fields_resolved =
      !summary.registration_descriptor_identifier.empty() &&
      !summary.image_root_identifier.empty();
  summary.ready_for_descriptor_artifact_emission =
      summary.source_surface_contract_ready &&
      summary.registration_manifest_contract_ready &&
      summary.descriptor_fields_resolved;
  summary.ready_for_registration_descriptor_lowering =
      summary.ready_for_descriptor_artifact_emission;
  if (summary.source_surface_contract_ready) {
    summary.source_surface_replay_key = source_surface_summary.replay_key;
  }
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key =
        registration_manifest_summary.replay_key;
  }
  if (summary.ready_for_registration_descriptor_lowering) {
    summary.replay_key =
        BuildRuntimeRegistrationDescriptorFrontendClosureReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          summary)) {
    summary.failure_reason =
        "registration descriptor frontend closure summary is incomplete";
  }
  return summary;
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
