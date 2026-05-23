#include "opt/objc3_semantic_optimization_executor.h"

#include <sstream>
#include <unordered_map>

namespace objc3c::opt {
namespace {

const std::vector<Objc3SemanticOptimizationPassPlan> &PassPlans() {
  static const std::vector<Objc3SemanticOptimizationPassPlan> plans = {
      {"semantic-precondition-gate",
       10,
       Objc3SemanticOptimizationPassMode::kVerifierOnly,
       true,
       false,
       false,
       "REJECT_FAIL_CLOSED",
       {"typed sema handoff is deterministic",
        "lowering boundary replay key is present",
        "parse-to-lowering readiness is ready"},
       "semantic optimization precondition gate failed"},
      {"nil-receiver-folding",
       20,
       Objc3SemanticOptimizationPassMode::kEnabled,
       true,
       true,
       false,
       "SKIP_FAIL_CLOSED",
       {"receiver fact is compile_time_nil_receiver",
        "selector and argument lowering are side-effect free for optional nil send",
        "benchmark governance is ready"},
       "nil receiver folding requires compile-time nil receiver proof"},
      {"direct-dispatch-exact-call",
       30,
       Objc3SemanticOptimizationPassMode::kEnabled,
       true,
       true,
       true,
       "REJECT_FAIL_CLOSED",
       {"callee symbol and signature are both present",
        "explicit argument types cover every explicit argument",
        "direct dispatch result owns explicit IR storage",
        "benchmark governance is ready"},
       "direct dispatch lowering requires exact symbol, signature, and result ownership"},
      {"arc-retained-result-cleanup",
       40,
       Objc3SemanticOptimizationPassMode::kEnabled,
       true,
       true,
       false,
       "SKIP_FAIL_CLOSED",
       {"ARC mode is enabled",
        "method family returns a retained object result",
        "related-result receiver cleanup is disarmed before transfer",
        "benchmark governance is ready"},
       "ARC retained-result cleanup requires method-family ownership proof"},
      {"runtime-dispatch-preservation",
       50,
       Objc3SemanticOptimizationPassMode::kVerifierOnly,
       true,
       false,
       false,
       "REJECT_FAIL_CLOSED",
       {"runtime dispatch result owner is explicit",
        "dispatch symbol is canonical or typed canonical from-class variant",
        "retired routes and compatibility routes remain disabled"},
       "runtime dispatch preservation rejected non-canonical dispatch emission"},
      {"devirtualization",
       60,
       Objc3SemanticOptimizationPassMode::kEnabled,
       true,
       true,
       true,
       "REJECT_FAIL_CLOSED",
       {"sealed or final dispatch evidence is present",
        "static receiver type proof identifies one concrete class target",
        "exact method target identity resolves selector to one ABI-compatible implementation",
        "class category and method mutation generation snapshot is pinned",
        "runtime cache version dependency is pinned before bypassing dispatch lookup",
        "ownership and ARC transfer safety is preserved",
        "source-map and line-table debug preservation is proven",
        "package import ABI identity and runtime ABI compatibility are identical"},
       "exact-target devirtualization requires sealed or final dispatch, static receiver, mutation invalidation, runtime cache, ownership, source-map, ABI, and package proofs"},
      {"method-inlining",
       70,
       Objc3SemanticOptimizationPassMode::kEnabled,
       true,
       true,
       true,
       "REJECT_FAIL_CLOSED",
       {"callee body identity is present",
        "original call source span and callee source span are linked",
        "inline frame id and inlined callsite source span are present",
        "imported and emitted debug maps both carry inline-frame records",
        "debug stepping policy is explicit and unambiguous",
        "optimized IR/source correlation is present",
        "inline-frame maps are compiler-owned source maps, not generated-only reports",
        "receiver and dispatch assumptions are pinned",
        "function or final method belongs to the scalar inline subset",
        "ownership and ARC effects are replayable",
        "side-effect summary is pure and call-free",
        "side-effect replay is complete and contains no hidden writes, calls, allocations, runtime helpers, ownership transfers, or error edges",
        "source-map inline frame and line-table preservation are proven",
        "diagnostic source location is preserved",
        "debug stepping evidence preserves caller and callee frames",
        "runtime ABI and package import identity are unchanged",
        "inlining depth is within limit and recursion is absent",
        "callee generation snapshot is pinned",
        "runtime cache and dispatch assumptions are fresh",
        "runtime invalidation replay is present for stale dispatch/cache assumptions",
        "invalidation covers callee body, generation, ownership, source-map, diagnostic, debug stepping, runtime cache, ABI, and package proof state"},
       "method inlining requires callee body identity, original call/source spans, inline-frame source-map identity, receiver/dispatch assumptions, scalar subset, ownership, side-effect replay, source-map, debug stepping, diagnostic, ABI/package, depth, recursion, generation, runtime cache freshness, and invalidation replay proofs"},
      {"cache-aware-dispatch",
       80,
       Objc3SemanticOptimizationPassMode::kEnabled,
       true,
       true,
       true,
       "REJECT_FAIL_CLOSED",
       {"runtime cache invalidation semantics are public",
        "ABI-stable helper symbol exists",
        "semantic replay preserves dispatch miss behavior",
        "strict dispatch status envelope is checked",
        "source-map and debug trace anchors are preserved"},
       "cache-aware dispatch requires runtime cache ABI, strict status handling, semantic replay, and source-map debug preservation"},
      {"ir-cleanup-verifier",
       90,
       Objc3SemanticOptimizationPassMode::kVerifierOnly,
       true,
       false,
       false,
       "REJECT_FAIL_CLOSED",
       {"pass order trace is deterministic",
        "all mutating passes declared invalidation",
        "all unsupported opportunities emitted skip diagnostics"},
       "post optimization IR cleanup verifier failed semantic equivalence"},
  };
  return plans;
}

std::unordered_map<std::string, Objc3SemanticOptimizationPassPlan> BuildPlanMap() {
  std::unordered_map<std::string, Objc3SemanticOptimizationPassPlan> plans;
  for (const auto &plan : PassPlans()) {
    plans.emplace(plan.pass_id, plan);
  }
  return plans;
}

const Objc3SemanticOptimizationPassPlan *FindPlan(const std::string &pass_id) {
  static const auto plans = BuildPlanMap();
  const auto found = plans.find(pass_id);
  return found == plans.end() ? nullptr : &found->second;
}

bool UsesSkipFailClosed(const Objc3SemanticOptimizationPassPlan &plan) {
  return plan.missing_proof_action == "SKIP_FAIL_CLOSED";
}

Objc3SemanticOptimizationDecision MissingProofDecision(
    const Objc3SemanticOptimizationPassPlan &plan) {
  return UsesSkipFailClosed(plan)
             ? Objc3SemanticOptimizationDecision::kSkippedFailClosed
             : Objc3SemanticOptimizationDecision::kRejectedFailClosed;
}

std::string BuildObjc3SemanticOptimizationMetadataKey(
    const Objc3SemanticOptimizationPassPlan &plan,
    Objc3SemanticOptimizationDecision decision,
    const Objc3SemanticOptimizationCandidate &candidate) {
  std::ostringstream key;
  key << "objc3-semantic-optimization:v1"
      << ";pass=" << plan.pass_id
      << ";ordinal=" << plan.ordinal
      << ";decision=" << Objc3SemanticOptimizationDecisionName(decision)
      << ";rewrites-ir=" << (plan.rewrites_ir ? "true" : "false")
      << ";invalidates-global-proof-state="
      << (plan.invalidates_global_proof_state ? "true" : "false")
      << ";success-claim="
      << (decision == Objc3SemanticOptimizationDecision::kApplied ? "true"
                                                                   : "false")
      << ";source=" << candidate.source_replay_key;
  if (plan.pass_id == "method-inlining") {
    key << ";inline-callee-body-identity="
        << (candidate.method_inline_callee_body_identity_present ? "true"
                                                                 : "false")
        << ";inline-original-call-source-span="
        << candidate.method_inline_original_call_source_span_key
        << ";inline-callee-source-span="
        << candidate.method_inline_callee_source_span_key
        << ";inline-original-call-source-span-present="
        << (candidate.method_inline_original_call_source_span_present ? "true"
                                                                      : "false")
        << ";inline-frame-id="
        << candidate.method_inline_inline_frame_id_key
        << ";inline-frame-id-present="
        << (candidate.method_inline_inline_frame_id_present ? "true" : "false")
        << ";inline-inlined-callsite-source-span="
        << candidate.method_inline_inlined_callsite_source_span_key
        << ";inline-inlined-callsite-source-span-present="
        << (candidate.method_inline_inlined_callsite_source_span_present
                ? "true"
                : "false")
        << ";inline-imported-debug-map-frame-present="
        << (candidate.method_inline_imported_debug_map_inline_frame_present
                ? "true"
                : "false")
        << ";inline-emitted-debug-map-frame-present="
        << (candidate.method_inline_emitted_debug_map_inline_frame_present
                ? "true"
                : "false")
        << ";inline-stepping-policy="
        << candidate.method_inline_stepping_policy_key
        << ";inline-stepping-policy-unambiguous="
        << (candidate.method_inline_stepping_policy_unambiguous ? "true"
                                                                : "false")
        << ";inline-optimized-ir-source-correlation="
        << candidate.method_inline_optimized_ir_source_correlation_key
        << ";inline-optimized-ir-source-correlation-present="
        << (candidate.method_inline_optimized_ir_source_correlation_present
                ? "true"
                : "false")
        << ";inline-generated-only-source-map="
        << (candidate.method_inline_generated_only_source_map ? "true"
                                                              : "false")
        << ";inline-receiver-dispatch-assumption="
        << candidate.method_inline_receiver_dispatch_assumption_key
        << ";inline-receiver-dispatch-assumptions-pinned="
        << (candidate.method_inline_receiver_dispatch_assumptions_pinned
                ? "true"
                : "false")
        << ";inline-scalar-subset="
        << (candidate.method_inline_scalar_subset ? "true" : "false")
        << ";inline-ownership-arc-effects="
        << (candidate.method_inline_ownership_arc_effects_safe ? "true"
                                                               : "false")
        << ";inline-side-effect-summary="
        << (candidate.method_inline_side_effect_summary_safe ? "true"
                                                             : "false")
        << ";inline-side-effect-replay="
        << candidate.method_inline_side_effect_replay_key
        << ";inline-side-effect-replay-complete="
        << (candidate.method_inline_side_effect_replay_complete ? "true"
                                                                : "false")
        << ";inline-source-map-debug="
        << (candidate.method_inline_source_map_debug_preserved ? "true"
                                                               : "false")
        << ";inline-diagnostic-location="
        << (candidate.method_inline_diagnostic_location_preserved ? "true"
                                                                  : "false")
        << ";inline-debug-stepping-evidence="
        << candidate.method_inline_debug_stepping_evidence_key
        << ";inline-debug-stepping-evidence-present="
        << (candidate.method_inline_debug_stepping_evidence_present ? "true"
                                                                    : "false")
        << ";inline-runtime-abi="
        << (candidate.method_inline_runtime_abi_safe ? "true" : "false")
        << ";inline-package-abi="
        << (candidate.method_inline_package_abi_identical ? "true" : "false")
        << ";inline-depth-within-limit="
        << (candidate.method_inline_depth_within_limit ? "true" : "false")
        << ";inline-recursion-absent="
        << (candidate.method_inline_recursion_absent ? "true" : "false")
        << ";inline-callee-generation-pinned="
        << (candidate.method_inline_callee_generation_pinned ? "true"
                                                             : "false")
        << ";inline-runtime-cache-assumptions-fresh="
        << (candidate.method_inline_runtime_cache_assumptions_fresh ? "true"
                                                                    : "false")
        << ";inline-runtime-invalidation-replay="
        << candidate.method_inline_runtime_invalidation_replay_key
        << ";inline-runtime-invalidation-replay-present="
        << (candidate.method_inline_runtime_invalidation_replay_present
                ? "true"
                : "false")
        << ";inline-invalidation-complete="
        << (candidate.method_inline_invalidation_complete ? "true" : "false");
  }
  if (plan.pass_id == "cache-aware-dispatch") {
    key << ";runtime-cache-invalidation-public="
        << (candidate.runtime_cache_invalidation_semantics_public ? "true"
                                                                  : "false")
        << ";helper-symbol-present="
        << (candidate.cache_aware_helper_symbol_present ? "true" : "false")
        << ";miss-replay-preserved="
        << (candidate.cache_aware_semantic_replay_preserves_miss_behavior
                ? "true"
                : "false")
        << ";strict-status-envelope-checked="
        << (candidate.cache_aware_strict_status_envelope_checked ? "true"
                                                                 : "false")
        << ";source-map-debug-preserved="
        << (candidate.cache_aware_source_map_debug_preserved ? "true"
                                                            : "false");
  }
  return key.str();
}

void AppendMissingGate(bool present,
                       const char *gate,
                       std::vector<std::string> &missing) {
  if (!present) {
    missing.push_back(gate);
  }
}

void AppendMissingTextGate(const std::string &value,
                           const char *gate,
                           std::vector<std::string> &missing) {
  if (value.empty()) {
    missing.push_back(gate);
  }
}

std::string BuildMethodInliningFailClosedDiagnostic(
    const Objc3SemanticOptimizationPassPlan &plan,
    const Objc3SemanticOptimizationCandidate &candidate) {
  std::vector<std::string> missing;
  AppendMissingGate(candidate.benchmark_governance_ready,
                    "benchmark_governance_ready", missing);
  AppendMissingGate(candidate.method_inline_callee_body_identity_present,
                    "method_inline_callee_body_identity_present", missing);
  AppendMissingTextGate(candidate.method_inline_original_call_source_span_key,
                        "method_inline_original_call_source_span_key", missing);
  AppendMissingTextGate(candidate.method_inline_callee_source_span_key,
                        "method_inline_callee_source_span_key", missing);
  AppendMissingGate(candidate.method_inline_original_call_source_span_present,
                    "method_inline_original_call_source_span_present", missing);
  AppendMissingTextGate(candidate.method_inline_inline_frame_id_key,
                        "method_inline_inline_frame_id_key", missing);
  AppendMissingGate(candidate.method_inline_inline_frame_id_present,
                    "method_inline_inline_frame_id_present", missing);
  AppendMissingTextGate(candidate.method_inline_inlined_callsite_source_span_key,
                        "method_inline_inlined_callsite_source_span_key",
                        missing);
  AppendMissingGate(candidate.method_inline_inlined_callsite_source_span_present,
                    "method_inline_inlined_callsite_source_span_present",
                    missing);
  AppendMissingGate(
      candidate.method_inline_imported_debug_map_inline_frame_present,
      "method_inline_imported_debug_map_inline_frame_present", missing);
  AppendMissingGate(
      candidate.method_inline_emitted_debug_map_inline_frame_present,
      "method_inline_emitted_debug_map_inline_frame_present", missing);
  AppendMissingTextGate(candidate.method_inline_stepping_policy_key,
                        "method_inline_stepping_policy_key", missing);
  AppendMissingGate(candidate.method_inline_stepping_policy_unambiguous,
                    "method_inline_stepping_policy_unambiguous", missing);
  AppendMissingTextGate(
      candidate.method_inline_optimized_ir_source_correlation_key,
      "method_inline_optimized_ir_source_correlation_key", missing);
  AppendMissingGate(
      candidate.method_inline_optimized_ir_source_correlation_present,
      "method_inline_optimized_ir_source_correlation_present", missing);
  AppendMissingGate(!candidate.method_inline_generated_only_source_map,
                    "method_inline_generated_only_source_map", missing);
  AppendMissingTextGate(
      candidate.method_inline_receiver_dispatch_assumption_key,
      "method_inline_receiver_dispatch_assumption_key", missing);
  AppendMissingGate(
      candidate.method_inline_receiver_dispatch_assumptions_pinned,
      "method_inline_receiver_dispatch_assumptions_pinned", missing);
  AppendMissingGate(candidate.method_inline_scalar_subset,
                    "method_inline_scalar_subset", missing);
  AppendMissingGate(candidate.method_inline_ownership_arc_effects_safe,
                    "method_inline_ownership_arc_effects_safe", missing);
  AppendMissingGate(candidate.method_inline_side_effect_summary_safe,
                    "method_inline_side_effect_summary_safe", missing);
  AppendMissingTextGate(candidate.method_inline_side_effect_replay_key,
                        "method_inline_side_effect_replay_key", missing);
  AppendMissingGate(candidate.method_inline_side_effect_replay_complete,
                    "method_inline_side_effect_replay_complete", missing);
  AppendMissingGate(candidate.method_inline_source_map_debug_preserved,
                    "method_inline_source_map_debug_preserved", missing);
  AppendMissingGate(candidate.method_inline_diagnostic_location_preserved,
                    "method_inline_diagnostic_location_preserved", missing);
  AppendMissingTextGate(candidate.method_inline_debug_stepping_evidence_key,
                        "method_inline_debug_stepping_evidence_key", missing);
  AppendMissingGate(candidate.method_inline_debug_stepping_evidence_present,
                    "method_inline_debug_stepping_evidence_present", missing);
  AppendMissingGate(candidate.method_inline_runtime_abi_safe,
                    "method_inline_runtime_abi_safe", missing);
  AppendMissingGate(candidate.method_inline_package_abi_identical,
                    "method_inline_package_abi_identical", missing);
  AppendMissingGate(candidate.method_inline_depth_within_limit,
                    "method_inline_depth_within_limit", missing);
  AppendMissingGate(candidate.method_inline_recursion_absent,
                    "method_inline_recursion_absent", missing);
  AppendMissingGate(candidate.method_inline_callee_generation_pinned,
                    "method_inline_callee_generation_pinned", missing);
  AppendMissingGate(candidate.method_inline_runtime_cache_assumptions_fresh,
                    "method_inline_runtime_cache_assumptions_fresh", missing);
  AppendMissingTextGate(candidate.method_inline_runtime_invalidation_replay_key,
                        "method_inline_runtime_invalidation_replay_key",
                        missing);
  AppendMissingGate(candidate.method_inline_runtime_invalidation_replay_present,
                    "method_inline_runtime_invalidation_replay_present",
                    missing);
  AppendMissingGate(candidate.method_inline_invalidation_complete,
                    "method_inline_invalidation_complete", missing);

  if (missing.empty()) {
    return plan.failure_diagnostic;
  }

  std::ostringstream diagnostic;
  diagnostic << plan.failure_diagnostic << "; failed gates: ";
  for (std::size_t i = 0; i < missing.size(); ++i) {
    if (i != 0) {
      diagnostic << ", ";
    }
    diagnostic << missing[i];
  }
  return diagnostic.str();
}

Objc3SemanticOptimizationResult MakeResult(
    const Objc3SemanticOptimizationPassPlan &plan,
    Objc3SemanticOptimizationDecision decision,
    const Objc3SemanticOptimizationCandidate &candidate,
    const std::string &diagnostic) {
  Objc3SemanticOptimizationResult result;
  result.pass_id = plan.pass_id;
  result.decision = decision;
  result.semantic_preserving = plan.semantic_preserving;
  result.rewrites_ir = plan.rewrites_ir;
  result.invalidates_global_proof_state = plan.invalidates_global_proof_state;
  result.success_claim = decision == Objc3SemanticOptimizationDecision::kApplied;
  if (decision == Objc3SemanticOptimizationDecision::kSkippedFailClosed) {
    result.success_claim = false;
  }
  result.diagnostic = diagnostic;
  result.metadata_key =
      BuildObjc3SemanticOptimizationMetadataKey(plan, decision, candidate);
  return result;
}

Objc3SemanticOptimizationResult MissingProofResult(
    const Objc3SemanticOptimizationPassPlan &plan,
    const Objc3SemanticOptimizationCandidate &candidate) {
  if (plan.pass_id == "method-inlining") {
    return MakeResult(plan, MissingProofDecision(plan), candidate,
                      BuildMethodInliningFailClosedDiagnostic(plan, candidate));
  }
  if (plan.pass_id == "cache-aware-dispatch") {
    std::vector<std::string> missing;
    AppendMissingGate(
        candidate.runtime_cache_invalidation_semantics_public,
        "runtime_cache_invalidation_semantics_public", missing);
    AppendMissingGate(candidate.cache_aware_helper_symbol_present,
                      "cache_aware_helper_symbol_present", missing);
    AppendMissingGate(
        candidate.cache_aware_semantic_replay_preserves_miss_behavior,
        "cache_aware_semantic_replay_preserves_miss_behavior", missing);
    AppendMissingGate(candidate.cache_aware_strict_status_envelope_checked,
                      "cache_aware_strict_status_envelope_checked", missing);
    AppendMissingGate(candidate.cache_aware_source_map_debug_preserved,
                      "cache_aware_source_map_debug_preserved", missing);
    if (missing.empty()) {
      return MakeResult(plan, MissingProofDecision(plan), candidate,
                        plan.failure_diagnostic);
    }
    std::ostringstream diagnostic;
    diagnostic << plan.failure_diagnostic << "; failed gates: ";
    for (std::size_t i = 0; i < missing.size(); ++i) {
      if (i != 0) {
        diagnostic << ", ";
      }
      diagnostic << missing[i];
    }
    return MakeResult(plan, MissingProofDecision(plan), candidate,
                      diagnostic.str());
  }
  return MakeResult(plan, MissingProofDecision(plan), candidate,
                    plan.failure_diagnostic);
}

}  // namespace

const char *Objc3SemanticOptimizationDecisionName(
    Objc3SemanticOptimizationDecision decision) {
  switch (decision) {
    case Objc3SemanticOptimizationDecision::kApplied:
      return "APPLIED";
    case Objc3SemanticOptimizationDecision::kVerified:
      return "VERIFIED";
    case Objc3SemanticOptimizationDecision::kSkippedFailClosed:
      return "SKIPPED_FAIL_CLOSED";
    case Objc3SemanticOptimizationDecision::kRejectedFailClosed:
      return "REJECTED_FAIL_CLOSED";
  }
  return "REJECTED_FAIL_CLOSED";
}

std::vector<Objc3SemanticOptimizationPassPlan>
BuildObjc3SemanticOptimizationPassPlans() {
  return PassPlans();
}

Objc3SemanticOptimizationResult EvaluateObjc3SemanticOptimizationCandidate(
    const Objc3SemanticOptimizationCandidate &candidate) {
  const auto *plan = FindPlan(candidate.pass_id);
  if (plan == nullptr) {
    Objc3SemanticOptimizationPassPlan rejected;
    rejected.pass_id = candidate.pass_id.empty() ? "<missing>" : candidate.pass_id;
    rejected.failure_diagnostic =
        "semantic optimization candidate has no registered pass";
    return MakeResult(rejected,
                      Objc3SemanticOptimizationDecision::kRejectedFailClosed,
                      candidate, rejected.failure_diagnostic);
  }

  if (plan->mode == Objc3SemanticOptimizationPassMode::kReserved) {
    return MakeResult(*plan,
                      Objc3SemanticOptimizationDecision::kSkippedFailClosed,
                      candidate, plan->failure_diagnostic);
  }

  if (plan->rewrites_ir && !candidate.benchmark_governance_ready) {
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "semantic-precondition-gate") {
    if (candidate.typed_sema_handoff_deterministic &&
        candidate.lowering_boundary_replay_key_present &&
        candidate.parse_to_lowering_ready) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kVerified,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "nil-receiver-folding") {
    if (candidate.compile_time_nil_receiver &&
        candidate.optional_send_side_effect_free) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kApplied,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "direct-dispatch-exact-call") {
    if (candidate.direct_callee_symbol_present &&
        candidate.direct_signature_present &&
        candidate.explicit_argument_types_cover_arguments &&
        candidate.direct_result_owns_ir_storage) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kApplied,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "arc-retained-result-cleanup") {
    if (candidate.arc_mode_enabled &&
        candidate.method_family_returns_retained_result &&
        candidate.related_result_cleanup_disarmed) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kApplied,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "runtime-dispatch-preservation") {
    if (candidate.runtime_dispatch_result_owner_explicit &&
        candidate.canonical_runtime_dispatch_symbol &&
        candidate.retired_routes_disabled &&
        candidate.compatibility_routes_disabled) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kVerified,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "devirtualization") {
    if (candidate.exact_target_receiver_static_type_proven &&
        candidate.sealed_final_dispatch_evidence_present &&
        candidate.exact_method_target_identity_present &&
        candidate.class_category_method_mutation_generation_pinned &&
        candidate.runtime_cache_version_dependency_pinned &&
        candidate.devirtualization_ownership_arc_safe &&
        candidate.devirtualization_source_map_debug_preserved &&
        candidate.devirtualization_runtime_abi_safe &&
        candidate.devirtualization_package_abi_identical) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kApplied,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "method-inlining") {
    if (candidate.method_inline_callee_body_identity_present &&
        !candidate.method_inline_original_call_source_span_key.empty() &&
        !candidate.method_inline_callee_source_span_key.empty() &&
        candidate.method_inline_original_call_source_span_present &&
        !candidate.method_inline_inline_frame_id_key.empty() &&
        candidate.method_inline_inline_frame_id_present &&
        !candidate.method_inline_inlined_callsite_source_span_key.empty() &&
        candidate.method_inline_inlined_callsite_source_span_present &&
        candidate.method_inline_imported_debug_map_inline_frame_present &&
        candidate.method_inline_emitted_debug_map_inline_frame_present &&
        !candidate.method_inline_stepping_policy_key.empty() &&
        candidate.method_inline_stepping_policy_unambiguous &&
        !candidate.method_inline_optimized_ir_source_correlation_key.empty() &&
        candidate.method_inline_optimized_ir_source_correlation_present &&
        !candidate.method_inline_generated_only_source_map &&
        !candidate.method_inline_receiver_dispatch_assumption_key.empty() &&
        candidate.method_inline_receiver_dispatch_assumptions_pinned &&
        candidate.method_inline_scalar_subset &&
        candidate.method_inline_ownership_arc_effects_safe &&
        candidate.method_inline_side_effect_summary_safe &&
        !candidate.method_inline_side_effect_replay_key.empty() &&
        candidate.method_inline_side_effect_replay_complete &&
        candidate.method_inline_source_map_debug_preserved &&
        candidate.method_inline_diagnostic_location_preserved &&
        !candidate.method_inline_debug_stepping_evidence_key.empty() &&
        candidate.method_inline_debug_stepping_evidence_present &&
        candidate.method_inline_runtime_abi_safe &&
        candidate.method_inline_package_abi_identical &&
        candidate.method_inline_depth_within_limit &&
        candidate.method_inline_recursion_absent &&
        candidate.method_inline_callee_generation_pinned &&
        candidate.method_inline_runtime_cache_assumptions_fresh &&
        !candidate.method_inline_runtime_invalidation_replay_key.empty() &&
        candidate.method_inline_runtime_invalidation_replay_present &&
        candidate.method_inline_invalidation_complete) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kApplied,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "cache-aware-dispatch") {
    if (candidate.runtime_cache_invalidation_semantics_public &&
        candidate.cache_aware_helper_symbol_present &&
        candidate.cache_aware_semantic_replay_preserves_miss_behavior &&
        candidate.cache_aware_strict_status_envelope_checked &&
        candidate.cache_aware_source_map_debug_preserved) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kApplied,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  if (plan->pass_id == "ir-cleanup-verifier") {
    if (candidate.all_prior_mutations_declared_invalidation &&
        candidate.unsupported_skips_emit_no_success_claim &&
        candidate.semantic_equivalence_verdict_present) {
      return MakeResult(*plan, Objc3SemanticOptimizationDecision::kVerified,
                        candidate, "");
    }
    return MissingProofResult(*plan, candidate);
  }

  return MissingProofResult(*plan, candidate);
}

std::vector<Objc3SemanticOptimizationResult>
RunObjc3SemanticOptimizationPipelineTrace(
    const std::vector<Objc3SemanticOptimizationCandidate> &candidates) {
  std::vector<Objc3SemanticOptimizationResult> trace;
  trace.reserve(candidates.size());
  for (const auto &candidate : candidates) {
    trace.push_back(EvaluateObjc3SemanticOptimizationCandidate(candidate));
  }
  return trace;
}

bool AllObjc3SemanticOptimizationTraceMutationsDeclareInvalidation(
    const std::vector<Objc3SemanticOptimizationResult> &trace) {
  for (const auto &result : trace) {
    if (!result.rewrites_ir ||
        result.decision != Objc3SemanticOptimizationDecision::kApplied) {
      continue;
    }
    if (result.pass_id == "direct-dispatch-exact-call" &&
        !result.invalidates_global_proof_state) {
      return false;
    }
    if (result.pass_id == "devirtualization" &&
        !result.invalidates_global_proof_state) {
      return false;
    }
    if (result.pass_id == "method-inlining" &&
        !result.invalidates_global_proof_state) {
      return false;
    }
    if (result.pass_id == "cache-aware-dispatch" &&
        !result.invalidates_global_proof_state) {
      return false;
    }
    if (result.metadata_key.find("rewrites-ir=true") == std::string::npos) {
      return false;
    }
  }
  return !trace.empty();
}

bool AllObjc3SemanticOptimizationTraceReservedSkipsAreClaimless(
    const std::vector<Objc3SemanticOptimizationResult> &trace) {
  for (const auto &result : trace) {
    if (result.decision == Objc3SemanticOptimizationDecision::kSkippedFailClosed &&
        result.success_claim) {
      return false;
    }
  }
  return !trace.empty();
}

bool AllObjc3SemanticOptimizationTraceDecisionsFailClosed(
    const std::vector<Objc3SemanticOptimizationResult> &trace) {
  for (const auto &result : trace) {
    if (result.decision == Objc3SemanticOptimizationDecision::kRejectedFailClosed &&
        result.diagnostic.empty()) {
      return false;
    }
    if (result.decision == Objc3SemanticOptimizationDecision::kSkippedFailClosed &&
        result.diagnostic.empty()) {
      return false;
    }
  }
  return !trace.empty();
}

}  // namespace objc3c::opt
