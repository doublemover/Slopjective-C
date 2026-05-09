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
#include "pipeline/frontend_executable_metadata_semantic_surface_helpers.h"
#include "pipeline/frontend_interop_source_closure_helpers.h"
#include "pipeline/frontend_interop_source_completion_helpers.h"
#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "pipeline/frontend_metaprogramming_source_completion_helpers.h"
#include "pipeline/frontend_ownership_retainable_c_family_completion_helpers.h"
#include "pipeline/frontend_ownership_source_completion_helpers.h"
#include "pipeline/frontend_ownership_source_closure_helpers.h"
#include "pipeline/frontend_phase_publication_helpers.h"
#include "pipeline/frontend_pipeline_result_handoff.h"
#include "pipeline/frontend_pipeline_sema_stage_runner.h"
#include "pipeline/frontend_pipeline_stage_sequence.h"
#include "pipeline/frontend_pipeline_stage_runner.h"
#include "pipeline/frontend_runtime_export_enforcement_helpers.h"
#include "pipeline/frontend_runtime_metadata_boundary_helpers.h"
#include "pipeline/frontend_runtime_metadata_source_record_helpers.h"
#include "pipeline/frontend_semantic_metadata_summary_helpers.h"
#include "pipeline/frontend_tooling_source_completion_helpers.h"
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
      objc3c::pipeline::orchestration::BuildMetaprogrammingMetaprogrammingSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_macro_package_provenance_source_completion_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_property_behavior_source_completion_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(
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
      objc3c::pipeline::orchestration::BuildToolingDiagnosticsMigratorSourceInventorySummary(
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
      objc3c::pipeline::orchestration::BuildToolingMigrationCanonicalizationSourceCompletionSummary(
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
      objc3c::pipeline::orchestration::BuildRuntimeMetadataSourceRecordSet(
          Objc3ParsedProgramAst(result.program));
  result.executable_metadata_source_graph = BuildExecutableMetadataSourceGraph(
      Objc3ParsedProgramAst(result.program),
      result.runtime_metadata_source_records);
  result.executable_metadata_semantic_consistency_boundary =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSemanticConsistencyBoundary(
          result.executable_metadata_source_graph,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary);
  result.executable_metadata_semantic_validation_surface =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSemanticValidationSurface(
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
      objc3c::pipeline::orchestration::BuildRuntimeMetadataSourceOwnershipBoundary(
          result.runtime_metadata_source_records,
          result.sema_type_metadata_handoff);
  result.typed_sema_to_lowering_contract_surface =
      BuildObjc3TypedSemaToLoweringContractSurface(result, options);
  result.runtime_export_legality_boundary =
      objc3c::pipeline::orchestration::BuildRuntimeExportLegalityBoundary(
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
      objc3c::pipeline::orchestration::BuildRuntimeExportEnforcementSummary(
          result.runtime_metadata_source_records,
          result.runtime_export_legality_boundary);
  if (result.stage_diagnostics.semantic.empty() &&
      objc3c::pipeline::orchestration::HasRuntimeMetadataSourceRecords(
          result.runtime_metadata_source_records) &&
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          result.runtime_export_enforcement_summary)) {
    const std::vector<
        objc3c::pipeline::orchestration::Objc3RuntimeExportBlockingDiagnostic>
        runtime_export_blocking_diagnostics =
            objc3c::pipeline::orchestration::BuildRuntimeExportBlockingDiagnostics(
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
