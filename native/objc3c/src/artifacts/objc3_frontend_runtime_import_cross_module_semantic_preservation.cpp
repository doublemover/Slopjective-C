#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>

#include "io/json/json_writer.h"
#include "pipeline/results/evidence_record.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {

using objc3::io::json::JsonObjectWriter;

std::string BuildCrossModuleRuntimeMetadataSemanticPreservationReplayKey(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_frontend_closure_contract_id="
      << summary.source_frontend_closure_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";source_artifact_relative_path="
      << summary.source_artifact_relative_path
      << ";authority_model=" << summary.authority_model
      << ";conformance_shape_model=" << summary.conformance_shape_model
      << ";dispatch_trait_model=" << summary.dispatch_trait_model
      << ";effect_trait_model=" << summary.effect_trait_model
      << ";module_name=" << summary.module_name
      << ";class_record_count=" << summary.class_record_count
      << ";protocol_record_count=" << summary.protocol_record_count
      << ";category_record_count=" << summary.category_record_count
      << ";property_record_count=" << summary.property_record_count
      << ";method_record_count=" << summary.method_record_count
      << ";ivar_record_count=" << summary.ivar_record_count
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
      << ";class_method_trait_count=" << summary.class_method_trait_count
      << ";instance_method_trait_count="
      << summary.instance_method_trait_count
      << ";implemented_method_count=" << summary.implemented_method_count
      << ";declaration_only_method_count="
      << summary.declaration_only_method_count
      << ";property_attribute_profile_count="
      << summary.property_attribute_profile_count
      << ";ownership_effect_profile_count="
      << summary.ownership_effect_profile_count
      << ";executable_binding_trait_count="
      << summary.executable_binding_trait_count
      << ";source_frontend_closure_replay_key="
      << summary.source_frontend_closure_replay_key;
  return out.str();
}

Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
BuildCrossModuleRuntimeMetadataSemanticPreservationSummary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &source_frontend_closure,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary summary;
  summary.fail_closed = true;
  summary.source_frontend_closure_ready =
      IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          source_frontend_closure);
  summary.runtime_metadata_source_records_ready =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(runtime_metadata_source_records);
  summary.semantic_surface_published = true;
  summary.module_name = source_frontend_closure.module_name;
  summary.class_record_count =
      runtime_metadata_source_records.classes_lexicographic.size();
  summary.protocol_record_count =
      runtime_metadata_source_records.protocols_lexicographic.size();
  summary.category_record_count =
      runtime_metadata_source_records.categories_lexicographic.size();
  summary.property_record_count =
      runtime_metadata_source_records.properties_lexicographic.size();
  summary.method_record_count =
      runtime_metadata_source_records.methods_lexicographic.size();
  summary.ivar_record_count =
      runtime_metadata_source_records.ivars_lexicographic.size();
  summary.runtime_owned_declaration_count =
      summary.class_record_count + summary.protocol_record_count +
      summary.category_record_count + summary.property_record_count +
      summary.method_record_count + summary.ivar_record_count;
  for (const auto &class_record :
       runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      ++summary.superclass_edge_count;
    }
    summary.protocol_conformance_edge_count +=
        class_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &protocol_record :
       runtime_metadata_source_records.protocols_lexicographic) {
    summary.protocol_conformance_edge_count +=
        protocol_record.inherited_protocols_lexicographic.size();
  }
  summary.category_attachment_count =
      runtime_metadata_source_records.categories_lexicographic.size();
  for (const auto &category_record :
       runtime_metadata_source_records.categories_lexicographic) {
    summary.protocol_conformance_edge_count +=
        category_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &property_record :
       runtime_metadata_source_records.properties_lexicographic) {
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
    if (property_record.executable_synthesized_binding_kind != "none" &&
        !property_record.executable_synthesized_binding_kind.empty()) {
      ++summary.executable_binding_trait_count;
    }
  }
  for (const auto &method_record :
       runtime_metadata_source_records.methods_lexicographic) {
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
  for (const auto &ivar_record :
       runtime_metadata_source_records.ivars_lexicographic) {
    if ((ivar_record.executable_synthesized_binding_kind != "none" &&
         !ivar_record.executable_synthesized_binding_kind.empty()) ||
        !ivar_record.executable_ivar_layout_symbol.empty()) {
      ++summary.executable_binding_trait_count;
    }
  }
  summary.source_frontend_closure_replay_key =
      source_frontend_closure.replay_key;
  if (summary.source_frontend_closure_ready &&
      summary.runtime_metadata_source_records_ready) {
    summary.replay_key =
        BuildCrossModuleRuntimeMetadataSemanticPreservationReplayKey(summary);
  }
  if (!IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
          summary)) {
    summary.failure_reason =
        "cross-module runtime metadata semantic preservation summary is incomplete";
  }
  return summary;
}

std::string BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_frontend_closure_contract_id",
                     summary.source_frontend_closure_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("source_artifact_relative_path",
                     summary.source_artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("conformance_shape_model",
                     summary.conformance_shape_model);
  object.StringField("dispatch_trait_model", summary.dispatch_trait_model);
  object.StringField("effect_trait_model", summary.effect_trait_model);
  object.StringField("module_name", summary.module_name);
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
  object.BoolField(
      "ready",
      IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
          summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_frontend_closure_ready",
                   summary.source_frontend_closure_ready);
  object.BoolField("runtime_metadata_source_records_ready",
                   summary.runtime_metadata_source_records_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_conformance_shape_landed",
                   summary.imported_conformance_shape_landed);
  object.BoolField("imported_dispatch_traits_landed",
                   summary.imported_dispatch_traits_landed);
  object.BoolField("imported_effect_traits_landed",
                   summary.imported_effect_traits_landed);
  object.BoolField("imported_runtime_metadata_semantics_landed",
                   summary.imported_runtime_metadata_semantics_landed);
  object.BoolField("ready_for_imported_metadata_semantic_rules",
                   summary.ready_for_imported_metadata_semantic_rules);
  object.BoolField("ready_for_cross_module_dispatch_equivalence",
                   summary.ready_for_cross_module_dispatch_equivalence);
  object.StringField("source_frontend_closure_replay_key",
                     summary.source_frontend_closure_replay_key);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

}  // namespace objc3::artifacts::frontend
