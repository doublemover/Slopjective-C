#include "io/objc3_cross_module_runtime_link_plan_sections.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include "io/objc3_cross_module_runtime_link_plan_ordering.h"
#include "io/objc3_cross_module_runtime_link_plan_sections_validation.h"

namespace {

struct OrderedLinkInput {
  std::uint64_t registration_order_ordinal = 0;
  std::string translation_unit_identity_key;
  std::string module_name;
  std::string object_artifact_path;
  std::vector<std::string> driver_linker_flags;
};

void AddImportedLinkPlanInputToSections(
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    Objc3CrossModuleRuntimeLinkPlanSections &sections,
    std::vector<OrderedLinkInput> &ordered_link_inputs) {
  ordered_link_inputs.push_back(
      {imported_input.translation_unit_registration_order_ordinal,
       imported_input.translation_unit_identity_key,
       imported_input.module_name,
       imported_input.object_artifact_path,
       imported_input.driver_linker_flags});

  if (imported_input.error_handling_result_and_bridging_artifact_replay_present) {
    sections.imported_error_handling_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.concurrency_actor_mailbox_runtime_import_present) {
    sections.imported_concurrency_actor_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.concurrency_scheduler_task_runtime_import_present) {
    sections.imported_scheduler_task_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.interop_ffi_metadata_interface_preservation_present) {
    sections.imported_interop_ffi_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.interop_foreign_abi_runtime_closure_present) {
    sections.imported_foreign_abi_module_names_lexicographic.push_back(
        imported_input.module_name);
  }
  if (imported_input.interop_header_module_bridge_generation_present) {
    sections
        .imported_interop_header_module_bridge_module_names_lexicographic
        .push_back(imported_input.module_name);
  }
  if (imported_input
          .metaprogramming_macro_host_process_cache_runtime_integration_present) {
    sections
        .imported_metaprogramming_host_cache_module_names_lexicographic
        .push_back(imported_input.module_name);
  }

  sections.imported_class_descriptor_count +=
      imported_input.class_descriptor_count;
  sections.imported_protocol_descriptor_count +=
      imported_input.protocol_descriptor_count;
  sections.imported_category_descriptor_count +=
      imported_input.category_descriptor_count;
  sections.imported_property_descriptor_count +=
      imported_input.property_descriptor_count;
  sections.imported_ivar_descriptor_count += imported_input.ivar_descriptor_count;
  sections.imported_total_descriptor_count += imported_input.total_descriptor_count;

  sections.imported_block_ownership_block_literal_sites +=
      imported_input.block_ownership_local_block_literal_sites;
  sections.imported_block_ownership_invoke_trampoline_symbolized_sites +=
      imported_input.block_ownership_local_invoke_trampoline_symbolized_sites;
  sections.imported_block_ownership_copy_helper_required_sites +=
      imported_input.block_ownership_local_copy_helper_required_sites;
  sections.imported_block_ownership_dispose_helper_required_sites +=
      imported_input.block_ownership_local_dispose_helper_required_sites;
  sections.imported_block_ownership_copy_helper_symbolized_sites +=
      imported_input.block_ownership_local_copy_helper_symbolized_sites;
  sections.imported_block_ownership_dispose_helper_symbolized_sites +=
      imported_input.block_ownership_local_dispose_helper_symbolized_sites;
  sections.imported_block_ownership_escape_to_heap_sites +=
      imported_input.block_ownership_local_escape_to_heap_sites;
  sections.imported_block_ownership_byref_layout_symbolized_sites +=
      imported_input.block_ownership_local_byref_layout_symbolized_sites;

  sections.imported_storage_reflection_implementation_owned_property_entries +=
      imported_input.storage_reflection_implementation_owned_property_entries;
  sections.imported_storage_reflection_synthesized_accessor_owner_entries +=
      imported_input.storage_reflection_synthesized_accessor_owner_entries;
  sections.imported_storage_reflection_synthesized_getter_entries +=
      imported_input.storage_reflection_synthesized_getter_entries;
  sections.imported_storage_reflection_synthesized_setter_entries +=
      imported_input.storage_reflection_synthesized_setter_entries;
  sections.imported_storage_reflection_synthesized_accessor_entries +=
      imported_input.storage_reflection_synthesized_accessor_entries;
  sections.imported_storage_reflection_current_property_read_entries +=
      imported_input.storage_reflection_current_property_read_entries;
  sections.imported_storage_reflection_current_property_write_entries +=
      imported_input.storage_reflection_current_property_write_entries;
  sections.imported_storage_reflection_current_property_exchange_entries +=
      imported_input.storage_reflection_current_property_exchange_entries;
  sections.imported_storage_reflection_weak_current_property_load_entries +=
      imported_input.storage_reflection_weak_current_property_load_entries;
  sections.imported_storage_reflection_weak_current_property_store_entries +=
      imported_input.storage_reflection_weak_current_property_store_entries;
  sections.imported_storage_reflection_ivar_layout_entries +=
      imported_input.storage_reflection_ivar_layout_entries;
  sections.imported_storage_reflection_ivar_layout_owner_entries +=
      imported_input.storage_reflection_ivar_layout_owner_entries;
}

void SortLexicographicModuleSections(
    Objc3CrossModuleRuntimeLinkPlanSections &sections) {
  std::sort(sections.module_names_lexicographic.begin(),
            sections.module_names_lexicographic.end());
  std::sort(sections.imported_error_handling_module_names_lexicographic.begin(),
            sections.imported_error_handling_module_names_lexicographic.end());
  std::sort(
      sections.imported_concurrency_actor_module_names_lexicographic.begin(),
      sections.imported_concurrency_actor_module_names_lexicographic.end());
  std::sort(sections.imported_scheduler_task_module_names_lexicographic.begin(),
            sections.imported_scheduler_task_module_names_lexicographic.end());
  std::sort(sections.imported_interop_ffi_module_names_lexicographic.begin(),
            sections.imported_interop_ffi_module_names_lexicographic.end());
  std::sort(sections.imported_foreign_abi_module_names_lexicographic.begin(),
            sections.imported_foreign_abi_module_names_lexicographic.end());
  std::sort(
      sections
          .imported_interop_header_module_bridge_module_names_lexicographic
          .begin(),
      sections.imported_interop_header_module_bridge_module_names_lexicographic
          .end());
  std::sort(
      sections.imported_metaprogramming_host_cache_module_names_lexicographic
          .begin(),
      sections.imported_metaprogramming_host_cache_module_names_lexicographic
          .end());
}

}  // namespace

bool BuildObjc3CrossModuleRuntimeLinkPlanSections(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
        &imported_inputs,
    Objc3CrossModuleRuntimeLinkPlanSections &sections,
    std::string &error) {
  sections = Objc3CrossModuleRuntimeLinkPlanSections{};

  std::unordered_set<std::string> seen_translation_unit_identity_keys;
  std::unordered_set<std::uint64_t> seen_registration_ordinals;
  std::unordered_set<std::string> seen_driver_linker_flags;
  std::unordered_set<std::string> seen_direct_import_surface_paths;
  std::unordered_set<std::string> seen_error_handling_replay_keys;
  std::unordered_set<std::string> seen_concurrency_actor_replay_keys;
  std::unordered_set<std::string> seen_scheduler_task_replay_keys;
  std::unordered_set<std::string> seen_interop_ffi_replay_keys;
  std::unordered_set<std::string> seen_foreign_abi_replay_keys;
  std::unordered_set<std::string> seen_interop_header_module_bridge_replay_keys;
  std::unordered_set<std::string> seen_metaprogramming_host_cache_replay_keys;

  sections.direct_import_surface_artifact_paths =
      BuildOrderedObjc3CrossModuleDirectImportSurfaceArtifactPaths(
          inputs.direct_import_surface_artifact_paths);

  seen_translation_unit_identity_keys.insert(
      inputs.local_translation_unit_identity_key);
  seen_registration_ordinals.insert(
      inputs.local_translation_unit_registration_order_ordinal);
  for (const auto &surface_path :
       sections.direct_import_surface_artifact_paths) {
    if (surface_path.empty()) {
      error =
          "cross-module runtime link-plan direct import surface path missing";
      return false;
    }
    if (!seen_direct_import_surface_paths.insert(surface_path).second) {
      error =
          "cross-module runtime link-plan duplicate direct import surface "
          "path: " +
          surface_path;
      return false;
    }
  }
  for (const auto &local_flag : inputs.local_driver_linker_flags) {
    if (local_flag.empty()) {
      error =
          "cross-module runtime link-plan local driver-linker flag input missing";
      return false;
    }
  }

  std::vector<OrderedLinkInput> ordered_link_inputs;
  ordered_link_inputs.reserve(imported_inputs.size() + 1u);

  sections.module_names_lexicographic.reserve(imported_inputs.size() + 1u);
  sections.module_names_lexicographic.push_back(inputs.local_module_name);

  for (const auto &imported_input : imported_inputs) {
    if (!TryValidateImportedLinkPlanInput(
            inputs,
            imported_input,
            seen_translation_unit_identity_keys,
            seen_registration_ordinals,
            seen_error_handling_replay_keys,
            seen_concurrency_actor_replay_keys,
            seen_scheduler_task_replay_keys,
            seen_interop_ffi_replay_keys,
            seen_foreign_abi_replay_keys,
            seen_interop_header_module_bridge_replay_keys,
            seen_metaprogramming_host_cache_replay_keys,
            error)) {
      return false;
    }
    sections.module_names_lexicographic.push_back(imported_input.module_name);
    AddImportedLinkPlanInputToSections(imported_input,
                                       sections,
                                       ordered_link_inputs);
  }

  ordered_link_inputs.push_back(
      {inputs.local_translation_unit_registration_order_ordinal,
       inputs.local_translation_unit_identity_key,
       inputs.local_module_name,
       inputs.local_object_artifact_relative_path,
       inputs.local_driver_linker_flags});
  std::sort(ordered_link_inputs.begin(),
            ordered_link_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.registration_order_ordinal !=
                  rhs.registration_order_ordinal) {
                return lhs.registration_order_ordinal <
                       rhs.registration_order_ordinal;
              }
              return lhs.translation_unit_identity_key <
                     rhs.translation_unit_identity_key;
            });
  for (const auto &ordered_input : ordered_link_inputs) {
    sections.ordered_link_object_artifacts.push_back(
        ordered_input.object_artifact_path);
    for (const auto &flag : ordered_input.driver_linker_flags) {
      if (seen_driver_linker_flags.insert(flag).second) {
        sections.merged_driver_linker_flags.push_back(flag);
      }
    }
  }
  if (sections.merged_driver_linker_flags.empty()) {
    error =
        "cross-module runtime link-plan merged driver-linker flag list is empty";
    return false;
  }

  SortLexicographicModuleSections(sections);
  return true;
}
