#include "io/objc3_cross_module_runtime_link_plan_document_counts_storage.h"

#include <cstddef>
#include <ostream>

namespace {

void EmitCountField(std::ostream &out, const char *name, std::size_t value) {
  out << "  \"" << name << "\": " << value << ",\n";
}

}  // namespace

void EmitObjc3CrossModuleRuntimeLinkPlanStorageReflectionCounts(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanSections &sections) {
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
