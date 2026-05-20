from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_codegen_optimization_policy import (
    CONTRACT_ID,
    POLICY_PATH,
    REQUIRED_MATRIX_ROWS,
    validate_policy,
)
from scripts.objc3c_workflow.public_command_api import (
    public_workflow_action_payload,
    public_workflow_action_names,
)


ROOT = Path(__file__).resolve().parents[2]


OWNER_MODULES = (
    "scripts.objc3c_codegen_optimization_policy",
    "scripts.objc3c_workflow.actions.codegen_optimization_policy",
    "scripts.objc3c_workflow.action_catalog_core_codegen",
    "scripts.objc3c_workflow.action_handlers_core_codegen",
)


def test_codegen_optimization_policy_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_codegen_optimization_policy_fixture_validates_source_truth() -> None:
    result = validate_policy(POLICY_PATH)

    assert result.passed, result.failures
    assert result.payload["contract_id"] == (
        "objc3c.codegen.optimization.direct.dispatch.policy.validation.v1"
    )
    assert result.payload["policy_path"] == (
        "tests/tooling/fixtures/codegen_optimization_direct_dispatch/policy.json"
    )
    assert result.payload["optimizer_pass_count"] >= 3
    assert REQUIRED_MATRIX_ROWS.issubset(set(result.payload["direct_dispatch_matrix_rows"]))


def test_direct_dispatch_policy_covers_required_boundaries() -> None:
    policy = validate_policy(POLICY_PATH).payload

    assert policy["status"] == "PASS"
    assert policy["issue_mapping"]["issues"] == [8091, 8092]
    assert policy["issue_mapping"]["draft_ids"] == ["OC3-N33-001", "OC3-N33-002"]
    assert policy["workflow_action"] == "validate-codegen-optimization-policy"
    assert set(policy["coverage_terms"]) == {
        "cache_invalidation",
        "category",
        "marshalling",
        "nil_receiver",
        "protocol",
        "strict_fallback",
        "super",
    }


def test_codegen_optimization_public_workflow_action_is_registered() -> None:
    action = "validate-codegen-optimization-policy"
    payload = public_workflow_action_payload(action)

    assert action in public_workflow_action_names()
    assert payload["action"] == action
    assert payload["backend"] == (
        "python:scripts/check_objc3c_codegen_optimization_direct_dispatch_policy.py"
    )
    assert payload["validation_tier"] == "policy"
    assert "semantic-preserving" in str(payload["guarantee_owner"])


def test_policy_schema_and_contract_id_are_stable() -> None:
    schema = ROOT / "schemas" / "objc3c-codegen-optimization-direct-dispatch-policy-v1.schema.json"
    text = schema.read_text(encoding="utf-8")

    assert CONTRACT_ID == "objc3c.codegen.optimization.direct.dispatch.policy.v1"
    assert '"compatibility_shims_allowed": { "const": false }' in text
    assert '"fail_open_allowed": { "const": false }' in text
