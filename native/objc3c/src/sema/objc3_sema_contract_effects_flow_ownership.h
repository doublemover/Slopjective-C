#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3EffectsOwnershipSemanticModelContractId =
    "objc3c.effects.ownership.semantic.model.closure.v1";
inline constexpr const char *kObjc3EffectsOwnershipSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_effects_ownership_semantic_model";
inline constexpr const char *kObjc3EffectsOwnershipSemanticModelRule =
    "arc-block-throws-async-actor-and-foreign-boundary-semantics-share-one-deterministic-effects-and-ownership-summary-rooted-in-live-sema-lowering-surfaces";

struct Objc3EffectsOwnershipSemanticModelSummary {
  std::string contract_id = kObjc3EffectsOwnershipSemanticModelContractId;
  std::string surface_path = kObjc3EffectsOwnershipSemanticModelSurfacePath;
  std::string semantic_model = kObjc3EffectsOwnershipSemanticModelRule;
  std::size_t arc_ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t weak_zeroing_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t autoreleasepool_scope_sites = 0;
  std::size_t cleanup_order_exit_sites = 0;
  std::size_t block_literal_sites = 0;
  std::size_t stack_to_heap_promotion_sites = 0;
  std::size_t byref_forwarding_cell_sites = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t captured_object_lifetime_sites = 0;
  std::size_t throws_propagation_sites = 0;
  std::size_t unwind_cleanup_sites = 0;
  std::size_t bridged_error_sites = 0;
  std::size_t nested_cleanup_sites = 0;
  std::size_t foreign_boundary_sites = 0;
  std::size_t async_continuation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t reentrancy_policy_sites = 0;
  std::size_t imported_actor_api_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool arc_semantics_landed = false;
  bool block_escape_semantics_landed = false;
  bool throws_cleanup_semantics_landed = false;
  bool async_task_semantics_landed = false;
  bool actor_semantics_landed = false;
  bool foreign_boundary_semantics_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};
