#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_type_constraints.h"

struct Objc3IRFrontendTypeSystemMetadata
    : Objc3IRFrontendTypeConstraintsMetadata {
  std::string lowering_protocol_qualified_object_type_replay_key;
  std::size_t protocol_qualified_object_type_lowering_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_protocol_composition_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_object_pointer_type_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_terminated_protocol_composition_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_pointer_declarator_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_normalized_protocol_composition_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_contract_violation_sites = 0;
  bool deterministic_protocol_qualified_object_type_lowering_handoff = false;
  std::string lowering_variance_bridge_cast_replay_key;
  std::size_t variance_bridge_cast_lowering_sites = 0;
  std::size_t variance_bridge_cast_lowering_protocol_composition_sites = 0;
  std::size_t variance_bridge_cast_lowering_ownership_qualifier_sites = 0;
  std::size_t variance_bridge_cast_lowering_object_pointer_type_sites = 0;
  std::size_t variance_bridge_cast_lowering_pointer_declarator_sites = 0;
  std::size_t variance_bridge_cast_lowering_normalized_sites = 0;
  std::size_t variance_bridge_cast_lowering_contract_violation_sites = 0;
  bool deterministic_variance_bridge_cast_lowering_handoff = false;
  std::string lowering_generic_metadata_abi_replay_key;
  std::size_t generic_metadata_abi_lowering_sites = 0;
  std::size_t generic_metadata_abi_lowering_generic_suffix_sites = 0;
  std::size_t generic_metadata_abi_lowering_protocol_composition_sites = 0;
  std::size_t generic_metadata_abi_lowering_ownership_qualifier_sites = 0;
  std::size_t generic_metadata_abi_lowering_object_pointer_type_sites = 0;
  std::size_t generic_metadata_abi_lowering_pointer_declarator_sites = 0;
  std::size_t generic_metadata_abi_lowering_normalized_sites = 0;
  std::size_t generic_metadata_abi_lowering_contract_violation_sites = 0;
  bool deterministic_generic_metadata_abi_lowering_handoff = false;
};
