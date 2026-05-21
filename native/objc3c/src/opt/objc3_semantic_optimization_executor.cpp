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
       Objc3SemanticOptimizationPassMode::kReserved,
       true,
       false,
       false,
       "SKIP_FAIL_CLOSED",
       {"class finality proof exists",
        "override set is closed",
        "ABI stability allows direct target publication"},
       "devirtualization is reserved until closed-world finality proofs exist"},
      {"method-inlining",
       70,
       Objc3SemanticOptimizationPassMode::kReserved,
       true,
       false,
       false,
       "SKIP_FAIL_CLOSED",
       {"callee body is available",
        "ownership effects are replayable",
        "debug and diagnostic source mapping are preserved"},
       "method inlining is reserved until ownership and source-map proofs exist"},
      {"cache-aware-dispatch",
       80,
       Objc3SemanticOptimizationPassMode::kReserved,
       true,
       false,
       false,
       "SKIP_FAIL_CLOSED",
       {"runtime cache invalidation semantics are public",
        "ABI-stable helper symbol exists",
        "semantic replay preserves dispatch miss behavior"},
       "cache-aware dispatch is reserved behind the runtime cache contract"},
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
      << ";success-claim=false"
      << ";source=" << candidate.source_replay_key;
  return key.str();
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
