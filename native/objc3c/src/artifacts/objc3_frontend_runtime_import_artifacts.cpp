#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"
#include "sema/model/semantic_type.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;
using objc3::io::json::JsonObjectWriter;
using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << "\"" << EscapeJsonString(values[i]) << "\"";
  }
  out << "]";
  return out.str();
}

constexpr char kObjc3RuntimeAwareImportModuleSurfaceContractId[] =
    "objc3c.runtime.aware.import.module.surface.v1";
constexpr char kObjc3RuntimeAwareImportModuleSurfacePath[] =
    "frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract";
constexpr char kObjc3RuntimeAwareImportModuleSurfaceSourceModel[] =
    "runtime-aware-import-module-surface-freezes-frontend-owned-runtime-declaration-and-metadata-reference-boundaries-before-cross-translation-unit-realization";
constexpr char kObjc3RuntimeAwareImportModuleSurfaceNonGoalModel[] =
    "no-imported-module-artifact-reader-no-imported-runtime-declaration-materialization-no-imported-runtime-metadata-reference-lowering";
constexpr char kObjc3RuntimeAwareImportModuleSurfaceFailureModel[] =
    "fail-closed-on-runtime-aware-import-module-surface-drift-or-premature-capability-claims";

}  // namespace

std::string BuildRuntimeAwareImportModuleSurfaceReplayKey(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  std::ostringstream out;
  out << kObjc3RuntimeAwareImportModuleSurfaceContractId
      << ";module_name=" << program.module_name
      << ";protocol_decl_count=" << parser_contract_snapshot.protocol_decl_count
      << ";interface_decl_count="
      << parser_contract_snapshot.interface_decl_count
      << ";implementation_decl_count="
      << parser_contract_snapshot.implementation_decl_count
      << ";interface_category_decl_count="
      << parser_contract_snapshot.interface_category_decl_count
      << ";implementation_category_decl_count="
      << parser_contract_snapshot.implementation_category_decl_count
      << ";function_decl_count=" << parser_contract_snapshot.function_decl_count
      << ";module_import_graph_sites="
      << module_import_graph_lowering_contract.module_import_graph_sites
      << ";import_edge_candidate_sites="
      << module_import_graph_lowering_contract.import_edge_candidate_sites
      << ";object_pointer_type_sites="
      << module_import_graph_lowering_contract.object_pointer_type_sites
      << ";normalized_sites="
      << module_import_graph_lowering_contract.normalized_sites
      << ";runtime_aware_import_declarations_landed=false"
      << ";module_metadata_import_surface_landed=false"
      << ";runtime_owned_declaration_import_landed=false"
      << ";runtime_metadata_reference_import_landed=false"
      << ";public_frontend_api_module_surface_landed=false"
      << ";fail_closed=true"
      << ";deterministic="
      << (module_import_graph_lowering_contract.deterministic ? "true" : "false");
  return out.str();
}

std::string BuildRuntimeAwareImportModuleSurfaceSummaryJson(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceContractId
      << "\",\"surface_path\":\""
      << kObjc3RuntimeAwareImportModuleSurfacePath
      << "\",\"source_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceSourceModel
      << "\",\"non_goal_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceNonGoalModel
      << "\",\"failure_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceFailureModel
      << "\",\"module_name\":\"" << EscapeJsonString(program.module_name)
      << "\",\"protocol_decl_count\":"
      << parser_contract_snapshot.protocol_decl_count
      << ",\"interface_decl_count\":"
      << parser_contract_snapshot.interface_decl_count
      << ",\"implementation_decl_count\":"
      << parser_contract_snapshot.implementation_decl_count
      << ",\"interface_category_decl_count\":"
      << parser_contract_snapshot.interface_category_decl_count
      << ",\"implementation_category_decl_count\":"
      << parser_contract_snapshot.implementation_category_decl_count
      << ",\"function_decl_count\":"
      << parser_contract_snapshot.function_decl_count
      << ",\"module_import_graph_sites\":"
      << module_import_graph_lowering_contract.module_import_graph_sites
      << ",\"import_edge_candidate_sites\":"
      << module_import_graph_lowering_contract.import_edge_candidate_sites
      << ",\"namespace_segment_sites\":"
      << module_import_graph_lowering_contract.namespace_segment_sites
      << ",\"object_pointer_type_sites\":"
      << module_import_graph_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << module_import_graph_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << module_import_graph_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << module_import_graph_lowering_contract.contract_violation_sites
      << ",\"runtime_aware_import_declarations_landed\":false"
      << ",\"module_metadata_import_surface_landed\":false"
      << ",\"runtime_owned_declaration_import_landed\":false"
      << ",\"runtime_metadata_reference_import_landed\":false"
      << ",\"public_frontend_api_module_surface_landed\":false"
      << ",\"fail_closed\":true"
      << ",\"deterministic\":"
      << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
      << ",\"ready_for_core_feature_implementation\":true"
      << ",\"next_issue\":\"objc3c.importmodule.runtimeaware.corefeature.v1\""
      << ",\"replay_key\":\""
      << EscapeJsonString(BuildRuntimeAwareImportModuleSurfaceReplayKey(
             program,
             parser_contract_snapshot,
             module_import_graph_lowering_contract))
      << "\"}";
  return out.str();
}

namespace {

std::size_t CountRuntimeAwareImportProtocolReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &class_record : records.classes_lexicographic) {
    total += class_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &protocol_record : records.protocols_lexicographic) {
    total += protocol_record.inherited_protocols_lexicographic.size();
  }
  for (const auto &category_record : records.categories_lexicographic) {
    total += category_record.adopted_protocols_lexicographic.size();
  }
  return total;
}

std::size_t CountRuntimeAwareImportPropertyAccessorReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      ++total;
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      ++total;
    }
  }
  return total;
}

std::size_t CountRuntimeAwareImportPropertyIvarBindingReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.ivar_binding_symbol.empty()) {
      ++total;
    }
  }
  return total;
}

}  // namespace

std::string BuildRuntimeAwareImportModuleFrontendClosureReplayKey(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_surface_contract_id=" << summary.source_surface_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";payload_model=" << summary.payload_model
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";authority_model=" << summary.authority_model
      << ";payload_ownership_model=" << summary.payload_ownership_model
      << ";module_name=" << summary.module_name
      << ";protocol_decl_count=" << summary.protocol_decl_count
      << ";interface_decl_count=" << summary.interface_decl_count
      << ";implementation_decl_count=" << summary.implementation_decl_count
      << ";interface_category_decl_count="
      << summary.interface_category_decl_count
      << ";implementation_category_decl_count="
      << summary.implementation_category_decl_count
      << ";function_decl_count=" << summary.function_decl_count
      << ";module_import_graph_sites=" << summary.module_import_graph_sites
      << ";import_edge_candidate_sites="
      << summary.import_edge_candidate_sites
      << ";namespace_segment_sites=" << summary.namespace_segment_sites
      << ";object_pointer_type_sites=" << summary.object_pointer_type_sites
      << ";pointer_declarator_sites=" << summary.pointer_declarator_sites
      << ";normalized_sites=" << summary.normalized_sites
      << ";contract_violation_sites=" << summary.contract_violation_sites
      << ";class_record_count=" << summary.class_record_count
      << ";protocol_record_count=" << summary.protocol_record_count
      << ";category_record_count=" << summary.category_record_count
      << ";property_record_count=" << summary.property_record_count
      << ";method_record_count=" << summary.method_record_count
      << ";ivar_record_count=" << summary.ivar_record_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";superclass_reference_count=" << summary.superclass_reference_count
      << ";protocol_reference_count=" << summary.protocol_reference_count
      << ";property_accessor_reference_count="
      << summary.property_accessor_reference_count
      << ";property_ivar_binding_reference_count="
      << summary.property_ivar_binding_reference_count
      << ";method_selector_reference_count="
      << summary.method_selector_reference_count
      << ";metadata_reference_count=" << summary.metadata_reference_count
      << ";source_surface_replay_key=" << summary.source_surface_replay_key;
  return out.str();
}

Objc3RuntimeAwareImportModuleFrontendClosureSummary
BuildRuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3RuntimeAwareImportModuleFrontendClosureSummary summary;
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      !program.module_name.empty() &&
      IsValidObjc3ModuleImportGraphLoweringContract(
          module_import_graph_lowering_contract);
  summary.runtime_metadata_source_records_ready =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(runtime_metadata_source_records);
  summary.frontend_surface_published = true;
  summary.import_artifact_template_published = true;
  summary.runtime_aware_import_declarations_landed = true;
  summary.module_metadata_import_surface_landed = true;
  summary.runtime_owned_declaration_import_landed = true;
  summary.runtime_metadata_reference_import_landed = true;
  summary.public_frontend_api_module_surface_landed = true;
  summary.module_name = program.module_name;
  summary.protocol_decl_count = parser_contract_snapshot.protocol_decl_count;
  summary.interface_decl_count = parser_contract_snapshot.interface_decl_count;
  summary.implementation_decl_count =
      parser_contract_snapshot.implementation_decl_count;
  summary.interface_category_decl_count =
      parser_contract_snapshot.interface_category_decl_count;
  summary.implementation_category_decl_count =
      parser_contract_snapshot.implementation_category_decl_count;
  summary.function_decl_count = parser_contract_snapshot.function_decl_count;
  summary.module_import_graph_sites =
      module_import_graph_lowering_contract.module_import_graph_sites;
  summary.import_edge_candidate_sites =
      module_import_graph_lowering_contract.import_edge_candidate_sites;
  summary.namespace_segment_sites =
      module_import_graph_lowering_contract.namespace_segment_sites;
  summary.object_pointer_type_sites =
      module_import_graph_lowering_contract.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      module_import_graph_lowering_contract.pointer_declarator_sites;
  summary.normalized_sites = module_import_graph_lowering_contract.normalized_sites;
  summary.contract_violation_sites =
      module_import_graph_lowering_contract.contract_violation_sites;
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
      ++summary.superclass_reference_count;
    }
  }
  summary.protocol_reference_count =
      CountRuntimeAwareImportProtocolReferences(runtime_metadata_source_records);
  summary.property_accessor_reference_count =
      CountRuntimeAwareImportPropertyAccessorReferences(
          runtime_metadata_source_records);
  summary.property_ivar_binding_reference_count =
      CountRuntimeAwareImportPropertyIvarBindingReferences(
          runtime_metadata_source_records);
  summary.method_selector_reference_count =
      runtime_metadata_source_records.methods_lexicographic.size();
  summary.metadata_reference_count =
      summary.superclass_reference_count + summary.protocol_reference_count +
      summary.property_accessor_reference_count +
      summary.property_ivar_binding_reference_count +
      summary.method_selector_reference_count;
  summary.ready_for_import_artifact_emission =
      summary.source_surface_contract_ready &&
      summary.runtime_metadata_source_records_ready;
  summary.ready_for_frontend_module_consumption =
      summary.ready_for_import_artifact_emission;
  summary.source_surface_replay_key =
      BuildRuntimeAwareImportModuleSurfaceReplayKey(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
  if (summary.ready_for_frontend_module_consumption) {
    summary.replay_key =
        BuildRuntimeAwareImportModuleFrontendClosureReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(summary)) {
    summary.failure_reason =
        "runtime-aware import/module frontend closure summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_surface_contract_id",
                     summary.source_surface_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("payload_model", summary.payload_model);
  object.StringField("artifact_relative_path", summary.artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("payload_ownership_model",
                     summary.payload_ownership_model);
  object.StringField("module_name", summary.module_name);
  object.SizeField("protocol_decl_count", summary.protocol_decl_count);
  object.SizeField("interface_decl_count", summary.interface_decl_count);
  object.SizeField("implementation_decl_count",
                   summary.implementation_decl_count);
  object.SizeField("interface_category_decl_count",
                   summary.interface_category_decl_count);
  object.SizeField("implementation_category_decl_count",
                   summary.implementation_category_decl_count);
  object.SizeField("function_decl_count", summary.function_decl_count);
  object.SizeField("module_import_graph_sites",
                   summary.module_import_graph_sites);
  object.SizeField("import_edge_candidate_sites",
                   summary.import_edge_candidate_sites);
  object.SizeField("namespace_segment_sites", summary.namespace_segment_sites);
  object.SizeField("object_pointer_type_sites",
                   summary.object_pointer_type_sites);
  object.SizeField("pointer_declarator_sites",
                   summary.pointer_declarator_sites);
  object.SizeField("normalized_sites", summary.normalized_sites);
  object.SizeField("contract_violation_sites",
                   summary.contract_violation_sites);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_reference_count",
                   summary.superclass_reference_count);
  object.SizeField("protocol_reference_count",
                   summary.protocol_reference_count);
  object.SizeField("property_accessor_reference_count",
                   summary.property_accessor_reference_count);
  object.SizeField("property_ivar_binding_reference_count",
                   summary.property_ivar_binding_reference_count);
  object.SizeField("method_selector_reference_count",
                   summary.method_selector_reference_count);
  object.SizeField("metadata_reference_count", summary.metadata_reference_count);
  object.BoolField("ready",
                   IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
                       summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_surface_contract_ready",
                   summary.source_surface_contract_ready);
  object.BoolField("runtime_metadata_source_records_ready",
                   summary.runtime_metadata_source_records_ready);
  object.BoolField("frontend_surface_published",
                   summary.frontend_surface_published);
  object.BoolField("import_artifact_template_published",
                   summary.import_artifact_template_published);
  object.BoolField("runtime_aware_import_declarations_landed",
                   summary.runtime_aware_import_declarations_landed);
  object.BoolField("module_metadata_import_surface_landed",
                   summary.module_metadata_import_surface_landed);
  object.BoolField("runtime_owned_declaration_import_landed",
                   summary.runtime_owned_declaration_import_landed);
  object.BoolField("runtime_metadata_reference_import_landed",
                   summary.runtime_metadata_reference_import_landed);
  object.BoolField("public_frontend_api_module_surface_landed",
                   summary.public_frontend_api_module_surface_landed);
  object.BoolField("ready_for_import_artifact_emission",
                   summary.ready_for_import_artifact_emission);
  object.BoolField("ready_for_frontend_module_consumption",
                   summary.ready_for_frontend_module_consumption);
  object.StringField("source_surface_replay_key",
                     summary.source_surface_replay_key);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

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

std::string BuildSerializedRuntimeMetadataImportLoweringReplayKey(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_imported_semantic_rules_contract_id
      << ";input_model=" << summary.input_model
      << ";imported_input_path_count=" << summary.imported_input_path_count
      << ";imported_module_count=" << summary.imported_module_count
      << ";imported_surface_ingest_landed="
      << (summary.imported_surface_ingest_landed ? "true" : "false")
      << ";serialized_metadata_rehydration_landed="
      << (summary.serialized_metadata_rehydration_landed ? "true" : "false")
      << ";incremental_reuse_landed="
      << (summary.incremental_reuse_landed ? "true" : "false")
      << ";imported_metadata_ir_lowering_landed="
      << (summary.imported_metadata_ir_lowering_landed ? "true" : "false")
      << ";public_live_imported_payload_abi_landed="
      << (summary.public_live_imported_payload_abi_landed ? "true" : "false");
  return out.str();
}

Objc3SerializedRuntimeMetadataImportLoweringSummary
BuildSerializedRuntimeMetadataImportLoweringSummary(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules) {
  Objc3SerializedRuntimeMetadataImportLoweringSummary summary;
  summary.imported_input_path_count =
      imported_runtime_metadata_semantic_rules.imported_input_path_count;
  summary.imported_module_count =
      imported_runtime_metadata_semantic_rules.imported_module_count;
  summary.fail_closed = true;
  summary.source_imported_semantic_rules_ready =
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          imported_runtime_metadata_semantic_rules);
  summary.semantic_surface_published = true;
  summary.imported_surface_ingest_landed =
      imported_runtime_metadata_semantic_rules
          .ready_for_imported_metadata_semantic_rules;
  if (summary.source_imported_semantic_rules_ready) {
    summary.replay_key =
        BuildSerializedRuntimeMetadataImportLoweringReplayKey(summary);
  } else {
    summary.failure_reason =
        "serialized runtime metadata import/lowering summary is incomplete";
  }
  if (!IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "serialized runtime metadata import/lowering summary is incomplete";
  }
  return summary;
}

std::string BuildSerializedRuntimeMetadataImportLoweringSummaryJson(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_imported_semantic_rules_contract_id",
                     summary.source_imported_semantic_rules_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("source_artifact_relative_path",
                     summary.source_artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.SizeField("imported_input_path_count",
                   summary.imported_input_path_count);
  object.SizeField("imported_module_count", summary.imported_module_count);
  object.BoolField(
      "ready",
      IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_imported_semantic_rules_ready",
                   summary.source_imported_semantic_rules_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_surface_ingest_landed",
                   summary.imported_surface_ingest_landed);
  object.BoolField("serialized_metadata_rehydration_landed",
                   summary.serialized_metadata_rehydration_landed);
  object.BoolField("incremental_reuse_landed",
                   summary.incremental_reuse_landed);
  object.BoolField("imported_metadata_ir_lowering_landed",
                   summary.imported_metadata_ir_lowering_landed);
  object.BoolField("public_live_imported_payload_abi_landed",
                   summary.public_live_imported_payload_abi_landed);
  object.BoolField("ready_for_serialized_metadata_lowering_impl",
                   summary.ready_for_serialized_metadata_lowering_impl);
  object.BoolField("ready_for_incremental_reuse_impl",
                   summary.ready_for_incremental_reuse_impl);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

std::string BuildSerializedRuntimeMetadataArtifactReuseReplayKey(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary.source_serialized_import_lowering_contract_id
      << ";artifact=" << summary.artifact_relative_path
      << ";reused_module_count=" << summary.reused_module_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";metadata_reference_count=" << summary.metadata_reference_count
      << ";modules=";
  for (std::size_t i = 0; i < summary.reused_module_names_lexicographic.size();
       ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << summary.reused_module_names_lexicographic[i];
  }
  return out.str();
}

Objc3SerializedRuntimeMetadataArtifactReuseSummary
BuildSerializedRuntimeMetadataArtifactReuseSummary(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary
        &serialized_import_lowering,
    const std::string &local_module_name,
    const Objc3RuntimeMetadataSourceRecordSet &reused_runtime_metadata_source_records,
    const std::vector<std::string> &reused_module_names_lexicographic) {
  Objc3SerializedRuntimeMetadataArtifactReuseSummary summary;
  summary.reused_module_names_lexicographic = reused_module_names_lexicographic;
  summary.reused_module_count = reused_module_names_lexicographic.size();
  summary.class_record_count =
      reused_runtime_metadata_source_records.classes_lexicographic.size();
  summary.protocol_record_count =
      reused_runtime_metadata_source_records.protocols_lexicographic.size();
  summary.category_record_count =
      reused_runtime_metadata_source_records.categories_lexicographic.size();
  summary.property_record_count =
      reused_runtime_metadata_source_records.properties_lexicographic.size();
  summary.method_record_count =
      reused_runtime_metadata_source_records.methods_lexicographic.size();
  summary.ivar_record_count =
      reused_runtime_metadata_source_records.ivars_lexicographic.size();
  summary.runtime_owned_declaration_count =
      CountRuntimeMetadataSourceRecordSetDeclarations(
          reused_runtime_metadata_source_records);
  summary.metadata_reference_count =
      CountRuntimeMetadataSourceRecordSetReferences(
          reused_runtime_metadata_source_records);
  summary.fail_closed = true;
  summary.source_serialized_import_lowering_ready =
      IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
          serialized_import_lowering);
  summary.semantic_surface_published = true;
  summary.serialized_metadata_rehydration_landed =
      summary.source_serialized_import_lowering_ready;
  summary.artifact_reuse_landed =
      summary.source_serialized_import_lowering_ready;
  summary.downstream_module_consumption_ready =
      summary.source_serialized_import_lowering_ready;
  if (summary.source_serialized_import_lowering_ready) {
    summary.replay_key =
        BuildSerializedRuntimeMetadataArtifactReuseReplayKey(summary);
  } else {
    summary.failure_reason =
        "serialized runtime metadata artifact reuse summary is incomplete";
  }
  if (!IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "serialized runtime metadata artifact reuse summary is incomplete";
  }
  (void)local_module_name;
  return summary;
}

std::string BuildSerializedRuntimeMetadataArtifactReuseSummaryJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_serialized_import_lowering_contract_id",
                     summary.source_serialized_import_lowering_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("artifact_relative_path", summary.artifact_relative_path);
  object.StringField("payload_member_name", summary.payload_member_name);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.StringArrayField("reused_module_names_lexicographic",
                          summary.reused_module_names_lexicographic);
  object.SizeField("reused_module_count", summary.reused_module_count);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("metadata_reference_count", summary.metadata_reference_count);
  object.BoolField(
      "ready",
      IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_serialized_import_lowering_ready",
                   summary.source_serialized_import_lowering_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("serialized_metadata_rehydration_landed",
                   summary.serialized_metadata_rehydration_landed);
  object.BoolField("artifact_reuse_landed", summary.artifact_reuse_landed);
  object.BoolField("downstream_module_consumption_ready",
                   summary.downstream_module_consumption_ready);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

std::string BuildCrossModuleBuildRuntimeOrchestrationReplayKey(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_contract="
      << summary
             .source_serialized_runtime_metadata_artifact_reuse_contract_id
      << ";local_manifest_contract="
      << summary.source_local_registration_manifest_contract_id
      << ";imported_runtime_semantic_rules_contract="
      << summary.source_imported_runtime_metadata_semantic_rules_contract_id
      << ";module_image_count=" << summary.module_image_count
      << ";direct_import_input_count=" << summary.direct_import_input_count
      << ";local_total_descriptor_count="
      << summary.local_total_descriptor_count
      << ";transitive_runtime_owned_declaration_count="
      << summary.transitive_runtime_owned_declaration_count
      << ";transitive_metadata_reference_count="
      << summary.transitive_metadata_reference_count
      << ";imported_optional_send_site_count="
      << summary.imported_optional_send_site_count
      << ";imported_typed_keypath_literal_site_count="
      << summary.imported_typed_keypath_literal_site_count << ";modules=";
  for (std::size_t i = 0; i < summary.module_names_lexicographic.size(); ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << summary.module_names_lexicographic[i];
  }
  return out.str();
}

Objc3CrossModuleBuildRuntimeOrchestrationSummary
BuildCrossModuleBuildRuntimeOrchestrationSummary(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &local_runtime_registration_manifest,
    std::size_t direct_import_input_count) {
  Objc3CrossModuleBuildRuntimeOrchestrationSummary summary;
  summary.module_names_lexicographic =
      serialized_runtime_metadata_artifact_reuse
          .reused_module_names_lexicographic;
  summary.module_image_count = summary.module_names_lexicographic.size();
  summary.direct_import_input_count = direct_import_input_count;
  summary.local_class_descriptor_count =
      local_runtime_registration_manifest.class_descriptor_count;
  summary.local_protocol_descriptor_count =
      local_runtime_registration_manifest.protocol_descriptor_count;
  summary.local_category_descriptor_count =
      local_runtime_registration_manifest.category_descriptor_count;
  summary.local_property_descriptor_count =
      local_runtime_registration_manifest.property_descriptor_count;
  summary.local_ivar_descriptor_count =
      local_runtime_registration_manifest.ivar_descriptor_count;
  summary.local_total_descriptor_count =
      local_runtime_registration_manifest.total_descriptor_count;
  summary.transitive_runtime_owned_declaration_count =
      serialized_runtime_metadata_artifact_reuse
          .runtime_owned_declaration_count;
  summary.transitive_metadata_reference_count =
      serialized_runtime_metadata_artifact_reuse.metadata_reference_count;
  summary.imported_optional_send_site_count =
      imported_runtime_metadata_semantic_rules.optional_send_site_count;
  summary.imported_typed_keypath_literal_site_count =
      imported_runtime_metadata_semantic_rules.typed_keypath_literal_site_count;
  summary.imported_live_optional_lowering_site_count =
      imported_runtime_metadata_semantic_rules.live_optional_lowering_site_count;
  summary.imported_live_typed_keypath_artifact_site_count =
      imported_runtime_metadata_semantic_rules
          .live_typed_keypath_artifact_site_count;
  summary.fail_closed = true;
  summary.source_serialized_runtime_metadata_artifact_reuse_ready =
      IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
          serialized_runtime_metadata_artifact_reuse);
  summary.source_imported_runtime_metadata_semantic_rules_ready =
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          imported_runtime_metadata_semantic_rules);
  summary.source_local_registration_manifest_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          local_runtime_registration_manifest);
  summary.semantic_surface_published = true;
  summary.local_registration_manifest_emitted =
      local_runtime_registration_manifest
          .runtime_registration_artifact_emitted_by_driver;
  summary.imported_type_system_type_surface_landed =
      imported_runtime_metadata_semantic_rules.imported_type_system_type_surface_landed;
  summary.imported_optional_runtime_semantics_landed =
      imported_runtime_metadata_semantic_rules
          .imported_optional_runtime_semantics_landed;
  summary.imported_typed_keypath_runtime_semantics_landed =
      imported_runtime_metadata_semantic_rules
          .imported_typed_keypath_runtime_semantics_landed;
  summary.source_serialized_runtime_metadata_replay_key =
      serialized_runtime_metadata_artifact_reuse.replay_key;
  summary.source_local_registration_manifest_replay_key =
      local_runtime_registration_manifest.replay_key;
  summary.source_imported_runtime_metadata_semantic_rules_replay_key =
      imported_runtime_metadata_semantic_rules.replay_key;
  if (summary.source_serialized_runtime_metadata_artifact_reuse_ready &&
      summary.source_imported_runtime_metadata_semantic_rules_ready &&
      summary.source_local_registration_manifest_ready) {
    summary.replay_key =
        BuildCrossModuleBuildRuntimeOrchestrationReplayKey(summary);
  } else {
    summary.failure_reason =
        "cross-module build/runtime orchestration contract is incomplete";
  }
  if (!IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "cross-module build/runtime orchestration contract is incomplete";
  }
  return summary;
}

std::string BuildCrossModuleBuildRuntimeOrchestrationSummaryJson(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_serialized_runtime_metadata_artifact_reuse_contract_id\":\""
      << EscapeJsonString(
             summary
                 .source_serialized_runtime_metadata_artifact_reuse_contract_id)
      << "\",\"source_local_registration_manifest_contract_id\":\""
      << EscapeJsonString(
             summary.source_local_registration_manifest_contract_id)
      << "\",\"source_imported_runtime_metadata_semantic_rules_contract_id\":\""
      << EscapeJsonString(
             summary.source_imported_runtime_metadata_semantic_rules_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"import_artifact_relative_path\":\""
      << EscapeJsonString(summary.import_artifact_relative_path)
      << "\",\"local_registration_manifest_artifact_relative_path\":\""
      << EscapeJsonString(
             summary.local_registration_manifest_artifact_relative_path)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"input_model\":\"" << EscapeJsonString(summary.input_model)
      << "\",\"registration_scope_model\":\""
      << EscapeJsonString(summary.registration_scope_model)
      << "\",\"packaging_model\":\""
      << EscapeJsonString(summary.packaging_model)
      << "\",\"module_names_lexicographic\":"
      << BuildStringArrayJson(summary.module_names_lexicographic)
      << ",\"module_image_count\":" << summary.module_image_count
      << ",\"direct_import_input_count\":" << summary.direct_import_input_count
      << ",\"local_class_descriptor_count\":"
      << summary.local_class_descriptor_count
      << ",\"local_protocol_descriptor_count\":"
      << summary.local_protocol_descriptor_count
      << ",\"local_category_descriptor_count\":"
      << summary.local_category_descriptor_count
      << ",\"local_property_descriptor_count\":"
      << summary.local_property_descriptor_count
      << ",\"local_ivar_descriptor_count\":"
      << summary.local_ivar_descriptor_count
      << ",\"local_total_descriptor_count\":"
      << summary.local_total_descriptor_count
      << ",\"transitive_runtime_owned_declaration_count\":"
      << summary.transitive_runtime_owned_declaration_count
      << ",\"transitive_metadata_reference_count\":"
      << summary.transitive_metadata_reference_count
      << ",\"imported_optional_send_site_count\":"
      << summary.imported_optional_send_site_count
      << ",\"imported_typed_keypath_literal_site_count\":"
      << summary.imported_typed_keypath_literal_site_count
      << ",\"imported_live_optional_lowering_site_count\":"
      << summary.imported_live_optional_lowering_site_count
      << ",\"imported_live_typed_keypath_artifact_site_count\":"
      << summary.imported_live_typed_keypath_artifact_site_count
      << ",\"ready\":"
      << (IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_serialized_runtime_metadata_artifact_reuse_ready\":"
      << (summary.source_serialized_runtime_metadata_artifact_reuse_ready
              ? "true"
              : "false")
      << ",\"source_local_registration_manifest_ready\":"
      << (summary.source_local_registration_manifest_ready ? "true" : "false")
      << ",\"source_imported_runtime_metadata_semantic_rules_ready\":"
      << (summary.source_imported_runtime_metadata_semantic_rules_ready ? "true"
                                                                        : "false")
      << ",\"semantic_surface_published\":"
      << (summary.semantic_surface_published ? "true" : "false")
      << ",\"local_registration_manifest_emitted\":"
      << (summary.local_registration_manifest_emitted ? "true" : "false")
      << ",\"imported_type_system_type_surface_landed\":"
      << (summary.imported_type_system_type_surface_landed ? "true" : "false")
      << ",\"imported_optional_runtime_semantics_landed\":"
      << (summary.imported_optional_runtime_semantics_landed ? "true"
                                                             : "false")
      << ",\"imported_typed_keypath_runtime_semantics_landed\":"
      << (summary.imported_typed_keypath_runtime_semantics_landed ? "true"
                                                                  : "false")
      << ",\"cross_module_link_plan_artifact_landed\":"
      << (summary.cross_module_link_plan_artifact_landed ? "true" : "false")
      << ",\"imported_registration_manifest_loading_landed\":"
      << (summary.imported_registration_manifest_loading_landed ? "true"
                                                                : "false")
      << ",\"runtime_archive_aggregation_landed\":"
      << (summary.runtime_archive_aggregation_landed ? "true" : "false")
      << ",\"cross_module_runtime_registration_landed\":"
      << (summary.cross_module_runtime_registration_landed ? "true" : "false")
      << ",\"cross_module_launch_orchestration_landed\":"
      << (summary.cross_module_launch_orchestration_landed ? "true"
                                                           : "false")
      << ",\"public_cross_module_orchestration_abi_landed\":"
      << (summary.public_cross_module_orchestration_abi_landed ? "true"
                                                               : "false")
      << ",\"ready_for_packaging_and_runtime_registration_impl\":"
      << (summary.ready_for_packaging_and_runtime_registration_impl ? "true"
                                                                   : "false")
      << ",\"source_serialized_runtime_metadata_replay_key\":\""
      << EscapeJsonString(
             summary.source_serialized_runtime_metadata_replay_key)
      << "\",\"source_local_registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.source_local_registration_manifest_replay_key)
      << "\",\"source_imported_runtime_metadata_semantic_rules_replay_key\":\""
      << EscapeJsonString(
             summary.source_imported_runtime_metadata_semantic_rules_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string RenderRuntimeOwnedDeclarationsJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"classes\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.classes_lexicographic.size(); ++i) {
    const auto &class_record =
        runtime_metadata_source_records.classes_lexicographic[i];
    out << "      {\"record_kind\":\""
        << EscapeJsonString(class_record.record_kind)
        << "\",\"name\":\"" << EscapeJsonString(class_record.name)
        << "\",\"super_name\":\"" << EscapeJsonString(class_record.super_name)
        << "\",\"has_super\":" << (class_record.has_super ? "true" : "false")
        << ",\"objc_final_declared\":"
        << (class_record.objc_final_declared ? "true" : "false")
        << ",\"objc_sealed_declared\":"
        << (class_record.objc_sealed_declared ? "true" : "false")
        << ",\"adopted_protocols\":"
        << BuildStringArrayJson(class_record.adopted_protocols_lexicographic)
        << ",\"property_count\":" << class_record.property_count
        << ",\"method_count\":" << class_record.method_count
        << ",\"line\":" << class_record.line
        << ",\"column\":" << class_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.classes_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"protocols\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.protocols_lexicographic.size();
       ++i) {
    const auto &protocol_record =
        runtime_metadata_source_records.protocols_lexicographic[i];
    out << "      {\"name\":\"" << EscapeJsonString(protocol_record.name)
        << "\",\"inherited_protocols\":"
        << BuildStringArrayJson(
               protocol_record.inherited_protocols_lexicographic)
        << ",\"is_forward_declaration\":"
        << (protocol_record.is_forward_declaration ? "true" : "false")
        << ",\"property_count\":" << protocol_record.property_count
        << ",\"method_count\":" << protocol_record.method_count
        << ",\"line\":" << protocol_record.line
        << ",\"column\":" << protocol_record.column << "}";
    if (i + 1 !=
        runtime_metadata_source_records.protocols_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"categories\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.categories_lexicographic.size();
       ++i) {
    const auto &category_record =
        runtime_metadata_source_records.categories_lexicographic[i];
    out << "      {\"record_kind\":\""
        << EscapeJsonString(category_record.record_kind)
        << "\",\"class_name\":\""
        << EscapeJsonString(category_record.class_name)
        << "\",\"category_name\":\""
        << EscapeJsonString(category_record.category_name)
        << "\",\"adopted_protocols\":"
        << BuildStringArrayJson(category_record.adopted_protocols_lexicographic)
        << ",\"property_count\":" << category_record.property_count
        << ",\"method_count\":" << category_record.method_count
        << ",\"line\":" << category_record.line
        << ",\"column\":" << category_record.column << "}";
    if (i + 1 !=
        runtime_metadata_source_records.categories_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"properties\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.properties_lexicographic.size();
       ++i) {
    const auto &property_record =
        runtime_metadata_source_records.properties_lexicographic[i];
    out << "      {\"owner_kind\":\""
        << EscapeJsonString(property_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(property_record.owner_name)
        << "\",\"property_name\":\""
        << EscapeJsonString(property_record.property_name)
        << "\",\"type\":\"" << EscapeJsonString(property_record.type_name)
        << "\",\"effective_getter_selector\":\""
        << EscapeJsonString(property_record.effective_getter_selector)
        << "\",\"effective_setter_available\":"
        << (property_record.effective_setter_available ? "true" : "false")
        << ",\"effective_setter_selector\":\""
        << EscapeJsonString(property_record.effective_setter_selector)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(property_record.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(property_record.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(
               property_record.executable_synthesized_binding_symbol)
        << "\",\"property_attribute_profile\":\""
        << EscapeJsonString(property_record.property_attribute_profile)
        << "\",\"ownership_lifetime_profile\":\""
        << EscapeJsonString(property_record.ownership_lifetime_profile)
        << "\",\"ownership_runtime_hook_profile\":\""
        << EscapeJsonString(property_record.ownership_runtime_hook_profile)
        << "\",\"accessor_ownership_profile\":\""
        << EscapeJsonString(property_record.accessor_ownership_profile)
        << "\",\"synthesizes_executable_accessors\":"
        << (property_record.synthesizes_executable_accessors ? "true" : "false")
        << ",\"getter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(property_record.getter_storage_runtime_helper_symbol)
        << "\",\"setter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(property_record.setter_storage_runtime_helper_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(property_record.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << property_record.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << property_record.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << property_record.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_layout_offset_bytes\":"
        << property_record.executable_ivar_layout_offset_bytes
        << ",\"executable_ivar_layout_padding_bytes\":"
        << property_record.executable_ivar_layout_padding_bytes
        << ",\"executable_ivar_layout_inherited_slot_count\":"
        << property_record.executable_ivar_layout_inherited_slot_count
        << ",\"executable_ivar_layout_inherited_size_bytes\":"
        << property_record.executable_ivar_layout_inherited_size_bytes
        << ",\"executable_ivar_layout_owner_size_bytes\":"
        << property_record.executable_ivar_layout_owner_size_bytes
        << ",\"executable_ivar_init_order_index\":"
        << property_record.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << property_record.executable_ivar_destroy_order_index
        << ",\"executable_ivar_layout_valid\":"
        << (property_record.executable_ivar_layout_valid ? "true" : "false")
        << ",\"executable_ivar_layout_replay_key\":\""
        << EscapeJsonString(property_record.executable_ivar_layout_replay_key)
        << "\""
        << ",\"line\":" << property_record.line
        << ",\"column\":" << property_record.column << "}";
    if (i + 1 !=
        runtime_metadata_source_records.properties_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"methods\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.methods_lexicographic.size(); ++i) {
    const auto &method_record =
        runtime_metadata_source_records.methods_lexicographic[i];
    out << "      {\"owner_kind\":\""
        << EscapeJsonString(method_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(method_record.owner_name)
        << "\",\"selector\":\"" << EscapeJsonString(method_record.selector)
        << "\",\"is_class_method\":"
        << (method_record.is_class_method ? "true" : "false")
        << ",\"has_body\":" << (method_record.has_body ? "true" : "false")
        << ",\"effective_direct_dispatch\":"
        << (method_record.effective_direct_dispatch ? "true" : "false")
        << ",\"objc_final_declared\":"
        << (method_record.objc_final_declared ? "true" : "false")
        << ",\"parameter_count\":" << method_record.parameter_count
        << ",\"return_type\":\""
        << EscapeJsonString(method_record.return_type_name)
        << "\",\"line\":" << method_record.line
        << ",\"column\":" << method_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.methods_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"ivars\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.ivars_lexicographic.size(); ++i) {
    const auto &ivar_record =
        runtime_metadata_source_records.ivars_lexicographic[i];
    out << "      {\"owner_kind\":\""
        << EscapeJsonString(ivar_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(ivar_record.owner_name)
        << "\",\"property_name\":\""
        << EscapeJsonString(ivar_record.property_name)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(ivar_record.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(ivar_record.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(ivar_record.executable_synthesized_binding_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(ivar_record.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << ivar_record.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << ivar_record.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << ivar_record.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_layout_offset_bytes\":"
        << ivar_record.executable_ivar_layout_offset_bytes
        << ",\"executable_ivar_layout_padding_bytes\":"
        << ivar_record.executable_ivar_layout_padding_bytes
        << ",\"executable_ivar_layout_inherited_slot_count\":"
        << ivar_record.executable_ivar_layout_inherited_slot_count
        << ",\"executable_ivar_layout_inherited_size_bytes\":"
        << ivar_record.executable_ivar_layout_inherited_size_bytes
        << ",\"executable_ivar_layout_owner_size_bytes\":"
        << ivar_record.executable_ivar_layout_owner_size_bytes
        << ",\"executable_ivar_init_order_index\":"
        << ivar_record.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << ivar_record.executable_ivar_destroy_order_index
        << ",\"executable_ivar_layout_valid\":"
        << (ivar_record.executable_ivar_layout_valid ? "true" : "false")
        << ",\"executable_ivar_layout_replay_key\":\""
        << EscapeJsonString(ivar_record.executable_ivar_layout_replay_key)
        << "\""
        << ",\"source_model\":\"" << EscapeJsonString(ivar_record.source_model)
        << "\",\"line\":" << ivar_record.line
        << ",\"column\":" << ivar_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.ivars_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ]\n"
      << "  }";
  return out.str();
}

std::string RenderRuntimeMetadataReferencesJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "[\n";
  bool first_reference = true;
  auto emit_reference = [&](const std::string &reference_kind,
                            const std::string &owner_kind,
                            const std::string &owner_name,
                            const std::string &target_kind,
                            const std::string &target_name, unsigned line,
                            unsigned column) {
    if (!first_reference) {
      out << ",\n";
    }
    first_reference = false;
    out << "    {\"reference_kind\":\"" << EscapeJsonString(reference_kind)
        << "\",\"owner_kind\":\"" << EscapeJsonString(owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(owner_name)
        << "\",\"target_kind\":\"" << EscapeJsonString(target_kind)
        << "\",\"target_name\":\"" << EscapeJsonString(target_name)
        << "\",\"line\":" << line << ",\"column\":" << column << "}";
  };
  for (const auto &class_record :
       runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      emit_reference("class-superclass", class_record.record_kind,
                     class_record.name, "class", class_record.super_name,
                     class_record.line, class_record.column);
    }
    for (const auto &protocol_name :
         class_record.adopted_protocols_lexicographic) {
      emit_reference("class-adopted-protocol", class_record.record_kind,
                     class_record.name, "protocol", protocol_name,
                     class_record.line, class_record.column);
    }
  }
  for (const auto &protocol_record :
       runtime_metadata_source_records.protocols_lexicographic) {
    for (const auto &inherited_name :
         protocol_record.inherited_protocols_lexicographic) {
      emit_reference("protocol-inherited-protocol", "protocol",
                     protocol_record.name, "protocol", inherited_name,
                     protocol_record.line, protocol_record.column);
    }
  }
  for (const auto &category_record :
       runtime_metadata_source_records.categories_lexicographic) {
    for (const auto &protocol_name :
         category_record.adopted_protocols_lexicographic) {
      emit_reference("category-adopted-protocol", category_record.record_kind,
                     category_record.category_name, "protocol", protocol_name,
                     category_record.line, category_record.column);
    }
  }
  for (const auto &property_record :
       runtime_metadata_source_records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      emit_reference("property-getter-selector", property_record.owner_kind,
                     property_record.owner_name, "selector",
                     property_record.effective_getter_selector,
                     property_record.line, property_record.column);
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      emit_reference("property-setter-selector", property_record.owner_kind,
                     property_record.owner_name, "selector",
                     property_record.effective_setter_selector,
                     property_record.line, property_record.column);
    }
    if (!property_record.ivar_binding_symbol.empty()) {
      emit_reference("property-ivar-binding", property_record.owner_kind,
                     property_record.owner_name, "ivar-binding-symbol",
                     property_record.ivar_binding_symbol, property_record.line,
                     property_record.column);
    }
  }
  for (const auto &method_record :
       runtime_metadata_source_records.methods_lexicographic) {
    emit_reference("method-selector", method_record.owner_kind,
                   method_record.owner_name, "selector", method_record.selector,
                   method_record.line, method_record.column);
  }
  if (!first_reference) {
    out << "\n";
  }
  out << "  ]";
  return out.str();
}

std::string RenderSerializedRuntimeMetadataReusePayloadJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary,
    const std::string &module_name,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "    \"module_name\": \"" << EscapeJsonString(module_name) << "\",\n"
      << "    \"reused_module_names_lexicographic\": "
      << BuildStringArrayJson(summary.reused_module_names_lexicographic)
      << ",\n"
      << "    \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "    \"metadata_reference_count\": " << summary.metadata_reference_count
      << ",\n"
      << "    \"runtime_owned_declarations\": "
      << RenderRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"metadata_references\": "
      << RenderRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"ready\": "
      << (IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary)
              ? "true"
              : "false")
      << ",\n"
      << "    \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "  }";
  return out.str();
}

std::string RenderRuntimeAwareImportModuleArtifactJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const std::string &type_system_optional_keypath_lowering_contract_json,
    const std::string &type_system_optional_keypath_runtime_helper_contract_json,
    const std::string &type_system_generic_contract_preservation_json,
    const std::string &type_system_nullability_contract_preservation_json,
    const std::string &type_system_protocol_contract_preservation_json,
    const std::string &error_handling_result_and_bridging_artifact_replay_json,
    const std::string &concurrency_actor_mailbox_runtime_import_json,
    const std::string &interop_foreign_surface_interface_preservation_json,
    const std::string &interop_header_module_bridge_generation_json,
    const std::string &interop_ffi_metadata_interface_preservation_json,
    const std::string &metaprogramming_module_interface_replay_preservation_json,
    const std::string
        &metaprogramming_macro_host_process_cache_runtime_integration_json,
    const std::string &dispatch_dispatch_metadata_interface_preservation_json,
    const std::string &runtime_block_ownership_artifact_preservation_json,
    const std::string &runtime_storage_reflection_artifact_preservation_json,
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3RuntimeMetadataSourceRecordSet
        &serialized_runtime_metadata_reuse_records) {
  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "  \"source_surface_contract_id\": \""
      << EscapeJsonString(summary.source_surface_contract_id) << "\",\n"
      << "  \"frontend_surface_path\": \""
      << EscapeJsonString(summary.frontend_surface_path) << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(summary.artifact_relative_path)
      << "\",\n"
      << "  \"payload_model\": \"" << EscapeJsonString(summary.payload_model)
      << "\",\n"
      << "  \"authority_model\": \"" << EscapeJsonString(summary.authority_model)
      << "\",\n"
      << "  \"payload_ownership_model\": \""
      << EscapeJsonString(summary.payload_ownership_model) << "\",\n"
      << "  \"module_name\": \"" << EscapeJsonString(summary.module_name)
      << "\",\n"
      << "  \"protocol_decl_count\": " << summary.protocol_decl_count << ",\n"
      << "  \"interface_decl_count\": " << summary.interface_decl_count
      << ",\n"
      << "  \"implementation_decl_count\": " << summary.implementation_decl_count
      << ",\n"
      << "  \"interface_category_decl_count\": "
      << summary.interface_category_decl_count << ",\n"
      << "  \"implementation_category_decl_count\": "
      << summary.implementation_category_decl_count << ",\n"
      << "  \"function_decl_count\": " << summary.function_decl_count << ",\n"
      << "  \"module_import_graph_sites\": " << summary.module_import_graph_sites
      << ",\n"
      << "  \"import_edge_candidate_sites\": "
      << summary.import_edge_candidate_sites << ",\n"
      << "  \"namespace_segment_sites\": " << summary.namespace_segment_sites
      << ",\n"
      << "  \"object_pointer_type_sites\": " << summary.object_pointer_type_sites
      << ",\n"
      << "  \"pointer_declarator_sites\": " << summary.pointer_declarator_sites
      << ",\n"
      << "  \"normalized_sites\": " << summary.normalized_sites << ",\n"
      << "  \"contract_violation_sites\": " << summary.contract_violation_sites
      << ",\n"
      << "  \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "  \"metadata_reference_count\": " << summary.metadata_reference_count
      << ",\n"
      << "  \"runtime_aware_import_declarations_landed\": "
      << (summary.runtime_aware_import_declarations_landed ? "true" : "false")
      << ",\n"
      << "  \"module_metadata_import_surface_landed\": "
      << (summary.module_metadata_import_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declaration_import_landed\": "
      << (summary.runtime_owned_declaration_import_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_metadata_reference_import_landed\": "
      << (summary.runtime_metadata_reference_import_landed ? "true" : "false")
      << ",\n"
      << "  \"public_frontend_api_module_surface_landed\": "
      << (summary.public_frontend_api_module_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"ready_for_import_artifact_emission\": "
      << (summary.ready_for_import_artifact_emission ? "true" : "false")
      << ",\n"
      << "  \"ready_for_frontend_module_consumption\": "
      << (summary.ready_for_frontend_module_consumption ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declarations\": "
      << RenderRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"metadata_references\": "
      << RenderRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"objc_type_system_optional_keypath_lowering_contract\": "
      << type_system_optional_keypath_lowering_contract_json << ",\n"
      << "  \"objc_type_system_optional_keypath_runtime_helper_contract\": "
      << type_system_optional_keypath_runtime_helper_contract_json << ",\n"
      << "  \"objc_type_system_generic_contract_preservation\": "
      << type_system_generic_contract_preservation_json << ",\n"
      << "  \"objc_type_system_nullability_contract_preservation\": "
      << type_system_nullability_contract_preservation_json << ",\n"
      << "  \"objc_type_system_protocol_contract_preservation\": "
      << type_system_protocol_contract_preservation_json << ",\n"
      << "  \"objc_error_handling_result_and_bridging_artifact_replay\": "
      << error_handling_result_and_bridging_artifact_replay_json << ",\n"
      << "  \"objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface\": "
      << concurrency_actor_mailbox_runtime_import_json << ",\n"
      << "  \"objc_interop_foreign_surface_interface_and_module_preservation\": "
      << interop_foreign_surface_interface_preservation_json << ",\n"
      << "  \"objc_interop_header_module_and_bridge_generation\": "
      << interop_header_module_bridge_generation_json << ",\n"
      << "  \"objc_interop_ffi_metadata_and_interface_preservation\": "
      << interop_ffi_metadata_interface_preservation_json << ",\n"
      << "  \"objc_metaprogramming_module_interface_and_replay_preservation\": "
      << metaprogramming_module_interface_replay_preservation_json << ",\n"
      << "  \"objc_metaprogramming_macro_host_process_and_cache_runtime_integration\": "
      << metaprogramming_macro_host_process_cache_runtime_integration_json
      << ",\n"
      << "  \"objc_dispatch_dispatch_metadata_and_interface_preservation\": "
      << dispatch_dispatch_metadata_interface_preservation_json << ",\n"
      << "  \"objc_runtime_block_ownership_artifact_preservation\": "
      << runtime_block_ownership_artifact_preservation_json << ",\n"
      << "  \"objc_runtime_storage_reflection_artifact_preservation\": "
      << runtime_storage_reflection_artifact_preservation_json << ",\n"
      << "  \""
      << kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName
      << "\": "
      << RenderSerializedRuntimeMetadataReusePayloadJson(
             serialized_runtime_metadata_artifact_reuse, summary.module_name,
             serialized_runtime_metadata_reuse_records)
      << ",\n"
      << "  \"source_surface_replay_key\": \""
      << EscapeJsonString(summary.source_surface_replay_key) << "\",\n"
      << "  \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "}\n";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
