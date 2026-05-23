from __future__ import annotations

import importlib
import json
from copy import deepcopy
from pathlib import Path

from scripts.objc3c_semantic_optimization_pipeline import (
    CONTRACT_ID,
    OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_ID,
    OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_PATH,
    OPTIMIZATION_RUNTIME_DEBUG_SAFETY_UMBRELLA_CONTRACT_ID,
    PERFORMANCE_GOVERNANCE_CONTRACT_ID,
    PIPELINE_PATH,
    PROOF_MODEL_CONTRACT_ID,
    REQUIRED_PROOF_CASE_IDS,
    REQUIRED_PROOF_IDS,
    REQUIRED_PROOF_MODEL_PUBLIC_ACTIONS,
    REQUIRED_PROOF_RESULT_FIELDS,
    REQUIRED_PROOF_VERDICT_FIELDS,
    REQUIRED_PERFORMANCE_PUBLIC_ACTIONS,
    REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS,
    REQUIRED_CAPABILITY_ROWS,
    REQUIRED_EVIDENCE_IDS,
    REQUIRED_PASS_ORDER,
    REQUIRED_RESERVED_SKIP_DIAGNOSTIC_CODE,
    RUNTIME_EQUIVALENCE_CONTRACT_ID,
    RESERVED_SKIP_CONTRACT_ID,
    evaluate_optimization_proof_case,
    validate_pipeline,
)
from scripts.objc3c_workflow.public_command_api import (
    public_workflow_action_payload,
    public_workflow_action_names,
)


ROOT = Path(__file__).resolve().parents[2]


OWNER_MODULES = (
    "scripts.objc3c_semantic_optimization_pipeline",
    "scripts.objc3c_workflow.actions.semantic_optimization_pipeline",
    "scripts.objc3c_workflow.action_catalog_core_codegen",
    "scripts.objc3c_workflow.action_handlers_core_codegen",
)


def test_semantic_optimization_pipeline_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_semantic_optimization_pipeline_fixture_validates_source_truth() -> None:
    result = validate_pipeline(PIPELINE_PATH)

    assert result.passed, result.failures
    assert result.payload["contract_id"] == (
        "objc3c.optimization.semantic.pipeline.validation.v1"
    )
    assert result.payload["policy_path"] == (
        "tests/tooling/fixtures/semantic_optimization_pipeline/pipeline.json"
    )
    assert result.payload["pass_order"] == REQUIRED_PASS_ORDER
    assert result.payload["explicit_pass_order"] == REQUIRED_PASS_ORDER
    assert result.payload["pass_count"] == len(REQUIRED_PASS_ORDER)
    assert result.payload["semantic_preservation_contract_count"] == len(
        REQUIRED_PASS_ORDER
    )
    assert result.payload["proof_model_contract"] == PROOF_MODEL_CONTRACT_ID
    assert result.payload["proof_pass_contract_count"] == len(REQUIRED_PASS_ORDER)
    assert result.payload["proof_definition_count"] >= len(REQUIRED_PROOF_IDS)
    assert result.payload["proof_result_field_count"] >= len(REQUIRED_PROOF_RESULT_FIELDS)
    assert result.payload["proof_case_count"] == len(REQUIRED_PROOF_CASE_IDS)
    assert set(result.payload["proof_model_public_actions"]) >= (
        REQUIRED_PROOF_MODEL_PUBLIC_ACTIONS
    )
    assert result.payload["enabled_pass_count"] >= 3
    assert result.payload["reserved_pass_count"] == 0
    assert result.payload["reserved_skip_fixture_count"] == 0
    assert result.payload["reserved_skip_passes"] == []
    assert result.payload["performance_governance_contract"] == (
        PERFORMANCE_GOVERNANCE_CONTRACT_ID
    )
    assert set(result.payload["performance_public_actions"]) >= (
        REQUIRED_PERFORMANCE_PUBLIC_ACTIONS
    )
    assert result.payload["runtime_equivalence_contract"] == RUNTIME_EQUIVALENCE_CONTRACT_ID
    assert set(result.payload["runtime_equivalence_actions"]) >= (
        REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS
    )
    assert result.payload["runtime_equivalence_case_count"] == 2
    assert result.payload["runtime_equivalence_checked_path_count"] == 4
    assert result.payload["performance_workload_count"] == 2
    assert result.payload["performance_trace_count"] == 1
    assert result.payload["performance_digest_count"] == 3
    assert result.payload["optimization_safety_contract"] == (
        OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_PATH
    )
    assert result.payload["optimization_safety_benchmark_contract_count"] == 2
    assert result.payload["optimization_safety_boundary_count"] == 3
    assert result.payload["optimization_safety_negative_case_count"] == 12
    assert result.payload["optimization_safety_evidence_path_count"] == 13
    assert result.payload["optimization_safety_umbrella_status"] == (
        "bounded-source-owned-ready"
    )
    assert result.payload["optimization_safety_umbrella_source_contract_count"] == 8
    assert result.payload["optimization_safety_umbrella_negative_fixture_count"] == 4
    assert result.payload["optimization_safety_umbrella_public_action_count"] == 7
    assert result.payload["optimization_safety_umbrella_capability_row_count"] == 6
    assert set(result.payload["capability_rows_required"]) >= REQUIRED_CAPABILITY_ROWS
    assert set(result.payload["evidence_ids_required"]) >= REQUIRED_EVIDENCE_IDS


def test_semantic_optimization_pipeline_public_workflow_action_is_registered() -> None:
    action = "validate-semantic-optimization-pipeline"
    payload = public_workflow_action_payload(action)

    assert action in public_workflow_action_names()
    assert payload["action"] == action
    assert payload["backend"] == (
        "python:scripts/check_objc3c_semantic_optimization_pipeline.py"
    )
    assert payload["validation_tier"] == "policy"
    assert "fail-closed" in str(payload["guarantee_owner"])

    proof_action = "validate-optimization-proof-model"
    proof_payload = public_workflow_action_payload(proof_action)

    assert proof_action in public_workflow_action_names()
    assert proof_payload["action"] == proof_action
    assert proof_payload["backend"] == (
        "python:scripts/check_objc3c_optimization_proof_model.py"
    )
    assert proof_payload["validation_tier"] == "policy"
    assert "source graph" in str(proof_payload["guarantee_owner"])


def test_semantic_optimization_pipeline_schema_and_contract_are_stable() -> None:
    schema = ROOT / "schemas" / "objc3c-semantic-optimization-pipeline-v1.schema.json"
    text = schema.read_text(encoding="utf-8")

    assert CONTRACT_ID == "objc3c.optimization.semantic.pipeline.v1"
    assert '"drift_policy": { "const": "REJECT_FAIL_CLOSED" }' in text
    assert '"success_claim_on_skip": { "const": false }' in text
    assert '"reserved_pass_success_claims_allowed": { "const": false }' in text
    assert (
        '"const": "objc3c.optimization.semantic.pipeline.performance.governance.v1"'
        in text
    )
    assert (
        '"const": "objc3c.optimization.semantic.pipeline.runtime_equivalence.v1"'
        in text
    )
    assert '"const": "objc3c.optimization.semantic.proof_model.v1"' in text
    assert '"contains": { "const": "validate-optimization-proof-model" }' in text
    assert '"source_map_debug_impact_verdict": { "const": "required" }' in text
    assert '"contains": { "const": 8224 }' in text
    assert '"contains": { "const": 8226 }' in text
    assert '"contains": { "const": 8205 }' in text
    assert '"contains": { "const": 8227 }' in text
    assert '"contains": { "const": "source_identity_preservation" }' in text
    assert '"contains": { "const": "debug_identity_preservation" }' in text
    assert '"contains": { "const": "runtime_identity_preservation" }' in text
    assert '"contains": { "const": "runtime_invalidation_replay" }' in text
    assert '"contains": { "const": "exact_target_method_identity" }' in text
    assert '"contains": { "const": "ir_digest_before" }' in text
    assert '"contains": { "const": "ir_digest_after" }' in text
    for verdict_field in REQUIRED_PROOF_VERDICT_FIELDS:
        assert verdict_field in text
    assert '"pattern": "^[0-9a-f]{64}$"' in text
    assert '"workflow_action": { "const": "validate-semantic-optimization-pipeline" }' in text
    safety_schema = (
        ROOT / "schemas" / "objc3c-optimization-runtime-debug-safety-v1.schema.json"
    ).read_text(encoding="utf-8")
    assert OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_ID in safety_schema
    assert '"contains": { "const": 8205 }' in safety_schema
    assert '"contains": { "const": 8224 }' in safety_schema
    assert '"contains": { "const": 8226 }' in safety_schema
    assert '"contains": { "const": 8227 }' in safety_schema
    assert OPTIMIZATION_RUNTIME_DEBUG_SAFETY_UMBRELLA_CONTRACT_ID in safety_schema
    assert '"status": {' in safety_schema
    assert '"const": "bounded-source-owned-ready"' in safety_schema
    assert '"rejects_if_any_missing": { "const": true }' in safety_schema
    assert '"debug-source-map-preservation"' in safety_schema
    assert '"runtime-invalidation-replay"' in safety_schema


def test_semantic_optimization_pipeline_direct_dispatch_trace_is_semantic() -> None:
    before = ROOT / "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.before.ll"
    after = ROOT / "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.after.ll"

    before_text = before.read_text(encoding="utf-8")
    assert "objc3_runtime_dispatch_i32" in before_text
    assert "@.objc3.selector.isReady" in before_text
    assert "ret i32 %value" in before_text
    after_text = after.read_text(encoding="utf-8")
    assert "objc3_direct_Sample_isReady" in after_text
    assert "semantic-optimization.invalidate-global-proof-state" in after_text
    assert "objc3_runtime_dispatch_i32" not in after_text
    assert "zext i1 %direct to i32" in after_text
    assert "ret i32 %value" in after_text


def test_semantic_optimization_pipeline_method_inlining_trace_is_proof_backed() -> None:
    before = ROOT / "tests/native/ir/optimization/semantic_pipeline_method_inlining.before.ll"
    after = ROOT / "tests/native/ir/optimization/semantic_pipeline_method_inlining.after.ll"

    before_text = before.read_text(encoding="utf-8")
    assert "call i32 @objc3_inlineable_InlineMath_addOne" in before_text
    assert "callee body identity: body:InlineMath.addOne:v1" in before_text
    assert "exact callee identity: method:InlineMath.addOne:i32->i32" in before_text

    after_text = after.read_text(encoding="utf-8")
    assert "call i32 @objc3_inlineable_InlineMath_addOne" not in after_text
    assert "add nsw i32 %value, 1" in after_text
    assert "exact callee identity preserved: method:InlineMath.addOne:i32->i32" in after_text
    assert "source-map inline frame preserved" in after_text
    assert "diagnostic location preserved" in after_text
    assert "semantic-optimization.invalidate-global-proof-state" in after_text


def test_semantic_optimization_pipeline_cache_aware_dispatch_trace_is_strict() -> None:
    fixture = ROOT / "tests/native/ir/optimization/semantic_pipeline_cache_aware_dispatch.ll"
    text = fixture.read_text(encoding="utf-8")

    assert "objc3_runtime_cache_aware_dispatch_i32_checked" in text
    assert "objc3_runtime_prepare_cache_aware_dispatch_descriptor" in text
    assert "semantic-optimization.cache-aware-dispatch" in text
    assert "source-map.cache-aware-dispatch" in text
    assert "call i32 @objc3_runtime_prepare_cache_aware_dispatch_descriptor" in text
    assert "ptr null, i32 12, i32 7" in text
    assert "extractvalue" in text
    assert "icmp sge i32 %prepare_status, 0" in text
    assert "icmp sge i32 %status, 0" in text
    assert "cache_dispatch_strict_fail" in text
    assert "call void @abort()" in text


def _write_pipeline_variant(tmp_path: Path, payload: dict[str, object]) -> Path:
    path = tmp_path / "pipeline.json"
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    return path


def _proof_case(case_id: str) -> dict[str, object]:
    payload = json.loads(
        (
            ROOT
            / "tests/tooling/fixtures/semantic_optimization_pipeline/proof_cases.json"
        ).read_text(encoding="utf-8")
    )
    cases = {
        str(case["case_id"]): case
        for case in payload["cases"]
    }
    return cases[case_id]


def test_optimization_proof_model_accepts_full_proof_record() -> None:
    result = evaluate_optimization_proof_case(
        _proof_case("direct-dispatch-full-proof-record")
    )

    assert set(result) >= REQUIRED_PROOF_RESULT_FIELDS
    assert result["decision"] == "APPLIED"
    assert result["success_claim"] is True
    assert result["missing_proofs"] == []
    assert result["failed_proofs"] == []
    assert result["debug_safety_verdict"] == "PRESERVED"
    assert result["ownership_safety_verdict"] == "SAFE"


def test_optimization_proof_model_rejects_missing_source_graph_proof() -> None:
    result = evaluate_optimization_proof_case(
        _proof_case("direct-dispatch-missing-source-graph-proof")
    )

    assert result["decision"] == "REJECTED_FAIL_CLOSED"
    assert result["success_claim"] is False
    assert result["missing_proofs"] == ["source_graph_node_identity"]
    assert "source_graph_node_identity" in str(result["diagnostic"])


def test_optimization_proof_model_skips_missing_source_map_proof_without_success() -> None:
    result = evaluate_optimization_proof_case(
        _proof_case("nil-receiver-missing-source-map-proof")
    )

    assert result["decision"] == "SKIPPED_FAIL_CLOSED"
    assert result["success_claim"] is False
    assert result["missing_proofs"] == ["source_map_debug_identity"]
    assert result["failed_proofs"] == []


def test_optimization_proof_model_rejects_runtime_abi_drift() -> None:
    result = evaluate_optimization_proof_case(
        _proof_case("direct-dispatch-runtime-abi-drift")
    )

    assert result["decision"] == "REJECTED_FAIL_CLOSED"
    assert result["success_claim"] is False
    assert result["failed_proofs"] == ["runtime_abi_safety"]
    assert result["runtime_abi_safety_verdict"] == "DRIFTED"


def test_optimization_proof_model_skips_ownership_unsafe_candidate() -> None:
    result = evaluate_optimization_proof_case(
        _proof_case("arc-retained-result-ownership-unsafe")
    )

    assert result["decision"] == "SKIPPED_FAIL_CLOSED"
    assert result["success_claim"] is False
    assert result["failed_proofs"] == ["ownership_arc_safety"]
    assert result["ownership_safety_verdict"] == "UNSAFE"


def test_optimization_proof_model_rejects_stale_package_identity() -> None:
    result = evaluate_optimization_proof_case(
        _proof_case("direct-dispatch-stale-package-identity")
    )

    assert result["decision"] == "REJECTED_FAIL_CLOSED"
    assert result["success_claim"] is False
    assert result["failed_proofs"] == ["package_import_abi_identity"]
    assert result["package_import_abi_identity_verdict"] == "STALE"


def test_optimization_proof_model_accepts_safe_method_inlining() -> None:
    proof_case = _proof_case("method-inlining-safe-scalar-function-full-proof-record")
    result = evaluate_optimization_proof_case(proof_case)

    assert result["decision"] == "APPLIED"
    assert result["success_claim"] is True
    assert result["missing_proofs"] == []
    assert result["failed_proofs"] == []
    assert result["inlined_target_symbol"] == "objc3_inlineable_InlineMath_addOne"
    assert "runtime_generation_dependency" in result["invalidated_proof_state"]
    assert "invalidation_replay" in result["invalidated_proof_state"]
    replay = proof_case["candidate"]["runtime_invalidation_replay"][0]
    assert replay["expected_behavior"] == "guard-fail-closed-to-runtime-dispatch"
    assert "fallback" not in replay["expected_behavior"]


def test_optimization_proof_model_rechecks_method_inlining_candidate_semantics() -> None:
    base = _proof_case("method-inlining-safe-scalar-function-full-proof-record")

    dynamic_candidate = deepcopy(base)
    dynamic_candidate["candidate"]["inline_candidate_kind"] = "dynamic-method"
    dynamic_result = evaluate_optimization_proof_case(dynamic_candidate)
    assert dynamic_result["decision"] == "REJECTED_FAIL_CLOSED"
    assert dynamic_result["failed_proofs"] == ["scalar_inline_subset"]

    side_effecting_candidate = deepcopy(base)
    side_effecting_candidate["candidate"]["side_effect_summaries"] = [
        "pure",
        "reads:none",
        "writes:globalCounter",
        "calls:none",
    ]
    side_effecting_result = evaluate_optimization_proof_case(side_effecting_candidate)
    assert side_effecting_result["decision"] == "REJECTED_FAIL_CLOSED"
    assert side_effecting_result["failed_proofs"] == ["side_effect_summary"]

    stale_generation_candidate = deepcopy(base)
    stale_generation_candidate["candidate"][
        "callee_generation_snapshot"
    ] = "callee-generation=G11"
    stale_generation_result = evaluate_optimization_proof_case(stale_generation_candidate)
    assert stale_generation_result["decision"] == "REJECTED_FAIL_CLOSED"
    assert stale_generation_result["failed_proofs"] == [
        "callee_generation_invalidation",
        "runtime_identity_preservation",
    ]


def test_optimization_proof_model_rejects_method_inlining_missing_body_identity() -> None:
    result = evaluate_optimization_proof_case(
        _proof_case("method-inlining-missing-callee-body-identity")
    )

    assert result["decision"] == "REJECTED_FAIL_CLOSED"
    assert result["success_claim"] is False
    assert result["missing_proofs"] == ["callee_body_identity"]


def test_optimization_proof_model_rejects_method_inlining_safety_drift() -> None:
    unsafe_ownership = evaluate_optimization_proof_case(
        _proof_case("method-inlining-unsafe-ownership-effects")
    )
    side_effects = evaluate_optimization_proof_case(
        _proof_case("method-inlining-side-effecting-callee")
    )
    source_map = evaluate_optimization_proof_case(
        _proof_case("method-inlining-source-map-drift")
    )
    package_abi = evaluate_optimization_proof_case(
        _proof_case("method-inlining-package-abi-drift")
    )
    recursion = evaluate_optimization_proof_case(
        _proof_case("method-inlining-recursion-depth-limit")
    )
    stale_generation = evaluate_optimization_proof_case(
        _proof_case("method-inlining-stale-callee-generation")
    )

    assert unsafe_ownership["decision"] == "REJECTED_FAIL_CLOSED"
    assert "ownership_arc_effects_replay" in unsafe_ownership["failed_proofs"]
    assert side_effects["failed_proofs"] == ["side_effect_summary"]
    assert "source_map_inline_frame_preservation" in source_map["failed_proofs"]
    assert package_abi["failed_proofs"] == ["package_import_abi_identity"]
    assert recursion["failed_proofs"] == ["inlining_depth_recursion_limit"]
    assert stale_generation["failed_proofs"] == ["callee_generation_invalidation"]


def test_optimization_proof_model_rejects_missing_preservation_evidence() -> None:
    missing_source = evaluate_optimization_proof_case(
        _proof_case("method-inlining-missing-source-identity-preservation")
    )
    missing_debug = evaluate_optimization_proof_case(
        _proof_case("method-inlining-missing-debug-identity-preservation")
    )
    missing_replay = evaluate_optimization_proof_case(
        _proof_case("method-inlining-missing-runtime-invalidation-replay")
    )

    assert missing_source["decision"] == "REJECTED_FAIL_CLOSED"
    assert missing_source["missing_proofs"] == ["source_identity_preservation"]
    assert missing_source["success_claim"] is False

    assert missing_debug["decision"] == "REJECTED_FAIL_CLOSED"
    assert missing_debug["missing_proofs"] == ["debug_identity_preservation"]
    assert missing_debug["success_claim"] is False

    assert missing_replay["decision"] == "REJECTED_FAIL_CLOSED"
    assert missing_replay["missing_proofs"] == ["runtime_invalidation_replay"]
    assert missing_replay["success_claim"] is False


def test_semantic_optimization_pipeline_order_drift_fails_closed(tmp_path: Path) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    ordered = payload["pass_order_contract"]["ordered_pass_ids"]
    ordered[1], ordered[2] = ordered[2], ordered[1]

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "explicit pass order contract drifted" in failure for failure in result.failures
    )


def test_semantic_optimization_pipeline_rejects_reserved_success_claim(tmp_path: Path) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    for contract in payload["semantic_preservation_contracts"]:
        if contract["pass_id"] == "devirtualization":
            contract["success_claim_on_skip"] = True
            break

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "allows skip success claim" in failure for failure in result.failures
    )


def test_semantic_optimization_pipeline_requires_cache_aware_dispatch_fixture(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    for pass_row in payload["pass_registry"]:
        if pass_row["pass_id"] == "cache-aware-dispatch":
            pass_row["fixtures"] = []
            break

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "cache-aware dispatch enabled pass must cite cache-aware IR fixture" in failure
        for failure in result.failures
    )


def test_semantic_optimization_pipeline_rejects_generated_report_performance_input(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    payload["performance_governance"]["required_checked_in_paths"].append(
        "tmp/reports/performance-governance/dashboard-summary.json"
    )

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "uses generated-report input" in failure for failure in result.failures
    )
    assert any(
        "checked path is generated output" in failure for failure in result.failures
    )


def test_semantic_optimization_pipeline_rejects_workload_digest_drift(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    payload["performance_governance"]["workload_evidence"][0]["source_sha256"] = (
        "0" * 64
    )

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "workload digest drifted: compile-cold-wrapper" in failure
        for failure in result.failures
    )


def test_semantic_optimization_pipeline_requires_runtime_equivalence_actions(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    runtime_equivalence = payload["performance_governance"][
        "runtime_equivalence_validation"
    ]
    runtime_equivalence["required_public_actions"].remove("test-execution-replay")

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "runtime equivalence actions incomplete" in failure
        for failure in result.failures
    )


def test_semantic_optimization_pipeline_requires_manifest_backed_budget_metric(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    payload["performance_governance"]["workload_evidence"][1]["metric_id"] = (
        "missing_dispatch_claim_ms"
    )

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "workload budget metric missing: dispatch-cache" in failure
        for failure in result.failures
    )


def test_semantic_optimization_pipeline_rejects_optimization_safety_drift(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    safety_contract = payload["performance_governance"][
        "optimization_runtime_debug_safety_contract"
    ]
    safety = json.loads((ROOT / safety_contract).read_text(encoding="utf-8"))
    safety["runtime_debug_safety_boundaries"][1]["required_proof_ids"].remove(
        "debug_identity_preservation"
    )
    safety_path = tmp_path / "optimization_runtime_debug_safety_contract.json"
    safety_path.write_text(json.dumps(safety, indent=2), encoding="utf-8")
    payload["performance_governance"]["optimization_runtime_debug_safety_contract"] = (
        safety_path.as_posix()
    )

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "optimization runtime/debug safety contract path drifted" in failure
        for failure in result.failures
    )
    assert any(
        "method-inlining safety boundary missing debug/runtime/side-effect proofs"
        in failure
        for failure in result.failures
    )


def test_semantic_optimization_pipeline_rejects_optimization_umbrella_overclaim(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    safety_contract = payload["performance_governance"][
        "optimization_runtime_debug_safety_contract"
    ]
    safety = json.loads((ROOT / safety_contract).read_text(encoding="utf-8"))
    umbrella = safety["optimization_safety_umbrella_readiness"]
    umbrella["optimizer_success_path_fail_closed_policy"][
        "rejects_if_any_missing"
    ] = False
    umbrella["capability_row_recommendations"][0]["support_level"] = "full"
    safety_path = tmp_path / "optimization_runtime_debug_safety_contract.json"
    safety_path.write_text(json.dumps(safety, indent=2), encoding="utf-8")
    payload["performance_governance"]["optimization_runtime_debug_safety_contract"] = (
        safety_path.as_posix()
    )

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "optimization safety umbrella success policy must reject missing proof"
        in failure
        for failure in result.failures
    )
    assert any(
        "optimization safety umbrella overclaims full support" in failure
        for failure in result.failures
    )


def test_semantic_optimization_pipeline_rejects_negative_fixture_success_claim(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    safety_contract = payload["performance_governance"][
        "optimization_runtime_debug_safety_contract"
    ]
    safety = json.loads((ROOT / safety_contract).read_text(encoding="utf-8"))
    bad_fixture = tmp_path / "negative_method_inlining_success_claim.json"
    bad_fixture.write_text(
        json.dumps(
            {
                "contract_id": (
                    "objc3c.optimization.method_inlining.negative.success_claim.v1"
                ),
                "issue_refs": [8224],
                "pass_id": "method-inlining",
                "case_id": "method-inlining-negative-success-claim",
                "candidate_overrides": {
                    "method_inline_callee_body_identity_present": False
                },
                "expected_result": {
                    "decision": "REJECTED_FAIL_CLOSED",
                    "success_claim": True,
                    "diagnostic_contains": [
                        "method_inline_callee_body_identity_present"
                    ],
                },
            },
            indent=2,
        ),
        encoding="utf-8",
    )
    safety["optimization_safety_umbrella_readiness"][
        "negative_fixture_contracts"
    ].append(bad_fixture.as_posix())
    safety_path = tmp_path / "optimization_runtime_debug_safety_contract.json"
    safety_path.write_text(json.dumps(safety, indent=2), encoding="utf-8")
    payload["performance_governance"]["optimization_runtime_debug_safety_contract"] = (
        safety_path.as_posix()
    )

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "optimization method-inlining negative fixture permits success claim"
        in failure
        for failure in result.failures
    )


def test_semantic_optimization_pipeline_reserved_skip_fixtures_link_diagnostics() -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    reserved_passes = {
        pass_row["pass_id"]: pass_row
        for pass_row in payload["pass_registry"]
        if pass_row["mode"] == "reserved"
    }
    contracts = {
        contract["pass_id"]: contract
        for contract in payload["semantic_preservation_contracts"]
    }

    for pass_id, pass_row in reserved_passes.items():
        assert len(pass_row["fixtures"]) == 1
        fixture = json.loads((ROOT / pass_row["fixtures"][0]).read_text(encoding="utf-8"))

        assert fixture["contract_id"] == RESERVED_SKIP_CONTRACT_ID
        assert fixture["pass_id"] == pass_id
        assert fixture["status"] == "SKIPPED_FAIL_CLOSED"
        assert fixture["success_claim"] is False
        assert fixture["diagnostic_code"] == REQUIRED_RESERVED_SKIP_DIAGNOSTIC_CODE
        assert fixture["diagnostic"] in pass_row["fail_closed_diagnostics"]
        assert fixture["required_missing_proofs"] == contracts[pass_id]["required_proofs"]
