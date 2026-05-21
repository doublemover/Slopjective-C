#include "pipeline/objc3_semantic_optimization_pipeline.h"

#include <sstream>
#include <string>
#include <unordered_set>

namespace {

bool HasPassId(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes,
    const std::string &pass_id) {
  for (const auto &pass : passes) {
    if (pass.pass_id == pass_id) {
      return true;
    }
  }
  return false;
}

bool AllTypedContractsReady(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes) {
  if (passes.empty()) {
    return false;
  }
  for (const auto &pass : passes) {
    if (!pass.semantic_preserving || pass.input_contract.empty() ||
        pass.output_contract.empty() || pass.required_preconditions.empty() ||
        pass.invalidation_contract.empty() || pass.failure_diagnostic.empty() ||
        !pass.verifies_after_pass) {
      return false;
    }
  }
  return true;
}

bool AllReservedPassesFailClosed(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes) {
  for (const auto &pass : passes) {
    if (pass.mode != Objc3SemanticOptimizationPassMode::kReserved) {
      continue;
    }
    std::string reason;
    if (!IsObjc3SemanticOptimizationPassContractFailClosed(pass, reason)) {
      return false;
    }
  }
  return true;
}

bool AllPassesVerifyAfterExecution(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes) {
  for (const auto &pass : passes) {
    if (!pass.verifies_after_pass) {
      return false;
    }
  }
  return !passes.empty();
}

bool AllMutatingPassesDeclareInvalidation(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes) {
  for (const auto &pass : passes) {
    if (!pass.rewrites_ir) {
      continue;
    }
    if (pass.invalidation_contract.empty()) {
      return false;
    }
    if (pass.pass_id == "direct-dispatch-exact-call" &&
        !pass.invalidates_global_proof_state) {
      return false;
    }
  }
  return !passes.empty();
}

bool AllPassesFailClosedWithDiagnostics(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes) {
  for (const auto &pass : passes) {
    if (!pass.fail_closed_when_preconditions_missing ||
        pass.failure_diagnostic.empty()) {
      return false;
    }
  }
  return !passes.empty();
}

}  // namespace

const char *Objc3SemanticOptimizationPassModeName(
    Objc3SemanticOptimizationPassMode mode) {
  switch (mode) {
    case Objc3SemanticOptimizationPassMode::kEnabled:
      return "enabled";
    case Objc3SemanticOptimizationPassMode::kReserved:
      return "reserved";
    case Objc3SemanticOptimizationPassMode::kVerifierOnly:
      return "verifier-only";
  }
  return "reserved";
}

std::vector<Objc3SemanticOptimizationPassContract>
BuildObjc3SemanticOptimizationPassRegistry() {
  return {
      {
          "semantic-precondition-gate",
          10,
          "typed-sema-to-lowering",
          Objc3SemanticOptimizationPassMode::kVerifierOnly,
          "Objc3TypedSemaToLoweringContractSurface",
          "Objc3SemanticOptimizationPipelineSurface.preconditions",
          {"typed sema handoff is deterministic",
           "lowering boundary replay key is present",
           "parse-to-lowering readiness is ready"},
          "no IR mutation; gate publishes immutable precondition state",
          true,
          false,
          false,
          true,
          true,
          false,
          "semantic optimization precondition gate failed",
      },
      {
          "nil-receiver-folding",
          20,
          "ir-message-send-lowering",
          Objc3SemanticOptimizationPassMode::kEnabled,
          "Objc3IRReceiverDispatchFacts",
          "Objc3IRMessageSendLoweringPlan",
          {"receiver fact is compile_time_nil_receiver",
           "selector and argument lowering are side-effect free for optional nil send",
           "runtime dispatch entrypoint remains canonical for non-nil receivers"},
          "no runtime or direct dispatch proof is created for folded nil sends",
          true,
          true,
          false,
          true,
          true,
          false,
          "nil receiver folding requires compile-time nil receiver proof",
      },
      {
          "direct-dispatch-exact-call",
          30,
          "ir-message-send-emission",
          Objc3SemanticOptimizationPassMode::kEnabled,
          "Objc3IRDirectDispatchCallRequest",
          "BuildObjc3IRDirectDispatchCall",
          {"callee symbol and signature are both present",
           "explicit argument types cover every explicit argument",
           "direct dispatch result owns explicit IR storage"},
          "invalidate global proof state after exact direct call emission",
          true,
          true,
          true,
          true,
          true,
          false,
          "direct dispatch lowering requires exact symbol, signature, and result ownership",
      },
      {
          "arc-retained-result-cleanup",
          40,
          "ir-arc-cleanup-lowering",
          Objc3SemanticOptimizationPassMode::kEnabled,
          "method-family ARC ownership facts",
          "registered retained-result cleanup storage",
          {"ARC mode is enabled",
           "method family returns a retained object result",
           "related-result receiver cleanup is disarmed before transfer"},
          "local ARC cleanup state is rewritten; global proof state is unchanged",
          true,
          true,
          false,
          true,
          true,
          false,
          "ARC retained-result cleanup requires method-family ownership proof",
      },
      {
          "runtime-dispatch-preservation",
          50,
          "ir-runtime-dispatch-emission",
          Objc3SemanticOptimizationPassMode::kVerifierOnly,
          "Objc3IRRuntimeDispatchCallRequest",
          "canonical runtime dispatch call",
          {"runtime dispatch result owner is explicit",
           "dispatch symbol is canonical or typed canonical from-class variant",
           "retired routes and compatibility routes remain disabled"},
          "runtime cache state remains runtime-owned behind dispatch entrypoints",
          true,
          false,
          false,
          true,
          true,
          false,
          "runtime dispatch preservation rejected non-canonical dispatch emission",
      },
      {
          "devirtualization",
          60,
          "semantic-optimization-reserved",
          Objc3SemanticOptimizationPassMode::kReserved,
          "closed-world receiver finality proof",
          "exact direct dispatch call",
          {"class finality proof exists",
           "override set is closed",
           "ABI stability allows direct target publication"},
          "would invalidate receiver, selector, and callee proof state",
          true,
          false,
          false,
          true,
          true,
          false,
          "devirtualization is reserved until closed-world finality proofs exist",
      },
      {
          "method-inlining",
          70,
          "semantic-optimization-reserved",
          Objc3SemanticOptimizationPassMode::kReserved,
          "side-effect and ownership-preserving inline candidate",
          "expanded IR body",
          {"callee body is available",
           "ownership effects are replayable",
           "debug and diagnostic source mapping are preserved"},
          "would invalidate local value, ownership, and diagnostic proof state",
          true,
          false,
          false,
          true,
          true,
          false,
          "method inlining is reserved until ownership and source-map proofs exist",
      },
      {
          "cache-aware-dispatch",
          80,
          "runtime-owned-dispatch-optimization-reserved",
          Objc3SemanticOptimizationPassMode::kReserved,
          "runtime cache invalidation contract",
          "runtime-owned cache-aware dispatch helper",
          {"runtime cache invalidation semantics are public",
           "ABI-stable helper symbol exists",
           "semantic replay preserves dispatch miss behavior"},
          "cache invalidation remains runtime-owned; no IR helper is materialized",
          true,
          false,
          false,
          true,
          true,
          false,
          "cache-aware dispatch is reserved behind the runtime cache contract",
      },
      {
          "ir-cleanup-verifier",
          90,
          "post-ir-optimization-verification",
          Objc3SemanticOptimizationPassMode::kVerifierOnly,
          "post-pass IR trace",
          "semantic equivalence verdict",
          {"pass order trace is deterministic",
           "all mutating passes declared invalidation",
           "all unsupported opportunities emitted skip diagnostics"},
          "no mutation; verifier consumes post-pass trace",
          true,
          false,
          false,
          true,
          true,
          false,
          "post optimization IR cleanup verifier failed semantic equivalence",
      },
  };
}

bool IsObjc3SemanticOptimizationPassRegistryDeterministic(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes,
    std::string &reason) {
  if (passes.empty()) {
    reason = "semantic optimization pass registry is empty";
    return false;
  }

  std::unordered_set<std::string> pass_ids;
  std::size_t previous_ordinal = 0;
  for (const auto &pass : passes) {
    if (pass.pass_id.empty()) {
      reason = "semantic optimization pass registry contains an empty pass id";
      return false;
    }
    if (!pass_ids.insert(pass.pass_id).second) {
      reason = "semantic optimization pass registry contains duplicate pass id " +
               pass.pass_id;
      return false;
    }
    if (pass.ordinal <= previous_ordinal) {
      reason =
          "semantic optimization pass registry ordinals are not strictly ordered";
      return false;
    }
    previous_ordinal = pass.ordinal;
  }
  reason.clear();
  return true;
}

bool IsObjc3SemanticOptimizationPassContractFailClosed(
    const Objc3SemanticOptimizationPassContract &pass,
    std::string &reason) {
  if (!pass.semantic_preserving) {
    reason = "optimization pass is not marked semantic-preserving: " +
             pass.pass_id;
    return false;
  }
  if (!pass.fail_closed_when_preconditions_missing) {
    reason = "optimization pass does not fail closed: " + pass.pass_id;
    return false;
  }
  if (pass.failure_diagnostic.empty()) {
    reason = "optimization pass is missing fail-closed diagnostic: " +
             pass.pass_id;
    return false;
  }
  if (pass.mode == Objc3SemanticOptimizationPassMode::kReserved &&
      pass.emits_success_claim_on_skip) {
    reason = "reserved optimization pass emits success claim on skip: " +
             pass.pass_id;
    return false;
  }
  reason.clear();
  return true;
}

std::string BuildObjc3SemanticOptimizationPipelineKey(
    const Objc3SemanticOptimizationPipelineSurface &surface) {
  std::ostringstream key;
  key << "semantic-optimization-pipeline:v1:"
      << "pass-count=" << surface.pass_count
      << ";enabled-pass-count=" << surface.enabled_pass_count
      << ";reserved-pass-count=" << surface.reserved_pass_count
      << ";verifier-only-pass-count=" << surface.verifier_only_pass_count
      << ";lowering-pass-graph-ready="
      << (surface.lowering_pass_graph_ready ? "true" : "false")
      << ";order-deterministic="
      << (surface.pass_registry_order_deterministic ? "true" : "false")
      << ";typed-contracts-ready="
      << (surface.typed_pass_contracts_ready ? "true" : "false")
      << ";direct-dispatch-bound="
      << (surface.direct_dispatch_pass_bound_to_ir ? "true" : "false")
      << ";arc-cleanup-bound="
      << (surface.arc_cleanup_pass_bound_to_ir ? "true" : "false")
      << ";reserved-fail-closed="
      << (surface.reserved_passes_fail_closed ? "true" : "false")
      << ";explicit-invalidation="
      << (surface.explicit_invalidation_ready ? "true" : "false")
      << ";verify-after-each-pass="
      << (surface.verification_after_each_pass_ready ? "true" : "false")
      << ";fail-closed-diagnostics="
      << (surface.fail_closed_diagnostics_ready ? "true" : "false")
      << ";benchmark-governance="
      << (surface.benchmark_governance_bound ? "true" : "false")
      << ";ready="
      << (surface.semantic_optimization_pipeline_ready ? "true" : "false")
      << ";passes=";
  for (std::size_t i = 0; i < surface.pass_order.size(); ++i) {
    if (i != 0) {
      key << ",";
    }
    key << surface.pass_order[i];
  }
  return key.str();
}

Objc3SemanticOptimizationPipelineSurface
BuildObjc3SemanticOptimizationPipelineSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3SemanticOptimizationPipelineSurface surface;
  const auto passes = BuildObjc3SemanticOptimizationPassRegistry();
  surface.pass_count = passes.size();
  for (const auto &pass : passes) {
    surface.pass_order.push_back(pass.pass_id);
    switch (pass.mode) {
      case Objc3SemanticOptimizationPassMode::kEnabled:
        ++surface.enabled_pass_count;
        break;
      case Objc3SemanticOptimizationPassMode::kReserved:
        ++surface.reserved_pass_count;
        break;
      case Objc3SemanticOptimizationPassMode::kVerifierOnly:
        ++surface.verifier_only_pass_count;
        break;
    }
  }

  surface.lowering_pipeline_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .core_feature_key;
  surface.lowering_pass_graph_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold.pass_graph_ready &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .core_feature_ready;

  std::string order_failure;
  surface.pass_registry_order_deterministic =
      IsObjc3SemanticOptimizationPassRegistryDeterministic(passes,
                                                           order_failure);
  surface.typed_pass_contracts_ready = AllTypedContractsReady(passes);
  surface.semantic_precondition_gate_ready =
      HasPassId(passes, "semantic-precondition-gate");
  surface.direct_dispatch_pass_bound_to_ir =
      HasPassId(passes, "direct-dispatch-exact-call") &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .direct_ir_entrypoint_enabled;
  surface.arc_cleanup_pass_bound_to_ir =
      HasPassId(passes, "arc-retained-result-cleanup") &&
      (options.arc_mode == Objc3FrontendArcMode::kEnabled ||
       options.arc_mode == Objc3FrontendArcMode::kDisabled);
  surface.runtime_dispatch_preservation_bound =
      HasPassId(passes, "runtime-dispatch-preservation");
  surface.reserved_passes_fail_closed = AllReservedPassesFailClosed(passes);
  surface.explicit_invalidation_ready =
      AllMutatingPassesDeclareInvalidation(passes);
  surface.verification_after_each_pass_ready =
      AllPassesVerifyAfterExecution(passes);
  surface.fail_closed_diagnostics_ready =
      AllPassesFailClosedWithDiagnostics(passes);
  surface.benchmark_governance_bound =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  surface.semantic_optimization_pipeline_ready =
      surface.lowering_pass_graph_ready &&
      surface.pass_registry_order_deterministic &&
      surface.typed_pass_contracts_ready &&
      surface.semantic_precondition_gate_ready &&
      surface.direct_dispatch_pass_bound_to_ir &&
      surface.arc_cleanup_pass_bound_to_ir &&
      surface.runtime_dispatch_preservation_bound &&
      surface.reserved_passes_fail_closed &&
      surface.explicit_invalidation_ready &&
      surface.verification_after_each_pass_ready &&
      surface.fail_closed_diagnostics_ready &&
      surface.benchmark_governance_bound;
  surface.semantic_pipeline_key =
      BuildObjc3SemanticOptimizationPipelineKey(surface);

  if (!surface.semantic_optimization_pipeline_ready) {
    if (!order_failure.empty()) {
      surface.failure_reason = order_failure;
    } else if (!surface.lowering_pass_graph_ready) {
      surface.failure_reason = "lowering pass graph is not ready";
    } else if (!surface.typed_pass_contracts_ready) {
      surface.failure_reason =
          "semantic optimization pass contracts are incomplete";
    } else if (!surface.direct_dispatch_pass_bound_to_ir) {
      surface.failure_reason =
          "direct-dispatch optimization pass is not bound to IR emission";
    } else if (!surface.benchmark_governance_bound) {
      surface.failure_reason =
          "semantic optimization pipeline is missing benchmark governance";
    } else {
      surface.failure_reason =
          "semantic optimization pipeline is not ready";
    }
  }

  return surface;
}

bool IsObjc3SemanticOptimizationPipelineSurfaceReady(
    const Objc3SemanticOptimizationPipelineSurface &surface,
    std::string &reason) {
  if (surface.semantic_optimization_pipeline_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "semantic optimization pipeline is not ready"
               : surface.failure_reason;
  return false;
}
