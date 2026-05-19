#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendTypeConstraintsMetadata {
  std::string lowering_lightweight_generic_constraint_replay_key;
  std::size_t lightweight_generic_constraint_lowering_generic_constraint_sites =
      0;
  std::size_t lightweight_generic_constraint_lowering_generic_suffix_sites = 0;
  std::size_t lightweight_generic_constraint_lowering_object_pointer_type_sites =
      0;
  std::size_t
      lightweight_generic_constraint_lowering_terminated_generic_suffix_sites =
          0;
  std::size_t lightweight_generic_constraint_lowering_pointer_declarator_sites =
      0;
  std::size_t
      lightweight_generic_constraint_lowering_normalized_constraint_sites = 0;
  std::size_t
      lightweight_generic_constraint_lowering_contract_violation_sites = 0;
  bool deterministic_lightweight_generic_constraint_lowering_handoff = false;
  std::string lowering_nullability_flow_warning_precision_replay_key;
  std::size_t nullability_flow_warning_precision_lowering_sites = 0;
  std::size_t
      nullability_flow_warning_precision_lowering_object_pointer_type_sites = 0;
  std::size_t
      nullability_flow_warning_precision_lowering_nullability_suffix_sites = 0;
  std::size_t
      nullability_flow_warning_precision_lowering_nullable_suffix_sites = 0;
  std::size_t
      nullability_flow_warning_precision_lowering_nonnull_suffix_sites = 0;
  std::size_t nullability_flow_warning_precision_lowering_normalized_sites = 0;
  std::size_t
      nullability_flow_warning_precision_lowering_contract_violation_sites = 0;
  bool deterministic_nullability_flow_warning_precision_lowering_handoff =
      false;
};
