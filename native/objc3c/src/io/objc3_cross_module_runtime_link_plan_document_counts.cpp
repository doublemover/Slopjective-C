#include "io/objc3_cross_module_runtime_link_plan_document_counts.h"

#include <cstddef>
#include <ostream>

namespace {

void EmitCountField(std::ostream &out, const char *name, std::size_t value) {
  out << "  \"" << name << "\": " << value << ",\n";
}

}  // namespace

void EmitObjc3CrossModuleRuntimeLinkPlanDocumentCounts(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanSections &sections,
    std::size_t imported_input_count) {
  EmitCountField(out, "module_image_count", imported_input_count + 1);
  EmitCountField(out, "direct_import_input_count", imported_input_count);
  EmitCountField(out,
                 "local_class_descriptor_count",
                 inputs.local_class_descriptor_count);
  EmitCountField(out,
                 "local_protocol_descriptor_count",
                 inputs.local_protocol_descriptor_count);
  EmitCountField(out,
                 "local_category_descriptor_count",
                 inputs.local_category_descriptor_count);
  EmitCountField(out,
                 "local_property_descriptor_count",
                 inputs.local_property_descriptor_count);
  EmitCountField(out,
                 "local_ivar_descriptor_count",
                 inputs.local_ivar_descriptor_count);
  EmitCountField(out,
                 "local_total_descriptor_count",
                 inputs.local_total_descriptor_count);
  EmitCountField(out,
                 "imported_class_descriptor_count",
                 sections.imported_class_descriptor_count);
  EmitCountField(out,
                 "imported_protocol_descriptor_count",
                 sections.imported_protocol_descriptor_count);
  EmitCountField(out,
                 "imported_category_descriptor_count",
                 sections.imported_category_descriptor_count);
  EmitCountField(out,
                 "imported_property_descriptor_count",
                 sections.imported_property_descriptor_count);
  EmitCountField(out,
                 "imported_ivar_descriptor_count",
                 sections.imported_ivar_descriptor_count);
  EmitCountField(out,
                 "imported_total_descriptor_count",
                 sections.imported_total_descriptor_count);
  EmitCountField(out,
                 "transitive_class_descriptor_count",
                 inputs.local_class_descriptor_count +
                     sections.imported_class_descriptor_count);
  EmitCountField(out,
                 "transitive_protocol_descriptor_count",
                 inputs.local_protocol_descriptor_count +
                     sections.imported_protocol_descriptor_count);
  EmitCountField(out,
                 "transitive_category_descriptor_count",
                 inputs.local_category_descriptor_count +
                     sections.imported_category_descriptor_count);
  EmitCountField(out,
                 "transitive_property_descriptor_count",
                 inputs.local_property_descriptor_count +
                     sections.imported_property_descriptor_count);
  EmitCountField(out,
                 "transitive_ivar_descriptor_count",
                 inputs.local_ivar_descriptor_count +
                     sections.imported_ivar_descriptor_count);
  EmitCountField(out,
                 "transitive_total_descriptor_count",
                 inputs.local_total_descriptor_count +
                     sections.imported_total_descriptor_count);

  EmitCountField(out,
                 "local_block_ownership_block_literal_sites",
                 inputs.local_block_ownership_block_literal_sites);
  EmitCountField(
      out,
      "local_block_ownership_invoke_trampoline_symbolized_sites",
      inputs.local_block_ownership_invoke_trampoline_symbolized_sites);
  EmitCountField(out,
                 "local_block_ownership_copy_helper_required_sites",
                 inputs.local_block_ownership_copy_helper_required_sites);
  EmitCountField(out,
                 "local_block_ownership_dispose_helper_required_sites",
                 inputs.local_block_ownership_dispose_helper_required_sites);
  EmitCountField(out,
                 "local_block_ownership_copy_helper_symbolized_sites",
                 inputs.local_block_ownership_copy_helper_symbolized_sites);
  EmitCountField(out,
                 "local_block_ownership_dispose_helper_symbolized_sites",
                 inputs.local_block_ownership_dispose_helper_symbolized_sites);
  EmitCountField(out,
                 "local_block_ownership_escape_to_heap_sites",
                 inputs.local_block_ownership_escape_to_heap_sites);
  EmitCountField(out,
                 "local_block_ownership_byref_layout_symbolized_sites",
                 inputs.local_block_ownership_byref_layout_symbolized_sites);
  EmitCountField(out,
                 "imported_block_ownership_block_literal_sites",
                 sections.imported_block_ownership_block_literal_sites);
  EmitCountField(
      out,
      "imported_block_ownership_invoke_trampoline_symbolized_sites",
      sections.imported_block_ownership_invoke_trampoline_symbolized_sites);
  EmitCountField(out,
                 "imported_block_ownership_copy_helper_required_sites",
                 sections.imported_block_ownership_copy_helper_required_sites);
  EmitCountField(
      out,
      "imported_block_ownership_dispose_helper_required_sites",
      sections.imported_block_ownership_dispose_helper_required_sites);
  EmitCountField(out,
                 "imported_block_ownership_copy_helper_symbolized_sites",
                 sections.imported_block_ownership_copy_helper_symbolized_sites);
  EmitCountField(
      out,
      "imported_block_ownership_dispose_helper_symbolized_sites",
      sections.imported_block_ownership_dispose_helper_symbolized_sites);
  EmitCountField(out,
                 "imported_block_ownership_escape_to_heap_sites",
                 sections.imported_block_ownership_escape_to_heap_sites);
  EmitCountField(out,
                 "imported_block_ownership_byref_layout_symbolized_sites",
                 sections.imported_block_ownership_byref_layout_symbolized_sites);
  EmitCountField(out,
                 "transitive_block_ownership_block_literal_sites",
                 inputs.local_block_ownership_block_literal_sites +
                     sections.imported_block_ownership_block_literal_sites);
  EmitCountField(
      out,
      "transitive_block_ownership_invoke_trampoline_symbolized_sites",
      inputs.local_block_ownership_invoke_trampoline_symbolized_sites +
          sections.imported_block_ownership_invoke_trampoline_symbolized_sites);
  EmitCountField(out,
                 "transitive_block_ownership_copy_helper_required_sites",
                 inputs.local_block_ownership_copy_helper_required_sites +
                     sections.imported_block_ownership_copy_helper_required_sites);
  EmitCountField(
      out,
      "transitive_block_ownership_dispose_helper_required_sites",
      inputs.local_block_ownership_dispose_helper_required_sites +
          sections.imported_block_ownership_dispose_helper_required_sites);
  EmitCountField(
      out,
      "transitive_block_ownership_copy_helper_symbolized_sites",
      inputs.local_block_ownership_copy_helper_symbolized_sites +
          sections.imported_block_ownership_copy_helper_symbolized_sites);
  EmitCountField(
      out,
      "transitive_block_ownership_dispose_helper_symbolized_sites",
      inputs.local_block_ownership_dispose_helper_symbolized_sites +
          sections.imported_block_ownership_dispose_helper_symbolized_sites);
  EmitCountField(out,
                 "transitive_block_ownership_escape_to_heap_sites",
                 inputs.local_block_ownership_escape_to_heap_sites +
                     sections.imported_block_ownership_escape_to_heap_sites);
  EmitCountField(
      out,
      "transitive_block_ownership_byref_layout_symbolized_sites",
      inputs.local_block_ownership_byref_layout_symbolized_sites +
          sections.imported_block_ownership_byref_layout_symbolized_sites);

  EmitCountField(
      out,
      "local_storage_reflection_implementation_owned_property_entries",
      inputs.local_storage_reflection_implementation_owned_property_entries);
  EmitCountField(
      out,
      "local_storage_reflection_synthesized_accessor_owner_entries",
      inputs.local_storage_reflection_synthesized_accessor_owner_entries);
  EmitCountField(out,
                 "local_storage_reflection_synthesized_getter_entries",
                 inputs.local_storage_reflection_synthesized_getter_entries);
  EmitCountField(out,
                 "local_storage_reflection_synthesized_setter_entries",
                 inputs.local_storage_reflection_synthesized_setter_entries);
  EmitCountField(out,
                 "local_storage_reflection_synthesized_accessor_entries",
                 inputs.local_storage_reflection_synthesized_accessor_entries);
  EmitCountField(out,
                 "local_storage_reflection_current_property_read_entries",
                 inputs.local_storage_reflection_current_property_read_entries);
  EmitCountField(
      out,
      "local_storage_reflection_current_property_write_entries",
      inputs.local_storage_reflection_current_property_write_entries);
  EmitCountField(
      out,
      "local_storage_reflection_current_property_exchange_entries",
      inputs.local_storage_reflection_current_property_exchange_entries);
  EmitCountField(
      out,
      "local_storage_reflection_weak_current_property_load_entries",
      inputs.local_storage_reflection_weak_current_property_load_entries);
  EmitCountField(
      out,
      "local_storage_reflection_weak_current_property_store_entries",
      inputs.local_storage_reflection_weak_current_property_store_entries);
  EmitCountField(out,
                 "local_storage_reflection_ivar_layout_entries",
                 inputs.local_storage_reflection_ivar_layout_entries);
  EmitCountField(out,
                 "local_storage_reflection_ivar_layout_owner_entries",
                 inputs.local_storage_reflection_ivar_layout_owner_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_implementation_owned_property_entries",
      sections
          .imported_storage_reflection_implementation_owned_property_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_synthesized_accessor_owner_entries",
      sections.imported_storage_reflection_synthesized_accessor_owner_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_synthesized_getter_entries",
      sections.imported_storage_reflection_synthesized_getter_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_synthesized_setter_entries",
      sections.imported_storage_reflection_synthesized_setter_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_synthesized_accessor_entries",
      sections.imported_storage_reflection_synthesized_accessor_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_current_property_read_entries",
      sections.imported_storage_reflection_current_property_read_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_current_property_write_entries",
      sections.imported_storage_reflection_current_property_write_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_current_property_exchange_entries",
      sections.imported_storage_reflection_current_property_exchange_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_weak_current_property_load_entries",
      sections.imported_storage_reflection_weak_current_property_load_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_weak_current_property_store_entries",
      sections.imported_storage_reflection_weak_current_property_store_entries);
  EmitCountField(out,
                 "imported_storage_reflection_ivar_layout_entries",
                 sections.imported_storage_reflection_ivar_layout_entries);
  EmitCountField(
      out,
      "imported_storage_reflection_ivar_layout_owner_entries",
      sections.imported_storage_reflection_ivar_layout_owner_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_implementation_owned_property_entries",
      inputs.local_storage_reflection_implementation_owned_property_entries +
          sections
              .imported_storage_reflection_implementation_owned_property_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_synthesized_accessor_owner_entries",
      inputs.local_storage_reflection_synthesized_accessor_owner_entries +
          sections.imported_storage_reflection_synthesized_accessor_owner_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_synthesized_getter_entries",
      inputs.local_storage_reflection_synthesized_getter_entries +
          sections.imported_storage_reflection_synthesized_getter_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_synthesized_setter_entries",
      inputs.local_storage_reflection_synthesized_setter_entries +
          sections.imported_storage_reflection_synthesized_setter_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_synthesized_accessor_entries",
      inputs.local_storage_reflection_synthesized_accessor_entries +
          sections.imported_storage_reflection_synthesized_accessor_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_current_property_read_entries",
      inputs.local_storage_reflection_current_property_read_entries +
          sections.imported_storage_reflection_current_property_read_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_current_property_write_entries",
      inputs.local_storage_reflection_current_property_write_entries +
          sections.imported_storage_reflection_current_property_write_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_current_property_exchange_entries",
      inputs.local_storage_reflection_current_property_exchange_entries +
          sections.imported_storage_reflection_current_property_exchange_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_weak_current_property_load_entries",
      inputs.local_storage_reflection_weak_current_property_load_entries +
          sections.imported_storage_reflection_weak_current_property_load_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_weak_current_property_store_entries",
      inputs.local_storage_reflection_weak_current_property_store_entries +
          sections.imported_storage_reflection_weak_current_property_store_entries);
  EmitCountField(out,
                 "transitive_storage_reflection_ivar_layout_entries",
                 inputs.local_storage_reflection_ivar_layout_entries +
                     sections.imported_storage_reflection_ivar_layout_entries);
  EmitCountField(
      out,
      "transitive_storage_reflection_ivar_layout_owner_entries",
      inputs.local_storage_reflection_ivar_layout_owner_entries +
          sections.imported_storage_reflection_ivar_layout_owner_entries);
}
