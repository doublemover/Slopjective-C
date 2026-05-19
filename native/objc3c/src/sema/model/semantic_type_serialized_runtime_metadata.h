#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/model/semantic_type_imported_runtime_metadata_rules.h"

inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringContractId =
    "objc3c.serialized.runtime.metadata.import.lowering.v1";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_serialized_runtime_metadata_import_lowering_contract";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringAuthorityModel =
    "serialized-metadata-import-lowering-freeze-derived-from-imported-runtime-semantic-rules";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3SerializedRuntimeMetadataImportLoweringSummary {
  std::string contract_id =
      kObjc3SerializedRuntimeMetadataImportLoweringContractId;
  std::string source_imported_semantic_rules_contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string frontend_surface_path =
      kObjc3SerializedRuntimeMetadataImportLoweringSurfacePath;
  std::string source_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3SerializedRuntimeMetadataImportLoweringAuthorityModel;
  std::string input_model =
      kObjc3SerializedRuntimeMetadataImportLoweringInputModel;
  std::size_t imported_input_path_count = 0;
  std::size_t imported_module_count = 0;
  bool fail_closed = false;
  bool source_imported_semantic_rules_ready = false;
  bool semantic_surface_published = false;
  bool imported_surface_ingest_landed = false;
  bool serialized_metadata_rehydration_landed = false;
  bool incremental_reuse_landed = false;
  bool imported_metadata_ir_lowering_landed = false;
  bool public_live_imported_payload_abi_landed = false;
  bool ready_for_serialized_metadata_lowering_impl = false;
  bool ready_for_incremental_reuse_impl = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_imported_semantic_rules_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.source_artifact_relative_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.imported_input_path_count >= summary.imported_module_count &&
         summary.fail_closed &&
         summary.source_imported_semantic_rules_ready &&
         summary.semantic_surface_published &&
         summary.imported_surface_ingest_landed &&
         !summary.serialized_metadata_rehydration_landed &&
         !summary.incremental_reuse_landed &&
         !summary.imported_metadata_ir_lowering_landed &&
         !summary.public_live_imported_payload_abi_landed &&
         !summary.ready_for_serialized_metadata_lowering_impl &&
         !summary.ready_for_incremental_reuse_impl &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseContractId =
    "objc3c.serialized.runtime.metadata.artifact.reuse.v1";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_serialized_runtime_metadata_artifact_reuse";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName =
    "serialized_runtime_metadata_reuse_payload";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseAuthorityModel =
    "runtime-import-surface-nested-serialized-runtime-metadata-payload";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3SerializedRuntimeMetadataArtifactReuseSummary {
  std::string contract_id =
      kObjc3SerializedRuntimeMetadataArtifactReuseContractId;
  std::string source_serialized_import_lowering_contract_id =
      kObjc3SerializedRuntimeMetadataImportLoweringContractId;
  std::string frontend_surface_path =
      kObjc3SerializedRuntimeMetadataArtifactReuseSurfacePath;
  std::string artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string payload_member_name =
      kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName;
  std::string authority_model =
      kObjc3SerializedRuntimeMetadataArtifactReuseAuthorityModel;
  std::string input_model =
      kObjc3SerializedRuntimeMetadataArtifactReuseInputModel;
  std::vector<std::string> reused_module_names_lexicographic;
  std::size_t reused_module_count = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t metadata_reference_count = 0;
  bool fail_closed = false;
  bool source_serialized_import_lowering_ready = false;
  bool semantic_surface_published = false;
  bool serialized_metadata_rehydration_landed = false;
  bool artifact_reuse_landed = false;
  bool downstream_module_consumption_ready = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_serialized_import_lowering_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.payload_member_name.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.reused_module_count ==
             summary.reused_module_names_lexicographic.size() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.fail_closed &&
         summary.source_serialized_import_lowering_ready &&
         summary.semantic_surface_published &&
         summary.serialized_metadata_rehydration_landed &&
         summary.artifact_reuse_landed &&
         summary.downstream_module_consumption_ready &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
