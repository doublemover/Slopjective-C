#include "artifacts/objc3_runtime_import_preservation_artifact_builders.h"

#include <cstddef>
#include <sstream>

#include "io/objc3_json.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend::runtime_import_preservation {
namespace {

using objc3::io::EscapeJsonString;

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

std::size_t CountProtocolReferences(
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

std::size_t CountPropertyAccessorReferences(
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

std::size_t CountPropertyIvarBindingReferences(
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

std::string RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildReplayKey(
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

std::string RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildSummaryJson(
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
      << EscapeJsonString(BuildReplayKey(program,
                                         parser_contract_snapshot,
                                         module_import_graph_lowering_contract))
      << "\"}";
  return out.str();
}

std::string
RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildReplayKey(
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
RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildSummary(
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
      CountProtocolReferences(runtime_metadata_source_records);
  summary.property_accessor_reference_count =
      CountPropertyAccessorReferences(runtime_metadata_source_records);
  summary.property_ivar_binding_reference_count =
      CountPropertyIvarBindingReferences(runtime_metadata_source_records);
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
      RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildReplayKey(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
  if (summary.ready_for_frontend_module_consumption) {
    summary.replay_key = BuildReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(summary)) {
    summary.failure_reason =
        "runtime-aware import/module frontend closure summary is incomplete";
  }
  return summary;
}

}  // namespace objc3::artifacts::frontend::runtime_import_preservation
