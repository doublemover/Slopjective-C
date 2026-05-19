#include "pipeline/runtime_import_preservation_runtime_artifact_evidence_owners.h"

#include <string>
#include <utility>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedRuntimeStorageReflectionArtifactPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value = FindMember(
      root,
      kObjc3RuntimeStorageReflectionArtifactPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
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
      !ReadSizeMember(
          *preservation_object, "current_property_exchange_entries",
          surface.storage_reflection_current_property_exchange_entries, error) ||
      !ReadSizeMember(
          *preservation_object, "weak_current_property_load_entries",
          surface.storage_reflection_weak_current_property_load_entries,
          error) ||
      !ReadSizeMember(
          *preservation_object, "weak_current_property_store_entries",
          surface.storage_reflection_weak_current_property_store_entries,
          error) ||
      !ReadSizeMember(*preservation_object, "ivar_layout_entries",
                      surface.storage_reflection_ivar_layout_entries, error) ||
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

}  // namespace objc3c::pipeline::runtime_import_preservation
