from __future__ import annotations

from copy import deepcopy

import pytest

from scripts.check_objc3c_foundations_umbrella_source_truth import (
    CONTRACT_PATH,
    _load_json,
    _validate_capability_truth,
    _validate_umbrella_readiness,
    _validate_workflow_actions,
    validate_foundations_source_truth,
    validate_foundations_source_truth_payload,
)


def test_archived_foundations_umbrella_contract_rejects_missing_prototype_source() -> None:
    with pytest.raises(RuntimeError, match="evidence_paths references missing path: native/objc3c/src/"):
        validate_foundations_source_truth()


def test_foundations_umbrella_rejects_capability_state_drift() -> None:
    payload = deepcopy(_load_json(CONTRACT_PATH))
    row = payload["capability_truth_rows"][2]
    row["expected_state"] = "reserved"
    payload["capability_truth_rows"] = [row]

    with pytest.raises(RuntimeError, match="expected_state reserved does not match matrix state implemented"):
        _validate_capability_truth(payload)


def test_foundations_umbrella_rejects_unregistered_workflow_action() -> None:
    payload = deepcopy(_load_json(CONTRACT_PATH))
    row = payload["workflow_action_contracts"][0]
    row["action"] = "validate-invented-foundation"
    row["command"] = "npm run objc3c -- validate-invented-foundation"

    with pytest.raises(RuntimeError, match="not in ACTION_SPECS"):
        _validate_workflow_actions(payload)


def test_foundations_umbrella_rejects_missing_projection_source() -> None:
    payload = deepcopy(_load_json(CONTRACT_PATH))
    payload["umbrella_readiness_contract"]["projection_authoritative_data_must_include"] = [
        "tests/tooling/fixtures/foundations_umbrella_readiness/missing.json",
    ]

    with pytest.raises(RuntimeError, match="authoritative_data missing"):
        _validate_umbrella_readiness(payload)


def test_foundations_umbrella_source_truth_rejects_wrapper_only_action() -> None:
    payload = deepcopy(_load_json(CONTRACT_PATH))
    payload["workflow_action_contracts"][0]["wrapper_only"] = True

    with pytest.raises(Exception, match="schema validation|wrapper-only"):
        validate_foundations_source_truth_payload(payload)


def test_foundations_umbrella_source_truth_rejects_generated_only_schema_path() -> None:
    payload = deepcopy(_load_json(CONTRACT_PATH))
    payload["schema_contracts"][0]["path"] = "generated/schemas/standalone.json"

    with pytest.raises(Exception, match="schema validation|non-source"):
        validate_foundations_source_truth_payload(payload)
