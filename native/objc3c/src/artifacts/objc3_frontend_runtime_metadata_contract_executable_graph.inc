inline constexpr const char *kArtifactRuntimeClassMetaclassEmissionContractId =
    "objc3c.runtime.class.metaclass.data.emission.v1";
inline constexpr const char *kArtifactRuntimeClassMetaclassEmissionPayloadModel =
    "class-source-record-descriptor-bundles-with-inline-metaclass-records-and-final-sealed-flags";
inline constexpr const char *kArtifactRuntimeClassMetaclassEmissionNameModel =
    "shared-class-name-cstring-per-bundle";
inline constexpr const char *kArtifactRuntimeClassMetaclassEmissionSuperLinkModel =
    "nullable-super-source-record-bundle-pointer";
inline constexpr const char
    *kArtifactRuntimeClassMetaclassEmissionMethodListReferenceModel =
        "count-plus-owner-identity-pointer-method-list-ref";
inline constexpr const char *kArtifactRuntimeProtocolCategoryEmissionContractId =
    "objc3c.runtime.protocol.category.data.emission.v1";
inline constexpr const char *kArtifactRuntimeProtocolEmissionPayloadModel =
    "protocol-descriptor-bundles-with-inherited-protocol-ref-lists";
inline constexpr const char *kArtifactRuntimeCategoryEmissionPayloadModel =
    "category-descriptor-bundles-with-attachment-and-protocol-ref-lists";
inline constexpr const char *kArtifactRuntimeProtocolReferenceModel =
    "count-plus-descriptor-pointer-protocol-ref-lists";
inline constexpr const char *kArtifactRuntimeCategoryAttachmentModel =
    "count-plus-owner-identity-pointer-attachment-lists";
inline constexpr const char *kArtifactRuntimeMemberTableEmissionContractId =
    "objc3c.runtime.member.table.emission.v1";
inline constexpr const char *kArtifactRuntimeMethodListEmissionPayloadModel =
    "owner-scoped-method-table-globals-with-inline-entry-records-and-direct-final-flags";
inline constexpr const char *kArtifactRuntimeMethodListEmissionGroupingModel =
    "declaration-owner-plus-class-kind-lexicographic";
inline constexpr const char
    *kArtifactRuntimePropertyDescriptorEmissionPayloadModel =
        "property-descriptor-records-with-accessor-binding-and-sema-ivar-layout-fields";
inline constexpr const char *kArtifactRuntimeIvarDescriptorEmissionPayloadModel =
    "ivar-descriptor-records-with-property-binding-layout-replay-key-and-offset-global";

struct RuntimeMetadataSourceGraphEdgeCounts {
  std::size_t class_metaclass_parent_identity_edge_count = 0;
  std::size_t class_metaclass_method_owner_identity_edge_count = 0;
  std::size_t class_metaclass_object_identity_edge_count = 0;
  std::size_t protocol_inheritance_identity_edge_count = 0;
  std::size_t category_attachment_identity_edge_count = 0;
  std::size_t protocol_category_conformance_identity_edge_count = 0;
};

RuntimeMetadataSourceGraphEdgeCounts CountRuntimeMetadataSourceGraphEdges(
    const Objc3ExecutableMetadataSourceGraph &source_graph) {
  RuntimeMetadataSourceGraphEdgeCounts counts;
  for (const auto &edge : source_graph.owner_edges_lexicographic) {
    if (edge.edge_kind == "interface-to-superclass" ||
        edge.edge_kind == "interface-to-super-metaclass" ||
        edge.edge_kind == "implementation-to-superclass" ||
        edge.edge_kind == "implementation-to-super-metaclass" ||
        edge.edge_kind == "class-to-superclass" ||
        edge.edge_kind == "metaclass-to-super-metaclass") {
      ++counts.class_metaclass_parent_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-instance-method-owner" ||
        edge.edge_kind == "interface-to-class-method-owner" ||
        edge.edge_kind == "implementation-to-instance-method-owner" ||
        edge.edge_kind == "implementation-to-class-method-owner") {
      ++counts.class_metaclass_method_owner_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-class" ||
        edge.edge_kind == "interface-to-metaclass" ||
        edge.edge_kind == "implementation-to-class" ||
        edge.edge_kind == "implementation-to-metaclass" ||
        edge.edge_kind == "class-to-metaclass") {
      ++counts.class_metaclass_object_identity_edge_count;
    }
    if (edge.edge_kind == "protocol-to-inherited-protocol") {
      ++counts.protocol_inheritance_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-class" ||
        edge.edge_kind == "category-to-interface" ||
        edge.edge_kind == "category-to-implementation") {
      ++counts.category_attachment_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-protocol") {
      ++counts.protocol_category_conformance_identity_edge_count;
    }
  }
  return counts;
}

void ApplyRuntimeMetadataClassMetaclassEmissionContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph,
    const std::string &typed_handoff_replay_key) {
  ir_frontend_metadata.runtime_metadata_class_metaclass_emission_contract_id =
      kArtifactRuntimeClassMetaclassEmissionContractId;
  ir_frontend_metadata.runtime_metadata_class_metaclass_payload_model =
      kArtifactRuntimeClassMetaclassEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_name_model =
      kArtifactRuntimeClassMetaclassEmissionNameModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_super_link_model =
      kArtifactRuntimeClassMetaclassEmissionSuperLinkModel;
  ir_frontend_metadata
      .runtime_metadata_class_metaclass_method_list_reference_model =
      kArtifactRuntimeClassMetaclassEmissionMethodListReferenceModel;
  ir_frontend_metadata.executable_class_metaclass_source_closure_contract_id =
      source_graph.class_metaclass_source_closure_contract_id;
  ir_frontend_metadata.executable_class_metaclass_parent_identity_model =
      source_graph.class_metaclass_parent_identity_model;
  ir_frontend_metadata
      .executable_class_metaclass_method_owner_identity_model =
      source_graph.class_metaclass_method_owner_identity_model;
  ir_frontend_metadata.executable_class_metaclass_object_identity_model =
      source_graph.class_metaclass_object_identity_model;
  ir_frontend_metadata.executable_class_metaclass_source_closure_ready =
      source_graph.class_metaclass_declaration_closure_complete &&
      source_graph.class_metaclass_parent_identity_closure_complete &&
      source_graph.class_metaclass_method_owner_identity_closure_complete &&
      source_graph.class_metaclass_object_identity_closure_complete;
  ir_frontend_metadata.executable_class_metaclass_declaration_node_count =
      source_graph.interface_nodes_lexicographic.size() +
      source_graph.implementation_nodes_lexicographic.size() +
      source_graph.class_nodes_lexicographic.size() +
      source_graph.metaclass_nodes_lexicographic.size();
  ir_frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key =
      typed_handoff_replay_key;
}

void ApplyRuntimeMetadataProtocolCategorySourceClosureContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataSourceGraph &source_graph) {
  ir_frontend_metadata.executable_protocol_category_source_closure_contract_id =
      source_graph.protocol_category_source_closure_contract_id;
  ir_frontend_metadata.executable_protocol_inheritance_identity_model =
      source_graph.protocol_inheritance_identity_model;
  ir_frontend_metadata.executable_category_attachment_identity_model =
      source_graph.category_attachment_identity_model;
  ir_frontend_metadata
      .executable_protocol_category_conformance_identity_model =
      source_graph.protocol_category_conformance_identity_model;
  ir_frontend_metadata.executable_protocol_category_source_closure_ready =
      source_graph.protocol_category_declaration_closure_complete &&
      source_graph.protocol_inheritance_identity_closure_complete &&
      source_graph.category_attachment_identity_closure_complete &&
      source_graph.protocol_category_conformance_identity_closure_complete;
  ir_frontend_metadata.executable_protocol_category_protocol_node_count =
      source_graph.protocol_nodes_lexicographic.size();
  ir_frontend_metadata.executable_protocol_category_category_node_count =
      source_graph.category_nodes_lexicographic.size();
}

void ApplyRuntimeMetadataSourceGraphEdgeEvidence(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const RuntimeMetadataSourceGraphEdgeCounts &edge_counts) {
  ir_frontend_metadata.executable_class_metaclass_parent_identity_edge_count =
      edge_counts.class_metaclass_parent_identity_edge_count;
  ir_frontend_metadata
      .executable_class_metaclass_method_owner_identity_edge_count =
      edge_counts.class_metaclass_method_owner_identity_edge_count;
  ir_frontend_metadata.executable_class_metaclass_object_identity_edge_count =
      edge_counts.class_metaclass_object_identity_edge_count;
  ir_frontend_metadata.executable_protocol_inheritance_identity_edge_count =
      edge_counts.protocol_inheritance_identity_edge_count;
  ir_frontend_metadata.executable_category_attachment_identity_edge_count =
      edge_counts.category_attachment_identity_edge_count;
  ir_frontend_metadata.executable_protocol_category_conformance_identity_edge_count =
      edge_counts.protocol_category_conformance_identity_edge_count;
}

void ApplyRuntimeMetadataProtocolCategoryEmissionContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &typed_handoff_replay_key) {
  ir_frontend_metadata.runtime_metadata_protocol_category_emission_contract_id =
      kArtifactRuntimeProtocolCategoryEmissionContractId;
  ir_frontend_metadata.runtime_metadata_protocol_emission_payload_model =
      kArtifactRuntimeProtocolEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_category_emission_payload_model =
      kArtifactRuntimeCategoryEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_protocol_reference_model =
      kArtifactRuntimeProtocolReferenceModel;
  ir_frontend_metadata.runtime_metadata_category_attachment_model =
      kArtifactRuntimeCategoryAttachmentModel;
  ir_frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key =
      typed_handoff_replay_key;
}

void ApplyRuntimeMetadataMemberTableEmissionContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &typed_handoff_replay_key) {
  ir_frontend_metadata.runtime_metadata_member_table_emission_contract_id =
      kArtifactRuntimeMemberTableEmissionContractId;
  ir_frontend_metadata.runtime_metadata_method_list_emission_payload_model =
      kArtifactRuntimeMethodListEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_method_list_grouping_model =
      kArtifactRuntimeMethodListEmissionGroupingModel;
  ir_frontend_metadata
      .runtime_metadata_property_descriptor_emission_payload_model =
      kArtifactRuntimePropertyDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_ivar_descriptor_emission_payload_model =
      kArtifactRuntimeIvarDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key =
      typed_handoff_replay_key;
}

void ApplyRuntimeMetadataExecutableGraphContracts(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff) {
  const auto &source_graph =
      executable_metadata_typed_lowering_handoff.source_graph;
  const auto edge_counts = CountRuntimeMetadataSourceGraphEdges(source_graph);
  ApplyRuntimeMetadataClassMetaclassEmissionContract(
      ir_frontend_metadata,
      source_graph,
      executable_metadata_typed_lowering_handoff.replay_key);
  ApplyRuntimeMetadataProtocolCategorySourceClosureContract(
      ir_frontend_metadata, source_graph);
  ApplyRuntimeMetadataSourceGraphEdgeEvidence(ir_frontend_metadata, edge_counts);
  ApplyRuntimeMetadataProtocolCategoryEmissionContract(
      ir_frontend_metadata,
      executable_metadata_typed_lowering_handoff.replay_key);
  ApplyRuntimeMetadataMemberTableEmissionContract(
      ir_frontend_metadata, executable_metadata_typed_lowering_handoff.replay_key);
}
