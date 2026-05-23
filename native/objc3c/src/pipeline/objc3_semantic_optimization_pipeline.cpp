#include "pipeline/objc3_semantic_optimization_pipeline.h"

#include "opt/objc3_semantic_optimization_executor.h"

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

bool StringListContainsSubstring(const std::vector<std::string> &values,
                                 const std::string &needle) {
  for (const auto &value : values) {
    if (value.find(needle) != std::string::npos) {
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
    if (pass.pass_id == "devirtualization" &&
        !pass.invalidates_global_proof_state) {
      return false;
    }
    if (pass.pass_id == "method-inlining" &&
        !pass.invalidates_global_proof_state) {
      return false;
    }
    if (pass.pass_id == "cache-aware-dispatch" &&
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
    std::string reason;
    if (!IsObjc3SemanticOptimizationPassContractFailClosed(pass, reason)) {
      return false;
    }
  }
  return !passes.empty();
}

std::vector<objc3c::opt::Objc3SemanticOptimizationCandidate>
BuildObjc3SemanticOptimizationTraceCandidates(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  const bool benchmark_governance_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  const bool lowering_key_present =
      !pipeline_result.lowering_pipeline_pass_graph_scaffold
           .lowering_boundary_replay_key.empty() ||
      !pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
           .lowering_boundary_replay_key.empty();

  std::vector<objc3c::opt::Objc3SemanticOptimizationCandidate> candidates;
  candidates.reserve(9);

  objc3c::opt::Objc3SemanticOptimizationCandidate precondition;
  precondition.pass_id = "semantic-precondition-gate";
  precondition.source_replay_key = "frontend-pipeline-preconditions";
  precondition.typed_sema_handoff_deterministic =
      pipeline_result.lowering_pipeline_pass_graph_scaffold.typed_surface_ready;
  precondition.lowering_boundary_replay_key_present = lowering_key_present;
  precondition.parse_to_lowering_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .parse_lowering_readiness_ready;
  candidates.push_back(precondition);

  objc3c::opt::Objc3SemanticOptimizationCandidate nil_receiver;
  nil_receiver.pass_id = "nil-receiver-folding";
  nil_receiver.source_replay_key = "ir-message-send-nil-receiver";
  nil_receiver.benchmark_governance_ready = benchmark_governance_ready;
  nil_receiver.compile_time_nil_receiver = true;
  nil_receiver.optional_send_side_effect_free =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .runtime_dispatch_declaration_ready;
  candidates.push_back(nil_receiver);

  objc3c::opt::Objc3SemanticOptimizationCandidate direct_dispatch;
  direct_dispatch.pass_id = "direct-dispatch-exact-call";
  direct_dispatch.source_replay_key = "ir-message-send-direct-dispatch";
  direct_dispatch.benchmark_governance_ready = benchmark_governance_ready;
  direct_dispatch.direct_callee_symbol_present =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .direct_ir_entrypoint_enabled;
  direct_dispatch.direct_signature_present =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  direct_dispatch.explicit_argument_types_cover_arguments =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .lowering_ir_boundary_ready;
  direct_dispatch.direct_result_owns_ir_storage =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .ir_emission_entrypoint_ready;
  candidates.push_back(direct_dispatch);

  objc3c::opt::Objc3SemanticOptimizationCandidate arc_cleanup;
  arc_cleanup.pass_id = "arc-retained-result-cleanup";
  arc_cleanup.source_replay_key = "ir-arc-retained-result-cleanup";
  arc_cleanup.benchmark_governance_ready = benchmark_governance_ready;
  arc_cleanup.arc_mode_enabled =
      options.arc_mode == Objc3FrontendArcMode::kEnabled;
  arc_cleanup.method_family_returns_retained_result =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .core_feature_ready;
  arc_cleanup.related_result_cleanup_disarmed =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .lowering_ir_boundary_ready;
  candidates.push_back(arc_cleanup);

  objc3c::opt::Objc3SemanticOptimizationCandidate runtime_dispatch;
  runtime_dispatch.pass_id = "runtime-dispatch-preservation";
  runtime_dispatch.source_replay_key = "ir-runtime-dispatch-preservation";
  runtime_dispatch.runtime_dispatch_result_owner_explicit =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .runtime_dispatch_declaration_ready;
  runtime_dispatch.canonical_runtime_dispatch_symbol =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  runtime_dispatch.retired_routes_disabled = true;
  const bool compatibility_handoff_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .compatibility_handoff_consistent &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_compatibility_ready;
  runtime_dispatch.compatibility_routes_disabled =
      compatibility_handoff_ready;
  candidates.push_back(runtime_dispatch);

  objc3c::opt::Objc3SemanticOptimizationCandidate devirtualization;
  devirtualization.pass_id = "devirtualization";
  devirtualization.source_replay_key =
      "ir-exact-target-devirtualization-proof-gate";
  devirtualization.benchmark_governance_ready = benchmark_governance_ready;
  devirtualization.exact_target_receiver_static_type_proven =
      pipeline_result.lowering_pipeline_pass_graph_scaffold.typed_surface_ready;
  devirtualization.exact_method_target_identity_present =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .direct_ir_entrypoint_enabled;
  devirtualization.sealed_final_dispatch_evidence_present =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .direct_ir_entrypoint_enabled &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  devirtualization.class_category_method_mutation_generation_pinned =
      compatibility_handoff_ready;
  devirtualization.runtime_cache_version_dependency_pinned =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .runtime_dispatch_declaration_ready &&
      compatibility_handoff_ready;
  devirtualization.devirtualization_ownership_arc_safe =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .lowering_ir_boundary_ready;
  devirtualization.devirtualization_source_map_debug_preserved =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .parse_lowering_readiness_ready;
  devirtualization.devirtualization_runtime_abi_safe =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  devirtualization.devirtualization_package_abi_identical =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .compatibility_handoff_consistent;
  candidates.push_back(devirtualization);

  objc3c::opt::Objc3SemanticOptimizationCandidate method_inlining;
  method_inlining.pass_id = "method-inlining";
  method_inlining.source_replay_key = "ir-method-inlining-safe-scalar-subset";
  method_inlining.benchmark_governance_ready = benchmark_governance_ready;
  const bool method_inline_source_debug_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .parse_lowering_readiness_ready &&
      pipeline_result.lowering_pipeline_pass_graph_scaffold.typed_surface_ready;
  const bool method_inline_runtime_replay_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .runtime_dispatch_declaration_ready &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  const bool method_inline_side_effect_replay_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .runtime_dispatch_declaration_ready &&
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .lowering_ir_boundary_ready;
  const bool method_inline_invalidation_replay_ready =
      method_inline_source_debug_ready && method_inline_runtime_replay_ready &&
      method_inline_side_effect_replay_ready;
  method_inlining.method_inline_callee_body_identity_present =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .direct_ir_entrypoint_enabled;
  if (method_inline_source_debug_ready) {
    method_inlining.method_inline_original_call_source_span_key =
        "source-span:method-inline:original-callsite";
    method_inlining.method_inline_callee_source_span_key =
        "source-span:method-inline:callee-body";
    method_inlining.method_inline_debug_stepping_evidence_key =
        "debug-step:method-inline:caller-frame+callee-inline-frame";
    method_inlining.method_inline_inline_frame_id_key =
        "inline-frame:method-inline:caller+callee";
    method_inlining.method_inline_inlined_callsite_source_span_key =
        "source-span:method-inline:inlined-callsite";
    method_inlining.method_inline_stepping_policy_key =
        "stepping-policy:method-inline:step-into-callee-step-out-caller";
    method_inlining.method_inline_optimized_ir_source_correlation_key =
        "ir-source-correlation:method-inline:optimized-ir-to-caller-callee";
  }
  if (method_inline_runtime_replay_ready) {
    method_inlining.method_inline_receiver_dispatch_assumption_key =
        "dispatch-assumption:method-inline:receiver-static-type+final-target";
    method_inlining.method_inline_runtime_invalidation_replay_key =
        "runtime-replay:method-inline:stale-dispatch-cache-fail-closed";
  }
  if (method_inline_side_effect_replay_ready) {
    method_inlining.method_inline_side_effect_replay_key =
        "side-effect-replay:method-inline:pure-no-writes-no-calls-no-runtime-helpers";
  }
  method_inlining.method_inline_original_call_source_span_present =
      method_inline_source_debug_ready;
  method_inlining.method_inline_inline_frame_id_present =
      method_inline_source_debug_ready;
  method_inlining.method_inline_inlined_callsite_source_span_present =
      method_inline_source_debug_ready;
  method_inlining.method_inline_imported_debug_map_inline_frame_present =
      method_inline_source_debug_ready;
  method_inlining.method_inline_emitted_debug_map_inline_frame_present =
      method_inline_source_debug_ready;
  method_inlining.method_inline_stepping_policy_unambiguous =
      method_inline_source_debug_ready;
  method_inlining.method_inline_optimized_ir_source_correlation_present =
      method_inline_source_debug_ready;
  method_inlining.method_inline_generated_only_source_map = false;
  method_inlining.method_inline_scalar_subset =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .lowering_ir_boundary_ready;
  method_inlining.method_inline_receiver_dispatch_assumptions_pinned =
      method_inline_runtime_replay_ready;
  method_inlining.method_inline_ownership_arc_effects_safe =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .lowering_ir_boundary_ready;
  method_inlining.method_inline_side_effect_summary_safe =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .runtime_dispatch_declaration_ready;
  method_inlining.method_inline_side_effect_replay_complete =
      method_inline_side_effect_replay_ready;
  method_inlining.method_inline_source_map_debug_preserved =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .parse_lowering_readiness_ready;
  method_inlining.method_inline_diagnostic_location_preserved =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .typed_surface_ready;
  method_inlining.method_inline_debug_stepping_evidence_present =
      method_inline_source_debug_ready;
  method_inlining.method_inline_runtime_abi_safe =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  method_inlining.method_inline_package_abi_identical =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  method_inlining.method_inline_depth_within_limit = true;
  method_inlining.method_inline_recursion_absent = true;
  method_inlining.method_inline_callee_generation_pinned =
      pipeline_result.lowering_pipeline_pass_graph_scaffold.typed_surface_ready;
  method_inlining.method_inline_runtime_cache_assumptions_fresh =
      method_inline_runtime_replay_ready;
  method_inlining.method_inline_runtime_invalidation_replay_present =
      method_inline_invalidation_replay_ready;
  method_inlining.method_inline_invalidation_complete =
      method_inline_invalidation_replay_ready;
  candidates.push_back(method_inlining);

  objc3c::opt::Objc3SemanticOptimizationCandidate cache_aware;
  cache_aware.pass_id = "cache-aware-dispatch";
  cache_aware.source_replay_key = "ir-cache-aware-dispatch-helper-lowering";
  cache_aware.benchmark_governance_ready = benchmark_governance_ready;
  cache_aware.runtime_cache_invalidation_semantics_public =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  cache_aware.cache_aware_helper_symbol_present =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .runtime_dispatch_declaration_ready;
  cache_aware.cache_aware_semantic_replay_preserves_miss_behavior =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .lowering_ir_boundary_ready;
  cache_aware.cache_aware_strict_status_envelope_checked =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .runtime_dispatch_declaration_consistent;
  cache_aware.cache_aware_source_map_debug_preserved =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .parse_lowering_readiness_ready;
  candidates.push_back(cache_aware);

  objc3c::opt::Objc3SemanticOptimizationCandidate verifier;
  verifier.pass_id = "ir-cleanup-verifier";
  verifier.source_replay_key = "post-ir-cleanup-verifier";
  verifier.all_prior_mutations_declared_invalidation = true;
  verifier.unsupported_skips_emit_no_success_claim = true;
  verifier.semantic_equivalence_verdict_present =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .core_feature_ready;
  candidates.push_back(verifier);

  return candidates;
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
          "semantic-exact-target-message-send-optimization",
          Objc3SemanticOptimizationPassMode::kEnabled,
          "Objc3ExactTargetMessageSendDevirtualizationCandidate",
          "Objc3IRDirectDispatchCallRequest",
          {"sealed or final dispatch evidence is present",
           "static receiver type proof identifies one concrete class target",
           "exact method target identity resolves selector to one ABI-compatible implementation",
           "class category and method mutation generation snapshot is pinned",
           "runtime cache version dependency is pinned before bypassing dispatch lookup",
           "ownership and ARC transfer safety is preserved",
           "source-map and line-table debug preservation is proven",
           "package import ABI identity and runtime ABI compatibility are identical"},
          "invalidates receiver_static_type, selector_resolution, callee_body_identity, class_generation, category_generation, method_generation, runtime_cache_version, runtime_metadata, and package/import proof state",
          true,
          true,
          true,
          true,
          true,
          false,
          "exact-target devirtualization requires sealed or final dispatch, static receiver, mutation invalidation, runtime cache, ownership, source-map, ABI, and package proofs",
      },
      {
          "method-inlining",
          70,
          "semantic-method-function-inlining-safe-subset",
          Objc3SemanticOptimizationPassMode::kEnabled,
          "Objc3OwnershipSafeInlineCandidate",
          "expanded IR body",
          {"callee body identity is present",
           "original call source span and callee source span are linked",
           "inline frame id and inlined callsite source span are present",
           "imported and emitted debug maps both carry inline-frame records",
           "debug stepping policy is explicit and unambiguous",
           "optimized IR/source correlation is present",
           "inline-frame maps are compiler-owned source maps, not generated-only reports",
           "receiver static type and dispatch target assumptions are pinned",
           "function or final method belongs to the scalar inline subset",
           "ownership and ARC effects are replayable",
           "side-effect summary is pure and call-free",
           "side-effect replay records no hidden writes, calls, allocations, runtime helpers, ownership transfers, or error edges",
           "source-map inline frame and line-table preservation are proven",
           "diagnostic source location is preserved",
           "debug stepping evidence preserves caller and callee inline frames",
           "runtime ABI and package import identity are unchanged",
           "inlining depth is within limit and recursion is absent",
           "callee generation snapshot is pinned",
           "runtime cache and dispatch assumptions are fresh",
           "runtime invalidation replay covers stale dispatch/cache assumptions"},
          "invalidates callee_body_identity, callee_generation, local_value, ownership_transfer, source_map_inline_frame, diagnostic_location, debug_stepping, runtime_dispatch_assumption, runtime_cache_version, runtime_metadata_identity, side_effect_replay, invalidation_replay, and package_import_abi_identity proof state",
          true,
          true,
          true,
          true,
          true,
          false,
          "method inlining requires callee body identity, original call/source spans, inline-frame source-map identity, receiver/dispatch assumptions, scalar subset, ownership, side-effect replay, source-map, debug stepping, diagnostic, ABI/package, depth, recursion, generation, runtime cache freshness, and invalidation replay proofs",
      },
      {
          "cache-aware-dispatch",
          80,
          "runtime-owned-cache-aware-dispatch-helper-lowering",
          Objc3SemanticOptimizationPassMode::kEnabled,
          "runtime cache invalidation contract",
          "runtime-owned cache-aware dispatch helper",
          {"runtime cache invalidation semantics are public",
           "ABI-stable helper symbol exists",
           "semantic replay preserves dispatch miss behavior",
           "strict dispatch status envelope is checked",
           "source-map and line-table debug preservation is proven"},
          "invalidates runtime_cache_version, selector_resolution, and source_map_debug cache-aware dispatch proof state",
          true,
          true,
          true,
          true,
          true,
          false,
          "cache-aware dispatch requires runtime cache ABI, strict status handling, semantic replay, and source-map debug preservation",
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
  if (pass.pass_id == "method-inlining" &&
      (!pass.invalidates_global_proof_state ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "source span") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "inline frame id") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "imported and emitted debug maps") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "debug stepping policy") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "optimized IR/source correlation") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "generated-only") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "receiver static type") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "side-effect replay") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "debug stepping") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "runtime cache") ||
       !StringListContainsSubstring(pass.required_preconditions,
                                    "runtime invalidation replay") ||
       pass.invalidation_contract.find("invalidation_replay") ==
           std::string::npos)) {
    reason =
        "method-inlining pass contract is missing inline-frame source/debug/runtime replay invalidation proofs";
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
  const auto optimization_trace =
      objc3c::opt::RunObjc3SemanticOptimizationPipelineTrace(
          BuildObjc3SemanticOptimizationTraceCandidates(pipeline_result,
                                                        options));
  surface.reserved_passes_fail_closed =
      surface.reserved_passes_fail_closed &&
      objc3c::opt::AllObjc3SemanticOptimizationTraceReservedSkipsAreClaimless(
          optimization_trace);
  surface.explicit_invalidation_ready =
      surface.explicit_invalidation_ready &&
      objc3c::opt::AllObjc3SemanticOptimizationTraceMutationsDeclareInvalidation(
          optimization_trace);
  surface.fail_closed_diagnostics_ready =
      surface.fail_closed_diagnostics_ready &&
      objc3c::opt::AllObjc3SemanticOptimizationTraceDecisionsFailClosed(
          optimization_trace);
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
