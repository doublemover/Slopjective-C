#include "pipeline/objc3_runtime_import_surface.h"

#include <string>
#include <utility>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "pipeline/runtime_import_frontend_closure_boundary.h"
#include "pipeline/runtime_import_link_plan.h"
#include "pipeline/runtime_import_packaging_peer_artifacts.h"
#include "pipeline/runtime_import_json_helpers.h"
#include "pipeline/runtime_import_record_parsing.h"
#include "pipeline/runtime_import_type_system_preservation.h"
#include "support/objc3_file_reading.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace {

using JsonParser = objc3c::pipeline::RuntimeImportJsonParser;
using JsonValue = objc3c::pipeline::RuntimeImportJsonValue;
using objc3c::pipeline::AsArray;
using objc3c::pipeline::AsObject;
using objc3c::pipeline::BuildObjc3ImportedRuntimeModuleLinkPlan;
using objc3c::pipeline::FindMember;
using objc3c::pipeline::Objc3ImportedRuntimeModuleLinkPlan;
using objc3c::pipeline::ParseRuntimeMetadataSourceRecordSet;
using objc3c::pipeline::ParseSerializedRuntimeMetadataReusePayload;
using objc3c::pipeline::PopulateImportedRuntimeRegistrationManifestPeerArtifacts;
using objc3c::pipeline::PopulateImportedTypeSystemGenericContractPreservation;
using objc3c::pipeline::PopulateImportedTypeSystemNullabilityContractPreservation;
using objc3c::pipeline::PopulateImportedTypeSystemOptionalKeypathSurface;
using objc3c::pipeline::PopulateImportedTypeSystemProtocolContractPreservation;
using objc3c::pipeline::PreserveImportedRuntimeMetadataSourceRecordInventory;
using objc3c::pipeline::ReadBoolMember;
using objc3c::pipeline::ReadSizeMember;
using objc3c::pipeline::ReadStringMember;
using objc3c::pipeline::SplitRuntimeImportLinkerResponseFlags;
using objc3c::pipeline::ValidateImportedRuntimeDiscoveryPeerArtifacts;
using objc3c::pipeline::ValidateImportedRuntimeFrontendClosureHardCutoverBoundary;
using objc3c::pipeline::ValidateImportedRuntimeFrontendClosureInventory;

bool PopulateFrontendClosureSummary(const JsonValue::Object &root,
                                    Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
                                    std::string &error) {
  summary = Objc3RuntimeAwareImportModuleFrontendClosureSummary{};
  if (!ReadStringMember(root, "contract_id", summary.contract_id, error) ||
      !ReadStringMember(root, "source_surface_contract_id", summary.source_surface_contract_id, error) ||
      !ReadStringMember(root, "frontend_surface_path", summary.frontend_surface_path, error) ||
      !ReadStringMember(root, "payload_model", summary.payload_model, error) ||
      !ReadStringMember(root, "artifact", summary.artifact_relative_path, error) ||
      !ReadStringMember(root, "authority_model", summary.authority_model, error) ||
      !ReadStringMember(root, "payload_ownership_model", summary.payload_ownership_model, error) ||
      !ReadStringMember(root, "module_name", summary.module_name, error) ||
      !ReadSizeMember(root, "protocol_decl_count", summary.protocol_decl_count, error) ||
      !ReadSizeMember(root, "interface_decl_count", summary.interface_decl_count, error) ||
      !ReadSizeMember(root, "implementation_decl_count", summary.implementation_decl_count, error) ||
      !ReadSizeMember(root, "interface_category_decl_count", summary.interface_category_decl_count, error) ||
      !ReadSizeMember(root, "implementation_category_decl_count", summary.implementation_category_decl_count, error) ||
      !ReadSizeMember(root, "function_decl_count", summary.function_decl_count, error) ||
      !ReadSizeMember(root, "module_import_graph_sites", summary.module_import_graph_sites, error) ||
      !ReadSizeMember(root, "import_edge_candidate_sites", summary.import_edge_candidate_sites, error) ||
      !ReadSizeMember(root, "namespace_segment_sites", summary.namespace_segment_sites, error) ||
      !ReadSizeMember(root, "object_pointer_type_sites", summary.object_pointer_type_sites, error) ||
      !ReadSizeMember(root, "pointer_declarator_sites", summary.pointer_declarator_sites, error) ||
      !ReadSizeMember(root, "normalized_sites", summary.normalized_sites, error) ||
      !ReadSizeMember(root, "contract_violation_sites", summary.contract_violation_sites, error) ||
      !ReadSizeMember(root, "runtime_owned_declaration_count", summary.runtime_owned_declaration_count, error) ||
      !ReadSizeMember(root, "metadata_reference_count", summary.metadata_reference_count, error) ||
      !ReadBoolMember(root, "runtime_aware_import_declarations_landed", summary.runtime_aware_import_declarations_landed, error) ||
      !ReadBoolMember(root, "module_metadata_import_surface_landed", summary.module_metadata_import_surface_landed, error) ||
      !ReadBoolMember(root, "runtime_owned_declaration_import_landed", summary.runtime_owned_declaration_import_landed, error) ||
      !ReadBoolMember(root, "runtime_metadata_reference_import_landed", summary.runtime_metadata_reference_import_landed, error) ||
      !ReadBoolMember(root, "public_frontend_api_module_surface_landed", summary.public_frontend_api_module_surface_landed, error) ||
      !ReadBoolMember(root, "ready_for_import_artifact_emission", summary.ready_for_import_artifact_emission, error) ||
      !ReadBoolMember(root, "ready_for_frontend_module_consumption", summary.ready_for_frontend_module_consumption, error) ||
      !ReadStringMember(root, "source_surface_replay_key", summary.source_surface_replay_key, error) ||
      !ReadStringMember(root, "replay_key", summary.replay_key, error)) {
    return false;
  }
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      !summary.source_surface_replay_key.empty();
  summary.runtime_metadata_source_records_ready = false;
  summary.frontend_surface_published = true;
  summary.import_artifact_template_published = true;
  summary.failure_reason.clear();
  return true;
}

bool PopulateImportedErrorHandlingResultAndBridgingArtifactReplay(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *replay_value =
      FindMember(root, "objc_error_handling_result_and_bridging_artifact_replay");
  if (replay_value == nullptr) {
    return true;
  }

  const JsonValue::Object *replay_object = AsObject(*replay_value);
  if (replay_object == nullptr) {
    error =
        "error_handling result/bridging imported replay contract must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*replay_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*replay_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*replay_object, "binary_artifact_replay_ready",
                      surface.error_handling_binary_artifact_replay_ready, error) ||
      !ReadBoolMember(*replay_object, "runtime_import_artifact_ready",
                      surface.error_handling_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*replay_object, "separate_compilation_replay_ready",
                      surface.error_handling_separate_compilation_replay_ready, error) ||
      !ReadBoolMember(*replay_object, "deterministic",
                      surface.error_handling_deterministic, error) ||
      !ReadStringMember(*replay_object, "replay_key",
                        surface.error_handling_result_and_bridging_artifact_replay_key,
                        error) ||
      !ReadStringMember(*replay_object, "error_handling_replay_key",
                        surface.error_handling_error_handling_replay_key, error) ||
      !ReadStringMember(*replay_object, "throws_replay_key",
                        surface.error_handling_throws_replay_key, error) ||
      !ReadStringMember(*replay_object, "result_like_replay_key",
                        surface.error_handling_result_like_replay_key, error) ||
      !ReadStringMember(*replay_object, "ns_error_replay_key",
                        surface.error_handling_ns_error_replay_key, error) ||
      !ReadStringMember(*replay_object, "unwind_replay_key",
                        surface.error_handling_unwind_replay_key, error)) {
    return false;
  }

  if (contract_id != kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId) {
    error =
        "unexpected Part 6 result/bridging artifact replay contract id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId) {
    error =
        "unexpected Part 6 throws ABI propagation source contract id in import surface";
    return false;
  }

  surface.error_handling_result_and_bridging_artifact_replay_present = true;
  surface.error_handling_contract_id = std::move(contract_id);
  surface.error_handling_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedConcurrencyActorMailboxRuntimeImport(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *runtime_value = FindMember(
      root, "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface");
  if (runtime_value == nullptr) {
    return true;
  }

  const JsonValue::Object *runtime_object = AsObject(*runtime_value);
  if (runtime_object == nullptr) {
    error = "concurrency actor mailbox runtime import surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*runtime_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*runtime_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*runtime_object, "actor_mailbox_runtime_ready",
                      surface.concurrency_actor_mailbox_runtime_ready, error) ||
      !ReadBoolMember(*runtime_object, "deterministic",
                      surface.concurrency_actor_mailbox_runtime_deterministic,
                      error) ||
      !ReadStringMember(*runtime_object, "replay_key",
                        surface.concurrency_actor_mailbox_runtime_replay_key,
                        error) ||
      !ReadStringMember(*runtime_object, "actor_lowering_replay_key",
                        surface.concurrency_actor_lowering_replay_key, error) ||
      !ReadStringMember(*runtime_object, "actor_isolation_lowering_replay_key",
                        surface.concurrency_actor_isolation_lowering_replay_key,
                        error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1") {
    error = "unexpected Part 7 actor mailbox runtime import contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.concurrency.actor.lowering.and.metadata.contract.v1") {
    error = "unexpected Part 7 actor mailbox runtime source contract id in import surface";
    return false;
  }

  if (!surface.concurrency_actor_mailbox_runtime_ready) {
    return true;
  }

  surface.concurrency_actor_mailbox_runtime_import_present = true;
  surface.concurrency_actor_mailbox_runtime_contract_id = std::move(contract_id);
  surface.concurrency_actor_mailbox_runtime_source_contract_id =
      std::move(source_contract_id);
  return true;
}

bool PopulateImportedMetaprogrammingModuleInterfaceReplayPreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "metaprogramming module/interface replay preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.metaprogramming_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.metaprogramming_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.metaprogramming_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.metaprogramming_replay_key, error) ||
      !ReadStringMember(*preservation_object, "expansion_lowering_replay_key",
                        surface.metaprogramming_expansion_lowering_replay_key, error) ||
      !ReadStringMember(*preservation_object,
                        "synthesized_emission_replay_key",
                        surface.metaprogramming_synthesized_emission_replay_key,
                        error) ||
      !ReadSizeMember(*preservation_object, "local_derive_method_count",
                      surface.metaprogramming_local_derive_method_count, error) ||
      !ReadSizeMember(*preservation_object, "local_macro_artifact_count",
                      surface.metaprogramming_local_macro_artifact_count, error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_interface_property_behavior_artifact_count",
          surface.metaprogramming_local_interface_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_implementation_property_behavior_artifact_count",
          surface.metaprogramming_local_implementation_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(*preservation_object, "local_runtime_method_list_count",
                      surface.metaprogramming_local_runtime_method_list_count, error)) {
    return false;
  }

  if (contract_id !=
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation source contract id in import surface";
    return false;
  }

  surface.metaprogramming_module_interface_replay_preservation_present = true;
  surface.metaprogramming_contract_id = std::move(contract_id);
  surface.metaprogramming_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedInteropForeignSurfaceInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  // import-surface anchor: Part 11 preservation stays at the
  // manifest/runtime-import-surface layer so provider foreign/import and
  // C++/Swift-facing annotation facts survive separate compilation before any
  // ABI lowering or runnable bridge generation claims land.
  const JsonValue *preservation_value = FindMember(
      root, kObjc3InteropForeignSurfaceInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "interop foreign surface interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "foreign_import_source_contract_id",
                        surface.interop_foreign_import_source_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "cpp_swift_source_contract_id",
                        surface.interop_cpp_swift_source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.interop_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.interop_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.interop_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.interop_replay_key, error) ||
      !ReadStringMember(*preservation_object,
                        "foreign_import_source_replay_key",
                        surface.interop_foreign_import_source_replay_key,
                        error) ||
      !ReadStringMember(*preservation_object, "cpp_swift_source_replay_key",
                        surface.interop_cpp_swift_source_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_foreign_callable_count",
                      surface.interop_local_foreign_callable_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_import_module_annotation_count",
                      surface.interop_local_import_module_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_imported_module_name_count",
                      surface.interop_local_imported_module_name_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_swift_name_annotation_count",
                      surface.interop_local_swift_name_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_swift_private_annotation_count",
                      surface.interop_local_swift_private_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_cpp_name_annotation_count",
                      surface.interop_local_cpp_name_annotation_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_header_name_annotation_count",
                      surface.interop_local_header_name_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_named_annotation_payload_count",
                      surface.interop_local_named_annotation_payload_count,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3InteropForeignSurfaceInterfacePreservationContractId) {
    error =
        "unexpected Part 11 foreign surface interface preservation contract id in import surface";
    return false;
  }
  if (surface.interop_foreign_import_source_contract_id !=
      kObjc3InteropForeignImportSourceClosureContractId) {
    error =
        "unexpected Part 11 foreign/import source contract id in import surface";
    return false;
  }
  if (surface.interop_cpp_swift_source_contract_id !=
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId) {
    error =
        "unexpected Part 11 C++/Swift annotation source contract id in import surface";
    return false;
  }

  surface.interop_foreign_surface_interface_preservation_present = true;
  surface.interop_contract_id = std::move(contract_id);
  return true;
}

bool PopulateImportedMetaprogrammingMacroHostProcessCacheRuntimeIntegration(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *integration_value = FindMember(
      root,
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName);
  if (integration_value == nullptr) {
    return true;
  }

  const JsonValue::Object *integration_object = AsObject(*integration_value);
  if (integration_object == nullptr) {
    error =
        "metaprogramming macro host process/cache runtime integration surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*integration_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*integration_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*integration_object, "runtime_import_artifact_ready",
                      surface.metaprogramming_macro_host_process_cache_runtime_ready,
                      error) ||
      !ReadBoolMember(*integration_object, "separate_compilation_ready",
                      surface.metaprogramming_macro_host_process_cache_separate_compilation_ready,
                      error) ||
      !ReadBoolMember(*integration_object, "deterministic",
                      surface.metaprogramming_macro_host_process_cache_deterministic,
                      error) ||
      !ReadStringMember(
          *integration_object, "replay_key",
          surface.metaprogramming_macro_host_process_cache_replay_key, error) ||
      !ReadStringMember(*integration_object, "host_executable_relative_path",
                        surface
                            .metaprogramming_macro_host_process_cache_host_executable_relative_path,
                        error) ||
      !ReadStringMember(*integration_object, "cache_root_relative_path",
                        surface.metaprogramming_macro_host_process_cache_root_relative_path,
                        error)) {
    return false;
  }

  if (contract_id !=
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId) {
    error =
        "unexpected metaprogramming macro host process/cache runtime integration contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId) {
    error =
        "unexpected metaprogramming macro host process/cache runtime integration source contract id in import surface";
    return false;
  }

  surface.metaprogramming_macro_host_process_cache_runtime_integration_present = true;
  surface.metaprogramming_macro_host_process_cache_contract_id = std::move(contract_id);
  surface.metaprogramming_macro_host_process_cache_source_contract_id =
      std::move(source_contract_id);
  return true;
}

bool PopulateImportedInteropFfiMetadataInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "interop ffi metadata/interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string preservation_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*preservation_object, "preservation_contract_id",
                        preservation_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.interop_ffi_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.interop_ffi_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.interop_ffi_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.interop_ffi_replay_key, error) ||
      !ReadStringMember(*preservation_object, "lowering_replay_key",
                        surface.interop_ffi_lowering_replay_key, error) ||
      !ReadStringMember(*preservation_object, "preservation_replay_key",
                        surface.interop_ffi_preservation_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_foreign_callable_count",
                      surface.interop_ffi_local_foreign_callable_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_metadata_preservation_sites",
                      surface.interop_ffi_local_metadata_preservation_sites,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_interface_annotation_sites",
                      surface.interop_ffi_local_interface_annotation_sites,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3InteropFfiMetadataInterfacePreservationContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3InteropFfiMetadataInterfacePreservationSourceContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation source contract id in import surface";
    return false;
  }
  if (preservation_contract_id !=
      kObjc3InteropForeignSurfaceInterfacePreservationContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation dependency contract id in import surface";
    return false;
  }

  surface.interop_ffi_metadata_interface_preservation_present = true;
  surface.interop_ffi_contract_id = std::move(contract_id);
  surface.interop_ffi_source_contract_id = std::move(source_contract_id);
  surface.interop_ffi_preservation_contract_id =
      std::move(preservation_contract_id);
  return true;
}

bool PopulateImportedInteropHeaderModuleBridgeGeneration(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *generation_value = FindMember(
      root, kObjc3InteropHeaderModuleBridgeGenerationImportArtifactMemberName);
  if (generation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *generation_object = AsObject(*generation_value);
  if (generation_object == nullptr) {
    error =
        "interop header/module/bridge generation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string preservation_contract_id;
  if (!ReadStringMember(*generation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*generation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*generation_object, "preservation_contract_id",
                        preservation_contract_id, error) ||
      !ReadStringMember(*generation_object, "header_artifact_relative_path",
                        surface.interop_bridge_header_artifact_relative_path,
                        error) ||
      !ReadStringMember(*generation_object, "module_artifact_relative_path",
                        surface.interop_bridge_module_artifact_relative_path,
                        error) ||
      !ReadStringMember(*generation_object, "bridge_artifact_relative_path",
                        surface.interop_bridge_artifact_relative_path, error) ||
      !ReadBoolMember(*generation_object, "runtime_generation_ready",
                      surface.interop_header_module_bridge_runtime_generation_ready,
                      error) ||
      !ReadBoolMember(
          *generation_object, "cross_module_packaging_ready",
          surface.interop_header_module_bridge_cross_module_packaging_ready,
          error) ||
      !ReadBoolMember(*generation_object, "deterministic",
                      surface.interop_header_module_bridge_deterministic,
                      error) ||
      !ReadStringMember(*generation_object, "replay_key",
                        surface.interop_header_module_bridge_replay_key, error) ||
      !ReadStringMember(*generation_object, "preservation_replay_key",
                        surface.interop_header_module_bridge_preservation_replay_key,
                        error) ||
      !ReadSizeMember(*generation_object, "local_foreign_callable_count",
                      surface.interop_header_module_bridge_local_foreign_callable_count,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3InteropHeaderModuleBridgeGenerationContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3InteropHeaderModuleBridgeGenerationSourceContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation source contract id in import surface";
    return false;
  }
  if (preservation_contract_id !=
      kObjc3InteropHeaderModuleBridgeGenerationPreservationContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation preservation contract id in import surface";
    return false;
  }

  // Deferred Part 11 bridge packets are still emitted into the import surface
  // for documentation/provenance, but cross-module runtime link planning should
  // only treat them as active when the bridge path itself is live.
  if (!surface.interop_header_module_bridge_runtime_generation_ready ||
      !surface.interop_header_module_bridge_cross_module_packaging_ready) {
    return true;
  }

  surface.interop_header_module_bridge_generation_present = true;
  surface.interop_header_module_bridge_contract_id = std::move(contract_id);
  surface.interop_header_module_bridge_source_contract_id =
      std::move(source_contract_id);
  surface.interop_header_module_bridge_preservation_contract_id =
      std::move(preservation_contract_id);
  return true;
}

bool PopulateImportedDispatchDispatchMetadataInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, "objc_dispatch_dispatch_metadata_and_interface_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "dispatch dispatch metadata/interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.dispatch_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.dispatch_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.dispatch_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.dispatch_replay_key, error) ||
      !ReadStringMember(*preservation_object, "lowering_replay_key",
                        surface.dispatch_lowering_replay_key, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_direct_callable_record_count",
                      surface.dispatch_local_direct_callable_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_callable_record_count",
                      surface.dispatch_local_final_callable_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_container_record_count",
                      surface.dispatch_local_final_container_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_sealed_container_record_count",
                      surface.dispatch_local_sealed_container_record_count,
                      error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.dispatch.dispatch.metadata.interface.preservation.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.dispatch.dispatch.control.lowering.contract.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation source contract id in import surface";
    return false;
  }

  surface.dispatch_dispatch_metadata_interface_preservation_present = true;
  surface.dispatch_contract_id = std::move(contract_id);
  surface.dispatch_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedRuntimeStorageReflectionArtifactPreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root,
      kObjc3RuntimeStorageReflectionArtifactPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "runtime storage/reflection artifact preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string dispatch_and_synthesized_accessor_lowering_surface_contract_id;
  std::string executable_property_accessor_layout_lowering_contract_id;
  std::string executable_ivar_layout_emission_contract_id;
  std::string executable_synthesized_accessor_property_lowering_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(
          *preservation_object,
          "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
          dispatch_and_synthesized_accessor_lowering_surface_contract_id,
          error) ||
      !ReadStringMember(*preservation_object,
                        "executable_property_accessor_layout_lowering_contract_id",
                        executable_property_accessor_layout_lowering_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "executable_ivar_layout_emission_contract_id",
                        executable_ivar_layout_emission_contract_id, error) ||
      !ReadStringMember(
          *preservation_object,
          "executable_synthesized_accessor_property_lowering_contract_id",
          executable_synthesized_accessor_property_lowering_contract_id,
          error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.storage_reflection_runtime_import_artifact_ready,
                      error) ||
      !ReadBoolMember(
          *preservation_object, "separate_compilation_preservation_ready",
          surface.storage_reflection_separate_compilation_preservation_ready,
          error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.storage_reflection_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.storage_reflection_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_property_descriptor_count",
                      surface.storage_reflection_local_property_descriptor_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_ivar_descriptor_count",
                      surface.storage_reflection_local_ivar_descriptor_count,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "implementation_owned_property_entries",
          surface.storage_reflection_implementation_owned_property_entries,
          error) ||
      !ReadSizeMember(
          *preservation_object, "synthesized_accessor_owner_entries",
          surface.storage_reflection_synthesized_accessor_owner_entries,
          error) ||
      !ReadSizeMember(*preservation_object, "synthesized_getter_entries",
                      surface.storage_reflection_synthesized_getter_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "synthesized_setter_entries",
                      surface.storage_reflection_synthesized_setter_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "synthesized_accessor_entries",
                      surface.storage_reflection_synthesized_accessor_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "current_property_read_entries",
                      surface.storage_reflection_current_property_read_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "current_property_write_entries",
                      surface.storage_reflection_current_property_write_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "current_property_exchange_entries",
                      surface.storage_reflection_current_property_exchange_entries,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "weak_current_property_load_entries",
          surface.storage_reflection_weak_current_property_load_entries,
          error) ||
      !ReadSizeMember(
          *preservation_object, "weak_current_property_store_entries",
          surface.storage_reflection_weak_current_property_store_entries,
          error) ||
      !ReadSizeMember(*preservation_object, "ivar_layout_entries",
                      surface.storage_reflection_ivar_layout_entries,
                      error) ||
      !ReadSizeMember(*preservation_object, "ivar_layout_owner_entries",
                      surface.storage_reflection_ivar_layout_owner_entries,
                      error)) {
    return false;
  }

  if (contract_id !=
      kObjc3RuntimeStorageReflectionArtifactPreservationContractId) {
    error =
        "unexpected runtime storage/reflection artifact preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId) {
    error =
        "unexpected runtime storage/reflection source contract id in import surface";
    return false;
  }
  if (dispatch_and_synthesized_accessor_lowering_surface_contract_id !=
      kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId) {
    error =
        "unexpected runtime storage/reflection lowering contract id in import surface";
    return false;
  }
  if (executable_property_accessor_layout_lowering_contract_id !=
      kObjc3ExecutablePropertyAccessorLayoutLoweringContractId) {
    error =
        "unexpected runtime storage/reflection accessor-layout contract id in import surface";
    return false;
  }
  if (executable_ivar_layout_emission_contract_id !=
      kObjc3ExecutableIvarLayoutEmissionContractId) {
    error =
        "unexpected runtime storage/reflection ivar-layout contract id in import surface";
    return false;
  }
  if (executable_synthesized_accessor_property_lowering_contract_id !=
      kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId) {
    error =
        "unexpected runtime storage/reflection synthesized-accessor contract id in import surface";
    return false;
  }

  surface.storage_reflection_artifact_preservation_present = true;
  surface.storage_reflection_contract_id = std::move(contract_id);
  surface.storage_reflection_source_contract_id =
      std::move(source_contract_id);
  surface
      .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
      std::move(
          dispatch_and_synthesized_accessor_lowering_surface_contract_id);
  surface
      .storage_reflection_executable_property_accessor_layout_lowering_contract_id =
      std::move(executable_property_accessor_layout_lowering_contract_id);
  surface.storage_reflection_executable_ivar_layout_emission_contract_id =
      std::move(executable_ivar_layout_emission_contract_id);
  surface
      .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
      std::move(
          executable_synthesized_accessor_property_lowering_contract_id);
  return true;
}

bool PopulateImportedRuntimeBlockOwnershipArtifactPreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3RuntimeBlockOwnershipArtifactPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "runtime block-ownership artifact preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string block_object_invoke_thunk_lowering_contract_id;
  std::string block_byref_helper_lowering_contract_id;
  std::string block_escape_runtime_hook_lowering_contract_id;
  std::string runtime_support_library_link_wiring_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*preservation_object,
                        "block_object_invoke_thunk_lowering_contract_id",
                        block_object_invoke_thunk_lowering_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "block_byref_helper_lowering_contract_id",
                        block_byref_helper_lowering_contract_id, error) ||
      !ReadStringMember(*preservation_object,
                        "block_escape_runtime_hook_lowering_contract_id",
                        block_escape_runtime_hook_lowering_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "runtime_support_library_link_wiring_contract_id",
                        runtime_support_library_link_wiring_contract_id,
                        error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.block_ownership_runtime_import_artifact_ready,
                      error) ||
      !ReadBoolMember(
          *preservation_object, "separate_compilation_preservation_ready",
          surface.block_ownership_separate_compilation_preservation_ready,
          error) ||
      !ReadBoolMember(
          *preservation_object, "runtime_support_library_link_wiring_ready",
          surface.block_ownership_runtime_support_library_link_wiring_ready,
          error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.block_ownership_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.block_ownership_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_block_literal_sites",
                      surface.block_ownership_local_block_literal_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_invoke_trampoline_symbolized_sites",
          surface.block_ownership_local_invoke_trampoline_symbolized_sites,
          error) ||
      !ReadSizeMember(*preservation_object, "local_copy_helper_required_sites",
                      surface.block_ownership_local_copy_helper_required_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_dispose_helper_required_sites",
          surface.block_ownership_local_dispose_helper_required_sites,
          error) ||
      !ReadSizeMember(*preservation_object, "local_copy_helper_symbolized_sites",
                      surface.block_ownership_local_copy_helper_symbolized_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_dispose_helper_symbolized_sites",
          surface.block_ownership_local_dispose_helper_symbolized_sites,
          error) ||
      !ReadSizeMember(*preservation_object, "local_escape_to_heap_sites",
                      surface.block_ownership_local_escape_to_heap_sites,
                      error) ||
      !ReadSizeMember(
          *preservation_object, "local_byref_layout_symbolized_sites",
          surface.block_ownership_local_byref_layout_symbolized_sites,
          error)) {
    return false;
  }

  if (contract_id !=
      kObjc3RuntimeBlockOwnershipArtifactPreservationContractId) {
    error =
        "unexpected runtime block-ownership artifact preservation contract id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId) {
    error =
        "unexpected runtime block-ownership source contract id in import surface";
    return false;
  }
  if (block_object_invoke_thunk_lowering_contract_id !=
      Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId) {
    error =
        "unexpected runtime block-ownership invoke-thunk contract id in import surface";
    return false;
  }
  if (block_byref_helper_lowering_contract_id !=
      Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId) {
    error =
        "unexpected runtime block-ownership byref-helper contract id in import surface";
    return false;
  }
  if (block_escape_runtime_hook_lowering_contract_id !=
      Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId) {
    error =
        "unexpected runtime block-ownership escape-runtime-hook contract id in import surface";
    return false;
  }
  if (runtime_support_library_link_wiring_contract_id !=
      kObjc3RuntimeSupportLibraryLinkWiringContractId) {
    error =
        "unexpected runtime block-ownership runtime-link contract id in import surface";
    return false;
  }

  surface.block_ownership_artifact_preservation_present = true;
  surface.block_ownership_contract_id = std::move(contract_id);
  surface.block_ownership_source_contract_id = std::move(source_contract_id);
  surface.block_ownership_object_invoke_thunk_lowering_contract_id =
      std::move(block_object_invoke_thunk_lowering_contract_id);
  surface.block_ownership_byref_helper_lowering_contract_id =
      std::move(block_byref_helper_lowering_contract_id);
  surface.block_ownership_escape_runtime_hook_lowering_contract_id =
      std::move(block_escape_runtime_hook_lowering_contract_id);
  surface.block_ownership_runtime_support_library_link_wiring_contract_id =
      std::move(runtime_support_library_link_wiring_contract_id);
  return true;
}

bool ParseImportedRuntimeModuleSurface(const JsonValue::Object &root,
                                       Objc3ImportedRuntimeModuleSurface &surface,
                                       std::string &error) {
  if (!PopulateFrontendClosureSummary(root,
                                      surface.frontend_closure_summary,
                                      error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemOptionalKeypathSurface(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemGenericContractPreservation(root, surface,
                                                            error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemNullabilityContractPreservation(root, surface,
                                                                error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemProtocolContractPreservation(root, surface,
                                                             error)) {
    return false;
  }
  if (!PopulateImportedErrorHandlingResultAndBridgingArtifactReplay(root, surface,
                                                            error)) {
    return false;
  }
  if (!PopulateImportedConcurrencyActorMailboxRuntimeImport(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedInteropForeignSurfaceInterfacePreservation(root, surface,
                                                                 error)) {
    return false;
  }
  if (!PopulateImportedInteropFfiMetadataInterfacePreservation(root, surface,
                                                              error)) {
    return false;
  }
  if (!PopulateImportedInteropHeaderModuleBridgeGeneration(root, surface,
                                                          error)) {
    return false;
  }
  if (!PopulateImportedMetaprogrammingModuleInterfaceReplayPreservation(root, surface,
                                                               error)) {
    return false;
  }
  if (!PopulateImportedMetaprogrammingMacroHostProcessCacheRuntimeIntegration(root,
                                                                     surface,
                                                                     error)) {
    return false;
  }
  if (!PopulateImportedDispatchDispatchMetadataInterfacePreservation(root, surface,
                                                                  error)) {
    return false;
  }
  if (!PopulateImportedRuntimeBlockOwnershipArtifactPreservation(root, surface,
                                                                 error)) {
    return false;
  }
  if (!PopulateImportedRuntimeStorageReflectionArtifactPreservation(root,
                                                                    surface,
                                                                    error)) {
    return false;
  }
  Objc3RuntimeMetadataSourceRecordSet local_runtime_metadata_source_records;
  if (!ParseRuntimeMetadataSourceRecordSet(root, "runtime_owned_declarations",
                                           local_runtime_metadata_source_records,
                                           error)) {
    return false;
  }
  PreserveImportedRuntimeMetadataSourceRecordInventory(
      surface.frontend_closure_summary, local_runtime_metadata_source_records);

  const JsonValue *references_value = FindMember(root, "metadata_references");
  if (references_value == nullptr) {
    error = "missing JSON array member 'metadata_references'";
    return false;
  }
  const JsonValue::Array *references_array = AsArray(*references_value);
  if (references_array == nullptr) {
    error = "JSON member 'metadata_references' must be an array";
    return false;
  }

  surface.frontend_closure_summary.superclass_reference_count = 0;
  surface.frontend_closure_summary.protocol_reference_count = 0;
  surface.frontend_closure_summary.property_accessor_reference_count = 0;
  surface.frontend_closure_summary.property_ivar_binding_reference_count = 0;
  surface.frontend_closure_summary.method_selector_reference_count = 0;
  for (const JsonValue &reference_value : *references_array) {
    const JsonValue::Object *reference_object = AsObject(reference_value);
    if (reference_object == nullptr) {
      error = "metadata_references must contain objects";
      return false;
    }
    std::string reference_kind;
    if (!ReadStringMember(*reference_object, "reference_kind", reference_kind,
                          error)) {
      return false;
    }
    if (reference_kind == "class-superclass") {
      ++surface.frontend_closure_summary.superclass_reference_count;
    } else if (reference_kind == "class-adopted-protocol" ||
               reference_kind == "protocol-inherited-protocol" ||
               reference_kind == "category-adopted-protocol") {
      ++surface.frontend_closure_summary.protocol_reference_count;
    } else if (reference_kind == "property-getter-selector" ||
               reference_kind == "property-setter-selector") {
      ++surface.frontend_closure_summary.property_accessor_reference_count;
    } else if (reference_kind == "property-ivar-binding") {
      ++surface.frontend_closure_summary.property_ivar_binding_reference_count;
    } else if (reference_kind == "method-selector") {
      ++surface.frontend_closure_summary.method_selector_reference_count;
    }
  }

  if (!ValidateImportedRuntimeFrontendClosureInventory(
          surface.frontend_closure_summary,
          local_runtime_metadata_source_records, references_array->size(),
          error) ||
      !ValidateImportedRuntimeFrontendClosureHardCutoverBoundary(
          surface.frontend_closure_summary, error)) {
    return false;
  }
  if (!ParseSerializedRuntimeMetadataReusePayload(root, surface, error)) {
    return false;
  }
  if (!surface.uses_serialized_runtime_metadata_payload) {
    surface.runtime_metadata_source_records =
        std::move(local_runtime_metadata_source_records);
  }
  return true;
}

}  // namespace

bool TryLoadObjc3ImportedRuntimeModuleSurface(
    const std::filesystem::path &path,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  std::string io_error;
  std::string payload;
  if (!objc3c::support::TryReadTextFile(path, payload, io_error, "unable to open file", "failed to read file")) {
    error = path.generic_string() + ": " + io_error;
    return false;
  }

  JsonParser parser(payload);
  JsonValue root_value;
  std::string parse_error;
  if (!parser.Parse(root_value, parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  const JsonValue::Object *root_object = AsObject(root_value);
  if (root_object == nullptr) {
    error = path.generic_string() + ": import surface payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModuleSurface parsed_surface;
  parsed_surface.source_path = path;
  if (!ParseImportedRuntimeModuleSurface(*root_object, parsed_surface,
                                         parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  if (!IsReadyObjc3ImportedRuntimeModuleSurfaceCrossModuleContract(
          parsed_surface)) {
    error = path.generic_string() +
            ": imported runtime module surface is not ready for cross-module consumption";
    return false;
  }
  surface = std::move(parsed_surface);
  return true;
}

bool TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  artifacts = Objc3ImportedRuntimeModulePackagingPeerArtifacts{};
  error.clear();

  Objc3ImportedRuntimeModuleLinkPlan link_plan;
  if (!BuildObjc3ImportedRuntimeModuleLinkPlan(surface.source_path, link_plan,
                                               error)) {
    return false;
  }

  std::string manifest_io_error;
  std::string manifest_payload;
  if (!objc3c::support::TryReadTextFile(link_plan.registration_manifest_path,
                                        manifest_payload,
                                        manifest_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = link_plan.registration_manifest_path.generic_string() + ": " +
            manifest_io_error;
    return false;
  }
  JsonParser manifest_parser(manifest_payload);
  JsonValue manifest_root_value;
  std::string manifest_parse_error;
  if (!manifest_parser.Parse(manifest_root_value, manifest_parse_error)) {
    error = link_plan.registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }
  const JsonValue::Object *manifest_root_object = AsObject(manifest_root_value);
  if (manifest_root_object == nullptr) {
    error = link_plan.registration_manifest_path.generic_string() +
            ": runtime registration manifest payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModulePackagingPeerArtifacts parsed_artifacts;
  if (!PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
          *manifest_root_object, parsed_artifacts, manifest_parse_error)) {
    error = link_plan.registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }

  std::string discovery_io_error;
  std::string discovery_payload;
  if (!objc3c::support::TryReadTextFile(link_plan.discovery_artifact_path,
                                        discovery_payload,
                                        discovery_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = link_plan.discovery_artifact_path.generic_string() + ": " +
            discovery_io_error;
    return false;
  }
  JsonParser discovery_parser(discovery_payload);
  JsonValue discovery_root_value;
  std::string discovery_parse_error;
  if (!discovery_parser.Parse(discovery_root_value, discovery_parse_error)) {
    error = link_plan.discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }
  const JsonValue::Object *discovery_root_object = AsObject(discovery_root_value);
  if (discovery_root_object == nullptr) {
    error = link_plan.discovery_artifact_path.generic_string() +
            ": runtime metadata discovery payload must be a JSON object";
    return false;
  }

  std::string object_artifact_relative_path;
  if (!ValidateImportedRuntimeDiscoveryPeerArtifacts(*discovery_root_object,
                                                     parsed_artifacts,
                                                     object_artifact_relative_path,
                                                     discovery_parse_error)) {
    error = link_plan.discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }

  std::string response_io_error;
  std::string linker_response_payload;
  if (!objc3c::support::TryReadTextFile(link_plan.linker_response_artifact_path,
                                        linker_response_payload,
                                        response_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = link_plan.linker_response_artifact_path.generic_string() + ": " +
            response_io_error;
    return false;
  }
  const std::vector<std::string> response_flags =
      SplitRuntimeImportLinkerResponseFlags(linker_response_payload);
  if (response_flags != parsed_artifacts.driver_linker_flags) {
    error = link_plan.linker_response_artifact_path.generic_string() +
            ": linker response payload drifted from imported driver linker flags";
    return false;
  }

  const std::filesystem::path object_artifact_path =
      (link_plan.import_surface_parent_path / object_artifact_relative_path)
          .lexically_normal();
  if (!std::filesystem::exists(object_artifact_path)) {
    error = object_artifact_path.generic_string() +
            ": imported object artifact is missing";
    return false;
  }

  parsed_artifacts.registration_manifest_path =
      link_plan.registration_manifest_path;
  parsed_artifacts.discovery_artifact_path = link_plan.discovery_artifact_path;
  parsed_artifacts.linker_response_artifact_path =
      link_plan.linker_response_artifact_path;
  parsed_artifacts.object_artifact_path = object_artifact_path;
  if (!IsReadyObjc3ImportedRuntimeModulePackagingLinkPlan(parsed_artifacts)) {
    error = surface.source_path.generic_string() +
            ": imported runtime module packaging peer artifacts are not ready for link-plan consumption";
    return false;
  }
  artifacts = std::move(parsed_artifacts);
  return true;
}
