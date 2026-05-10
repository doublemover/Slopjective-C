#include "pipeline/runtime_import_preservation_runtime_artifact_evidence_owners.h"

#include <string>
#include <utility>

#include "lower/objc3_lowering_contract.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedRuntimeBlockOwnershipArtifactPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const RuntimeImportJsonValue *preservation_value = FindMember(
      root, kObjc3RuntimeBlockOwnershipArtifactPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const RuntimeImportJsonValue::Object *preservation_object =
      AsObject(*preservation_value);
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
          surface.block_ownership_local_byref_layout_symbolized_sites, error)) {
    return false;
  }

  if (contract_id !=
      kObjc3RuntimeBlockOwnershipArtifactPreservationContractId) {
    error =
        "unexpected runtime block-ownership artifact preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId) {
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

}  // namespace objc3c::pipeline::runtime_import_preservation
