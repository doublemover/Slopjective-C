"""Deterministic metadata model for semantic optimization pipeline fixtures."""

from __future__ import annotations

from typing import Any


REQUIRED_PASS_ORDER = [
    "semantic-precondition-gate",
    "nil-receiver-folding",
    "direct-dispatch-exact-call",
    "arc-retained-result-cleanup",
    "runtime-dispatch-preservation",
    "devirtualization",
    "method-inlining",
    "cache-aware-dispatch",
    "ir-cleanup-verifier",
]

PASS_PLANS: dict[str, dict[str, Any]] = {
    "semantic-precondition-gate": {
        "ordinal": 10,
        "mode": "verifier-only",
        "rewrites_ir": False,
        "invalidates_global_proof_state": False,
        "missing_proof_action": "REJECT_FAIL_CLOSED",
        "diagnostic": "semantic optimization precondition gate failed",
    },
    "nil-receiver-folding": {
        "ordinal": 20,
        "mode": "enabled",
        "rewrites_ir": True,
        "invalidates_global_proof_state": False,
        "missing_proof_action": "SKIP_FAIL_CLOSED",
        "diagnostic": "nil receiver folding requires compile-time nil receiver proof",
    },
    "direct-dispatch-exact-call": {
        "ordinal": 30,
        "mode": "enabled",
        "rewrites_ir": True,
        "invalidates_global_proof_state": True,
        "missing_proof_action": "REJECT_FAIL_CLOSED",
        "diagnostic": (
            "direct dispatch lowering requires exact symbol, signature, "
            "and result ownership"
        ),
    },
    "arc-retained-result-cleanup": {
        "ordinal": 40,
        "mode": "enabled",
        "rewrites_ir": True,
        "invalidates_global_proof_state": False,
        "missing_proof_action": "SKIP_FAIL_CLOSED",
        "diagnostic": "ARC retained-result cleanup requires method-family ownership proof",
    },
    "runtime-dispatch-preservation": {
        "ordinal": 50,
        "mode": "verifier-only",
        "rewrites_ir": False,
        "invalidates_global_proof_state": False,
        "missing_proof_action": "REJECT_FAIL_CLOSED",
        "diagnostic": "runtime dispatch preservation rejected non-canonical dispatch emission",
    },
    "devirtualization": {
        "ordinal": 60,
        "mode": "enabled",
        "rewrites_ir": True,
        "invalidates_global_proof_state": True,
        "missing_proof_action": "REJECT_FAIL_CLOSED",
        "diagnostic": (
            "exact-target devirtualization requires sealed or final dispatch, "
            "static receiver, mutation invalidation, runtime cache, ownership, "
            "source-map, ABI, and package proofs"
        ),
    },
    "method-inlining": {
        "ordinal": 70,
        "mode": "enabled",
        "rewrites_ir": True,
        "invalidates_global_proof_state": True,
        "missing_proof_action": "REJECT_FAIL_CLOSED",
        "diagnostic": (
            "method inlining requires callee body identity, original call/source "
            "spans, inline-frame source-map identity, receiver/dispatch "
            "assumptions, scalar subset, ownership, side-effect replay, source-map, "
            "debug stepping, diagnostic, ABI/package, depth, recursion, generation, "
            "runtime cache freshness, and invalidation replay proofs"
        ),
    },
    "cache-aware-dispatch": {
        "ordinal": 80,
        "mode": "enabled",
        "rewrites_ir": True,
        "invalidates_global_proof_state": True,
        "missing_proof_action": "REJECT_FAIL_CLOSED",
        "diagnostic": (
            "cache-aware dispatch requires runtime cache ABI, strict status "
            "handling, semantic replay, and source-map debug preservation"
        ),
    },
    "ir-cleanup-verifier": {
        "ordinal": 90,
        "mode": "verifier-only",
        "rewrites_ir": False,
        "invalidates_global_proof_state": False,
        "missing_proof_action": "REJECT_FAIL_CLOSED",
        "diagnostic": "post optimization IR cleanup verifier failed semantic equivalence",
    },
}

METHOD_INLINING_TEXT_GATES = (
    "method_inline_original_call_source_span_key",
    "method_inline_callee_source_span_key",
    "method_inline_inline_frame_id_key",
    "method_inline_inlined_callsite_source_span_key",
    "method_inline_stepping_policy_key",
    "method_inline_optimized_ir_source_correlation_key",
    "method_inline_receiver_dispatch_assumption_key",
    "method_inline_debug_stepping_evidence_key",
    "method_inline_side_effect_replay_key",
    "method_inline_runtime_invalidation_replay_key",
)

METHOD_INLINING_TRUE_GATES = (
    "method_inline_callee_body_identity_present",
    "method_inline_original_call_source_span_present",
    "method_inline_inline_frame_id_present",
    "method_inline_inlined_callsite_source_span_present",
    "method_inline_imported_debug_map_inline_frame_present",
    "method_inline_emitted_debug_map_inline_frame_present",
    "method_inline_stepping_policy_unambiguous",
    "method_inline_optimized_ir_source_correlation_present",
    "method_inline_scalar_subset",
    "method_inline_receiver_dispatch_assumptions_pinned",
    "method_inline_ownership_arc_effects_safe",
    "method_inline_side_effect_summary_safe",
    "method_inline_side_effect_replay_complete",
    "method_inline_source_map_debug_preserved",
    "method_inline_diagnostic_location_preserved",
    "method_inline_debug_stepping_evidence_present",
    "method_inline_runtime_abi_safe",
    "method_inline_package_abi_identical",
    "method_inline_depth_within_limit",
    "method_inline_recursion_absent",
    "method_inline_callee_generation_pinned",
    "method_inline_runtime_cache_assumptions_fresh",
    "method_inline_runtime_invalidation_replay_present",
    "method_inline_invalidation_complete",
)

METHOD_INLINING_FALSE_GATES = (
    "method_inline_generated_only_source_map",
)


def metadata_key(plan: dict[str, Any], decision: str, candidate: dict[str, Any]) -> str:
    success_claim = "true" if decision == "APPLIED" else "false"
    key = (
        "objc3-semantic-optimization:v1"
        f";pass={candidate['pass_id']}"
        f";ordinal={plan['ordinal']}"
        f";decision={decision}"
        f";rewrites-ir={str(plan['rewrites_ir']).lower()}"
        f";invalidates-global-proof-state={str(plan['invalidates_global_proof_state']).lower()}"
        f";success-claim={success_claim}"
        f";source={candidate.get('source_replay_key', '')}"
    )
    if candidate.get("pass_id") == "method-inlining":
        for gate in METHOD_INLINING_TEXT_GATES:
            key += f";{gate}={candidate.get(gate, '')}"
        for gate in METHOD_INLINING_TRUE_GATES + METHOD_INLINING_FALSE_GATES:
            key += f";{gate}={str(bool(candidate.get(gate))).lower()}"
    if candidate.get("pass_id") == "cache-aware-dispatch":
        cache_gates = (
            "runtime_cache_invalidation_semantics_public",
            "cache_aware_helper_symbol_present",
            "cache_aware_semantic_replay_preserves_miss_behavior",
            "cache_aware_strict_status_envelope_checked",
            "cache_aware_source_map_debug_preserved",
        )
        for gate in cache_gates:
            key += f";{gate}={str(bool(candidate.get(gate))).lower()}"
    return key


def _missing_proof_decision(plan: dict[str, Any]) -> str:
    if plan["missing_proof_action"] == "SKIP_FAIL_CLOSED":
        return "SKIPPED_FAIL_CLOSED"
    return "REJECTED_FAIL_CLOSED"


def _result(
    candidate: dict[str, Any],
    plan: dict[str, Any],
    decision: str,
    diagnostic: str = "",
) -> dict[str, Any]:
    success_claim = decision == "APPLIED"
    if decision == "SKIPPED_FAIL_CLOSED":
        success_claim = False
    return {
        "pass_id": candidate["pass_id"],
        "decision": decision,
        "success_claim": success_claim,
        "semantic_preserving": True,
        "rewrites_ir": plan["rewrites_ir"],
        "invalidates_global_proof_state": plan["invalidates_global_proof_state"],
        "diagnostic": diagnostic,
        "metadata_key": metadata_key(plan, decision, candidate),
    }


def _missing(candidate: dict[str, Any], plan: dict[str, Any]) -> dict[str, Any]:
    if candidate.get("pass_id") == "method-inlining":
        missing_gates = []
        if not candidate.get("benchmark_governance_ready"):
            missing_gates.append("benchmark_governance_ready")
        missing_gates.extend(
            gate for gate in METHOD_INLINING_TEXT_GATES if not candidate.get(gate)
        )
        missing_gates.extend(
            gate for gate in METHOD_INLINING_TRUE_GATES if not candidate.get(gate)
        )
        missing_gates.extend(
            gate for gate in METHOD_INLINING_FALSE_GATES if candidate.get(gate)
        )
        if missing_gates:
            diagnostic = plan["diagnostic"] + "; failed gates: " + ", ".join(missing_gates)
            return _result(candidate, plan, _missing_proof_decision(plan), diagnostic)
    if candidate.get("pass_id") == "cache-aware-dispatch":
        missing_gates = [
            gate
            for gate in (
                "runtime_cache_invalidation_semantics_public",
                "cache_aware_helper_symbol_present",
                "cache_aware_semantic_replay_preserves_miss_behavior",
                "cache_aware_strict_status_envelope_checked",
                "cache_aware_source_map_debug_preserved",
            )
            if not candidate.get(gate)
        ]
        if missing_gates:
            diagnostic = plan["diagnostic"] + "; failed gates: " + ", ".join(missing_gates)
            return _result(candidate, plan, _missing_proof_decision(plan), diagnostic)
    return _result(candidate, plan, _missing_proof_decision(plan), plan["diagnostic"])


def evaluate_candidate(candidate: dict[str, Any]) -> dict[str, Any]:
    pass_id = str(candidate.get("pass_id", ""))
    plan = PASS_PLANS.get(pass_id)
    if plan is None:
        rejected = {
            "ordinal": 0,
            "mode": "rejected",
            "rewrites_ir": False,
            "invalidates_global_proof_state": False,
            "missing_proof_action": "REJECT_FAIL_CLOSED",
            "diagnostic": "semantic optimization candidate has no registered pass",
        }
        return _result(
            {"pass_id": pass_id or "<missing>", **candidate},
            rejected,
            "REJECTED_FAIL_CLOSED",
            rejected["diagnostic"],
        )

    if plan["mode"] == "reserved":
        return _result(candidate, plan, "SKIPPED_FAIL_CLOSED", plan["diagnostic"])

    if plan["rewrites_ir"] and not candidate.get("benchmark_governance_ready"):
        return _missing(candidate, plan)

    if pass_id == "semantic-precondition-gate":
        if (
            candidate.get("typed_sema_handoff_deterministic")
            and candidate.get("lowering_boundary_replay_key_present")
            and candidate.get("parse_to_lowering_ready")
        ):
            return _result(candidate, plan, "VERIFIED")
        return _missing(candidate, plan)

    if pass_id == "nil-receiver-folding":
        if candidate.get("compile_time_nil_receiver") and candidate.get(
            "optional_send_side_effect_free"
        ):
            return _result(candidate, plan, "APPLIED")
        return _missing(candidate, plan)

    if pass_id == "direct-dispatch-exact-call":
        if (
            candidate.get("direct_callee_symbol_present")
            and candidate.get("direct_signature_present")
            and candidate.get("explicit_argument_types_cover_arguments")
            and candidate.get("direct_result_owns_ir_storage")
        ):
            return _result(candidate, plan, "APPLIED")
        return _missing(candidate, plan)

    if pass_id == "arc-retained-result-cleanup":
        if (
            candidate.get("arc_mode_enabled")
            and candidate.get("method_family_returns_retained_result")
            and candidate.get("related_result_cleanup_disarmed")
        ):
            return _result(candidate, plan, "APPLIED")
        return _missing(candidate, plan)

    if pass_id == "runtime-dispatch-preservation":
        if (
            candidate.get("runtime_dispatch_result_owner_explicit")
            and candidate.get("canonical_runtime_dispatch_symbol")
            and candidate.get("retired_routes_disabled")
            and candidate.get("compatibility_routes_disabled")
        ):
            return _result(candidate, plan, "VERIFIED")
        return _missing(candidate, plan)

    if pass_id == "devirtualization":
        if (
            candidate.get("exact_target_receiver_static_type_proven")
            and candidate.get("sealed_final_dispatch_evidence_present")
            and candidate.get("exact_method_target_identity_present")
            and candidate.get("class_category_method_mutation_generation_pinned")
            and candidate.get("runtime_cache_version_dependency_pinned")
            and candidate.get("devirtualization_ownership_arc_safe")
            and candidate.get("devirtualization_source_map_debug_preserved")
            and candidate.get("devirtualization_runtime_abi_safe")
            and candidate.get("devirtualization_package_abi_identical")
        ):
            return _result(candidate, plan, "APPLIED")
        return _missing(candidate, plan)

    if pass_id == "method-inlining":
        if (
            all(candidate.get(gate) for gate in METHOD_INLINING_TEXT_GATES)
            and all(candidate.get(gate) for gate in METHOD_INLINING_TRUE_GATES)
            and not any(candidate.get(gate) for gate in METHOD_INLINING_FALSE_GATES)
        ):
            return _result(candidate, plan, "APPLIED")
        return _missing(candidate, plan)

    if pass_id == "cache-aware-dispatch":
        if (
            candidate.get("runtime_cache_invalidation_semantics_public")
            and candidate.get("cache_aware_helper_symbol_present")
            and candidate.get("cache_aware_semantic_replay_preserves_miss_behavior")
            and candidate.get("cache_aware_strict_status_envelope_checked")
            and candidate.get("cache_aware_source_map_debug_preserved")
        ):
            return _result(candidate, plan, "APPLIED")
        return _missing(candidate, plan)

    if pass_id == "ir-cleanup-verifier":
        if (
            candidate.get("all_prior_mutations_declared_invalidation")
            and candidate.get("unsupported_skips_emit_no_success_claim")
            and candidate.get("semantic_equivalence_verdict_present")
        ):
            return _result(candidate, plan, "VERIFIED")
        return _missing(candidate, plan)

    return _missing(candidate, plan)


def evaluate_candidates(candidates: list[dict[str, Any]]) -> list[dict[str, Any]]:
    return [evaluate_candidate(candidate) for candidate in candidates]
