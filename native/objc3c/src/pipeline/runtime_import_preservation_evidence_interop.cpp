#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <utility>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {
namespace {

bool PopulateImportedInteropForeignSurfaceInterfacePreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  // import-surface anchor: Part 11 preservation stays at the
  // manifest/runtime-import-surface layer so provider foreign/import and
  // C++/Swift-facing annotation facts survive separate compilation before any
  // ABI lowering or runnable bridge generation claims land.
  const RuntimeImportJsonValue *preservation_value = FindMember(
      root, kObjc3InteropForeignSurfaceInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
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

bool PopulateImportedInteropFfiMetadataInterfacePreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value = FindMember(
      root, kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
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
                      surface.interop_ffi_runtime_import_artifact_ready,
                      error) ||
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
                      surface.interop_ffi_local_foreign_callable_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_metadata_preservation_sites",
                      surface.interop_ffi_local_metadata_preservation_sites,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_interface_annotation_sites",
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
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *generation_value = FindMember(
      root, kObjc3InteropHeaderModuleBridgeGenerationImportArtifactMemberName);
  if (generation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *generation_object =
      AsObject(*generation_value);
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
      !ReadBoolMember(
          *generation_object, "runtime_generation_ready",
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
                        surface.interop_header_module_bridge_replay_key,
                        error) ||
      !ReadStringMember(
          *generation_object, "preservation_replay_key",
          surface.interop_header_module_bridge_preservation_replay_key,
          error) ||
      !ReadSizeMember(
          *generation_object, "local_foreign_callable_count",
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

}  // namespace

bool PopulateImportedInteropEvidence(const RuntimeImportJsonValue::Object &root,
                                     Objc3ImportedRuntimeModuleSurface &surface,
                                     std::string &error) {
  if (!PopulateImportedInteropForeignSurfaceInterfacePreservation(root, surface,
                                                                 error)) {
    return false;
  }
  if (!PopulateImportedInteropFfiMetadataInterfacePreservation(root, surface,
                                                              error)) {
    return false;
  }
  return PopulateImportedInteropHeaderModuleBridgeGeneration(root, surface,
                                                            error);
}

}  // namespace objc3c::pipeline::runtime_import_preservation
