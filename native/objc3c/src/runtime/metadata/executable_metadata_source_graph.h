#pragma once

#include "runtime/metadata/executable_metadata_graph_nodes.h"
#include "sema/model/semantic_type_executable_metadata_contracts.h"

struct Objc3ExecutableMetadataSourceGraph {
  std::string contract_id = kObjc3ExecutableMetadataSourceGraphContractId;
  std::string owner_identity_model =
      kObjc3ExecutableMetadataSourceGraphOwnerIdentityModel;
  std::string metaclass_node_policy =
      kObjc3ExecutableMetadataMetaclassNodePolicy;
  std::string edge_ordering_model =
      kObjc3ExecutableMetadataSourceGraphEdgeOrderingModel;
  std::string class_metaclass_source_closure_contract_id =
      kObjc3ExecutableMetadataClassMetaclassSourceClosureContractId;
  std::string class_metaclass_parent_identity_model =
      kObjc3ExecutableMetadataClassMetaclassParentIdentityModel;
  std::string class_metaclass_method_owner_identity_model =
      kObjc3ExecutableMetadataClassMetaclassMethodOwnerIdentityModel;
  std::string class_metaclass_object_identity_model =
      kObjc3ExecutableMetadataClassMetaclassObjectIdentityModel;
  std::string protocol_category_source_closure_contract_id =
      kObjc3ExecutableMetadataProtocolCategorySourceClosureContractId;
  std::string protocol_inheritance_identity_model =
      kObjc3ExecutableMetadataProtocolInheritanceIdentityModel;
  std::string category_attachment_identity_model =
      kObjc3ExecutableMetadataCategoryAttachmentIdentityModel;
  std::string protocol_category_conformance_identity_model =
      kObjc3ExecutableMetadataProtocolCategoryConformanceIdentityModel;
  std::vector<Objc3ExecutableMetadataInterfaceGraphNode>
      interface_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataImplementationGraphNode>
      implementation_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataClassGraphNode> class_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataMetaclassGraphNode>
      metaclass_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataProtocolGraphNode>
      protocol_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataCategoryGraphNode>
      category_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataPropertyGraphNode>
      property_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataMethodGraphNode>
      method_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataIvarGraphNode> ivar_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataGraphEdge> owner_edges_lexicographic;
  bool deterministic = false;
  bool class_metaclass_declaration_closure_complete = false;
  bool class_metaclass_parent_identity_closure_complete = false;
  bool class_metaclass_method_owner_identity_closure_complete = false;
  bool class_metaclass_object_identity_closure_complete = false;
  bool protocol_category_declaration_closure_complete = false;
  bool protocol_inheritance_identity_closure_complete = false;
  bool category_attachment_identity_closure_complete = false;
  bool protocol_category_conformance_identity_closure_complete = false;
  bool source_graph_complete = false;
  bool ready_for_semantic_closure = false;
  bool ready_for_lowering = false;
};

inline bool IsReadyObjc3ExecutableMetadataSourceGraph(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  return graph.deterministic && graph.source_graph_complete &&
         graph.ready_for_semantic_closure && !graph.ready_for_lowering &&
         !graph.contract_id.empty() && !graph.owner_identity_model.empty() &&
         !graph.metaclass_node_policy.empty() &&
         !graph.edge_ordering_model.empty() &&
         !graph.class_metaclass_source_closure_contract_id.empty() &&
         !graph.class_metaclass_parent_identity_model.empty() &&
         !graph.class_metaclass_method_owner_identity_model.empty() &&
         !graph.class_metaclass_object_identity_model.empty() &&
         !graph.protocol_category_source_closure_contract_id.empty() &&
         !graph.protocol_inheritance_identity_model.empty() &&
         !graph.category_attachment_identity_model.empty() &&
         !graph.protocol_category_conformance_identity_model.empty() &&
         graph.class_metaclass_declaration_closure_complete &&
         graph.class_metaclass_parent_identity_closure_complete &&
         graph.class_metaclass_method_owner_identity_closure_complete &&
         graph.class_metaclass_object_identity_closure_complete &&
         graph.protocol_category_declaration_closure_complete &&
         graph.protocol_inheritance_identity_closure_complete &&
         graph.category_attachment_identity_closure_complete &&
         graph.protocol_category_conformance_identity_closure_complete;
}
