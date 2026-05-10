#include "io/objc3_cross_module_imported_modules_document_record_header.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void EmitObjc3CrossModuleImportedModuleRecordHeaderJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input) {
  out << "    {\n"
      << "      \"module_name\": \""
      << EscapeJsonString(imported_input.module_name) << "\",\n"
      << "      \"import_surface_artifact_path\": \""
      << EscapeJsonString(imported_input.import_surface_artifact_path)
      << "\",\n"
      << "      \"registration_manifest_artifact_path\": \""
      << EscapeJsonString(imported_input.registration_manifest_artifact_path)
      << "\",\n"
      << "      \"object_artifact_path\": \""
      << EscapeJsonString(imported_input.object_artifact_path) << "\",\n"
      << "      \"discovery_artifact_path\": \""
      << EscapeJsonString(imported_input.discovery_artifact_path) << "\",\n"
      << "      \"linker_response_artifact_path\": \""
      << EscapeJsonString(imported_input.linker_response_artifact_path)
      << "\",\n"
      << "      \"translation_unit_identity_model\": \""
      << EscapeJsonString(imported_input.translation_unit_identity_model)
      << "\",\n"
      << "      \"translation_unit_identity_key\": \""
      << EscapeJsonString(imported_input.translation_unit_identity_key)
      << "\",\n"
      << "      \"translation_unit_registration_order_ordinal\": "
      << imported_input.translation_unit_registration_order_ordinal << ",\n"
      << "      \"class_descriptor_count\": "
      << imported_input.class_descriptor_count << ",\n"
      << "      \"protocol_descriptor_count\": "
      << imported_input.protocol_descriptor_count << ",\n"
      << "      \"category_descriptor_count\": "
      << imported_input.category_descriptor_count << ",\n"
      << "      \"property_descriptor_count\": "
      << imported_input.property_descriptor_count << ",\n"
      << "      \"ivar_descriptor_count\": "
      << imported_input.ivar_descriptor_count << ",\n"
      << "      \"total_descriptor_count\": "
      << imported_input.total_descriptor_count << ",\n"
      << "      \"object_format\": \""
      << EscapeJsonString(imported_input.object_format) << "\",\n"
      << "      \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(
             imported_input.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "      \"ready_for_live_registration_discovery_replay\": "
      << (imported_input.ready_for_live_registration_discovery_replay
              ? "true"
              : "false")
      << ",\n"
      << "      \"ready_for_live_restart_hardening\": "
      << (imported_input.ready_for_live_restart_hardening ? "true"
                                                          : "false")
      << ",\n"
      << "      \"bootstrap_live_registration_contract_id\": \""
      << EscapeJsonString(imported_input.bootstrap_live_registration_contract_id)
      << "\",\n"
      << "      \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_restart_hardening_contract_id)
      << "\",\n"
      << "      \"bootstrap_live_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_replay_registered_images_symbol)
      << "\",\n"
      << "      \"bootstrap_live_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "      \"bootstrap_live_restart_reset_for_testing_symbol\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_restart_reset_for_testing_symbol)
      << "\",\n"
      << "      \"bootstrap_live_restart_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             imported_input
                 .bootstrap_live_restart_replay_registered_images_symbol)
      << "\",\n"
      << "      \"bootstrap_live_restart_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             imported_input
                 .bootstrap_live_restart_reset_replay_state_snapshot_symbol)
      << "\",\n";
}
