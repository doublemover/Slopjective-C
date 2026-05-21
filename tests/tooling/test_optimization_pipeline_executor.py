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
        f"{result['pass_id']}:{result['metadata_key'].split(';source=', 1)[1]}": result
        for result in results
    }


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

    reserved = results["devirtualization:fixture:reserved:devirtualization"]
    assert reserved["decision"] == "SKIPPED_FAIL_CLOSED"
    assert reserved["success_claim"] is False
    assert "reserved" in str(reserved["diagnostic"])


def test_optimization_pipeline_metadata_is_deterministic_and_source_backed() -> None:
    left = _candidate_results()
    right = _candidate_results()
    assert left == right
    for result in left.values():
        metadata_key = str(result["metadata_key"])
        assert "timestamp" not in metadata_key.lower()
        assert "success-claim=false" in metadata_key

    cpp_text = CPP_SOURCE.read_text(encoding="utf-8")
    assert "EvaluateObjc3SemanticOptimizationCandidate" in cpp_text
    assert "BuildObjc3SemanticOptimizationMetadataKey" in cpp_text
    assert "REJECTED_FAIL_CLOSED" in cpp_text
    assert "success_claim = false" in cpp_text

    pipeline_text = PIPELINE_SOURCE.read_text(encoding="utf-8")
    assert "RunObjc3SemanticOptimizationPipelineTrace" in pipeline_text
    assert "AllObjc3SemanticOptimizationTraceMutationsDeclareInvalidation" in pipeline_text
