from __future__ import annotations

import importlib
import json
from pathlib import Path

from scripts.objc3c_semantic_optimization_pipeline import (
    CONTRACT_ID,
    PERFORMANCE_GOVERNANCE_CONTRACT_ID,
    PIPELINE_PATH,
    REQUIRED_PERFORMANCE_PUBLIC_ACTIONS,
    REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS,
    REQUIRED_CAPABILITY_ROWS,
    REQUIRED_EVIDENCE_IDS,
    REQUIRED_PASS_ORDER,
    REQUIRED_RESERVED_SKIP_DIAGNOSTIC_CODE,
    RUNTIME_EQUIVALENCE_CONTRACT_ID,
    RESERVED_SKIP_CONTRACT_ID,
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
    assert result.payload["enabled_pass_count"] >= 3
    assert result.payload["reserved_pass_count"] == 3
    assert result.payload["reserved_skip_fixture_count"] == (
        result.payload["reserved_pass_count"]
    )
    assert result.payload["reserved_skip_passes"] == [
        "cache-aware-dispatch",
        "devirtualization",
        "method-inlining",
    ]
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
    assert '"pattern": "^[0-9a-f]{64}$"' in text
    assert '"workflow_action": { "const": "validate-semantic-optimization-pipeline" }' in text


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


def _write_pipeline_variant(tmp_path: Path, payload: dict[str, object]) -> Path:
    path = tmp_path / "pipeline.json"
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    return path


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


def test_semantic_optimization_pipeline_requires_pass_specific_reserved_skip_fixture(
    tmp_path: Path,
) -> None:
    payload = json.loads(PIPELINE_PATH.read_text(encoding="utf-8"))
    for pass_row in payload["pass_registry"]:
        if pass_row["pass_id"] == "method-inlining":
            pass_row["fixtures"] = [
                "tests/tooling/fixtures/semantic_optimization_pipeline/reserved_devirtualization_skip.json"
            ]
            break

    result = validate_pipeline(_write_pipeline_variant(tmp_path, payload))

    assert not result.passed
    assert any(
        "skip fixture pass_id mismatch for method-inlining" in failure
        for failure in result.failures
    )
    assert any(
        "missing proofs drift from preservation contract: method-inlining" in failure
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
