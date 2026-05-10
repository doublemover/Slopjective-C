#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendOwnershipSupportMetadata {
  std::string lowering_ownership_system_extension_replay_key;
  std::size_t ownership_system_extension_lowering_cleanup_hook_sites = 0;
  std::size_t ownership_system_extension_lowering_resource_local_sites = 0;
  std::size_t ownership_system_extension_lowering_cleanup_owned_local_sites = 0;
  std::size_t ownership_system_extension_lowering_resource_move_capture_sites = 0;
  std::size_t ownership_system_extension_lowering_borrowed_parameter_sites = 0;
  std::size_t ownership_system_extension_lowering_borrowed_return_callable_sites = 0;
  std::size_t ownership_system_extension_lowering_borrowed_escape_candidate_sites = 0;
  std::size_t ownership_system_extension_lowering_explicit_capture_item_sites = 0;
  std::size_t ownership_system_extension_lowering_retainable_family_callable_sites = 0;
  std::size_t ownership_system_extension_lowering_retainable_family_operation_callable_sites = 0;
  std::size_t ownership_system_extension_lowering_retainable_family_alias_callable_sites = 0;
  std::size_t ownership_system_extension_lowering_guard_blocked_sites = 0;
  std::size_t ownership_system_extension_lowering_contract_violation_sites = 0;
  bool deterministic_ownership_system_extension_lowering_handoff = false;
  std::string ownership_borrowed_retainable_abi_completion_replay_key;
  std::size_t ownership_borrowed_retainable_returns_borrowed_attribute_sites = 0;
  std::size_t ownership_borrowed_retainable_family_retain_sites = 0;
  std::size_t ownership_borrowed_retainable_family_release_sites = 0;
  std::size_t ownership_borrowed_retainable_family_autorelease_sites = 0;
  std::size_t ownership_borrowed_retainable_compatibility_returns_retained_sites = 0;
  std::size_t ownership_borrowed_retainable_compatibility_returns_not_retained_sites = 0;
  std::size_t ownership_borrowed_retainable_compatibility_consumed_sites = 0;
  bool deterministic_ownership_borrowed_retainable_abi_completion_handoff = false;
};
