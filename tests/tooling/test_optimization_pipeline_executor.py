from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_optimization_pipeline.semantic_metadata import (
    REQUIRED_PASS_ORDER,
    evaluate_candidates,
)
from scripts.objc3c_semantic_optimization_pipeline import (
    REQUIRED_PASS_ORDER as VALIDATOR_PASS_ORDER,
)


ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / "tests/tooling/fixtures/optimization_pipeline/candidates.json"
CPP_SOURCE = (
    ROOT / "native/objc3c/src/opt/objc3_semantic_optimization_executor.cpp"
)
PIPELINE_SOURCE = (
    ROOT / "native/objc3c/src/pipeline/objc3_semantic_optimization_pipeline.cpp"
)


def _candidate_results() -> dict[str, dict[str, object]]:
    payload = json.loads(FIXTURE.read_text(encoding="utf-8"))
    results = evaluate_candidates(payload["candidates"])
    return {
        f"{result['pass_id']}:{_metadata_source(str(result['metadata_key']))}": result
        for result in results
    }


def _metadata_source(metadata_key: str) -> str:
    for segment in metadata_key.split(";"):
        if segment.startswith("source="):
            return segment.removeprefix("source=")
    raise AssertionError(f"metadata key is missing source segment: {metadata_key}")


def test_optimization_pipeline_order_matches_public_validator() -> None:
    assert REQUIRED_PASS_ORDER == VALIDATOR_PASS_ORDER


def test_optimization_pipeline_applies_only_proven_mutating_passes() -> None:
    results = _candidate_results()

    direct = results["direct-dispatch-exact-call:fixture:direct-dispatch:exact"]
    assert direct["decision"] == "APPLIED"
    assert direct["success_claim"] is True
    assert direct["invalidates_global_proof_state"] is True
    assert "decision=APPLIED" in str(direct["metadata_key"])

    nil_fold = results["nil-receiver-folding:fixture:nil-receiver:compile-time-nil"]
    assert nil_fold["decision"] == "APPLIED"
    assert nil_fold["rewrites_ir"] is True

    arc = results["arc-retained-result-cleanup:fixture:arc:retained-result-cleanup"]
    assert arc["decision"] == "APPLIED"
    assert arc["success_claim"] is True

    inline = results["method-inlining:fixture:method-inlining:safe-scalar-function"]
    assert inline["decision"] == "APPLIED"
    assert inline["success_claim"] is True
    assert inline["rewrites_ir"] is True
    assert inline["invalidates_global_proof_state"] is True
    assert "invalidates-global-proof-state=true" in str(inline["metadata_key"])
    assert "method_inline_source_map_debug_preserved=true" in str(inline["metadata_key"])
    assert "method_inline_runtime_abi_safe=true" in str(inline["metadata_key"])

    cache_aware = results[
        "cache-aware-dispatch:fixture:cache-aware-dispatch:strict-helper"
    ]
    assert cache_aware["decision"] == "APPLIED"
    assert cache_aware["success_claim"] is True
    assert cache_aware["rewrites_ir"] is True
    assert cache_aware["invalidates_global_proof_state"] is True
    assert "cache_aware_strict_status_envelope_checked=true" in str(
        cache_aware["metadata_key"]
    )


def test_optimization_pipeline_rejects_or_skips_missing_proofs_fail_closed() -> None:
    results = _candidate_results()

    missing_direct = results[
        "direct-dispatch-exact-call:fixture:direct-dispatch:missing-signature"
    ]
    assert missing_direct["decision"] == "REJECTED_FAIL_CLOSED"
    assert missing_direct["success_claim"] is False
    assert "signature" in str(missing_direct["diagnostic"])

    runtime = results["runtime-dispatch-preservation:fixture:runtime-dispatch:compatibility-route"]
    assert runtime["decision"] == "REJECTED_FAIL_CLOSED"
    assert runtime["success_claim"] is False
    assert "non-canonical" in str(runtime["diagnostic"])

    missing_devirt = results[
        "devirtualization:fixture:devirtualization:missing-sealed-proof"
    ]
    assert missing_devirt["decision"] == "REJECTED_FAIL_CLOSED"
    assert missing_devirt["success_claim"] is False
    assert "exact-target devirtualization requires" in str(missing_devirt["diagnostic"])

    missing_inline = results[
        "method-inlining:fixture:method-inlining:missing-callee-body"
    ]
    assert missing_inline["decision"] == "REJECTED_FAIL_CLOSED"
    assert missing_inline["success_claim"] is False
    assert "method inlining requires" in str(missing_inline["diagnostic"])
    assert "method_inline_callee_body_identity_present" in str(
        missing_inline["diagnostic"]
    )
    assert "method_inline_callee_body_identity_present=false" in str(
        missing_inline["metadata_key"]
    )

    side_effecting_inline = results[
        "method-inlining:fixture:method-inlining:side-effecting-callee"
    ]
    assert side_effecting_inline["decision"] == "REJECTED_FAIL_CLOSED"
    assert "method_inline_side_effect_summary_safe" in str(
        side_effecting_inline["diagnostic"]
    )

    missing_source_map = results[
        "method-inlining:fixture:method-inlining:missing-source-map"
    ]
    assert missing_source_map["decision"] == "REJECTED_FAIL_CLOSED"
    assert "method_inline_source_map_debug_preserved" in str(
        missing_source_map["diagnostic"]
    )
    assert "method_inline_diagnostic_location_preserved" in str(
        missing_source_map["diagnostic"]
    )

    missing_invalidation = results[
        "method-inlining:fixture:method-inlining:missing-invalidation"
    ]
    assert missing_invalidation["decision"] == "REJECTED_FAIL_CLOSED"
    assert "method_inline_invalidation_complete" in str(
        missing_invalidation["diagnostic"]
    )

    missing_cache_status = results[
        "cache-aware-dispatch:fixture:cache-aware-dispatch:missing-status-envelope"
    ]
    assert missing_cache_status["decision"] == "REJECTED_FAIL_CLOSED"
    assert missing_cache_status["success_claim"] is False
    assert "cache_aware_strict_status_envelope_checked" in str(
        missing_cache_status["diagnostic"]
    )


def test_optimization_pipeline_applies_exact_target_devirtualization() -> None:
    results = _candidate_results()

    devirt = results["devirtualization:fixture:devirtualization:exact-target"]
    assert devirt["decision"] == "APPLIED"
    assert devirt["success_claim"] is True
    assert devirt["rewrites_ir"] is True
    assert devirt["invalidates_global_proof_state"] is True
    assert "success-claim=true" in str(devirt["metadata_key"])


def test_optimization_pipeline_metadata_is_deterministic_and_source_backed() -> None:
    left = _candidate_results()
    right = _candidate_results()
    assert left == right
    for result in left.values():
        metadata_key = str(result["metadata_key"])
        assert "timestamp" not in metadata_key.lower()
        expected_success_claim = "true" if result["success_claim"] else "false"
        assert f"success-claim={expected_success_claim}" in metadata_key

    cpp_text = CPP_SOURCE.read_text(encoding="utf-8")
    assert "EvaluateObjc3SemanticOptimizationCandidate" in cpp_text
    assert "BuildObjc3SemanticOptimizationMetadataKey" in cpp_text
    assert "REJECTED_FAIL_CLOSED" in cpp_text
    assert "success_claim = false" in cpp_text
    assert "method_inline_callee_body_identity_present" in cpp_text
    assert "method inlining requires callee body identity" in cpp_text

    pipeline_text = PIPELINE_SOURCE.read_text(encoding="utf-8")
    assert "RunObjc3SemanticOptimizationPipelineTrace" in pipeline_text
    assert "AllObjc3SemanticOptimizationTraceMutationsDeclareInvalidation" in pipeline_text
