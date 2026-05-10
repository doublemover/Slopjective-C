#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"

struct Objc3IRFrontendRuntimeSourceClosureMetadata {
  std::string runtime_metadata_class_metaclass_emission_contract_id;
  std::string runtime_metadata_class_metaclass_payload_model;
  std::string runtime_metadata_class_metaclass_name_model;
  std::string runtime_metadata_class_metaclass_super_link_model;
  std::string runtime_metadata_class_metaclass_method_list_reference_model;
  std::string executable_class_metaclass_source_closure_contract_id;
  std::string executable_class_metaclass_parent_identity_model;
  std::string executable_class_metaclass_method_owner_identity_model;
  std::string executable_class_metaclass_object_identity_model;
  bool runtime_metadata_class_metaclass_emission_ready = false;
  bool runtime_metadata_class_metaclass_emission_fail_closed = false;
  bool executable_class_metaclass_source_closure_ready = false;
  std::size_t executable_class_metaclass_declaration_node_count = 0;
  std::size_t executable_class_metaclass_parent_identity_edge_count = 0;
  std::size_t executable_class_metaclass_method_owner_identity_edge_count = 0;
  std::size_t executable_class_metaclass_object_identity_edge_count = 0;
  std::string runtime_metadata_class_metaclass_typed_handoff_replay_key;
  std::vector<Objc3IRRuntimeMetadataClassMetaclassBundle>
      runtime_metadata_class_metaclass_bundles_lexicographic;
  std::string executable_protocol_category_source_closure_contract_id;
  std::string executable_protocol_inheritance_identity_model;
  std::string executable_category_attachment_identity_model;
  std::string executable_protocol_category_conformance_identity_model;
  std::string runtime_metadata_protocol_category_emission_contract_id;
  std::string runtime_metadata_protocol_emission_payload_model;
  std::string runtime_metadata_category_emission_payload_model;
  std::string runtime_metadata_protocol_reference_model;
  std::string runtime_metadata_category_attachment_model;
  bool executable_protocol_category_source_closure_ready = false;
  bool runtime_metadata_protocol_category_emission_ready = false;
  bool runtime_metadata_protocol_category_emission_fail_closed = false;
  std::size_t executable_protocol_category_protocol_node_count = 0;
  std::size_t executable_protocol_category_category_node_count = 0;
  std::size_t executable_protocol_inheritance_identity_edge_count = 0;
  std::size_t executable_category_attachment_identity_edge_count = 0;
  std::size_t executable_protocol_category_conformance_identity_edge_count = 0;
  std::string runtime_metadata_protocol_category_typed_handoff_replay_key;
  std::vector<Objc3IRRuntimeMetadataProtocolBundle>
      runtime_metadata_protocol_bundles_lexicographic;
  std::vector<Objc3IRRuntimeMetadataCategoryBundle>
      runtime_metadata_category_bundles_lexicographic;
};
