#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "io/objc3_process.h"

struct Objc3CrossModuleRuntimeLinkPlanSections {
  std::vector<std::string> direct_import_surface_artifact_paths;
  std::vector<std::string> ordered_link_object_artifacts;
  std::vector<std::string> merged_driver_linker_flags;
  std::vector<std::string> module_names_lexicographic;
  std::vector<std::string> imported_error_handling_module_names_lexicographic;
  std::vector<std::string>
      imported_concurrency_actor_module_names_lexicographic;
  std::vector<std::string> imported_interop_ffi_module_names_lexicographic;
  std::vector<std::string>
      imported_interop_header_module_bridge_module_names_lexicographic;
  std::vector<std::string>
      imported_metaprogramming_host_cache_module_names_lexicographic;

  std::size_t imported_class_descriptor_count = 0;
  std::size_t imported_protocol_descriptor_count = 0;
  std::size_t imported_category_descriptor_count = 0;
  std::size_t imported_property_descriptor_count = 0;
  std::size_t imported_ivar_descriptor_count = 0;
  std::size_t imported_total_descriptor_count = 0;

  std::size_t imported_block_ownership_block_literal_sites = 0;
  std::size_t imported_block_ownership_invoke_trampoline_symbolized_sites = 0;
  std::size_t imported_block_ownership_copy_helper_required_sites = 0;
  std::size_t imported_block_ownership_dispose_helper_required_sites = 0;
  std::size_t imported_block_ownership_copy_helper_symbolized_sites = 0;
  std::size_t imported_block_ownership_dispose_helper_symbolized_sites = 0;
  std::size_t imported_block_ownership_escape_to_heap_sites = 0;
  std::size_t imported_block_ownership_byref_layout_symbolized_sites = 0;

  std::size_t
      imported_storage_reflection_implementation_owned_property_entries = 0;
  std::size_t
      imported_storage_reflection_synthesized_accessor_owner_entries = 0;
  std::size_t imported_storage_reflection_synthesized_getter_entries = 0;
  std::size_t imported_storage_reflection_synthesized_setter_entries = 0;
  std::size_t imported_storage_reflection_synthesized_accessor_entries = 0;
  std::size_t imported_storage_reflection_current_property_read_entries = 0;
  std::size_t imported_storage_reflection_current_property_write_entries = 0;
  std::size_t imported_storage_reflection_current_property_exchange_entries = 0;
  std::size_t imported_storage_reflection_weak_current_property_load_entries = 0;
  std::size_t imported_storage_reflection_weak_current_property_store_entries =
      0;
  std::size_t imported_storage_reflection_ivar_layout_entries = 0;
  std::size_t imported_storage_reflection_ivar_layout_owner_entries = 0;
};

bool BuildObjc3CrossModuleRuntimeLinkPlanSections(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
        &imported_inputs,
    Objc3CrossModuleRuntimeLinkPlanSections &sections,
    std::string &error);
