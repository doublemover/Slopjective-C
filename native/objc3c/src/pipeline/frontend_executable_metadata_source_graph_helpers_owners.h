#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "pipeline/frontend_executable_metadata_source_graph_aggregation.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

struct ExecutableMetadataPropertySynthesisIndex {
  std::unordered_set<std::string> class_implementation_names;
  std::unordered_set<std::string> implementation_property_keys;
};

struct ExecutableMetadataSourceGraphTopologyContext {
  std::unordered_map<std::string, ExecutableMetadataAggregatedClassSurface>
      aggregated_classes;
  std::unordered_map<std::string, ExecutableMetadataAggregatedCategorySurface>
      aggregated_categories;
  std::vector<ExecutableMetadataMethodEdgeRecord> method_edge_records;
  ExecutableMetadataPropertySynthesisIndex property_synthesis_index;
};

void PopulateExecutableMetadataSourceGraphTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph);

void BuildExecutableMetadataPropertySynthesisIndex(
    const Objc3Program &program,
    ExecutableMetadataPropertySynthesisIndex &property_synthesis_index);

void AddExecutableMetadataOwnerEdge(
    Objc3ExecutableMetadataSourceGraph &graph,
    const std::string &edge_kind,
    const std::string &source_owner_identity,
    const std::string &target_owner_identity,
    unsigned line,
    unsigned column);

void AddExecutableMetadataPropertyNodes(
    const std::vector<Objc3PropertyDecl> &properties,
    const std::string &owner_kind,
    const std::string &owner_name,
    const std::string &declaration_owner_identity,
    const std::string &export_owner_identity,
    const ExecutableMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3ExecutableMetadataSourceGraph &graph);

void AddExecutableMetadataMethodNodes(
    const std::vector<Objc3MethodDecl> &methods,
    const std::string &owner_kind,
    const std::string &owner_name,
    const std::string &declaration_owner_identity,
    const std::string &instance_export_owner_identity,
    const std::string &class_export_owner_identity,
    bool direct_members_declared,
    Objc3ExecutableMetadataSourceGraph &graph,
    std::vector<ExecutableMetadataMethodEdgeRecord> &method_edge_records);

void LinkExecutableMetadataPropertyAccessorEdges(
    Objc3ExecutableMetadataSourceGraph &graph,
    const std::vector<ExecutableMetadataMethodEdgeRecord> &method_edge_records);

void LinkExecutableMetadataMethodOverrideEdges(
    Objc3ExecutableMetadataSourceGraph &graph);

void PopulateExecutableMetadataInterfaceTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph,
    ExecutableMetadataSourceGraphTopologyContext &context);

void PopulateExecutableMetadataProtocolTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph,
    ExecutableMetadataSourceGraphTopologyContext &context);

void FinalizeExecutableMetadataClassTopology(
    Objc3ExecutableMetadataSourceGraph &graph,
    const ExecutableMetadataSourceGraphTopologyContext &context);

void PopulateExecutableMetadataImplementationTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph,
    ExecutableMetadataSourceGraphTopologyContext &context);

void FinalizeExecutableMetadataCategoryTopology(
    Objc3ExecutableMetadataSourceGraph &graph,
    const ExecutableMetadataSourceGraphTopologyContext &context);

void FinalizeExecutableMetadataSourceGraphReadiness(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    Objc3ExecutableMetadataSourceGraph &graph);

}  // namespace objc3c::pipeline::orchestration
