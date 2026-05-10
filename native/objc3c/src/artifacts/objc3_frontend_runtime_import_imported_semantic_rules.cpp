#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include "io/json/json_writer.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {

using objc3::io::json::JsonObjectWriter;

std::string BuildImportedRuntimeMetadataSemanticRulesReplayKey(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_semantic_preservation_contract_id
      << ";input_model=" << summary.input_model
      << ";imported_module_count=" << summary.imported_module_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";superclass_edge_count=" << summary.superclass_edge_count
      << ";protocol_conformance_edge_count="
      << summary.protocol_conformance_edge_count
      << ";category_attachment_count=" << summary.category_attachment_count
      << ";property_accessor_trait_count="
      << summary.property_accessor_trait_count
      << ";property_ivar_binding_trait_count="
      << summary.property_ivar_binding_trait_count
      << ";method_selector_trait_count="
      << summary.method_selector_trait_count
      << ";class_method_trait_count="
      << summary.class_method_trait_count
      << ";instance_method_trait_count="
      << summary.instance_method_trait_count
      << ";implemented_method_count="
      << summary.implemented_method_count
      << ";declaration_only_method_count="
      << summary.declaration_only_method_count
      << ";property_attribute_profile_count="
      << summary.property_attribute_profile_count
      << ";ownership_effect_profile_count="
      << summary.ownership_effect_profile_count
      << ";executable_binding_trait_count="
      << summary.executable_binding_trait_count
      << ";optional_send_site_count=" << summary.optional_send_site_count
      << ";typed_keypath_literal_site_count="
      << summary.typed_keypath_literal_site_count
      << ";live_optional_lowering_site_count="
      << summary.live_optional_lowering_site_count
      << ";live_typed_keypath_artifact_site_count="
      << summary.live_typed_keypath_artifact_site_count
      << ";imported_type_system_optional_keypath_module_count="
      << summary.imported_type_system_optional_keypath_module_count
      << ";imported_type_system_generic_contract_module_count="
      << summary.imported_type_system_generic_contract_module_count
      << ";imported_generic_interface_count="
      << summary.imported_generic_interface_count
      << ";imported_generic_parameter_count="
      << summary.imported_generic_parameter_count
      << ";imported_generic_variance_annotation_count="
      << summary.imported_generic_variance_annotation_count
      << ";imported_generic_argument_reference_count="
      << summary.imported_generic_argument_reference_count
      << ";imported_protocol_qualified_generic_argument_count="
      << summary.imported_protocol_qualified_generic_argument_count
      << ";imported_type_system_nullability_contract_module_count="
      << summary.imported_type_system_nullability_contract_module_count
      << ";imported_nullability_canonical_type_count="
      << summary.imported_nullability_canonical_type_count
      << ";imported_nullability_object_type_count="
      << summary.imported_nullability_object_type_count
      << ";imported_nullable_entry_count="
      << summary.imported_nullable_entry_count
      << ";imported_nonnull_entry_count="
      << summary.imported_nonnull_entry_count
      << ";imported_implicitly_unwrapped_entry_count="
      << summary.imported_implicitly_unwrapped_entry_count
      << ";imported_null_resettable_entry_count="
      << summary.imported_null_resettable_entry_count
      << ";imported_unspecified_nullability_entry_count="
      << summary.imported_unspecified_nullability_entry_count
      << ";imported_invalid_nullability_entry_count="
      << summary.imported_invalid_nullability_entry_count
      << ";imported_type_system_protocol_contract_module_count="
      << summary.imported_type_system_protocol_contract_module_count
      << ";imported_protocol_decl_count="
      << summary.imported_protocol_decl_count
      << ";imported_protocol_inheritance_edge_count="
      << summary.imported_protocol_inheritance_edge_count
      << ";imported_protocol_required_method_count="
      << summary.imported_protocol_required_method_count
      << ";imported_protocol_optional_method_count="
      << summary.imported_protocol_optional_method_count
      << ";imported_protocol_required_property_count="
      << summary.imported_protocol_required_property_count
      << ";imported_protocol_optional_property_count="
      << summary.imported_protocol_optional_property_count
      << ";imported_class_protocol_adoption_count="
      << summary.imported_class_protocol_adoption_count
      << ";imported_category_protocol_adoption_count="
      << summary.imported_category_protocol_adoption_count
      << ";modules=";
  for (std::size_t i = 0; i < summary.imported_module_names_lexicographic.size();
       ++i) {
    if (i != 0) {
      out << ",";
    }
    out << summary.imported_module_names_lexicographic[i];
  }
  return out.str();
}

Objc3ImportedRuntimeMetadataSemanticRulesSummary
BuildImportedRuntimeMetadataSemanticRulesSummary(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
        &source_semantic_preservation,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces,
    const std::size_t imported_input_path_count) {
  Objc3ImportedRuntimeMetadataSemanticRulesSummary summary;
  summary.fail_closed = true;
  summary.semantic_surface_published = true;
  summary.imported_input_path_count = imported_input_path_count;
  summary.imported_runtime_surface_inputs_present = imported_input_path_count > 0u;
  summary.imported_runtime_surface_inputs_loaded = true;
  summary.source_semantic_preservation_contract_ready =
      IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
          source_semantic_preservation);
  summary.imported_module_count = imported_surfaces.size();
  summary.imported_module_names_lexicographic.reserve(imported_surfaces.size());

  for (const auto &surface : imported_surfaces) {
    summary.imported_module_names_lexicographic.push_back(
        surface.frontend_closure_summary.module_name);
    const auto &records = surface.runtime_metadata_source_records;
    summary.class_record_count += records.classes_lexicographic.size();
    summary.protocol_record_count += records.protocols_lexicographic.size();
    summary.category_record_count += records.categories_lexicographic.size();
    summary.property_record_count += records.properties_lexicographic.size();
    summary.method_record_count += records.methods_lexicographic.size();
    summary.ivar_record_count += records.ivars_lexicographic.size();
    for (const auto &class_record : records.classes_lexicographic) {
      if (class_record.has_super && !class_record.super_name.empty()) {
        ++summary.superclass_edge_count;
      }
      summary.protocol_conformance_edge_count +=
          class_record.adopted_protocols_lexicographic.size();
    }
    for (const auto &protocol_record : records.protocols_lexicographic) {
      summary.protocol_conformance_edge_count +=
          protocol_record.inherited_protocols_lexicographic.size();
    }
    summary.category_attachment_count += records.categories_lexicographic.size();
    for (const auto &category_record : records.categories_lexicographic) {
      summary.protocol_conformance_edge_count +=
          category_record.adopted_protocols_lexicographic.size();
    }
    for (const auto &property_record : records.properties_lexicographic) {
      if (!property_record.effective_getter_selector.empty()) {
        ++summary.property_accessor_trait_count;
      }
      if (property_record.effective_setter_available &&
          !property_record.effective_setter_selector.empty()) {
        ++summary.property_accessor_trait_count;
      }
      if (!property_record.ivar_binding_symbol.empty()) {
        ++summary.property_ivar_binding_trait_count;
      }
      if (!property_record.property_attribute_profile.empty()) {
        ++summary.property_attribute_profile_count;
      }
      if (!property_record.ownership_lifetime_profile.empty() ||
          !property_record.ownership_runtime_hook_profile.empty() ||
          !property_record.accessor_ownership_profile.empty()) {
        ++summary.ownership_effect_profile_count;
      }
      if ((property_record.executable_synthesized_binding_kind != "none" &&
           !property_record.executable_synthesized_binding_kind.empty()) ||
          !property_record.executable_synthesized_binding_symbol.empty() ||
          !property_record.executable_ivar_layout_symbol.empty()) {
        ++summary.executable_binding_trait_count;
      }
    }
    for (const auto &method_record : records.methods_lexicographic) {
      if (!method_record.selector.empty()) {
        ++summary.method_selector_trait_count;
      }
      if (method_record.is_class_method) {
        ++summary.class_method_trait_count;
      } else {
        ++summary.instance_method_trait_count;
      }
      if (method_record.has_body) {
        ++summary.implemented_method_count;
      } else {
        ++summary.declaration_only_method_count;
      }
    }
    for (const auto &ivar_record : records.ivars_lexicographic) {
      if ((ivar_record.executable_synthesized_binding_kind != "none" &&
           !ivar_record.executable_synthesized_binding_kind.empty()) ||
          !ivar_record.executable_synthesized_binding_symbol.empty() ||
          !ivar_record.executable_ivar_layout_symbol.empty()) {
        ++summary.executable_binding_trait_count;
      }
    }
    if (surface.type_system_optional_keypath_lowering_contract_present) {
      ++summary.imported_type_system_optional_keypath_module_count;
      summary.optional_send_site_count += surface.type_system_optional_send_sites;
      summary.typed_keypath_literal_site_count +=
          surface.type_system_typed_keypath_literal_sites;
      summary.live_optional_lowering_site_count +=
          surface.type_system_live_optional_lowering_sites;
      summary.live_typed_keypath_artifact_site_count +=
          surface.type_system_live_typed_keypath_artifact_sites;
      if (surface.type_system_optional_send_runtime_ready) {
        ++summary.imported_optional_runtime_ready_module_count;
      }
      if (surface.type_system_typed_keypath_descriptor_handles_ready &&
          surface.type_system_typed_keypath_runtime_execution_helper_landed) {
        ++summary.imported_typed_keypath_runtime_ready_module_count;
      }
    }
    if (surface.type_system_generic_contract_preservation_present) {
      ++summary.imported_type_system_generic_contract_module_count;
      summary.imported_generic_interface_count +=
          surface.type_system_generic_interface_count;
      summary.imported_generic_parameter_count +=
          surface.type_system_generic_parameter_count;
      summary.imported_generic_variance_annotation_count +=
          surface.type_system_generic_variance_annotation_count;
      summary.imported_generic_argument_reference_count +=
          surface.type_system_generic_argument_reference_count;
      summary.imported_protocol_qualified_generic_argument_count +=
          surface.type_system_protocol_qualified_generic_argument_count;
    }
    if (surface.type_system_nullability_contract_preservation_present) {
      ++summary.imported_type_system_nullability_contract_module_count;
      summary.imported_nullability_canonical_type_count +=
          surface.type_system_nullability_canonical_type_count;
      summary.imported_nullability_object_type_count +=
          surface.type_system_nullability_object_type_count;
      summary.imported_nullable_entry_count +=
          surface.type_system_nullable_entry_count;
      summary.imported_nonnull_entry_count +=
          surface.type_system_nonnull_entry_count;
      summary.imported_implicitly_unwrapped_entry_count +=
          surface.type_system_implicitly_unwrapped_entry_count;
      summary.imported_null_resettable_entry_count +=
          surface.type_system_null_resettable_entry_count;
      summary.imported_unspecified_nullability_entry_count +=
          surface.type_system_unspecified_nullability_entry_count;
      summary.imported_invalid_nullability_entry_count +=
          surface.type_system_invalid_nullability_entry_count;
    }
    if (surface.type_system_protocol_contract_preservation_present) {
      ++summary.imported_type_system_protocol_contract_module_count;
      summary.imported_protocol_decl_count +=
          surface.type_system_protocol_decl_count;
      summary.imported_protocol_forward_declaration_count +=
          surface.type_system_protocol_forward_declaration_count;
      summary.imported_protocol_inheritance_edge_count +=
          surface.type_system_protocol_inheritance_edge_count;
      summary.imported_protocol_required_method_count +=
          surface.type_system_protocol_required_method_count;
      summary.imported_protocol_optional_method_count +=
          surface.type_system_protocol_optional_method_count;
      summary.imported_protocol_required_property_count +=
          surface.type_system_protocol_required_property_count;
      summary.imported_protocol_optional_property_count +=
          surface.type_system_protocol_optional_property_count;
      summary.imported_class_protocol_adoption_count +=
          surface.type_system_class_protocol_adoption_count;
      summary.imported_category_protocol_adoption_count +=
          surface.type_system_category_protocol_adoption_count;
    }
  }

  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  summary.runtime_owned_declaration_count =
      summary.class_record_count + summary.protocol_record_count +
      summary.category_record_count + summary.property_record_count +
      summary.method_record_count + summary.ivar_record_count;
  summary.imported_conformance_shape_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_dispatch_traits_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_effect_traits_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_runtime_metadata_semantics_landed =
      summary.source_semantic_preservation_contract_ready;
  const bool imported_optional_keypath_landed =
      summary.imported_type_system_optional_keypath_module_count == 0u ||
      summary.imported_type_system_optional_keypath_module_count ==
          summary.imported_optional_runtime_ready_module_count;
  const bool imported_generic_contract_landed =
      summary.imported_type_system_generic_contract_module_count == 0u ||
      summary.imported_generic_variance_annotation_count >=
          summary.imported_generic_parameter_count;
  const bool imported_nullability_contract_landed =
      summary.imported_type_system_nullability_contract_module_count == 0u ||
      summary.imported_nullability_canonical_type_count ==
          summary.imported_nullable_entry_count +
              summary.imported_nonnull_entry_count +
              summary.imported_implicitly_unwrapped_entry_count +
              summary.imported_null_resettable_entry_count +
              summary.imported_unspecified_nullability_entry_count;
  const bool imported_protocol_contract_landed =
      summary.imported_type_system_protocol_contract_module_count == 0u ||
      summary.imported_protocol_decl_count >=
          summary.imported_protocol_forward_declaration_count;
  summary.imported_type_system_type_surface_landed =
      imported_optional_keypath_landed && imported_generic_contract_landed &&
      imported_nullability_contract_landed && imported_protocol_contract_landed;
  summary.imported_optional_runtime_semantics_landed =
      summary.optional_send_site_count == 0u ||
      summary.imported_optional_runtime_ready_module_count > 0u;
  summary.imported_typed_keypath_runtime_semantics_landed =
      summary.typed_keypath_literal_site_count == 0u ||
      summary.imported_typed_keypath_runtime_ready_module_count > 0u;
  summary.ready_for_imported_metadata_semantic_rules =
      summary.source_semantic_preservation_contract_ready &&
      summary.imported_runtime_surface_inputs_loaded;
  summary.ready_for_cross_module_dispatch_equivalence =
      summary.ready_for_imported_metadata_semantic_rules;

  if (summary.ready_for_imported_metadata_semantic_rules) {
    summary.replay_key =
        BuildImportedRuntimeMetadataSemanticRulesReplayKey(summary);
  } else {
    summary.failure_reason =
        "imported runtime metadata semantic rules summary is incomplete";
  }
  if (!IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "imported runtime metadata semantic rules summary is incomplete";
  }
  return summary;
}

std::string BuildImportedRuntimeMetadataSemanticRulesSummaryJson(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_semantic_preservation_contract_id",
                     summary.source_semantic_preservation_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.StringArrayField("imported_module_names_lexicographic",
                          summary.imported_module_names_lexicographic);
  object.SizeField("imported_input_path_count",
                   summary.imported_input_path_count);
  object.SizeField("imported_module_count", summary.imported_module_count);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_edge_count", summary.superclass_edge_count);
  object.SizeField("protocol_conformance_edge_count",
                   summary.protocol_conformance_edge_count);
  object.SizeField("category_attachment_count",
                   summary.category_attachment_count);
  object.SizeField("property_accessor_trait_count",
                   summary.property_accessor_trait_count);
  object.SizeField("property_ivar_binding_trait_count",
                   summary.property_ivar_binding_trait_count);
  object.SizeField("method_selector_trait_count",
                   summary.method_selector_trait_count);
  object.SizeField("class_method_trait_count", summary.class_method_trait_count);
  object.SizeField("instance_method_trait_count",
                   summary.instance_method_trait_count);
  object.SizeField("implemented_method_count",
                   summary.implemented_method_count);
  object.SizeField("declaration_only_method_count",
                   summary.declaration_only_method_count);
  object.SizeField("property_attribute_profile_count",
                   summary.property_attribute_profile_count);
  object.SizeField("ownership_effect_profile_count",
                   summary.ownership_effect_profile_count);
  object.SizeField("executable_binding_trait_count",
                   summary.executable_binding_trait_count);
  object.SizeField("optional_send_site_count", summary.optional_send_site_count);
  object.SizeField("typed_keypath_literal_site_count",
                   summary.typed_keypath_literal_site_count);
  object.SizeField("live_optional_lowering_site_count",
                   summary.live_optional_lowering_site_count);
  object.SizeField("live_typed_keypath_artifact_site_count",
                   summary.live_typed_keypath_artifact_site_count);
  object.SizeField("imported_type_system_optional_keypath_module_count",
                   summary.imported_type_system_optional_keypath_module_count);
  object.SizeField("imported_optional_runtime_ready_module_count",
                   summary.imported_optional_runtime_ready_module_count);
  object.SizeField("imported_typed_keypath_runtime_ready_module_count",
                   summary.imported_typed_keypath_runtime_ready_module_count);
  object.SizeField("imported_type_system_generic_contract_module_count",
                   summary.imported_type_system_generic_contract_module_count);
  object.SizeField("imported_generic_interface_count",
                   summary.imported_generic_interface_count);
  object.SizeField("imported_generic_parameter_count",
                   summary.imported_generic_parameter_count);
  object.SizeField("imported_generic_variance_annotation_count",
                   summary.imported_generic_variance_annotation_count);
  object.SizeField("imported_generic_argument_reference_count",
                   summary.imported_generic_argument_reference_count);
  object.SizeField("imported_protocol_qualified_generic_argument_count",
                   summary.imported_protocol_qualified_generic_argument_count);
  object.SizeField("imported_type_system_nullability_contract_module_count",
                   summary.imported_type_system_nullability_contract_module_count);
  object.SizeField("imported_nullability_canonical_type_count",
                   summary.imported_nullability_canonical_type_count);
  object.SizeField("imported_nullability_object_type_count",
                   summary.imported_nullability_object_type_count);
  object.SizeField("imported_nullable_entry_count",
                   summary.imported_nullable_entry_count);
  object.SizeField("imported_nonnull_entry_count",
                   summary.imported_nonnull_entry_count);
  object.SizeField("imported_implicitly_unwrapped_entry_count",
                   summary.imported_implicitly_unwrapped_entry_count);
  object.SizeField("imported_null_resettable_entry_count",
                   summary.imported_null_resettable_entry_count);
  object.SizeField("imported_unspecified_nullability_entry_count",
                   summary.imported_unspecified_nullability_entry_count);
  object.SizeField("imported_invalid_nullability_entry_count",
                   summary.imported_invalid_nullability_entry_count);
  object.SizeField("imported_type_system_protocol_contract_module_count",
                   summary.imported_type_system_protocol_contract_module_count);
  object.SizeField("imported_protocol_decl_count",
                   summary.imported_protocol_decl_count);
  object.SizeField("imported_protocol_forward_declaration_count",
                   summary.imported_protocol_forward_declaration_count);
  object.SizeField("imported_protocol_inheritance_edge_count",
                   summary.imported_protocol_inheritance_edge_count);
  object.SizeField("imported_protocol_required_method_count",
                   summary.imported_protocol_required_method_count);
  object.SizeField("imported_protocol_optional_method_count",
                   summary.imported_protocol_optional_method_count);
  object.SizeField("imported_protocol_required_property_count",
                   summary.imported_protocol_required_property_count);
  object.SizeField("imported_protocol_optional_property_count",
                   summary.imported_protocol_optional_property_count);
  object.SizeField("imported_class_protocol_adoption_count",
                   summary.imported_class_protocol_adoption_count);
  object.SizeField("imported_category_protocol_adoption_count",
                   summary.imported_category_protocol_adoption_count);
  object.BoolField(
      "ready",
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_semantic_preservation_contract_ready",
                   summary.source_semantic_preservation_contract_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_runtime_surface_inputs_present",
                   summary.imported_runtime_surface_inputs_present);
  object.BoolField("imported_runtime_surface_inputs_loaded",
                   summary.imported_runtime_surface_inputs_loaded);
  object.BoolField("imported_conformance_shape_landed",
                   summary.imported_conformance_shape_landed);
  object.BoolField("imported_dispatch_traits_landed",
                   summary.imported_dispatch_traits_landed);
  object.BoolField("imported_effect_traits_landed",
                   summary.imported_effect_traits_landed);
  object.BoolField("imported_runtime_metadata_semantics_landed",
                   summary.imported_runtime_metadata_semantics_landed);
  object.BoolField("imported_type_system_type_surface_landed",
                   summary.imported_type_system_type_surface_landed);
  object.BoolField("imported_optional_runtime_semantics_landed",
                   summary.imported_optional_runtime_semantics_landed);
  object.BoolField("imported_typed_keypath_runtime_semantics_landed",
                   summary.imported_typed_keypath_runtime_semantics_landed);
  object.BoolField("ready_for_imported_metadata_semantic_rules",
                   summary.ready_for_imported_metadata_semantic_rules);
  object.BoolField("ready_for_cross_module_dispatch_equivalence",
                   summary.ready_for_cross_module_dispatch_equivalence);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

}  // namespace objc3::artifacts::frontend
