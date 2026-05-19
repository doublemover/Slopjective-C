#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendModuleSourceVisibilityMetadata {
  std::string lowering_namespace_collision_shadowing_replay_key;
  std::size_t namespace_collision_shadowing_lowering_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_namespace_segment_sites = 0;
  std::size_t
      namespace_collision_shadowing_lowering_import_edge_candidate_sites = 0;
  std::size_t
      namespace_collision_shadowing_lowering_object_pointer_type_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_pointer_declarator_sites =
      0;
  std::size_t namespace_collision_shadowing_lowering_normalized_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_contract_violation_sites =
      0;
  bool deterministic_namespace_collision_shadowing_lowering_handoff = false;
  std::string lowering_public_private_api_partition_replay_key;
  std::size_t public_private_api_partition_lowering_sites = 0;
  std::size_t public_private_api_partition_lowering_namespace_segment_sites = 0;
  std::size_t
      public_private_api_partition_lowering_import_edge_candidate_sites = 0;
  std::size_t public_private_api_partition_lowering_object_pointer_type_sites =
      0;
  std::size_t public_private_api_partition_lowering_pointer_declarator_sites =
      0;
  std::size_t public_private_api_partition_lowering_normalized_sites = 0;
  std::size_t public_private_api_partition_lowering_contract_violation_sites =
      0;
  bool deterministic_public_private_api_partition_lowering_handoff = false;
};
