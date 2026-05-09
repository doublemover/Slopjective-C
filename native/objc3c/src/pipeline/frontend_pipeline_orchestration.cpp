#include "pipeline/objc3_frontend_pipeline.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "lex/objc3_lexer.h"
#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_ast_builder_contract.h"
#include "parse/objc3_parse_support.h"
#include "pipeline/dispatch_surface_classification.h"
#include "pipeline/frontend_concurrency_source_closure_helpers.h"
#include "pipeline/frontend_control_flow_source_closure_helpers.h"
#include "pipeline/frontend_dispatch_source_completion_helpers.h"
#include "pipeline/frontend_error_handling_source_closure_helpers.h"
#include "pipeline/frontend_executable_metadata_handoff.h"
#include "pipeline/frontend_interop_source_closure_helpers.h"
#include "pipeline/frontend_interop_source_completion_helpers.h"
#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "pipeline/frontend_ownership_retainable_c_family_completion_helpers.h"
#include "pipeline/frontend_ownership_source_completion_helpers.h"
#include "pipeline/frontend_ownership_source_closure_helpers.h"
#include "pipeline/frontend_phase_publication_helpers.h"
#include "pipeline/frontend_pipeline_result_handoff.h"
#include "pipeline/frontend_pipeline_sema_stage_runner.h"
#include "pipeline/frontend_pipeline_stage_sequence.h"
#include "pipeline/frontend_pipeline_stage_runner.h"
#include "pipeline/frontend_semantic_metadata_summary_helpers.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "pipeline/frontend_type_system_source_closure_helpers.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_surface.h"
#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface.h"
#include "sema/objc3_semantic_passes.h"
#include "pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"
#include "sema/objc3_sema_pass_manager.h"
#include "support/objc3_property_storage_profile_helpers.h"

using objc3c::parse::support::MakeDiag;

namespace {

Objc3RuntimeMetadataSourceRecordSet BuildRuntimeMetadataSourceRecordSet(
    const Objc3Program &program) {
  Objc3RuntimeMetadataSourceRecordSet records;
  std::unordered_set<std::string> class_implementation_names;
  class_implementation_names.reserve(program.implementations.size());
  std::unordered_set<std::string> implementation_property_keys;
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      class_implementation_names.insert(implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }

  const auto apply_arc_property_interaction_metadata =
      [](Objc3RuntimeMetadataPropertySourceRecord &property_record) {
        objc3c::support::ApplyPropertyOwnershipProfileFallback(
            property_record, false);
        objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(
            property_record);
      };

  const auto append_property_records =
      [&records, &apply_arc_property_interaction_metadata,
       &class_implementation_names, &implementation_property_keys](
          const auto &properties, const std::string &owner_kind,
          const std::string &owner_name) {
        for (const auto &property : properties) {
          Objc3RuntimeMetadataPropertySourceRecord property_record;
          property_record.owner_kind = owner_kind;
          property_record.owner_name = owner_name;
          property_record.property_name = property.name;
          property_record.type_name = RuntimeMetadataTypeName(property.type);
          property_record.has_getter = property.has_getter;
          property_record.getter_selector = property.getter_selector;
          property_record.has_setter = property.has_setter;
          property_record.setter_selector = property.setter_selector;
          property_record.ivar_binding_symbol = property.ivar_binding_symbol;
          property_record.executable_synthesized_binding_kind =
              property.executable_synthesized_binding_kind;
          property_record.executable_synthesized_binding_symbol =
              property.executable_synthesized_binding_symbol;
          property_record.property_attribute_profile =
              property.property_attribute_profile;
          property_record.ownership_lifetime_profile =
              property.ownership_lifetime_profile;
          property_record.ownership_runtime_hook_profile =
              property.ownership_runtime_hook_profile;
          property_record.effective_getter_selector =
              property.effective_getter_selector;
          property_record.effective_setter_available =
              property.effective_setter_available;
          property_record.effective_setter_selector =
              property.effective_setter_selector;
          property_record.accessor_ownership_profile =
              property.accessor_ownership_profile;
          apply_arc_property_interaction_metadata(property_record);
          property_record.synthesizes_executable_accessors =
              ShouldSynthesizeExecutablePropertyAccessors(
                  owner_kind, owner_name, property.name,
                  class_implementation_names, implementation_property_keys);
          property_record.getter_storage_runtime_helper_symbol =
              BuildGetterStorageRuntimeHelperSymbol(
                  property_record.synthesizes_executable_accessors,
                  property_record.ownership_runtime_hook_profile);
          property_record.setter_storage_runtime_helper_symbol =
              BuildSetterStorageRuntimeHelperSymbol(
                  property_record.synthesizes_executable_accessors,
                  property_record.effective_setter_available,
                  property_record.ownership_lifetime_profile,
                  property_record.ownership_runtime_hook_profile,
                  property_record.accessor_ownership_profile);
          property_record.executable_ivar_layout_symbol =
              property.executable_ivar_layout_symbol;
          property_record.executable_ivar_layout_slot_index =
              property.executable_ivar_layout_slot_index;
          property_record.executable_ivar_layout_size_bytes =
              property.executable_ivar_layout_size_bytes;
          property_record.executable_ivar_layout_alignment_bytes =
              property.executable_ivar_layout_alignment_bytes;
          property_record.executable_ivar_layout_offset_bytes =
              property.executable_ivar_layout_offset_bytes;
          property_record.executable_ivar_layout_padding_bytes =
              property.executable_ivar_layout_padding_bytes;
          property_record.executable_ivar_layout_inherited_slot_count =
              property.executable_ivar_layout_inherited_slot_count;
          property_record.executable_ivar_layout_inherited_size_bytes =
              property.executable_ivar_layout_inherited_size_bytes;
          property_record.executable_ivar_layout_owner_size_bytes =
              property.executable_ivar_layout_owner_size_bytes;
          property_record.executable_ivar_init_order_index =
              property.executable_ivar_init_order_index;
          property_record.executable_ivar_destroy_order_index =
              property.executable_ivar_destroy_order_index;
          property_record.executable_ivar_layout_valid =
              property.executable_ivar_layout_valid;
          property_record.executable_ivar_layout_replay_key =
              property.executable_ivar_layout_replay_key;
          property_record.line = property.line;
          property_record.column = property.column;
          records.properties_lexicographic.push_back(std::move(property_record));

          if (!property.ivar_binding_symbol.empty()) {
            Objc3RuntimeMetadataIvarSourceRecord ivar_record;
            ivar_record.owner_kind = owner_kind;
            ivar_record.owner_name = owner_name;
            ivar_record.property_name = property.name;
            ivar_record.ivar_binding_symbol = property.ivar_binding_symbol;
            ivar_record.executable_synthesized_binding_kind =
                property.executable_synthesized_binding_kind;
            ivar_record.executable_synthesized_binding_symbol =
                property.executable_synthesized_binding_symbol;
            ivar_record.executable_ivar_layout_symbol =
                property.executable_ivar_layout_symbol;
            ivar_record.executable_ivar_layout_slot_index =
                property.executable_ivar_layout_slot_index;
            ivar_record.executable_ivar_layout_size_bytes =
                property.executable_ivar_layout_size_bytes;
            ivar_record.executable_ivar_layout_alignment_bytes =
                property.executable_ivar_layout_alignment_bytes;
            ivar_record.executable_ivar_layout_offset_bytes =
                property.executable_ivar_layout_offset_bytes;
            ivar_record.executable_ivar_layout_padding_bytes =
                property.executable_ivar_layout_padding_bytes;
            ivar_record.executable_ivar_layout_inherited_slot_count =
                property.executable_ivar_layout_inherited_slot_count;
            ivar_record.executable_ivar_layout_inherited_size_bytes =
                property.executable_ivar_layout_inherited_size_bytes;
            ivar_record.executable_ivar_layout_owner_size_bytes =
                property.executable_ivar_layout_owner_size_bytes;
            ivar_record.executable_ivar_init_order_index =
                property.executable_ivar_init_order_index;
            ivar_record.executable_ivar_destroy_order_index =
                property.executable_ivar_destroy_order_index;
            ivar_record.executable_ivar_layout_valid =
                property.executable_ivar_layout_valid;
            ivar_record.executable_ivar_layout_replay_key =
                property.executable_ivar_layout_replay_key;
            ivar_record.line = property.line;
            ivar_record.column = property.column;
            records.ivars_lexicographic.push_back(std::move(ivar_record));
          }
        }
      };

  const auto append_method_records =
      [&records](const auto &methods, const std::string &owner_kind,
                 const std::string &owner_name,
                 bool direct_members_declared = false) {
        for (const auto &method : methods) {
          Objc3RuntimeMetadataMethodSourceRecord method_record;
          method_record.owner_kind = owner_kind;
          method_record.owner_name = owner_name;
          method_record.selector = method.selector;
          method_record.is_class_method = method.is_class_method;
          method_record.has_body = method.has_body;
          method_record.effective_direct_dispatch =
              method.objc_direct_declared ||
              (direct_members_declared && !method.objc_dynamic_declared);
          method_record.objc_final_declared = method.objc_final_declared;
          method_record.parameter_count = method.params.size();
          method_record.return_type_name = RuntimeMetadataTypeName(method.return_type);
          method_record.line = method.line;
          method_record.column = method.column;
          records.methods_lexicographic.push_back(std::move(method_record));
        }
      };

  struct Objc3ClassDispatchProfile {
    bool objc_direct_members_declared = false;
    bool objc_final_declared = false;
    bool objc_sealed_declared = false;
  };
  std::unordered_map<std::string, Objc3ClassDispatchProfile> class_dispatch_profiles;
  class_dispatch_profiles.reserve(program.interfaces.size());
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      continue;
    }
    class_dispatch_profiles[interface_decl.name] = Objc3ClassDispatchProfile{
        interface_decl.objc_direct_members_declared,
        interface_decl.objc_final_declared,
        interface_decl.objc_sealed_declared};
  }

  for (const auto &protocol : program.protocols) {
    Objc3RuntimeMetadataProtocolSourceRecord record;
    record.name = protocol.name;
    record.inherited_protocols_lexicographic = protocol.inherited_protocols_lexicographic;
    record.is_forward_declaration = protocol.is_forward_declaration;
    record.property_count = protocol.properties.size();
    record.method_count = protocol.methods.size();
    record.line = protocol.line;
    record.column = protocol.column;
    records.protocols_lexicographic.push_back(record);
    append_property_records(protocol.properties, "protocol", protocol.name);
    append_method_records(protocol.methods, "protocol", protocol.name);
  }

  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "interface";
      record.class_name = interface_decl.name;
      record.category_name = interface_decl.category_name;
      record.adopted_protocols_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      record.property_count = interface_decl.properties.size();
      record.method_count = interface_decl.methods.size();
      record.line = interface_decl.line;
      record.column = interface_decl.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(interface_decl.name, interface_decl.category_name);
      append_property_records(interface_decl.properties, "category-interface", owner_name);
      append_method_records(interface_decl.methods, "category-interface", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    record.record_kind = "interface";
    record.name = interface_decl.name;
    record.super_name = interface_decl.super_name;
    record.adopted_protocols_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    record.has_super = !interface_decl.super_name.empty();
    record.objc_final_declared = interface_decl.objc_final_declared;
    record.objc_sealed_declared = interface_decl.objc_sealed_declared;
    record.property_count = interface_decl.properties.size();
    record.method_count = interface_decl.methods.size();
    record.line = interface_decl.line;
    record.column = interface_decl.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(interface_decl.properties, "class-interface", interface_decl.name);
    append_method_records(interface_decl.methods, "class-interface",
                          interface_decl.name,
                          interface_decl.objc_direct_members_declared);
  }

  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "implementation";
      record.class_name = implementation.name;
      record.category_name = implementation.category_name;
      record.property_count = implementation.properties.size();
      record.method_count = implementation.methods.size();
      record.line = implementation.line;
      record.column = implementation.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(implementation.name, implementation.category_name);
      append_property_records(implementation.properties, "category-implementation", owner_name);
      append_method_records(implementation.methods, "category-implementation", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    const auto profile_it = class_dispatch_profiles.find(implementation.name);
    record.record_kind = "implementation";
    record.name = implementation.name;
    if (profile_it != class_dispatch_profiles.end()) {
      record.objc_final_declared = profile_it->second.objc_final_declared;
      record.objc_sealed_declared = profile_it->second.objc_sealed_declared;
    }
    record.property_count = implementation.properties.size();
    record.method_count = implementation.methods.size();
    record.line = implementation.line;
    record.column = implementation.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(implementation.properties, "class-implementation", implementation.name);
    append_method_records(
        implementation.methods, "class-implementation", implementation.name,
        profile_it != class_dispatch_profiles.end() &&
            profile_it->second.objc_direct_members_declared);
  }

  std::sort(records.classes_lexicographic.begin(),
            records.classes_lexicographic.end(),
            IsClassSourceRecordLess);
  std::sort(records.protocols_lexicographic.begin(),
            records.protocols_lexicographic.end(),
            IsProtocolSourceRecordLess);
  std::sort(records.categories_lexicographic.begin(),
            records.categories_lexicographic.end(),
            IsCategorySourceRecordLess);
  std::sort(records.properties_lexicographic.begin(),
            records.properties_lexicographic.end(),
            IsPropertySourceRecordLess);
  std::sort(records.methods_lexicographic.begin(),
            records.methods_lexicographic.end(),
            IsMethodSourceRecordLess);
  std::sort(records.ivars_lexicographic.begin(),
            records.ivars_lexicographic.end(),
            IsIvarSourceRecordLess);

  records.deterministic = std::is_sorted(records.classes_lexicographic.begin(),
                                         records.classes_lexicographic.end(),
                                         IsClassSourceRecordLess) &&
                          std::is_sorted(records.protocols_lexicographic.begin(),
                                         records.protocols_lexicographic.end(),
                                         IsProtocolSourceRecordLess) &&
                          std::is_sorted(records.categories_lexicographic.begin(),
                                         records.categories_lexicographic.end(),
                                         IsCategorySourceRecordLess) &&
                          std::is_sorted(records.properties_lexicographic.begin(),
                                         records.properties_lexicographic.end(),
                                         IsPropertySourceRecordLess) &&
                          std::is_sorted(records.methods_lexicographic.begin(),
                                         records.methods_lexicographic.end(),
                                         IsMethodSourceRecordLess) &&
                          std::is_sorted(records.ivars_lexicographic.begin(),
                                         records.ivars_lexicographic.end(),
                                         IsIvarSourceRecordLess);
  return records;
}

Objc3ExecutableMetadataSourceGraph BuildExecutableMetadataSourceGraph(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3ExecutableMetadataSourceGraph graph;

  struct AggregatedClassSurface {
    bool has_interface = false;
    bool has_implementation = false;
    std::string interface_owner_identity;
    std::string implementation_owner_identity;
    std::string super_class_owner_identity;
    std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
    bool objc_direct_members_declared = false;
    bool objc_final_declared = false;
    bool objc_sealed_declared = false;
    std::size_t interface_property_count = 0;
    std::size_t implementation_property_count = 0;
    std::size_t interface_method_count = 0;
    std::size_t implementation_method_count = 0;
    std::size_t interface_class_method_count = 0;
    std::size_t implementation_class_method_count = 0;
    unsigned line = 1;
    unsigned column = 1;
  };

  struct AggregatedCategorySurface {
    bool has_interface = false;
    bool has_implementation = false;
    std::string interface_owner_identity;
    std::string implementation_owner_identity;
    std::string class_owner_identity;
    std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
    std::size_t interface_property_count = 0;
    std::size_t implementation_property_count = 0;
    std::size_t interface_method_count = 0;
    std::size_t implementation_method_count = 0;
    std::size_t interface_class_method_count = 0;
    std::size_t implementation_class_method_count = 0;
    unsigned line = 1;
    unsigned column = 1;
  };

  std::unordered_map<std::string, AggregatedClassSurface> aggregated_classes;
  aggregated_classes.reserve(program.interfaces.size() + program.implementations.size());
  std::unordered_map<std::string, AggregatedCategorySurface> aggregated_categories;
  aggregated_categories.reserve(program.interfaces.size() + program.implementations.size());

  auto &owner_edges = graph.owner_edges_lexicographic;
  const auto add_owner_edge = [&owner_edges](const std::string &edge_kind,
                                             const std::string &source_owner_identity,
                                             const std::string &target_owner_identity,
                                             unsigned line,
                                             unsigned column) {
    if (source_owner_identity.empty() || target_owner_identity.empty()) {
      return;
    }
    Objc3ExecutableMetadataGraphEdge edge;
    edge.edge_kind = edge_kind;
    edge.source_owner_identity = source_owner_identity;
    edge.target_owner_identity = target_owner_identity;
    edge.line = line;
    edge.column = column;
    owner_edges.push_back(std::move(edge));
  };

  struct MethodEdgeRecord {
    std::string owner_identity;
    std::string export_owner_identity;
    std::string selector;
    bool is_class_method = false;
  };
  std::vector<MethodEdgeRecord> method_edge_records;
  std::unordered_set<std::string> class_implementation_names;
  class_implementation_names.reserve(program.implementations.size());
  std::unordered_set<std::string> implementation_property_keys;
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      class_implementation_names.insert(implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }

  const auto apply_arc_property_interaction_metadata =
      [](Objc3ExecutableMetadataPropertyGraphNode &node) {
        objc3c::support::ApplyPropertyOwnershipProfileFallback(node, false);
        objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(node);
      };

  const auto add_property_nodes =
      [&graph, &add_owner_edge, &apply_arc_property_interaction_metadata,
       &class_implementation_names, &implementation_property_keys](
          const auto &properties, const std::string &owner_kind,
          const std::string &owner_name,
          const std::string &declaration_owner_identity,
          const std::string &export_owner_identity) {
        for (const auto &property : properties) {
          Objc3ExecutableMetadataPropertyGraphNode node;
          node.owner_kind = owner_kind;
          node.owner_name = owner_name;
          node.declaration_owner_identity = declaration_owner_identity;
          node.export_owner_identity = export_owner_identity;
          node.owner_identity =
              BuildPropertyNodeOwnerIdentity(declaration_owner_identity, property);
          node.property_name = property.name;
          node.type_name = RuntimeMetadataTypeName(property.type);
          node.has_getter = property.has_getter;
          node.getter_selector = property.getter_selector;
          node.has_setter = property.has_setter;
          node.setter_selector = property.setter_selector;
          node.ivar_binding_symbol = property.ivar_binding_symbol;
          node.executable_synthesized_binding_kind =
              property.executable_synthesized_binding_kind;
          node.executable_synthesized_binding_symbol =
              property.executable_synthesized_binding_symbol;
          node.property_attribute_profile = property.property_attribute_profile;
          node.ownership_lifetime_profile =
              property.ownership_lifetime_profile;
          node.ownership_runtime_hook_profile =
              property.ownership_runtime_hook_profile;
          node.effective_getter_selector =
              property.effective_getter_selector;
          node.effective_setter_available =
              property.effective_setter_available;
          node.effective_setter_selector =
              property.effective_setter_selector;
          node.accessor_ownership_profile =
              property.accessor_ownership_profile;
          apply_arc_property_interaction_metadata(node);
          node.synthesizes_executable_accessors =
              ShouldSynthesizeExecutablePropertyAccessors(
                  owner_kind, owner_name, property.name,
                  class_implementation_names, implementation_property_keys);
          node.getter_storage_runtime_helper_symbol =
              BuildGetterStorageRuntimeHelperSymbol(
                  node.synthesizes_executable_accessors,
                  node.ownership_runtime_hook_profile);
          node.setter_storage_runtime_helper_symbol =
              BuildSetterStorageRuntimeHelperSymbol(
                  node.synthesizes_executable_accessors,
                  node.effective_setter_available,
                  node.ownership_lifetime_profile,
                  node.ownership_runtime_hook_profile,
                  node.accessor_ownership_profile);
          node.executable_ivar_layout_symbol =
              property.executable_ivar_layout_symbol;
          node.executable_ivar_layout_slot_index =
              property.executable_ivar_layout_slot_index;
          node.executable_ivar_layout_size_bytes =
              property.executable_ivar_layout_size_bytes;
          node.executable_ivar_layout_alignment_bytes =
              property.executable_ivar_layout_alignment_bytes;
          node.executable_ivar_layout_offset_bytes =
              property.executable_ivar_layout_offset_bytes;
          node.executable_ivar_layout_padding_bytes =
              property.executable_ivar_layout_padding_bytes;
          node.executable_ivar_layout_inherited_slot_count =
              property.executable_ivar_layout_inherited_slot_count;
          node.executable_ivar_layout_inherited_size_bytes =
              property.executable_ivar_layout_inherited_size_bytes;
          node.executable_ivar_layout_owner_size_bytes =
              property.executable_ivar_layout_owner_size_bytes;
          node.executable_ivar_init_order_index =
              property.executable_ivar_init_order_index;
          node.executable_ivar_destroy_order_index =
              property.executable_ivar_destroy_order_index;
          node.executable_ivar_layout_valid =
              property.executable_ivar_layout_valid;
          node.executable_ivar_layout_replay_key =
              property.executable_ivar_layout_replay_key;
          node.line = property.line;
          node.column = property.column;
          graph.property_nodes_lexicographic.push_back(node);

          add_owner_edge("property-to-declaration-owner", node.owner_identity,
                         declaration_owner_identity, node.line, node.column);
          add_owner_edge("property-to-export-owner", node.owner_identity,
                         export_owner_identity, node.line, node.column);

          if (!property.ivar_binding_symbol.empty()) {
            Objc3ExecutableMetadataIvarGraphNode ivar_node;
            ivar_node.owner_kind = owner_kind;
            ivar_node.owner_name = owner_name;
            ivar_node.declaration_owner_identity = declaration_owner_identity;
            ivar_node.export_owner_identity = export_owner_identity;
            ivar_node.property_owner_identity = node.owner_identity;
            ivar_node.owner_identity =
                BuildIvarNodeOwnerIdentity(declaration_owner_identity, property);
            ivar_node.property_name = property.name;
            ivar_node.ivar_binding_symbol = property.ivar_binding_symbol;
            ivar_node.executable_synthesized_binding_kind =
                property.executable_synthesized_binding_kind;
            ivar_node.executable_synthesized_binding_symbol =
                property.executable_synthesized_binding_symbol;
            ivar_node.executable_ivar_layout_symbol =
                property.executable_ivar_layout_symbol;
            ivar_node.executable_ivar_layout_slot_index =
                property.executable_ivar_layout_slot_index;
            ivar_node.executable_ivar_layout_size_bytes =
                property.executable_ivar_layout_size_bytes;
            ivar_node.executable_ivar_layout_alignment_bytes =
                property.executable_ivar_layout_alignment_bytes;
            ivar_node.executable_ivar_layout_offset_bytes =
                property.executable_ivar_layout_offset_bytes;
            ivar_node.executable_ivar_layout_padding_bytes =
                property.executable_ivar_layout_padding_bytes;
            ivar_node.executable_ivar_layout_inherited_slot_count =
                property.executable_ivar_layout_inherited_slot_count;
            ivar_node.executable_ivar_layout_inherited_size_bytes =
                property.executable_ivar_layout_inherited_size_bytes;
            ivar_node.executable_ivar_layout_owner_size_bytes =
                property.executable_ivar_layout_owner_size_bytes;
            ivar_node.executable_ivar_init_order_index =
                property.executable_ivar_init_order_index;
            ivar_node.executable_ivar_destroy_order_index =
                property.executable_ivar_destroy_order_index;
            ivar_node.executable_ivar_layout_valid =
                property.executable_ivar_layout_valid;
            ivar_node.executable_ivar_layout_replay_key =
                property.executable_ivar_layout_replay_key;
            ivar_node.line = property.line;
            ivar_node.column = property.column;
            graph.ivar_nodes_lexicographic.push_back(ivar_node);

            add_owner_edge("ivar-to-declaration-owner", ivar_node.owner_identity,
                           declaration_owner_identity, ivar_node.line,
                           ivar_node.column);
            add_owner_edge("ivar-to-export-owner", ivar_node.owner_identity,
                           export_owner_identity, ivar_node.line, ivar_node.column);
            add_owner_edge("ivar-to-property", ivar_node.owner_identity,
                           node.owner_identity, ivar_node.line, ivar_node.column);
            add_owner_edge("property-to-ivar", node.owner_identity,
                           ivar_node.owner_identity, node.line, node.column);
          }
        }
      };

  const auto add_method_nodes =
      [&graph, &add_owner_edge, &method_edge_records](const auto &methods,
                                                      const std::string &owner_kind,
                                                      const std::string &owner_name,
                                                      const std::string &declaration_owner_identity,
                                                      const std::string &instance_export_owner_identity,
                                                      const std::string &class_export_owner_identity,
                                                      bool direct_members_declared) {
        for (const auto &method : methods) {
          Objc3ExecutableMetadataMethodGraphNode node;
          node.owner_kind = owner_kind;
          node.owner_name = owner_name;
          node.declaration_owner_identity = declaration_owner_identity;
          node.export_owner_identity =
              method.is_class_method ? class_export_owner_identity
                                     : instance_export_owner_identity;
          node.owner_identity =
              BuildMethodNodeOwnerIdentity(declaration_owner_identity, method);
          node.selector = method.selector;
          node.is_class_method = method.is_class_method;
          node.has_body = method.has_body;
          node.effective_direct_dispatch =
              method.objc_direct_declared ||
              (direct_members_declared && !method.objc_dynamic_declared);
          node.objc_final_declared = method.objc_final_declared;
          node.parameter_count = method.params.size();
          node.return_type_name = RuntimeMetadataTypeName(method.return_type);
          node.line = method.line;
          node.column = method.column;
          graph.method_nodes_lexicographic.push_back(node);

          add_owner_edge("method-to-declaration-owner", node.owner_identity,
                         declaration_owner_identity, node.line, node.column);
          add_owner_edge("method-to-export-owner", node.owner_identity,
                         node.export_owner_identity, node.line, node.column);

          method_edge_records.push_back(
              {node.owner_identity, node.export_owner_identity, node.selector,
               node.is_class_method});
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(interface_decl.name, interface_decl.category_name);
      AggregatedCategorySurface &aggregate =
          aggregated_categories[category_owner_name];
      if (!aggregate.has_interface && !aggregate.has_implementation) {
        aggregate.line = interface_decl.line;
        aggregate.column = interface_decl.column;
      }
      aggregate.has_interface = true;
      aggregate.interface_owner_identity = interface_decl.semantic_link_symbol;
      aggregate.class_owner_identity =
          BuildRuntimeClassOwnerIdentity(interface_decl.name);
      aggregate.adopted_protocol_owner_identities_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      aggregate.interface_property_count = interface_decl.properties.size();
      aggregate.interface_method_count = interface_decl.methods.size();
      aggregate.interface_class_method_count =
          CountClassMethods(interface_decl.methods);

      add_property_nodes(interface_decl.properties, "category-interface",
                         category_owner_name, interface_decl.semantic_link_symbol,
                         BuildRuntimeCategoryOwnerIdentity(
                             interface_decl.name, interface_decl.category_name));
      add_method_nodes(interface_decl.methods, "category-interface",
                       category_owner_name, interface_decl.semantic_link_symbol,
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       false);
      continue;
    }

    Objc3ExecutableMetadataInterfaceGraphNode node;
    node.class_name = interface_decl.name;
    node.owner_identity = interface_decl.semantic_link_symbol;
    node.class_owner_identity = BuildRuntimeClassOwnerIdentity(interface_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(interface_decl.name);
    node.super_class_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeClassOwnerIdentity(interface_decl.super_name);
    node.super_metaclass_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(interface_decl.super_name);
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_super = !interface_decl.super_name.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = interface_decl.properties.size();
    node.method_count = interface_decl.methods.size();
    node.class_method_count = CountClassMethods(interface_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = interface_decl.line;
    node.column = interface_decl.column;
    graph.interface_nodes_lexicographic.push_back(node);

    AggregatedClassSurface &aggregate = aggregated_classes[interface_decl.name];
    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = interface_decl.line;
      aggregate.column = interface_decl.column;
    }
    aggregate.has_interface = true;
    aggregate.interface_owner_identity = interface_decl.semantic_link_symbol;
    aggregate.super_class_owner_identity = node.super_class_owner_identity;
    aggregate.adopted_protocol_owner_identities_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    aggregate.objc_direct_members_declared =
        interface_decl.objc_direct_members_declared;
    aggregate.objc_final_declared = interface_decl.objc_final_declared;
    aggregate.objc_sealed_declared = interface_decl.objc_sealed_declared;
    aggregate.interface_property_count = node.property_count;
    aggregate.interface_method_count = node.method_count;
    aggregate.interface_class_method_count = node.class_method_count;

    add_owner_edge("interface-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-instance-method-owner", node.owner_identity,
                   node.instance_method_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("class-to-superclass", node.class_owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(interface_decl.properties, "class-interface",
                       interface_decl.name, interface_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(interface_decl.methods, "class-interface", interface_decl.name,
                     interface_decl.semantic_link_symbol, node.class_owner_identity,
                     node.metaclass_owner_identity,
                     interface_decl.objc_direct_members_declared);
  }

  for (const auto &implementation_decl : program.implementations) {
    if (implementation_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(implementation_decl.name,
                                 implementation_decl.category_name);
      AggregatedCategorySurface &aggregate =
          aggregated_categories[category_owner_name];
      if (!aggregate.has_interface && !aggregate.has_implementation) {
        aggregate.line = implementation_decl.line;
        aggregate.column = implementation_decl.column;
      }
      aggregate.has_implementation = true;
      aggregate.implementation_owner_identity =
          implementation_decl.semantic_link_symbol;
      aggregate.class_owner_identity =
          BuildRuntimeClassOwnerIdentity(implementation_decl.name);
      aggregate.implementation_property_count =
          implementation_decl.properties.size();
      aggregate.implementation_method_count = implementation_decl.methods.size();
      aggregate.implementation_class_method_count =
          CountClassMethods(implementation_decl.methods);

      add_property_nodes(
          implementation_decl.properties, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name));
      add_method_nodes(
          implementation_decl.methods, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          false);
      continue;
    }

    AggregatedClassSurface &aggregate = aggregated_classes[implementation_decl.name];
    Objc3ExecutableMetadataImplementationGraphNode node;
    node.class_name = implementation_decl.name;
    node.owner_identity = implementation_decl.semantic_link_symbol;
    node.interface_owner_identity =
        implementation_decl.semantic_link_interface_symbol;
    node.class_owner_identity =
        BuildRuntimeClassOwnerIdentity(implementation_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(implementation_decl.name);
    node.super_class_owner_identity = aggregate.super_class_owner_identity;
    node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_matching_interface = !node.interface_owner_identity.empty();
    node.has_super = !node.super_class_owner_identity.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = implementation_decl.properties.size();
    node.method_count = implementation_decl.methods.size();
    node.class_method_count = CountClassMethods(implementation_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = implementation_decl.line;
    node.column = implementation_decl.column;
    graph.implementation_nodes_lexicographic.push_back(node);

    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = implementation_decl.line;
      aggregate.column = implementation_decl.column;
    }
    aggregate.has_implementation = true;
    aggregate.implementation_owner_identity = implementation_decl.semantic_link_symbol;
    aggregate.implementation_property_count = node.property_count;
    aggregate.implementation_method_count = node.method_count;
    aggregate.implementation_class_method_count = node.class_method_count;

    add_owner_edge("implementation-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-instance-method-owner",
                   node.owner_identity, node.instance_method_owner_identity,
                   node.line, node.column);
    add_owner_edge("implementation-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(implementation_decl.properties, "class-implementation",
                       implementation_decl.name,
                       implementation_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(implementation_decl.methods, "class-implementation",
                     implementation_decl.name,
                     implementation_decl.semantic_link_symbol,
                     node.class_owner_identity, node.metaclass_owner_identity,
                     aggregate.objc_direct_members_declared);
  }

  for (const auto &protocol_decl : program.protocols) {
    Objc3ExecutableMetadataProtocolGraphNode node;
    node.protocol_name = protocol_decl.name;
    node.owner_identity = protocol_decl.semantic_link_symbol;
    node.inherited_protocol_owner_identities_lexicographic =
        protocol_decl.inherited_protocols_lexicographic;
    node.property_count = protocol_decl.properties.size();
    node.method_count = protocol_decl.methods.size();
    node.is_forward_declaration = protocol_decl.is_forward_declaration;
    node.declaration_complete =
        !node.protocol_name.empty() && !node.owner_identity.empty();
    node.line = protocol_decl.line;
    node.column = protocol_decl.column;
    std::sort(node.inherited_protocol_owner_identities_lexicographic.begin(),
              node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_owner_identities_lexicographic.erase(
        std::unique(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end()),
        node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_identity_complete =
        std::all_of(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.protocol_nodes_lexicographic.push_back(node);

    for (const auto &target : node.inherited_protocol_owner_identities_lexicographic) {
      add_owner_edge("protocol-to-inherited-protocol", node.owner_identity,
                     target, node.line, node.column);
    }

    add_property_nodes(protocol_decl.properties, "protocol", protocol_decl.name,
                       protocol_decl.semantic_link_symbol,
                       protocol_decl.semantic_link_symbol);
    add_method_nodes(protocol_decl.methods, "protocol", protocol_decl.name,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol, false);
  }

  std::vector<std::string> class_names;
  class_names.reserve(aggregated_classes.size());
  for (const auto &entry : aggregated_classes) {
    class_names.push_back(entry.first);
  }
  std::sort(class_names.begin(), class_names.end());

  graph.class_nodes_lexicographic.reserve(class_names.size());
  graph.metaclass_nodes_lexicographic.reserve(class_names.size());
  for (const std::string &class_name : class_names) {
    const AggregatedClassSurface &aggregate = aggregated_classes.at(class_name);

    Objc3ExecutableMetadataClassGraphNode class_node;
    class_node.class_name = class_name;
    class_node.owner_identity = BuildRuntimeClassOwnerIdentity(class_name);
    class_node.interface_owner_identity = aggregate.interface_owner_identity;
    class_node.implementation_owner_identity =
        aggregate.implementation_owner_identity;
    class_node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(class_name);
    class_node.super_class_owner_identity = aggregate.super_class_owner_identity;
    class_node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    class_node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    class_node.instance_method_owner_identity = class_node.owner_identity;
    class_node.class_method_owner_identity = class_node.metaclass_owner_identity;
    class_node.has_interface = aggregate.has_interface;
    class_node.has_implementation = aggregate.has_implementation;
    class_node.has_super = !aggregate.super_class_owner_identity.empty();
    class_node.objc_final_declared = aggregate.objc_final_declared;
    class_node.objc_sealed_declared = aggregate.objc_sealed_declared;
    class_node.realization_identity_complete =
        !class_node.owner_identity.empty() &&
        !class_node.metaclass_owner_identity.empty() &&
        !class_node.instance_method_owner_identity.empty() &&
        !class_node.class_method_owner_identity.empty() &&
        (!class_node.has_super ||
         (!class_node.super_class_owner_identity.empty() &&
          !class_node.super_metaclass_owner_identity.empty()));
    class_node.interface_property_count = aggregate.interface_property_count;
    class_node.implementation_property_count =
        aggregate.implementation_property_count;
    class_node.interface_method_count = aggregate.interface_method_count;
    class_node.implementation_method_count = aggregate.implementation_method_count;
    class_node.interface_class_method_count =
        aggregate.interface_class_method_count;
    class_node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    class_node.interface_instance_method_count =
        aggregate.interface_method_count -
        aggregate.interface_class_method_count;
    class_node.implementation_instance_method_count =
        aggregate.implementation_method_count -
        aggregate.implementation_class_method_count;
    class_node.line = aggregate.line;
    class_node.column = aggregate.column;
    graph.class_nodes_lexicographic.push_back(class_node);

    if (aggregate.has_interface) {
      Objc3ExecutableMetadataMetaclassGraphNode metaclass_node;
      metaclass_node.class_name = class_name;
      metaclass_node.owner_identity =
          BuildRuntimeMetaclassOwnerIdentity(class_name);
      metaclass_node.class_owner_identity = class_node.owner_identity;
      metaclass_node.interface_owner_identity = aggregate.interface_owner_identity;
      metaclass_node.implementation_owner_identity =
          aggregate.implementation_owner_identity;
      metaclass_node.super_metaclass_owner_identity =
          class_node.has_super
              ? BuildRuntimeMetaclassOwnerIdentity(
                    class_node.super_class_owner_identity.substr(6u))
              : std::string{};
      metaclass_node.derived_from_interface = true;
      metaclass_node.has_implementation = aggregate.has_implementation;
      metaclass_node.has_super = class_node.has_super;
      metaclass_node.interface_class_method_count =
          aggregate.interface_class_method_count;
      metaclass_node.implementation_class_method_count =
          aggregate.implementation_class_method_count;
      metaclass_node.line = aggregate.line;
      metaclass_node.column = aggregate.column;
      graph.metaclass_nodes_lexicographic.push_back(metaclass_node);

      add_owner_edge("class-to-metaclass", class_node.owner_identity,
                     metaclass_node.owner_identity, class_node.line,
                     class_node.column);
      add_owner_edge("metaclass-to-super-metaclass",
                     metaclass_node.owner_identity,
                     metaclass_node.super_metaclass_owner_identity,
                     metaclass_node.line, metaclass_node.column);
    }
  }

  std::vector<std::string> category_names;
  category_names.reserve(aggregated_categories.size());
  for (const auto &entry : aggregated_categories) {
    category_names.push_back(entry.first);
  }
  std::sort(category_names.begin(), category_names.end());

  graph.category_nodes_lexicographic.reserve(category_names.size());
  for (const std::string &category_owner_name : category_names) {
    const AggregatedCategorySurface &aggregate =
        aggregated_categories.at(category_owner_name);
    const std::size_t open_paren = category_owner_name.find('(');
    const std::string class_name =
        open_paren == std::string::npos
            ? category_owner_name
            : category_owner_name.substr(0u, open_paren);
    const std::string category_name =
        open_paren == std::string::npos
            ? std::string{}
            : category_owner_name.substr(open_paren + 1u,
                                         category_owner_name.size() - open_paren - 2u);

    Objc3ExecutableMetadataCategoryGraphNode node;
    node.class_name = class_name;
    node.category_name = category_name;
    node.owner_identity =
        BuildRuntimeCategoryOwnerIdentity(class_name, category_name);
    node.interface_owner_identity = aggregate.interface_owner_identity;
    node.implementation_owner_identity = aggregate.implementation_owner_identity;
    node.class_owner_identity = aggregate.class_owner_identity;
    node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    node.has_interface = aggregate.has_interface;
    node.has_implementation = aggregate.has_implementation;
    node.declaration_complete =
        !node.class_name.empty() && !node.category_name.empty() &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.attachment_identity_complete =
        !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.interface_property_count = aggregate.interface_property_count;
    node.implementation_property_count = aggregate.implementation_property_count;
    node.interface_method_count = aggregate.interface_method_count;
    node.implementation_method_count = aggregate.implementation_method_count;
    node.interface_class_method_count = aggregate.interface_class_method_count;
    node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    node.line = aggregate.line;
    node.column = aggregate.column;
    std::sort(node.adopted_protocol_owner_identities_lexicographic.begin(),
              node.adopted_protocol_owner_identities_lexicographic.end());
    node.adopted_protocol_owner_identities_lexicographic.erase(
        std::unique(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end()),
        node.adopted_protocol_owner_identities_lexicographic.end());
    node.conformance_identity_complete =
        std::all_of(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.category_nodes_lexicographic.push_back(node);

    add_owner_edge("category-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("category-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("category-to-implementation", node.owner_identity,
                   node.implementation_owner_identity, node.line, node.column);
    for (const auto &target : node.adopted_protocol_owner_identities_lexicographic) {
      add_owner_edge("category-to-protocol", node.owner_identity, target,
                     node.line, node.column);
    }
  }

  for (const auto &property_node : graph.property_nodes_lexicographic) {
    if (property_node.has_getter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.getter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-getter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }

    if (property_node.has_setter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.setter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-setter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<std::string,
                     const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, std::string>
      class_interface_method_owner_identity_by_key;
  class_interface_method_owner_identity_by_key.reserve(
      graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_method_owner_identity_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        method_node.owner_identity);
  }

  const auto find_overridden_interface_method_owner_identity =
      [&](const Objc3ExecutableMetadataInterfaceGraphNode &interface_node,
          const Objc3ExecutableMetadataMethodGraphNode &method_node) {
        std::string next_super_owner_identity =
            interface_node.super_class_owner_identity;
        std::unordered_set<std::string> visited;
        while (!next_super_owner_identity.empty()) {
          if (!visited.insert(next_super_owner_identity).second) {
            return std::string{};
          }
          const auto class_it =
              class_nodes_by_owner_identity.find(next_super_owner_identity);
          if (class_it == class_nodes_by_owner_identity.end() ||
              class_it->second->interface_owner_identity.empty()) {
            return std::string{};
          }
          const std::string key = build_interface_method_key(
              class_it->second->interface_owner_identity, method_node.selector,
              method_node.is_class_method);
          const auto method_it =
              class_interface_method_owner_identity_by_key.find(key);
          if (method_it !=
              class_interface_method_owner_identity_by_key.end()) {
            return method_it->second;
          }
          next_super_owner_identity =
              class_it->second->super_class_owner_identity;
        }
        return std::string{};
      };

  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }
    const std::string overridden_method_owner_identity =
        find_overridden_interface_method_owner_identity(*interface_it->second,
                                                        method_node);
    if (!overridden_method_owner_identity.empty()) {
      add_owner_edge("method-to-overridden-method", method_node.owner_identity,
                     overridden_method_owner_identity, method_node.line,
                     method_node.column);
    }
  }

  std::sort(graph.interface_nodes_lexicographic.begin(),
            graph.interface_nodes_lexicographic.end(),
            IsExecutableMetadataInterfaceNodeLess);
  std::sort(graph.implementation_nodes_lexicographic.begin(),
            graph.implementation_nodes_lexicographic.end(),
            IsExecutableMetadataImplementationNodeLess);
  std::sort(graph.class_nodes_lexicographic.begin(),
            graph.class_nodes_lexicographic.end(),
            IsExecutableMetadataClassNodeLess);
  std::sort(graph.metaclass_nodes_lexicographic.begin(),
            graph.metaclass_nodes_lexicographic.end(),
            IsExecutableMetadataMetaclassNodeLess);
  std::sort(graph.protocol_nodes_lexicographic.begin(),
            graph.protocol_nodes_lexicographic.end(),
            IsExecutableMetadataProtocolNodeLess);
  std::sort(graph.category_nodes_lexicographic.begin(),
            graph.category_nodes_lexicographic.end(),
            IsExecutableMetadataCategoryNodeLess);
  std::sort(graph.property_nodes_lexicographic.begin(),
            graph.property_nodes_lexicographic.end(),
            IsExecutableMetadataPropertyNodeLess);
  std::sort(graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            IsExecutableMetadataMethodNodeLess);
  std::sort(graph.ivar_nodes_lexicographic.begin(),
            graph.ivar_nodes_lexicographic.end(),
            IsExecutableMetadataIvarNodeLess);
  std::sort(graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            IsExecutableMetadataGraphEdgeLess);

  const auto has_graph_edge = [&graph](const std::string &edge_kind,
                                       const std::string &source_owner_identity,
                                       const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&edge_kind, &source_owner_identity, &target_owner_identity](
            const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  const std::size_t expected_class_interface_count = static_cast<std::size_t>(
      std::count_if(program.interfaces.begin(), program.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  const std::size_t expected_class_implementation_count = static_cast<std::size_t>(
      std::count_if(program.implementations.begin(), program.implementations.end(),
                    [](const Objc3ImplementationDecl &decl) {
                      return !decl.has_category;
                    }));
  const bool interface_count_aligned =
      graph.interface_nodes_lexicographic.size() == expected_class_interface_count;
  const bool implementation_count_aligned =
      graph.implementation_nodes_lexicographic.size() ==
      expected_class_implementation_count;
  const bool metaclass_count_aligned =
      graph.metaclass_nodes_lexicographic.size() ==
      graph.interface_nodes_lexicographic.size();
  const bool class_node_floor_satisfied =
      graph.class_nodes_lexicographic.size() >=
          graph.interface_nodes_lexicographic.size() &&
      graph.class_nodes_lexicographic.size() >=
          graph.implementation_nodes_lexicographic.size();
  const bool protocol_count_aligned =
      graph.protocol_nodes_lexicographic.size() ==
      runtime_metadata_source_records.protocols_lexicographic.size();
  const bool property_count_aligned =
      graph.property_nodes_lexicographic.size() ==
      runtime_metadata_source_records.properties_lexicographic.size();
  const bool method_count_aligned =
      graph.method_nodes_lexicographic.size() ==
      runtime_metadata_source_records.methods_lexicographic.size();
  const bool ivar_count_aligned =
      graph.ivar_nodes_lexicographic.size() ==
      runtime_metadata_source_records.ivars_lexicographic.size();
  std::vector<std::string> category_record_owner_names;
  category_record_owner_names.reserve(
      runtime_metadata_source_records.categories_lexicographic.size());
  for (const auto &record :
       runtime_metadata_source_records.categories_lexicographic) {
    category_record_owner_names.push_back(
        BuildCategoryOwnerName(record.class_name, record.category_name));
  }
  std::sort(category_record_owner_names.begin(),
            category_record_owner_names.end());
  category_record_owner_names.erase(
      std::unique(category_record_owner_names.begin(),
                  category_record_owner_names.end()),
      category_record_owner_names.end());
  const bool category_count_aligned =
      graph.category_nodes_lexicographic.size() ==
      category_record_owner_names.size();

  graph.class_metaclass_declaration_closure_complete = true;
  graph.class_metaclass_parent_identity_closure_complete = true;
  graph.class_metaclass_method_owner_identity_closure_complete = true;
  graph.class_metaclass_object_identity_closure_complete = true;
  graph.protocol_category_declaration_closure_complete = true;
  graph.protocol_inheritance_identity_closure_complete = true;
  graph.category_attachment_identity_closure_complete = true;
  graph.protocol_category_conformance_identity_closure_complete = true;

  for (const auto &node : graph.interface_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("interface-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("interface-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("interface-to-instance-method-owner", node.owner_identity,
                       node.instance_method_owner_identity) &&
        has_graph_edge("interface-to-class-method-owner", node.owner_identity,
                       node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("interface-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("interface-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.implementation_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("implementation-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("implementation-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("implementation-to-instance-method-owner",
                       node.owner_identity, node.instance_method_owner_identity) &&
        has_graph_edge("implementation-to-class-method-owner",
                       node.owner_identity, node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("implementation-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("implementation-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.class_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.realization_identity_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("class-to-superclass", node.owner_identity,
                         node.super_class_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity;
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.metaclass_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty();
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("metaclass-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.class_owner_identity,
                       node.owner_identity);
  }

  for (const auto &node : graph.protocol_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.protocol_inheritance_identity_closure_complete =
        graph.protocol_inheritance_identity_closure_complete &&
        node.inherited_protocol_identity_complete &&
        std::all_of(
            node.inherited_protocol_owner_identities_lexicographic.begin(),
            node.inherited_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("protocol-to-inherited-protocol",
                                    node.owner_identity,
                                    target_owner_identity);
            });
  }

  for (const auto &node : graph.category_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.category_attachment_identity_closure_complete =
        graph.category_attachment_identity_closure_complete &&
        node.attachment_identity_complete &&
        has_graph_edge("category-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        (!node.has_interface ||
         has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) &&
        (!node.has_implementation ||
         has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity));
    graph.protocol_category_conformance_identity_closure_complete =
        graph.protocol_category_conformance_identity_closure_complete &&
        node.conformance_identity_complete &&
        std::all_of(
            node.adopted_protocol_owner_identities_lexicographic.begin(),
            node.adopted_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("category-to-protocol", node.owner_identity,
                                    target_owner_identity);
            });
  }

  graph.deterministic =
      std::is_sorted(graph.interface_nodes_lexicographic.begin(),
                     graph.interface_nodes_lexicographic.end(),
                     IsExecutableMetadataInterfaceNodeLess) &&
      std::is_sorted(graph.implementation_nodes_lexicographic.begin(),
                     graph.implementation_nodes_lexicographic.end(),
                     IsExecutableMetadataImplementationNodeLess) &&
      std::is_sorted(graph.class_nodes_lexicographic.begin(),
                     graph.class_nodes_lexicographic.end(),
                     IsExecutableMetadataClassNodeLess) &&
      std::is_sorted(graph.metaclass_nodes_lexicographic.begin(),
                     graph.metaclass_nodes_lexicographic.end(),
                     IsExecutableMetadataMetaclassNodeLess) &&
      std::is_sorted(graph.protocol_nodes_lexicographic.begin(),
                     graph.protocol_nodes_lexicographic.end(),
                     IsExecutableMetadataProtocolNodeLess) &&
      std::is_sorted(graph.category_nodes_lexicographic.begin(),
                     graph.category_nodes_lexicographic.end(),
                     IsExecutableMetadataCategoryNodeLess) &&
      std::is_sorted(graph.property_nodes_lexicographic.begin(),
                     graph.property_nodes_lexicographic.end(),
                     IsExecutableMetadataPropertyNodeLess) &&
      std::is_sorted(graph.method_nodes_lexicographic.begin(),
                     graph.method_nodes_lexicographic.end(),
                     IsExecutableMetadataMethodNodeLess) &&
      std::is_sorted(graph.ivar_nodes_lexicographic.begin(),
                     graph.ivar_nodes_lexicographic.end(),
                     IsExecutableMetadataIvarNodeLess) &&
      std::is_sorted(graph.owner_edges_lexicographic.begin(),
                     graph.owner_edges_lexicographic.end(),
                     IsExecutableMetadataGraphEdgeLess);
  graph.source_graph_complete =
      graph.deterministic && interface_count_aligned &&
      implementation_count_aligned && metaclass_count_aligned &&
      class_node_floor_satisfied && protocol_count_aligned &&
      category_count_aligned && property_count_aligned &&
      method_count_aligned && ivar_count_aligned &&
      graph.class_metaclass_declaration_closure_complete &&
      graph.class_metaclass_parent_identity_closure_complete &&
      graph.class_metaclass_method_owner_identity_closure_complete &&
      graph.class_metaclass_object_identity_closure_complete &&
      graph.protocol_category_declaration_closure_complete &&
      graph.protocol_inheritance_identity_closure_complete &&
      graph.category_attachment_identity_closure_complete &&
      graph.protocol_category_conformance_identity_closure_complete;
  graph.ready_for_semantic_closure = graph.source_graph_complete;
  graph.ready_for_lowering = false;
  return graph;
}

Objc3ExecutableMetadataSemanticConsistencyBoundary
BuildExecutableMetadataSemanticConsistencyBoundary(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary) {
  Objc3ExecutableMetadataSemanticConsistencyBoundary boundary;
  boundary.executable_metadata_source_graph_contract_id = graph.contract_id;
  boundary.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  boundary.protocol_category_handoff_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary
          .deterministic_scope_resolution_handoff;
  boundary.protocol_node_count = graph.protocol_nodes_lexicographic.size();
  boundary.category_node_count = graph.category_nodes_lexicographic.size();
  boundary.property_node_count = graph.property_nodes_lexicographic.size();
  boundary.method_node_count = graph.method_nodes_lexicographic.size();
  boundary.ivar_node_count = graph.ivar_nodes_lexicographic.size();
  boundary.owner_edge_count = graph.owner_edges_lexicographic.size();

  const auto has_graph_edge =
      [&graph](const std::string &edge_kind, const std::string &source,
               const std::string &target) {
        return std::any_of(
            graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            [&](const Objc3ExecutableMetadataGraphEdge &edge) {
              return edge.edge_kind == edge_kind &&
                     edge.source_owner_identity == source &&
                     edge.target_owner_identity == target;
            });
      };

  boundary.protocol_inheritance_edges_complete = true;
  for (const auto &node : graph.protocol_nodes_lexicographic) {
    for (const auto &target :
         node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("protocol-to-inherited-protocol", node.owner_identity,
                          target)) {
        boundary.protocol_inheritance_edges_complete = false;
      }
    }
  }

  boundary.category_attachment_edges_complete = true;
  for (const auto &node : graph.category_nodes_lexicographic) {
    if (!has_graph_edge("category-to-class", node.owner_identity,
                        node.class_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_interface &&
        !has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_implementation &&
        !has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    for (const auto &target :
         node.adopted_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("category-to-protocol", node.owner_identity, target)) {
        boundary.category_attachment_edges_complete = false;
      }
    }
  }

  boundary.declaration_export_owner_split_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.property_name.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.method_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.selector.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.property_owner_identity.empty() ||
        node.owner_kind.empty() || node.owner_name.empty() ||
        node.ivar_binding_symbol.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }

  boundary.property_method_ivar_owner_edges_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (!has_graph_edge("property-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("property-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
    if (!node.ivar_binding_symbol.empty()) {
      const auto ivar_it = std::find_if(
          graph.ivar_nodes_lexicographic.begin(),
          graph.ivar_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataIvarGraphNode &ivar_node) {
            return ivar_node.property_owner_identity == node.owner_identity &&
                   ivar_node.ivar_binding_symbol == node.ivar_binding_symbol;
          });
      if (ivar_it == graph.ivar_nodes_lexicographic.end() ||
          !has_graph_edge("property-to-ivar", node.owner_identity,
                          ivar_it->owner_identity)) {
        boundary.property_method_ivar_owner_edges_complete = false;
      }
    }
    if (node.has_getter) {
      const bool getter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.getter_selector;
          });
      if (getter_exists) {
        const auto getter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.getter_selector &&
                     has_graph_edge("property-to-getter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (getter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
    if (node.has_setter) {
      const bool setter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.setter_selector;
          });
      if (setter_exists) {
        const auto setter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.setter_selector &&
                     has_graph_edge("property-to-setter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (setter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
  }

  for (const auto &node : graph.method_nodes_lexicographic) {
    if (!has_graph_edge("method-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("method-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (!has_graph_edge("ivar-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("ivar-to-export-owner", node.owner_identity,
                        node.export_owner_identity) ||
        !has_graph_edge("ivar-to-property", node.owner_identity,
                        node.property_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  if (boundary.contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency contract id is empty";
  } else if (boundary.executable_metadata_source_graph_contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency graph contract id is empty";
  } else if (!boundary.source_graph_ready) {
    boundary.failure_reason =
        "executable metadata source graph is not ready";
  } else if (!boundary.protocol_category_handoff_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason =
        "property attribute handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.protocol_inheritance_edges_complete) {
    boundary.failure_reason =
        "protocol inheritance edges are incomplete";
  } else if (!boundary.category_attachment_edges_complete) {
    boundary.failure_reason =
        "category attachment edges are incomplete";
  } else if (!boundary.declaration_export_owner_split_complete) {
    boundary.failure_reason =
        "declaration/export owner identities are incomplete";
  } else if (!boundary.property_method_ivar_owner_edges_complete) {
    boundary.failure_reason =
        "property/method/ivar owner edges are incomplete";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.lowering_admission_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.lowering_admission_ready &&
      boundary.semantic_conflict_diagnostics_enforcement_pending &&
      boundary.duplicate_export_owner_enforcement_pending &&
      boundary.lowering_admission_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "metadata semantic consistency freeze is not fail-closed";
  }
  return boundary;
}

Objc3ExecutableMetadataSemanticValidationSurface
BuildExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary) {
  Objc3ExecutableMetadataSemanticValidationSurface surface;
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);

  const Objc3MethodLookupOverrideConflictSummary
      &method_lookup_override_conflict_summary =
          sema_type_metadata_handoff.method_lookup_override_conflict_summary;
  surface.method_lookup_override_conflict_handoff_deterministic =
      method_lookup_override_conflict_summary.deterministic;
  surface.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;

  surface.override_lookup_sites =
      method_lookup_override_conflict_summary.override_lookup_sites;
  surface.override_lookup_hits =
      method_lookup_override_conflict_summary.override_lookup_hits;
  surface.override_lookup_misses =
      method_lookup_override_conflict_summary.override_lookup_misses;
  surface.override_conflicts =
      method_lookup_override_conflict_summary.override_conflicts;
  surface.unresolved_base_interfaces =
      method_lookup_override_conflict_summary.unresolved_base_interfaces;
  surface.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  surface.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  surface.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  surface.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  surface.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;

  const auto count_edges = [&](const std::string &edge_kind) {
    return static_cast<std::size_t>(std::count_if(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind;
        }));
  };
  const auto has_edge = [&](const std::string &edge_kind,
                            const std::string &source_owner_identity,
                            const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  surface.class_inheritance_edge_count = count_edges("class-to-superclass");
  surface.protocol_inheritance_edge_count =
      count_edges("protocol-to-inherited-protocol");
  surface.metaclass_super_edge_count =
      count_edges("metaclass-to-super-metaclass");
  surface.override_edge_count = count_edges("method-to-overridden-method");

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataMetaclassGraphNode *>
      metaclass_nodes_by_owner_identity;
  metaclass_nodes_by_owner_identity.reserve(
      graph.metaclass_nodes_lexicographic.size());
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    metaclass_nodes_by_owner_identity.emplace(metaclass_node.owner_identity,
                                              &metaclass_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataProtocolGraphNode *>
      protocol_nodes_by_owner_identity;
  protocol_nodes_by_owner_identity.reserve(
      graph.protocol_nodes_lexicographic.size());
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    protocol_nodes_by_owner_identity.emplace(protocol_node.owner_identity,
                                             &protocol_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataMethodGraphNode *>
      class_interface_methods_by_key;
  class_interface_methods_by_key.reserve(graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_methods_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        &method_node);
  }

  surface.class_inheritance_edges_complete = true;
  surface.inheritance_chain_cycle_free = true;
  surface.superclass_targets_resolved = true;
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    if (!class_node.has_super) {
      continue;
    }
    if (!has_edge("class-to-superclass", class_node.owner_identity,
                  class_node.super_class_owner_identity)) {
      surface.class_inheritance_edges_complete = false;
    }
    std::string next_super_owner_identity = class_node.super_class_owner_identity;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        surface.inheritance_chain_cycle_free = false;
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end()) {
        surface.superclass_targets_resolved = false;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
  }

  surface.protocol_inheritance_edges_complete = true;
  surface.protocol_inheritance_targets_resolved = true;
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    for (const auto &target_owner_identity :
         protocol_node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_edge("protocol-to-inherited-protocol",
                    protocol_node.owner_identity, target_owner_identity)) {
        surface.protocol_inheritance_edges_complete = false;
      }
      if (protocol_nodes_by_owner_identity.find(target_owner_identity) ==
          protocol_nodes_by_owner_identity.end()) {
        surface.protocol_inheritance_targets_resolved = false;
      }
    }
  }

  surface.metaclass_edges_complete = true;
  surface.metaclass_targets_resolved = true;
  surface.metaclass_lineage_aligned = true;
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    const auto class_it =
        class_nodes_by_owner_identity.find(metaclass_node.class_owner_identity);
    if (class_it == class_nodes_by_owner_identity.end()) {
      surface.metaclass_targets_resolved = false;
      surface.metaclass_lineage_aligned = false;
      continue;
    }
    if (!has_edge("class-to-metaclass", metaclass_node.class_owner_identity,
                  metaclass_node.owner_identity)) {
      surface.metaclass_edges_complete = false;
    }
    if (class_it->second->metaclass_owner_identity != metaclass_node.owner_identity) {
      surface.metaclass_lineage_aligned = false;
    }
    if (metaclass_node.has_super) {
      if (!has_edge("metaclass-to-super-metaclass", metaclass_node.owner_identity,
                    metaclass_node.super_metaclass_owner_identity)) {
        surface.metaclass_edges_complete = false;
      }
      const auto super_metaclass_it =
          metaclass_nodes_by_owner_identity.find(
              metaclass_node.super_metaclass_owner_identity);
      if (super_metaclass_it == metaclass_nodes_by_owner_identity.end()) {
        surface.metaclass_targets_resolved = false;
        surface.metaclass_lineage_aligned = false;
      } else if (class_it->second->has_super &&
                 super_metaclass_it->second->class_owner_identity !=
                     class_it->second->super_class_owner_identity) {
        surface.metaclass_lineage_aligned = false;
      }
    } else if (!metaclass_node.super_metaclass_owner_identity.empty()) {
      surface.metaclass_lineage_aligned = false;
    }
  }

  surface.method_override_edges_complete = true;
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }

    std::string next_super_owner_identity =
        interface_it->second->super_class_owner_identity;
    const Objc3ExecutableMetadataMethodGraphNode *overridden_method = nullptr;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end() ||
          class_it->second->interface_owner_identity.empty()) {
        break;
      }
      const auto base_method_it = class_interface_methods_by_key.find(
          build_interface_method_key(class_it->second->interface_owner_identity,
                                     method_node.selector,
                                     method_node.is_class_method));
      if (base_method_it != class_interface_methods_by_key.end()) {
        overridden_method = base_method_it->second;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
    if (overridden_method == nullptr) {
      continue;
    }
    if (!has_edge("method-to-overridden-method", method_node.owner_identity,
                  overridden_method->owner_identity)) {
      surface.method_override_edges_complete = false;
    } else if (method_node.is_class_method) {
      ++surface.class_method_override_edge_count;
    } else {
      ++surface.instance_method_override_edge_count;
    }
  }

  surface.override_lookup_complete =
      method_lookup_override_conflict_summary.unresolved_base_interfaces == 0u;
  surface.override_conflicts_absent =
      method_lookup_override_conflict_summary.override_conflicts == 0u;
  surface.protocol_composition_valid =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          0u &&
      class_protocol_category_linking_summary.protocol_composition_symbols >=
          class_protocol_category_linking_summary
              .category_composition_symbols &&
      class_protocol_category_linking_summary.protocol_composition_sites >=
          class_protocol_category_linking_summary.category_composition_sites;

  surface.inheritance_validation_ready =
      surface.class_inheritance_edges_complete &&
      surface.protocol_inheritance_edges_complete &&
      surface.inheritance_chain_cycle_free &&
      surface.superclass_targets_resolved &&
      surface.protocol_inheritance_targets_resolved;
  surface.override_validation_ready =
      surface.method_lookup_override_conflict_handoff_deterministic &&
      surface.method_override_edges_complete &&
      surface.override_lookup_complete &&
      surface.override_conflicts_absent;
  surface.protocol_composition_validation_ready =
      surface.class_protocol_category_linking_deterministic &&
      surface.protocol_composition_valid;
  surface.metaclass_relationship_validation_ready =
      surface.metaclass_edges_complete &&
      surface.metaclass_targets_resolved &&
      surface.metaclass_lineage_aligned;

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic validation contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic consistency dependency contract id is empty";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.method_lookup_override_conflict_handoff_deterministic) {
    surface.failure_reason =
        "method lookup/override conflict handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_deterministic) {
    surface.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!surface.inheritance_validation_ready) {
    surface.failure_reason =
        "inheritance validation is incomplete";
  } else if (!surface.override_validation_ready) {
    surface.failure_reason = "override validation is incomplete";
  } else if (!surface.protocol_composition_validation_ready) {
    surface.failure_reason =
        "protocol composition validation is incomplete";
  } else if (!surface.metaclass_relationship_validation_ready) {
    surface.failure_reason =
        "metaclass relationship validation is incomplete";
  }

  surface.semantic_validation_complete = surface.failure_reason.empty();
  surface.lowering_admission_ready = false;
  surface.fail_closed = surface.semantic_validation_complete &&
                        !surface.lowering_admission_ready;
  if (surface.failure_reason.empty() && !surface.fail_closed) {
    surface.failure_reason =
        "executable metadata semantic validation surface is not fail-closed";
  }
  return surface;
}

Objc3RuntimeMetadataSourceOwnershipBoundary BuildRuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3RuntimeMetadataSourceOwnershipBoundary boundary;
  const std::size_t sema_interface_implementation_record_count =
      type_metadata_handoff.interfaces_lexicographic.size() +
      type_metadata_handoff.implementations_lexicographic.size();
  const bool sema_interface_implementation_record_count_present =
      sema_interface_implementation_record_count > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_interfaces > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_implementations > 0u;

  boundary.frontend_owns_runtime_metadata_source_records = true;
  boundary.runtime_metadata_source_records_ready_for_lowering = false;
  boundary.native_runtime_library_present = false;
  boundary.runtime_link_test_only = true;
  boundary.class_record_count = records.classes_lexicographic.size();
  boundary.protocol_record_count = records.protocols_lexicographic.size();
  boundary.category_interface_record_count =
      static_cast<std::size_t>(std::count_if(records.categories_lexicographic.begin(),
                                             records.categories_lexicographic.end(),
                                             [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
                                               return record.record_kind == "interface";
                                             }));
  boundary.category_implementation_record_count =
      static_cast<std::size_t>(std::count_if(records.categories_lexicographic.begin(),
                                             records.categories_lexicographic.end(),
                                             [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
                                               return record.record_kind == "implementation";
                                             }));
  boundary.property_record_count = records.properties_lexicographic.size();
  boundary.method_record_count = records.methods_lexicographic.size();
  boundary.ivar_record_count = records.ivars_lexicographic.size();

  // diagnostic precision anchor: the semantic type-metadata handoff
  // now treats category containers as a separate runtime-metadata concern so
  // valid class-plus-category programs do not collapse into duplicate
  // class-owner diagnostics. The ownership boundary therefore validates the
  // sema surface against class records only; category records remain parser/AST
  // owned until later runtime-metadata milestones wire a dedicated sema lane.
  const std::size_t source_interface_implementation_record_count =
      boundary.class_record_count;
  const bool class_alignment_consistent =
      !sema_interface_implementation_record_count_present ||
      sema_interface_implementation_record_count ==
          source_interface_implementation_record_count;
  boundary.deterministic_source_schema =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(records) &&
      class_alignment_consistent &&
      boundary.ivar_record_count <= boundary.property_record_count &&
      !boundary.contract_id.empty() &&
      !boundary.canonical_source_schema.empty() &&
      !boundary.class_record_ast_anchor.empty() &&
      !boundary.protocol_record_ast_anchor.empty() &&
      !boundary.category_record_ast_anchor.empty() &&
      !boundary.property_record_ast_anchor.empty() &&
      !boundary.method_record_ast_anchor.empty() &&
      !boundary.ivar_record_ast_anchor.empty() &&
      !boundary.ivar_record_source_model.empty();
  boundary.fail_closed =
      boundary.frontend_owns_runtime_metadata_source_records &&
      !boundary.runtime_metadata_source_records_ready_for_lowering &&
      !boundary.native_runtime_library_present &&
      boundary.runtime_link_test_only;

  if (!class_alignment_consistent) {
    boundary.failure_reason = "AST/sema class metadata source counts diverged";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason = "ivar source records exceed property source records";
  } else if (!boundary.deterministic_source_schema) {
    boundary.failure_reason = "runtime metadata source schema anchors are incomplete";
  } else if (!boundary.fail_closed) {
    boundary.failure_reason = "runtime metadata source ownership boundary is not fail-closed";
  }

  return boundary;
}

Objc3RuntimeExportLegalityBoundary BuildRuntimeExportLegalityBoundary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary &runtime_metadata_source_ownership,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RuntimeExportLegalityBoundary boundary;
  boundary.semantic_integration_surface_built = integration_surface.built;
  boundary.sema_type_metadata_handoff_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      sema_parity_surface.deterministic_type_metadata_handoff;
  boundary.typed_sema_surface_ready =
      typed_surface.semantic_integration_surface_built;
  boundary.typed_sema_surface_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      typed_surface.protocol_category_handoff_deterministic &&
      typed_surface.class_protocol_category_linking_handoff_deterministic &&
      typed_surface.selector_normalization_handoff_deterministic &&
      typed_surface.property_attribute_handoff_deterministic &&
      typed_surface.object_pointer_type_handoff_deterministic &&
      typed_surface.symbol_graph_handoff_deterministic &&
      typed_surface.scope_resolution_handoff_deterministic;
  boundary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  boundary.protocol_category_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.object_pointer_surface_deterministic =
      object_pointer_summary
          .deterministic_object_pointer_nullability_generics_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  boundary.property_synthesis_ivar_binding_deterministic =
      sema_parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;

  boundary.class_record_count = runtime_metadata_source_ownership.class_record_count;
  boundary.protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  boundary.category_record_count =
      runtime_metadata_source_ownership.category_record_count();
  boundary.property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  boundary.method_record_count = runtime_metadata_source_ownership.method_record_count;
  boundary.ivar_record_count = runtime_metadata_source_ownership.ivar_record_count;
  boundary.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  boundary.property_attribute_invalid_entries =
      sema_parity_surface.property_attribute_invalid_attribute_entries_total;
  boundary.property_attribute_contract_violations =
      sema_parity_surface.property_attribute_contract_violations_total;
  boundary.invalid_type_annotation_sites =
      sema_parity_surface.type_annotation_invalid_generic_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_pointer_declarator_sites_total +
      sema_parity_surface.type_annotation_invalid_nullability_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  boundary.property_ivar_binding_missing =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_missing;
  boundary.property_ivar_binding_conflicts =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_conflicts;
  boundary.implementation_resolution_misses =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  boundary.method_resolution_misses =
      symbol_graph_scope_resolution_summary.method_resolution_misses;

  if (boundary.contract_id.empty()) {
    boundary.failure_reason = "runtime export legality contract id is empty";
  } else if (!boundary.sema_type_metadata_handoff_deterministic) {
    boundary.failure_reason = typed_surface.failure_reason.empty()
                                  ? "semantic type-metadata handoff is not deterministic"
                                  : typed_surface.failure_reason;
  } else if (!boundary.typed_sema_surface_ready) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not ready";
  } else if (!boundary.typed_sema_surface_deterministic) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not deterministic";
  } else if (!boundary.runtime_metadata_source_boundary_ready) {
    boundary.failure_reason =
        "runtime metadata source ownership boundary is not ready";
  } else if (!boundary.protocol_category_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason = "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason = "property attribute handoff is not deterministic";
  } else if (!boundary.object_pointer_surface_deterministic) {
    boundary.failure_reason =
        "object-pointer/nullability/generics handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.property_synthesis_ivar_binding_deterministic) {
    boundary.failure_reason =
        "property synthesis/ivar binding handoff is not deterministic";
  } else if (boundary.invalid_protocol_composition_sites >
             boundary.protocol_record_count + boundary.category_record_count) {
    boundary.failure_reason =
        "invalid protocol composition sites exceed export-bearing records";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason =
        "ivar export records exceed property export records";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.metadata_export_enforcement_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.metadata_export_enforcement_ready &&
      boundary.duplicate_runtime_identity_enforcement_pending &&
      boundary.incomplete_declaration_export_blocking_pending &&
      boundary.illegal_redeclaration_mix_export_blocking_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "runtime export legality freeze is not fail-closed";
  }
  return boundary;
}

struct Objc3RuntimeExportViolationAccumulator {
  std::size_t count = 0;
  unsigned first_line = 1;
  unsigned first_column = 1;
  bool has_location = false;

  void Add(std::size_t increment, unsigned line, unsigned column) {
    if (increment == 0) {
      return;
    }
    if (!has_location) {
      first_line = line;
      first_column = column;
      has_location = true;
    }
    count += increment;
  }

  void Merge(const Objc3RuntimeExportViolationAccumulator &other) {
    if (other.count == 0) {
      return;
    }
    Add(other.count, other.first_line, other.first_column);
  }
};

template <typename Record, typename KeyBuilder>
Objc3RuntimeExportViolationAccumulator CountDuplicateRuntimeExportIdentitySites(
    const std::vector<Record> &records,
    KeyBuilder build_key) {
  Objc3RuntimeExportViolationAccumulator violations;
  std::unordered_map<std::string, std::size_t> seen;
  seen.reserve(records.size());
  for (const auto &record : records) {
    std::size_t &count = seen[build_key(record)];
    if (count > 0u) {
      violations.Add(1u, record.line, record.column);
    }
    ++count;
  }
  return violations;
}

struct Objc3RuntimeExportPairPresence {
  std::size_t interface_records = 0;
  std::size_t implementation_records = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

struct Objc3RuntimeExportBlockingDiagnostic {
  unsigned line = 1;
  unsigned column = 1;
  std::string code;
  std::string message;
};

bool IsInterfaceRuntimePropertyOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" || owner_kind == "category-interface";
}

bool IsImplementationRuntimePropertyOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-implementation" || owner_kind == "category-implementation";
}

bool IsInterfaceRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" || owner_kind == "category-interface";
}

bool IsImplementationRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-implementation" || owner_kind == "category-implementation";
}

bool AreCompatibleRuntimePropertyRedeclarations(
    const Objc3RuntimeMetadataPropertySourceRecord &interface_record,
    const Objc3RuntimeMetadataPropertySourceRecord &implementation_record) {
  return interface_record.type_name == implementation_record.type_name &&
         interface_record.has_getter == implementation_record.has_getter &&
         interface_record.getter_selector == implementation_record.getter_selector &&
         interface_record.has_setter == implementation_record.has_setter &&
         interface_record.setter_selector == implementation_record.setter_selector &&
         interface_record.executable_synthesized_binding_kind ==
             implementation_record.executable_synthesized_binding_kind &&
         interface_record.property_attribute_profile ==
             implementation_record.property_attribute_profile &&
         interface_record.ownership_lifetime_profile ==
             implementation_record.ownership_lifetime_profile &&
         interface_record.ownership_runtime_hook_profile ==
             implementation_record.ownership_runtime_hook_profile &&
         interface_record.effective_getter_selector ==
             implementation_record.effective_getter_selector &&
         interface_record.effective_setter_available ==
             implementation_record.effective_setter_available &&
         interface_record.effective_setter_selector ==
             implementation_record.effective_setter_selector &&
         interface_record.accessor_ownership_profile ==
             implementation_record.accessor_ownership_profile &&
         interface_record.executable_ivar_layout_symbol ==
             implementation_record.executable_ivar_layout_symbol &&
         interface_record.executable_ivar_layout_slot_index ==
             implementation_record.executable_ivar_layout_slot_index &&
         interface_record.executable_ivar_layout_size_bytes ==
             implementation_record.executable_ivar_layout_size_bytes &&
         interface_record.executable_ivar_layout_alignment_bytes ==
             implementation_record.executable_ivar_layout_alignment_bytes &&
         interface_record.executable_ivar_layout_offset_bytes ==
             implementation_record.executable_ivar_layout_offset_bytes &&
         interface_record.executable_ivar_layout_padding_bytes ==
             implementation_record.executable_ivar_layout_padding_bytes &&
         interface_record.executable_ivar_layout_inherited_slot_count ==
             implementation_record.executable_ivar_layout_inherited_slot_count &&
         interface_record.executable_ivar_layout_inherited_size_bytes ==
             implementation_record.executable_ivar_layout_inherited_size_bytes &&
         interface_record.executable_ivar_layout_owner_size_bytes ==
             implementation_record.executable_ivar_layout_owner_size_bytes &&
         interface_record.executable_ivar_layout_valid ==
             implementation_record.executable_ivar_layout_valid &&
         interface_record.executable_ivar_layout_replay_key ==
             implementation_record.executable_ivar_layout_replay_key;
}

bool AreCompatibleRuntimeMethodRedeclarations(
    const Objc3RuntimeMetadataMethodSourceRecord &interface_record,
    const Objc3RuntimeMetadataMethodSourceRecord &implementation_record) {
  return interface_record.is_class_method == implementation_record.is_class_method &&
         interface_record.selector == implementation_record.selector &&
         interface_record.effective_direct_dispatch ==
             implementation_record.effective_direct_dispatch &&
         interface_record.objc_final_declared ==
             implementation_record.objc_final_declared &&
         interface_record.parameter_count == implementation_record.parameter_count &&
         interface_record.return_type_name == implementation_record.return_type_name &&
         !interface_record.has_body && implementation_record.has_body;
}

std::vector<Objc3RuntimeExportBlockingDiagnostic>
BuildRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary) {
  std::vector<Objc3RuntimeExportBlockingDiagnostic> diagnostics;
  struct Objc3RuntimeExportDuplicateSite {
    std::size_t count = 0;
    unsigned line = 1;
    unsigned column = 1;
    bool has_location = false;
  };
  const auto push_diagnostic =
      [&diagnostics](unsigned line, unsigned column, const std::string &code,
                     const std::string &message) {
        diagnostics.push_back({line, column, code, message});
      };
  const auto pluralize = [](std::size_t count, const std::string &singular,
                            const std::string &plural) {
    return count == 1u ? singular : plural;
  };

  if (summary.duplicate_runtime_identity_sites > 0u) {
    {
      std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
          category_presence;
      category_presence.reserve(records.categories_lexicographic.size());
      for (const auto &record : records.categories_lexicographic) {
        const std::string owner_name =
            BuildCategoryOwnerName(record.class_name, record.category_name);
        Objc3RuntimeExportPairPresence &presence = category_presence[owner_name];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        if (record.record_kind == "interface") {
          ++presence.interface_records;
        } else if (record.record_kind == "implementation") {
          ++presence.implementation_records;
        }
      }
      for (const auto &[owner_name, presence] : category_presence) {
        if (presence.interface_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S261",
              "runtime metadata export blocked: category attachment collision: "
              "category '" +
                  owner_name + "' has multiple @interface declarations");
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: category '" +
                  owner_name + "' has multiple @interface attachment "
                                "candidates");
        }
        if (presence.implementation_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S261",
              "runtime metadata export blocked: category attachment collision: "
              "category '" +
                  owner_name + "' has multiple @implementation declarations");
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: category '" +
                  owner_name + "' has multiple @implementation attachment "
                                "candidates");
        }
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
          class_presence;
      class_presence.reserve(records.classes_lexicographic.size());
      for (const auto &record : records.classes_lexicographic) {
        Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        if (record.record_kind == "interface") {
          ++presence.interface_records;
        } else if (record.record_kind == "implementation") {
          ++presence.implementation_records;
        }
      }
      for (const auto &[name, presence] : class_presence) {
        if (presence.interface_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: class '" +
                  name + "' has multiple @interface declarations");
        }
        if (presence.implementation_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: class '" +
                  name + "' has multiple @implementation declarations");
        }
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          property_presence;
      property_presence.reserve(records.properties_lexicographic.size());
      for (const auto &record : records.properties_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" + record.property_name;
        Objc3RuntimeExportDuplicateSite &presence = property_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : property_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string property_name = key.substr(second_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: "
            "property '" +
                property_name + "' in " + owner_kind + " '" + owner_name +
                "' has " + std::to_string(presence.count) + " export " +
                pluralize(presence.count, "record", "records"));
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          method_presence;
      method_presence.reserve(records.methods_lexicographic.size());
      for (const auto &record : records.methods_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" +
                                (record.is_class_method ? "+" : "-") + "\n" +
                                record.selector;
        Objc3RuntimeExportDuplicateSite &presence = method_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : method_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::size_t third_break = key.find('\n', second_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string polarity =
            key.substr(second_break + 1u, third_break - second_break - 1u);
        const std::string selector = key.substr(third_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: " +
                std::string(polarity == "+" ? "class" : "instance") +
                " selector '" + selector + "' in " + owner_kind + " '" +
                owner_name + "' has " + std::to_string(presence.count) +
                " export " + pluralize(presence.count, "record", "records"));
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          ivar_presence;
      ivar_presence.reserve(records.ivars_lexicographic.size());
      for (const auto &record : records.ivars_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" + record.ivar_binding_symbol;
        Objc3RuntimeExportDuplicateSite &presence = ivar_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : ivar_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string ivar_symbol = key.substr(second_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: "
            "ivar '" +
                ivar_symbol + "' in " + owner_kind + " '" + owner_name +
                "' has " + std::to_string(presence.count) + " export " +
                pluralize(presence.count, "record", "records"));
      }
    }
  }

  if (summary.incomplete_declaration_sites > 0u) {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> class_presence;
    class_presence.reserve(records.classes_lexicographic.size());
    for (const auto &record : records.classes_lexicographic) {
      Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &[name, presence] : class_presence) {
      if (presence.interface_records > 0u && presence.implementation_records == 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: interface '" +
                 name + "' is missing a matching @implementation"});
      } else if (presence.interface_records == 0u &&
                 presence.implementation_records > 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: implementation '" +
                 name + "' is missing a matching @interface"});
      }
    }
  }

  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
        category_presence;
    category_presence.reserve(records.categories_lexicographic.size());
    for (const auto &record : records.categories_lexicographic) {
      const std::string owner_name =
          BuildCategoryOwnerName(record.class_name, record.category_name);
      Objc3RuntimeExportPairPresence &presence = category_presence[owner_name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &[owner_name, presence] : category_presence) {
      if (presence.interface_records > 0u && presence.implementation_records == 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: category '" +
                 owner_name + "' is missing a matching @implementation"});
      } else if (presence.interface_records == 0u &&
                 presence.implementation_records > 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: category '" +
                 owner_name + "' is missing a matching @interface"});
      }
    }
  }

  if (summary.duplicate_runtime_identity_sites > 0u &&
      diagnostics.empty()) {
    push_diagnostic(summary.first_failure_line, summary.first_failure_column,
                    "O3S262",
                    "runtime metadata export blocked: duplicate runtime "
                    "metadata identities are not exportable");
  }

  std::sort(diagnostics.begin(), diagnostics.end(),
            [](const Objc3RuntimeExportBlockingDiagnostic &lhs,
               const Objc3RuntimeExportBlockingDiagnostic &rhs) {
              return std::tie(lhs.line, lhs.column, lhs.code, lhs.message) <
                     std::tie(rhs.line, rhs.column, rhs.code, rhs.message);
            });
  return diagnostics;
}

bool HasRuntimeMetadataSourceRecords(const Objc3RuntimeMetadataSourceRecordSet &records) {
  return !records.classes_lexicographic.empty() ||
         !records.protocols_lexicographic.empty() ||
         !records.categories_lexicographic.empty() ||
         !records.properties_lexicographic.empty() ||
         !records.methods_lexicographic.empty() ||
         !records.ivars_lexicographic.empty();
}

Objc3RuntimeExportEnforcementSummary BuildRuntimeExportEnforcementSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  Objc3RuntimeExportEnforcementSummary summary;
  summary.metadata_completeness_enforced = true;
  summary.duplicate_runtime_identity_suppression_enforced = true;
  summary.illegal_redeclaration_mix_blocking_enforced = true;
  summary.metadata_shape_drift_blocking_enforced = true;
  summary.fail_closed = true;

  Objc3RuntimeExportViolationAccumulator duplicate_violations;
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind + "\n" + record.name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.categories_lexicographic,
      [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
        return record.record_kind + "\n" + record.class_name + "\n" +
               record.category_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.properties_lexicographic,
      [](const Objc3RuntimeMetadataPropertySourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.property_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.methods_lexicographic,
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               (record.is_class_method ? "+" : "-") + "\n" + record.selector;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.ivars_lexicographic,
      [](const Objc3RuntimeMetadataIvarSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.ivar_binding_symbol;
      }));
  {
    std::unordered_map<std::string, std::size_t> seen_protocols;
    seen_protocols.reserve(records.protocols_lexicographic.size());
    for (const auto &record : records.protocols_lexicographic) {
      if (record.is_forward_declaration) {
        continue;
      }
      std::size_t &count = seen_protocols[record.name];
      if (count > 0u) {
        duplicate_violations.Add(1u, record.line, record.column);
      }
      ++count;
    }
  }
  summary.duplicate_runtime_identity_sites = duplicate_violations.count;

  Objc3RuntimeExportViolationAccumulator incomplete_violations;
  // Forward protocol declarations are dependency hints for later complete
  // protocol records or composition spelling; they are not themselves
  // exportable runtime metadata units and must not block the runnable path.
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> class_presence;
    class_presence.reserve(records.classes_lexicographic.size());
    for (const auto &record : records.classes_lexicographic) {
      Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : class_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u || presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> category_presence;
    category_presence.reserve(records.categories_lexicographic.size());
    for (const auto &record : records.categories_lexicographic) {
      const std::string key = record.class_name + "\n" + record.category_name;
      Objc3RuntimeExportPairPresence &presence = category_presence[key];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : category_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u || presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  incomplete_violations.count +=
      runtime_export_legality.implementation_resolution_misses +
      runtime_export_legality.method_resolution_misses +
      runtime_export_legality.property_ivar_binding_missing;
  summary.incomplete_declaration_sites = incomplete_violations.count;

  Objc3RuntimeExportViolationAccumulator illegal_redeclaration_violations;
  {
    struct RuntimePropertyRedeclarationPair {
      const Objc3RuntimeMetadataPropertySourceRecord *interface_record = nullptr;
      const Objc3RuntimeMetadataPropertySourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimePropertyRedeclarationPair>
        property_pairs;
    property_pairs.reserve(records.properties_lexicographic.size());
    for (const auto &record : records.properties_lexicographic) {
      if (!IsInterfaceRuntimePropertyOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" + record.property_name;
      RuntimePropertyRedeclarationPair &pair = property_pairs[key];
      if (IsInterfaceRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : property_pairs) {
      const RuntimePropertyRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr || pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimePropertyRedeclarations(*pair.interface_record,
                                                      *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  {
    struct RuntimeMethodRedeclarationPair {
      const Objc3RuntimeMetadataMethodSourceRecord *interface_record = nullptr;
      const Objc3RuntimeMetadataMethodSourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimeMethodRedeclarationPair> method_pairs;
    method_pairs.reserve(records.methods_lexicographic.size());
    for (const auto &record : records.methods_lexicographic) {
      if (!IsInterfaceRuntimeMethodOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" +
                              (record.is_class_method ? "+" : "-") + "\n" +
                              record.selector;
      RuntimeMethodRedeclarationPair &pair = method_pairs[key];
      if (IsInterfaceRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : method_pairs) {
      const RuntimeMethodRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr || pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimeMethodRedeclarations(*pair.interface_record,
                                                    *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  summary.illegal_redeclaration_mix_sites =
      illegal_redeclaration_violations.count +
      runtime_export_legality.invalid_protocol_composition_sites +
      runtime_export_legality.property_attribute_invalid_entries +
      runtime_export_legality.property_attribute_contract_violations +
      runtime_export_legality.invalid_type_annotation_sites +
      runtime_export_legality.property_ivar_binding_conflicts;

  summary.metadata_shape_drift_sites =
      (runtime_export_legality.semantic_integration_surface_built ? 0u : 1u) +
      (runtime_export_legality.sema_type_metadata_handoff_deterministic ? 0u : 1u) +
      (runtime_export_legality.typed_sema_surface_ready ? 0u : 1u) +
      (runtime_export_legality.typed_sema_surface_deterministic ? 0u : 1u) +
      (runtime_export_legality.runtime_metadata_source_boundary_ready ? 0u : 1u) +
      (runtime_export_legality.protocol_category_deterministic ? 0u : 1u) +
      (runtime_export_legality.class_protocol_category_linking_deterministic ? 0u : 1u) +
      (runtime_export_legality.selector_normalization_deterministic ? 0u : 1u) +
      (runtime_export_legality.property_attribute_deterministic ? 0u : 1u) +
      (runtime_export_legality.object_pointer_surface_deterministic ? 0u : 1u) +
      (runtime_export_legality.symbol_graph_scope_resolution_deterministic ? 0u : 1u) +
      (runtime_export_legality.property_synthesis_ivar_binding_deterministic ? 0u : 1u) +
      (runtime_export_legality.invalid_protocol_composition_sites <=
               runtime_export_legality.protocol_record_count +
                   runtime_export_legality.category_record_count
           ? 0u
           : 1u) +
      (runtime_export_legality.ivar_record_count <=
               runtime_export_legality.property_record_count
           ? 0u
           : 1u);

  summary.ready_for_runtime_export =
      summary.duplicate_runtime_identity_sites == 0u &&
      summary.incomplete_declaration_sites == 0u &&
      summary.illegal_redeclaration_mix_sites == 0u &&
      summary.metadata_shape_drift_sites == 0u;

  if (duplicate_violations.has_location) {
    summary.first_failure_line = duplicate_violations.first_line;
    summary.first_failure_column = duplicate_violations.first_column;
  } else if (incomplete_violations.has_location) {
    summary.first_failure_line = incomplete_violations.first_line;
    summary.first_failure_column = incomplete_violations.first_column;
  } else if (illegal_redeclaration_violations.has_location) {
    summary.first_failure_line = illegal_redeclaration_violations.first_line;
    summary.first_failure_column = illegal_redeclaration_violations.first_column;
  }

  if (summary.duplicate_runtime_identity_sites > 0u) {
    summary.failure_reason =
        "duplicate runtime metadata identities are not exportable";
  } else if (summary.incomplete_declaration_sites > 0u) {
    summary.failure_reason =
        "incomplete runtime metadata declarations are not exportable";
  } else if (summary.illegal_redeclaration_mix_sites > 0u) {
    summary.failure_reason =
        "illegal runtime metadata redeclaration mixes are not exportable";
  } else if (summary.metadata_shape_drift_sites > 0u) {
    summary.failure_reason =
        "runtime metadata export shape drift detected before lowering";
  }

  return summary;
}

Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
BuildMetaprogrammingMetaprogrammingSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary summary;

  for (const auto &fn : program.functions) {
    if (fn.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.objc_derive_declared) {
      ++summary.derive_marker_sites;
    }
    for (const auto &property : interface_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : interface_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : protocol_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : implementation.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }

  summary.derive_marker_source_supported = true;
  summary.macro_marker_source_supported = true;
  summary.property_behavior_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      objc3c::pipeline::orchestration::BuildMetaprogrammingMetaprogrammingSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
    if (decl.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (decl.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (decl.objc_macro_cache_key_declared) {
      ++summary.macro_cache_key_sites;
    }
    if (decl.objc_macro_sandbox_declared) {
      ++summary.macro_sandbox_policy_sites;
    }
    if (decl.objc_macro_declared && decl.objc_macro_package_declared &&
        decl.objc_macro_provenance_declared) {
      ++summary.expansion_visible_macro_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.macro_package_source_supported = true;
  summary.macro_provenance_source_supported = true;
  summary.macro_cache_key_source_supported = true;
  summary.macro_sandbox_policy_source_supported = true;
  summary.expansion_visible_source_supported = true;
  summary.deterministic_handoff =
      summary.expansion_visible_macro_sites <= summary.macro_marker_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_package_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_provenance_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      objc3c::pipeline::orchestration::BuildMetaprogrammingMacroPackageProvenanceSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(const Objc3Program &program) {
  Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary summary;

  const auto accumulate_property = [&summary](const Objc3PropertyDecl &property,
                                              const char *owner_kind) {
    if (!property.property_behavior_declared) {
      return;
    }
    ++summary.property_behavior_sites;
    const std::string kind(owner_kind);
    if (kind == "interface") {
      ++summary.interface_property_behavior_sites;
    } else if (kind == "implementation") {
      ++summary.implementation_property_behavior_sites;
    } else if (kind == "protocol") {
      ++summary.protocol_property_behavior_sites;
    }
    if (property.executable_synthesized_binding_kind == "implicit-ivar" &&
        !property.executable_synthesized_binding_symbol.empty()) {
      ++summary.synthesized_binding_visible_sites;
    }
    if (!property.effective_getter_selector.empty()) {
      ++summary.synthesized_getter_visible_sites;
    }
    if (property.effective_setter_available &&
        !property.effective_setter_selector.empty()) {
      ++summary.synthesized_setter_visible_sites;
    }
  };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &property : interface_decl.properties) {
      accumulate_property(property, "interface");
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      accumulate_property(property, "protocol");
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      accumulate_property(property, "implementation");
    }
  }

  summary.property_behavior_source_supported = true;
  summary.synthesized_declaration_visibility_supported = true;
  summary.deterministic_handoff =
      summary.interface_property_behavior_sites +
              summary.implementation_property_behavior_sites +
              summary.protocol_property_behavior_sites ==
          summary.property_behavior_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      objc3c::pipeline::orchestration::BuildMetaprogrammingPropertyBehaviorSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
BuildToolingDiagnosticsMigratorSourceInventorySummary(
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &error_handling_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &concurrency_async_summary,
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &concurrency_actor_summary,
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &concurrency_task_summary,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &ownership_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &ownership_cleanup_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_family_summary,
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary
        &dispatch_closure_summary,
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
        &dispatch_completion_summary,
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &metaprogramming_closure_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &metaprogramming_macro_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &metaprogramming_property_summary,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &interop_closure_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_completion_summary) {
  Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary summary;

  summary.advanced_feature_family_count = 6;
  summary.dependency_surface_count = summary.dependency_contract_ids.size();
  summary.aggregated_source_only_claim_count =
      error_handling_summary.source_only_claim_ids.size() +
      concurrency_async_summary.source_only_claim_ids.size() +
      concurrency_actor_summary.source_only_claim_ids.size() +
      concurrency_task_summary.source_only_claim_ids.size() +
      ownership_summary.source_only_claim_ids.size() +
      ownership_cleanup_summary.source_only_claim_ids.size() +
      ownership_family_summary.source_only_claim_ids.size() +
      dispatch_closure_summary.source_only_claim_ids.size() +
      dispatch_completion_summary.source_only_claim_ids.size() +
      metaprogramming_closure_summary.source_only_claim_ids.size() +
      metaprogramming_macro_summary.source_only_claim_ids.size() +
      metaprogramming_property_summary.source_only_claim_ids.size() +
      interop_closure_summary.source_only_claim_ids.size() +
      interop_completion_summary.source_only_claim_ids.size();
  summary.fail_closed_construct_count =
      error_handling_summary.fail_closed_construct_ids.size();
  summary.error_surface_sites =
      error_handling_summary.function_throws_declaration_sites +
      error_handling_summary.method_throws_declaration_sites +
      error_handling_summary.result_like_sites + error_handling_summary.result_payload_sites +
      error_handling_summary.ns_error_bridging_sites +
      error_handling_summary.ns_error_out_parameter_sites +
      error_handling_summary.ns_error_bridge_path_sites +
      error_handling_summary.objc_nserror_attribute_sites +
      error_handling_summary.objc_status_code_attribute_sites +
      error_handling_summary.try_keyword_sites + error_handling_summary.throw_keyword_sites +
      error_handling_summary.catch_keyword_sites;
  summary.concurrency_surface_sites =
      concurrency_async_summary.async_keyword_sites +
      concurrency_async_summary.await_keyword_sites +
      concurrency_async_summary.executor_attribute_sites +
      concurrency_actor_summary.actor_interface_sites +
      concurrency_actor_summary.actor_method_sites +
      concurrency_actor_summary.actor_property_sites +
      concurrency_actor_summary.objc_nonisolated_annotation_sites +
      concurrency_actor_summary.actor_member_executor_annotation_sites +
      concurrency_task_summary.task_creation_sites +
      concurrency_task_summary.task_group_scope_sites +
      concurrency_task_summary.task_group_add_task_sites +
      concurrency_task_summary.task_group_wait_next_sites +
      concurrency_task_summary.task_group_cancel_all_sites +
      concurrency_task_summary.cancellation_check_sites +
      concurrency_task_summary.cancellation_handler_sites;
  summary.system_surface_sites =
      ownership_summary.resource_attribute_sites +
      ownership_summary.resource_close_clause_sites +
      ownership_summary.borrowed_pointer_sites +
      ownership_summary.returns_borrowed_attribute_sites +
      ownership_summary.explicit_capture_list_sites +
      ownership_summary.explicit_capture_item_sites +
      ownership_cleanup_summary.cleanup_attribute_sites +
      ownership_cleanup_summary.cleanup_sugar_sites +
      ownership_cleanup_summary.resource_sugar_sites +
      ownership_family_summary.family_retain_sites +
      ownership_family_summary.family_release_sites +
      ownership_family_summary.family_autorelease_sites +
      ownership_family_summary.compatibility_returns_retained_sites +
      ownership_family_summary.compatibility_returns_not_retained_sites +
      ownership_family_summary.compatibility_consumed_sites;
  summary.dispatch_surface_sites =
      dispatch_closure_summary.direct_callable_sites +
      dispatch_closure_summary.final_callable_sites +
      dispatch_closure_summary.dynamic_callable_sites +
      dispatch_closure_summary.direct_members_container_sites +
      dispatch_closure_summary.final_container_sites +
      dispatch_closure_summary.sealed_container_sites +
      dispatch_completion_summary.prefixed_container_attribute_sites +
      dispatch_completion_summary.effective_direct_member_sites +
      dispatch_completion_summary.direct_members_defaulted_method_sites +
      dispatch_completion_summary.direct_members_dynamic_opt_out_sites;
  summary.metaprogramming_surface_sites =
      metaprogramming_closure_summary.derive_marker_sites +
      metaprogramming_closure_summary.macro_marker_sites +
      metaprogramming_closure_summary.property_behavior_sites +
      metaprogramming_macro_summary.macro_package_sites +
      metaprogramming_macro_summary.macro_provenance_sites +
      metaprogramming_macro_summary.expansion_visible_macro_sites +
      metaprogramming_property_summary.property_behavior_sites +
      metaprogramming_property_summary.synthesized_binding_visible_sites +
      metaprogramming_property_summary.synthesized_getter_visible_sites +
      metaprogramming_property_summary.synthesized_setter_visible_sites;
  summary.interop_surface_sites =
      interop_closure_summary.foreign_callable_sites +
      interop_closure_summary.import_module_annotation_sites +
      interop_closure_summary.imported_module_name_sites +
      interop_closure_summary.export_header_annotation_sites +
      interop_closure_summary.export_header_name_sites +
      interop_closure_summary.mixed_image_annotation_sites +
      interop_closure_summary.mixed_image_name_sites +
      interop_closure_summary.package_entry_annotation_sites +
      interop_closure_summary.package_entry_name_sites +
      interop_completion_summary.swift_name_annotation_sites +
      interop_completion_summary.swift_private_annotation_sites +
      interop_completion_summary.cpp_name_annotation_sites +
      interop_completion_summary.header_name_annotation_sites +
      interop_completion_summary.abi_alignment_annotation_sites +
      interop_completion_summary.foreign_type_annotation_sites +
      interop_completion_summary.named_annotation_payload_sites;
  summary.diagnostic_surface_sites =
      summary.error_surface_sites + summary.concurrency_surface_sites +
      summary.system_surface_sites + summary.dispatch_surface_sites +
      summary.metaprogramming_surface_sites + summary.interop_surface_sites;
  summary.fixit_surface_sites = summary.diagnostic_surface_sites;
  summary.migrator_surface_sites = summary.diagnostic_surface_sites;
  summary.canonicalization_hint_sites =
      canonical_literal_rejection_counts.total_literal_sites();

  const bool dependencies_ready =
      error_handling_summary.ready_for_semantic_expansion &&
      concurrency_async_summary.ready_for_semantic_expansion &&
      concurrency_actor_summary.ready_for_semantic_expansion &&
      concurrency_task_summary.ready_for_semantic_expansion &&
      ownership_summary.ready_for_semantic_expansion &&
      ownership_cleanup_summary.ready_for_semantic_expansion &&
      ownership_family_summary.ready_for_semantic_expansion &&
      dispatch_closure_summary.ready_for_semantic_expansion &&
      dispatch_completion_summary.ready_for_semantic_expansion &&
      metaprogramming_closure_summary.ready_for_semantic_expansion &&
      metaprogramming_macro_summary.ready_for_semantic_expansion &&
      metaprogramming_property_summary.ready_for_semantic_expansion &&
      interop_closure_summary.ready_for_semantic_expansion &&
      interop_completion_summary.ready_for_semantic_expansion;
  summary.diagnostics_inventory_source_supported = dependencies_ready;
  summary.fixit_inventory_source_supported = dependencies_ready;
  summary.migrator_inventory_source_supported = dependencies_ready;
  summary.deterministic_handoff =
      dependencies_ready && summary.dependency_surface_count == 14 &&
      summary.aggregated_source_only_claim_count > 0 &&
      summary.advanced_feature_family_count == 6 &&
      summary.diagnostic_surface_sites >= summary.error_surface_sites &&
      summary.diagnostic_surface_sites >= summary.interop_surface_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      objc3c::pipeline::orchestration::BuildToolingDiagnosticsMigratorSourceInventoryReplayKey(summary);
  return summary;
}

Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
BuildToolingMigrationCanonicalizationSourceCompletionSummary(
    const Objc3FrontendOptions &options,
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &inventory_summary) {
  Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary summary;
  summary.language_profile = "canonical";
  summary.legacy_yes_sites =
      canonical_literal_rejection_counts.yes_literal_sites;
  summary.legacy_no_sites =
      canonical_literal_rejection_counts.no_literal_sites;
  summary.legacy_null_sites =
      canonical_literal_rejection_counts.null_literal_sites;
  summary.legacy_total_sites =
      canonical_literal_rejection_counts.total_literal_sites();
  summary.canonical_true_rewrite_sites = summary.legacy_yes_sites;
  summary.canonical_false_rewrite_sites = summary.legacy_no_sites;
  summary.canonical_nil_rewrite_sites = summary.legacy_null_sites;
  summary.canonicalization_candidate_sites = summary.legacy_total_sites;
  summary.fixit_candidate_sites = summary.legacy_total_sites;
  summary.migrator_candidate_sites = summary.legacy_total_sites;
  summary.dependency_inventory_ready =
      inventory_summary.ready_for_semantic_expansion;
  summary.canonicalization_surface_supported = summary.dependency_inventory_ready;
  summary.fixit_migration_surface_supported = false;
  const bool counts_consistent =
      summary.legacy_total_sites ==
          summary.legacy_yes_sites + summary.legacy_no_sites +
              summary.legacy_null_sites &&
      summary.canonicalization_candidate_sites ==
          summary.canonical_true_rewrite_sites +
              summary.canonical_false_rewrite_sites +
              summary.canonical_nil_rewrite_sites &&
      summary.fixit_candidate_sites == summary.canonicalization_candidate_sites &&
      summary.migrator_candidate_sites ==
          summary.canonicalization_candidate_sites;
  summary.deterministic_handoff =
      summary.dependency_inventory_ready && counts_consistent &&
      inventory_summary.canonicalization_hint_sites ==
          summary.canonicalization_candidate_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  if (!summary.dependency_inventory_ready) {
    summary.failure_reason =
        "tooling diagnostics/fix-it/migrator inventory prerequisite is not ready";
  }
  summary.replay_key =
      objc3c::pipeline::orchestration::BuildToolingMigrationCanonicalizationSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendSymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummary(
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendSymbolGraphScopeResolutionSummary summary;
  const Objc3SymbolGraphScopeResolutionSummary &integration_summary =
      integration_surface.symbol_graph_scope_resolution_summary;
  const Objc3SymbolGraphScopeResolutionSummary &type_metadata_summary =
      type_metadata_handoff.symbol_graph_scope_resolution_summary;

  const auto select_value = [&](std::size_t integration_value, std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.global_symbol_nodes = select_value(integration_summary.global_symbol_nodes,
                                             type_metadata_summary.global_symbol_nodes);
  summary.function_symbol_nodes = select_value(integration_summary.function_symbol_nodes,
                                               type_metadata_summary.function_symbol_nodes);
  summary.interface_symbol_nodes = select_value(integration_summary.interface_symbol_nodes,
                                                type_metadata_summary.interface_symbol_nodes);
  summary.implementation_symbol_nodes = select_value(integration_summary.implementation_symbol_nodes,
                                                     type_metadata_summary.implementation_symbol_nodes);
  summary.interface_property_symbol_nodes = select_value(integration_summary.interface_property_symbol_nodes,
                                                         type_metadata_summary.interface_property_symbol_nodes);
  summary.implementation_property_symbol_nodes = select_value(integration_summary.implementation_property_symbol_nodes,
                                                              type_metadata_summary.implementation_property_symbol_nodes);
  summary.interface_method_symbol_nodes = select_value(integration_summary.interface_method_symbol_nodes,
                                                       type_metadata_summary.interface_method_symbol_nodes);
  summary.implementation_method_symbol_nodes = select_value(integration_summary.implementation_method_symbol_nodes,
                                                            type_metadata_summary.implementation_method_symbol_nodes);
  summary.top_level_scope_symbols = select_value(integration_summary.top_level_scope_symbols,
                                                 type_metadata_summary.top_level_scope_symbols);
  summary.nested_scope_symbols = select_value(integration_summary.nested_scope_symbols,
                                              type_metadata_summary.nested_scope_symbols);
  summary.scope_frames_total = select_value(integration_summary.scope_frames_total,
                                            type_metadata_summary.scope_frames_total);
  summary.implementation_interface_resolution_sites =
      select_value(integration_summary.implementation_interface_resolution_sites,
                   type_metadata_summary.implementation_interface_resolution_sites);
  summary.implementation_interface_resolution_hits =
      select_value(integration_summary.implementation_interface_resolution_hits,
                   type_metadata_summary.implementation_interface_resolution_hits);
  summary.implementation_interface_resolution_misses =
      select_value(integration_summary.implementation_interface_resolution_misses,
                   type_metadata_summary.implementation_interface_resolution_misses);
  summary.method_resolution_sites = select_value(integration_summary.method_resolution_sites,
                                                 type_metadata_summary.method_resolution_sites);
  summary.method_resolution_hits = select_value(integration_summary.method_resolution_hits,
                                                type_metadata_summary.method_resolution_hits);
  summary.method_resolution_misses = select_value(integration_summary.method_resolution_misses,
                                                  type_metadata_summary.method_resolution_misses);

  const bool symbol_graph_fields_match =
      integration_summary.global_symbol_nodes == type_metadata_summary.global_symbol_nodes &&
      integration_summary.function_symbol_nodes == type_metadata_summary.function_symbol_nodes &&
      integration_summary.interface_symbol_nodes == type_metadata_summary.interface_symbol_nodes &&
      integration_summary.implementation_symbol_nodes == type_metadata_summary.implementation_symbol_nodes &&
      integration_summary.interface_property_symbol_nodes == type_metadata_summary.interface_property_symbol_nodes &&
      integration_summary.implementation_property_symbol_nodes ==
          type_metadata_summary.implementation_property_symbol_nodes &&
      integration_summary.interface_method_symbol_nodes == type_metadata_summary.interface_method_symbol_nodes &&
      integration_summary.implementation_method_symbol_nodes ==
          type_metadata_summary.implementation_method_symbol_nodes;
  const bool scope_resolution_fields_match =
      integration_summary.top_level_scope_symbols == type_metadata_summary.top_level_scope_symbols &&
      integration_summary.nested_scope_symbols == type_metadata_summary.nested_scope_symbols &&
      integration_summary.scope_frames_total == type_metadata_summary.scope_frames_total &&
      integration_summary.implementation_interface_resolution_sites ==
          type_metadata_summary.implementation_interface_resolution_sites &&
      integration_summary.implementation_interface_resolution_hits ==
          type_metadata_summary.implementation_interface_resolution_hits &&
      integration_summary.implementation_interface_resolution_misses ==
          type_metadata_summary.implementation_interface_resolution_misses &&
      integration_summary.method_resolution_sites == type_metadata_summary.method_resolution_sites &&
      integration_summary.method_resolution_hits == type_metadata_summary.method_resolution_hits &&
      integration_summary.method_resolution_misses == type_metadata_summary.method_resolution_misses;

  summary.deterministic_symbol_graph_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      symbol_graph_fields_match &&
      summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols;
  summary.deterministic_scope_resolution_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      scope_resolution_fields_match &&
      summary.resolution_hits_total() <= summary.resolution_sites_total() &&
      summary.resolution_hits_total() + summary.resolution_misses_total() == summary.resolution_sites_total();
  summary.deterministic_handoff_key =
      objc3c::pipeline::orchestration::BuildSymbolGraphScopeResolutionHandoffKey(summary);
  return summary;
}

}  // namespace

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  std::vector<Objc3LexToken> tokens =
      RunObjc3FrontendLexParseStageSequence(source, options, result);
  result.selector_normalization_summary =
      objc3c::pipeline::orchestration::BuildSelectorNormalizationSummary(
          Objc3ParsedProgramAst(result.program));
  result.property_attribute_summary =
      objc3c::pipeline::orchestration::BuildPropertyAttributeSummary(
          Objc3ParsedProgramAst(result.program));
  result.object_pointer_nullability_generics_summary =
      objc3c::pipeline::orchestration::BuildObjectPointerNullabilityGenericsSummary(
          Objc3ParsedProgramAst(result.program));
  result.type_system_type_source_closure_summary =
      objc3c::pipeline::orchestration::BuildTypeSystemTypeSourceClosureSummary(
          Objc3ParsedProgramAst(result.program),
          result.object_pointer_nullability_generics_summary);
  result.control_flow_control_flow_source_closure_summary =
      objc3c::pipeline::orchestration::BuildControlFlowControlFlowSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.error_handling_error_source_closure_summary =
      objc3c::pipeline::orchestration::BuildErrorHandlingErrorSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.concurrency_async_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyAsyncSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.ownership_system_extension_source_closure_summary =
      objc3c::pipeline::orchestration::BuildOwnershipSystemExtensionSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_cleanup_resource_capture_source_completion_summary =
      objc3c::pipeline::orchestration::BuildOwnershipCleanupResourceCaptureSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_retainable_c_family_source_completion_summary =
      objc3c::pipeline::orchestration::BuildOwnershipRetainableCFamilySourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_closure_summary =
      objc3c::pipeline::orchestration::BuildDispatchDispatchIntentSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_completion_summary =
      objc3c::pipeline::orchestration::BuildDispatchDispatchIntentSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_metaprogramming_source_closure_summary =
      BuildMetaprogrammingMetaprogrammingSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_macro_package_provenance_source_completion_summary =
      BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_property_behavior_source_completion_summary =
      BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_foreign_import_source_closure_summary =
      objc3c::pipeline::orchestration::BuildInteropForeignImportSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_cpp_swift_interop_annotation_source_completion_summary =
      objc3c::pipeline::orchestration::BuildInteropCppSwiftInteropAnnotationSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_actor_member_isolation_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyActorMemberIsolationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_task_group_cancellation_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyTaskGroupCancellationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.tooling_diagnostics_migrator_source_inventory_summary =
      BuildToolingDiagnosticsMigratorSourceInventorySummary(
          result.canonical_literal_rejection_counts,
          result.error_handling_error_source_closure_summary,
          result.concurrency_async_source_closure_summary,
          result.concurrency_actor_member_isolation_source_closure_summary,
          result.concurrency_task_group_cancellation_source_closure_summary,
          result.ownership_system_extension_source_closure_summary,
          result.ownership_cleanup_resource_capture_source_completion_summary,
          result.ownership_retainable_c_family_source_completion_summary,
          result.dispatch_dispatch_intent_source_closure_summary,
          result.dispatch_dispatch_intent_source_completion_summary,
          result.metaprogramming_metaprogramming_source_closure_summary,
          result.metaprogramming_macro_package_provenance_source_completion_summary,
          result.metaprogramming_property_behavior_source_completion_summary,
          result.interop_foreign_import_source_closure_summary,
          result.interop_cpp_swift_interop_annotation_source_completion_summary);
  result.tooling_migration_canonicalization_source_completion_summary =
      BuildToolingMigrationCanonicalizationSourceCompletionSummary(
          options, result.canonical_literal_rejection_counts,
          result.tooling_diagnostics_migrator_source_inventory_summary);
  result.protocol_category_summary =
      objc3c::pipeline::orchestration::BuildProtocolCategorySummary(
          Objc3ParsedProgramAst(result.program),
          result.integration_surface,
          result.sema_type_metadata_handoff);
  result.class_protocol_category_linking_summary =
      objc3c::pipeline::orchestration::BuildClassProtocolCategoryLinkingSummary(
          result.sema_type_metadata_handoff.interface_implementation_summary,
          result.protocol_category_summary,
          result.integration_surface,
          result.sema_type_metadata_handoff);
  result.symbol_graph_scope_resolution_summary =
      BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                             result.sema_type_metadata_handoff);
  const bool allow_error_handling_error_runtime_surface = true;
  if (ShouldRunObjc3FrontendSemaStage(result)) {
    Objc3SemaPassManagerResult sema_result = RunObjc3FrontendSemaStage(
        result, options, allow_error_handling_error_runtime_surface);
    AdoptObjc3FrontendSemaResult(result, std::move(sema_result));
    result.protocol_category_summary =
        objc3c::pipeline::orchestration::BuildProtocolCategorySummary(
            Objc3ParsedProgramAst(result.program),
            result.integration_surface,
            result.sema_type_metadata_handoff);
    result.class_protocol_category_linking_summary =
        objc3c::pipeline::orchestration::BuildClassProtocolCategoryLinkingSummary(
            result.sema_type_metadata_handoff.interface_implementation_summary,
            result.protocol_category_summary,
            result.integration_surface,
            result.sema_type_metadata_handoff);
    result.symbol_graph_scope_resolution_summary =
        BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                               result.sema_type_metadata_handoff);
  }
  result.control_flow_control_flow_semantic_model_summary =
      BuildControlFlowControlFlowSemanticModelSummary(
          Objc3ParsedProgramAst(result.program));
  result.error_handling_error_semantic_model_summary =
      BuildErrorHandlingErrorSemanticModelSummary(
          result.error_handling_error_source_closure_summary, result.integration_surface);
  result.concurrency_actor_isolation_sendable_semantic_model_summary =
      BuildConcurrencyActorIsolationSendableSemanticModelSummary(
          result.concurrency_actor_member_isolation_source_closure_summary,
          result.integration_surface);
  result.concurrency_actor_isolation_sendability_enforcement_summary =
      BuildConcurrencyActorIsolationSendabilityEnforcementSummary(
          Objc3ParsedProgramAst(result.program),
          result.concurrency_actor_isolation_sendable_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_actor_race_hazard_escape_diagnostics_summary =
      BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummary(
          Objc3ParsedProgramAst(result.program),
          result.concurrency_actor_isolation_sendability_enforcement_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_task_executor_cancellation_semantic_model_summary =
      BuildConcurrencyTaskExecutorCancellationSemanticModelSummary(
          result.concurrency_task_group_cancellation_source_closure_summary,
          result.integration_surface);
  result.ownership_system_extension_semantic_model_summary =
      BuildOwnershipSystemExtensionSemanticModelSummary(
          result.ownership_system_extension_source_closure_summary,
          result.ownership_cleanup_resource_capture_source_completion_summary,
          result.ownership_retainable_c_family_source_completion_summary);
  result.metaprogramming_expansion_behavior_semantic_model_summary =
      BuildMetaprogrammingExpansionBehaviorSemanticModelSummary(
          result.metaprogramming_metaprogramming_source_closure_summary,
          result.metaprogramming_macro_package_provenance_source_completion_summary,
          result.metaprogramming_property_behavior_source_completion_summary,
          result.stage_diagnostics.semantic);
  result.metaprogramming_derive_expansion_inventory_summary =
      BuildMetaprogrammingDeriveExpansionInventorySummary(
          result.program.ast,
          result.metaprogramming_expansion_behavior_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.metaprogramming_macro_safety_sandbox_determinism_summary =
      BuildMetaprogrammingMacroSafetySandboxDeterminismSummary(
          result.program.ast,
          result.metaprogramming_derive_expansion_inventory_summary,
          result.stage_diagnostics.semantic);
  result.metaprogramming_property_behavior_legality_compatibility_summary =
      BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
          result.program.ast,
          result.metaprogramming_macro_safety_sandbox_determinism_summary,
          result.stage_diagnostics.semantic);
  result.dispatch_dispatch_intent_semantic_model_summary =
      BuildDispatchDispatchIntentSemanticModelSummary(
          result.dispatch_dispatch_intent_source_completion_summary,
          result.integration_surface);
  result.dispatch_dispatch_intent_legality_summary =
      BuildDispatchDispatchIntentLegalitySummary(
          Objc3ParsedProgramAst(result.program),
          result.dispatch_dispatch_intent_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.dispatch_dispatch_intent_compatibility_summary =
      BuildDispatchDispatchIntentCompatibilitySummary(
          Objc3ParsedProgramAst(result.program),
          result.dispatch_dispatch_intent_legality_summary,
          result.stage_diagnostics.semantic);
  result.ownership_resource_move_use_after_move_semantics_summary =
      BuildOwnershipResourceMoveUseAfterMoveSemanticsSummary(
          Objc3ParsedProgramAst(result.program),
          result.ownership_system_extension_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.ownership_borrowed_pointer_escape_analysis_summary =
      BuildOwnershipBorrowedPointerEscapeAnalysisSummary(
          Objc3ParsedProgramAst(result.program),
          result.ownership_resource_move_use_after_move_semantics_summary,
          result.stage_diagnostics.semantic);
  result.ownership_capture_list_retainable_family_legality_completion_summary =
      BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
          Objc3ParsedProgramAst(result.program),
          result.ownership_borrowed_pointer_escape_analysis_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_structured_task_cancellation_semantic_summary =
      BuildConcurrencyStructuredTaskCancellationSemanticSummary(
          result.concurrency_task_executor_cancellation_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_executor_hop_affinity_compatibility_summary =
      BuildConcurrencyExecutorHopAffinityCompatibilitySummary(
          result.concurrency_structured_task_cancellation_semantic_summary,
          result.concurrency_async_source_closure_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_async_effect_suspension_semantic_model_summary =
      BuildConcurrencyAsyncEffectSuspensionSemanticModelSummary(
          result.concurrency_async_source_closure_summary, result.integration_surface);
  result.concurrency_await_suspension_resume_semantic_summary =
      BuildConcurrencyAwaitSuspensionResumeSemanticSummary(
          result.concurrency_async_effect_suspension_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_async_diagnostics_compatibility_summary =
      BuildConcurrencyAsyncDiagnosticsCompatibilitySummary(
          result.concurrency_await_suspension_resume_semantic_summary,
          result.concurrency_async_source_closure_summary,
          Objc3ParsedProgramAst(result.program),
          result.stage_diagnostics.semantic);
  result.error_handling_try_do_catch_semantic_summary =
      BuildErrorHandlingTryDoCatchSemanticSummary(
          Objc3ParsedProgramAst(result.program),
          result.integration_surface,
          allow_error_handling_error_runtime_surface,
          result.stage_diagnostics.semantic);
  result.error_handling_error_bridge_legality_summary =
      BuildErrorHandlingErrorBridgeLegalitySummary(
          Objc3ParsedProgramAst(result.program),
          allow_error_handling_error_runtime_surface,
          result.stage_diagnostics.semantic);
  result.interop_interop_semantic_model_summary =
      BuildInteropInteropSemanticModelSummary(
          result.interop_foreign_import_source_closure_summary,
          result.interop_cpp_swift_interop_annotation_source_completion_summary,
          result.ownership_capture_list_retainable_family_legality_completion_summary,
          result.error_handling_error_bridge_legality_summary,
          result.concurrency_async_diagnostics_compatibility_summary,
          result.concurrency_actor_race_hazard_escape_diagnostics_summary);
  result.effects_ownership_semantic_model_summary =
      BuildEffectsOwnershipSemanticModelSummary(
          result.integration_surface,
          result.error_handling_error_semantic_model_summary,
          result.concurrency_async_effect_suspension_semantic_model_summary,
          result.concurrency_task_executor_cancellation_semantic_model_summary,
          result.concurrency_actor_isolation_sendable_semantic_model_summary,
          result.interop_interop_semantic_model_summary);
  result.cross_module_semantic_contracts_diagnostics_summary =
      BuildCrossModuleSemanticContractsDiagnosticsSummary(
          result.integration_surface,
          result.interop_interop_semantic_model_summary);
  result.interop_interop_runtime_parity_summary =
      BuildInteropInteropRuntimeParitySummary(
          Objc3ParsedProgramAst(result.program),
          result.interop_interop_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.interop_cpp_interop_interaction_summary =
      BuildInteropCppInteropInteractionSummary(
          Objc3ParsedProgramAst(result.program),
          result.interop_interop_runtime_parity_summary,
          result.stage_diagnostics.semantic);
  result.interop_swift_interop_isolation_summary =
      BuildInteropSwiftInteropIsolationSummary(
          Objc3ParsedProgramAst(result.program),
          result.interop_cpp_interop_interaction_summary,
          result.stage_diagnostics.semantic);
  result.runtime_metadata_source_records =
      BuildRuntimeMetadataSourceRecordSet(Objc3ParsedProgramAst(result.program));
  result.executable_metadata_source_graph = BuildExecutableMetadataSourceGraph(
      Objc3ParsedProgramAst(result.program),
      result.runtime_metadata_source_records);
  result.executable_metadata_semantic_consistency_boundary =
      BuildExecutableMetadataSemanticConsistencyBoundary(
          result.executable_metadata_source_graph,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary);
  result.executable_metadata_semantic_validation_surface =
      BuildExecutableMetadataSemanticValidationSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.sema_type_metadata_handoff,
          result.class_protocol_category_linking_summary);
  result.executable_metadata_lowering_handoff_surface =
      BuildExecutableMetadataLoweringHandoffSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.sema_type_metadata_handoff,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary,
          result.sema_parity_surface);
  result.executable_metadata_typed_lowering_handoff =
      BuildExecutableMetadataTypedLoweringHandoff(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.executable_metadata_lowering_handoff_surface);
  result.runtime_metadata_source_ownership_boundary =
      BuildRuntimeMetadataSourceOwnershipBoundary(result.runtime_metadata_source_records,
                                                  result.sema_type_metadata_handoff);
  result.typed_sema_to_lowering_contract_surface =
      BuildObjc3TypedSemaToLoweringContractSurface(result, options);
  result.runtime_export_legality_boundary = BuildRuntimeExportLegalityBoundary(
      result.runtime_metadata_source_ownership_boundary,
      result.typed_sema_to_lowering_contract_surface,
      result.integration_surface,
      result.protocol_category_summary,
      result.class_protocol_category_linking_summary,
      result.selector_normalization_summary,
      result.property_attribute_summary,
      result.object_pointer_nullability_generics_summary,
      result.symbol_graph_scope_resolution_summary,
      result.sema_parity_surface);
  result.runtime_export_enforcement_summary =
      BuildRuntimeExportEnforcementSummary(
          result.runtime_metadata_source_records,
          result.runtime_export_legality_boundary);
  if (result.stage_diagnostics.semantic.empty() &&
      HasRuntimeMetadataSourceRecords(result.runtime_metadata_source_records) &&
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          result.runtime_export_enforcement_summary)) {
    const std::vector<Objc3RuntimeExportBlockingDiagnostic>
        runtime_export_blocking_diagnostics =
            BuildRuntimeExportBlockingDiagnostics(
                result.runtime_metadata_source_records,
                result.runtime_export_enforcement_summary);
    if (!runtime_export_blocking_diagnostics.empty()) {
      for (const auto &diagnostic : runtime_export_blocking_diagnostics) {
        result.stage_diagnostics.semantic.push_back(
            MakeDiag(diagnostic.line, diagnostic.column, diagnostic.code,
                     diagnostic.message));
      }
    } else {
      std::string runtime_export_failure_reason =
          result.runtime_export_enforcement_summary.failure_reason;
      if (runtime_export_failure_reason ==
              "runtime metadata export shape drift detected before lowering" &&
          !result.runtime_export_legality_boundary.failure_reason.empty()) {
        runtime_export_failure_reason +=
            " (" + result.runtime_export_legality_boundary.failure_reason + ")";
      }
      result.stage_diagnostics.semantic.push_back(MakeDiag(
          result.runtime_export_enforcement_summary.first_failure_line,
          result.runtime_export_enforcement_summary.first_failure_column,
          "O3S260",
          "runtime metadata export blocked: " + runtime_export_failure_reason));
    }
  }
  objc3c::pipeline::orchestration::AdoptObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
      result,
      objc3c::pipeline::orchestration::BuildObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
          result, options));
  objc3c::pipeline::orchestration::PopulateObjc3FrontendReadinessLoweringPhaseResult(
      result, options);
  TransportObjc3FrontendPipelineDiagnostics(result);
  return result;
}
