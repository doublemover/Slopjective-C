#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace objc3c::opt {

enum class Objc3SemanticOptimizationPassMode {
  kEnabled,
  kReserved,
  kVerifierOnly,
};

enum class Objc3SemanticOptimizationDecision {
  kApplied,
  kVerified,
  kSkippedFailClosed,
  kRejectedFailClosed,
};

struct Objc3SemanticOptimizationPassPlan {
  std::string pass_id;
  std::size_t ordinal = 0;
  Objc3SemanticOptimizationPassMode mode =
      Objc3SemanticOptimizationPassMode::kReserved;
  bool semantic_preserving = true;
  bool rewrites_ir = false;
  bool invalidates_global_proof_state = false;
  std::string missing_proof_action = "REJECT_FAIL_CLOSED";
  std::vector<std::string> required_proofs;
  std::string failure_diagnostic;
};

struct Objc3SemanticOptimizationCandidate {
  std::string pass_id;
  std::string source_replay_key;
  bool benchmark_governance_ready = false;
  bool typed_sema_handoff_deterministic = false;
  bool lowering_boundary_replay_key_present = false;
  bool parse_to_lowering_ready = false;
  bool compile_time_nil_receiver = false;
  bool optional_send_side_effect_free = false;
  bool direct_callee_symbol_present = false;
  bool direct_signature_present = false;
  bool explicit_argument_types_cover_arguments = false;
  bool direct_result_owns_ir_storage = false;
  bool arc_mode_enabled = false;
  bool method_family_returns_retained_result = false;
  bool related_result_cleanup_disarmed = false;
  bool runtime_dispatch_result_owner_explicit = false;
  bool canonical_runtime_dispatch_symbol = false;
  bool retired_routes_disabled = true;
  bool compatibility_routes_disabled = true;
  bool exact_target_receiver_static_type_proven = false;
  bool sealed_final_dispatch_evidence_present = false;
  bool exact_method_target_identity_present = false;
  bool class_category_method_mutation_generation_pinned = false;
  bool runtime_cache_version_dependency_pinned = false;
  bool devirtualization_ownership_arc_safe = false;
  bool devirtualization_source_map_debug_preserved = false;
  bool devirtualization_runtime_abi_safe = false;
  bool devirtualization_package_abi_identical = false;
  bool all_prior_mutations_declared_invalidation = false;
  bool unsupported_skips_emit_no_success_claim = false;
  bool semantic_equivalence_verdict_present = false;
};

struct Objc3SemanticOptimizationResult {
  std::string pass_id;
  Objc3SemanticOptimizationDecision decision =
      Objc3SemanticOptimizationDecision::kRejectedFailClosed;
  bool semantic_preserving = true;
  bool rewrites_ir = false;
  bool invalidates_global_proof_state = false;
  bool success_claim = false;
  std::string diagnostic;
  std::string metadata_key;
};

const char *Objc3SemanticOptimizationDecisionName(
    Objc3SemanticOptimizationDecision decision);

std::vector<Objc3SemanticOptimizationPassPlan>
BuildObjc3SemanticOptimizationPassPlans();

Objc3SemanticOptimizationResult EvaluateObjc3SemanticOptimizationCandidate(
    const Objc3SemanticOptimizationCandidate &candidate);

std::vector<Objc3SemanticOptimizationResult>
RunObjc3SemanticOptimizationPipelineTrace(
    const std::vector<Objc3SemanticOptimizationCandidate> &candidates);

bool AllObjc3SemanticOptimizationTraceMutationsDeclareInvalidation(
    const std::vector<Objc3SemanticOptimizationResult> &trace);

bool AllObjc3SemanticOptimizationTraceReservedSkipsAreClaimless(
    const std::vector<Objc3SemanticOptimizationResult> &trace);

bool AllObjc3SemanticOptimizationTraceDecisionsFailClosed(
    const std::vector<Objc3SemanticOptimizationResult> &trace);

}  // namespace objc3c::opt
