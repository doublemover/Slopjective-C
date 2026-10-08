from __future__ import annotations

from copy import deepcopy

import pytest

from scripts.check_objc3c_foundations_umbrella_source_truth import (
    CONTRACT_PATH,
    _load_json,
    negative_foundations_source_truth_cases,
    validate_foundations_source_truth,
    validate_foundations_source_truth_payload,
)


def test_foundations_umbrella_source_truth_contract_is_schema_backed() -> None:
    summary = validate_foundations_source_truth()

    assert summary["status"] == "PASS"
    assert summary["roadmap_issue_refs"] == [8208, 8213, 8238]
    assert summary["capability_rows"] == [
        "capability.governance.umbrella-readiness",
        "language.advanced-runtime-closure",
        "modules.standalone-textual-interface-payload",
    ]
    assert summary["workflow_actions"] == [
        "validate-advanced-runtime-closure",
        "validate-foundations-umbrella-source-truth",
        "validate-standalone-textual-interface-payload",
        "validate-umbrella-readiness",
    ]


def test_foundations_umbrella_source_truth_negative_cases_fail_closed() -> None:
    failures = negative_foundations_source_truth_cases()

    assert set(failures) == {
        "capability-state-drift",
        "generated-source-path",
        "missing-umbrella-authoritative-data",
        "unregistered-public-command",
    }
    assert "expected_state reserved does not match matrix state implemented" in failures[
        "capability-state-drift"
    ]
    assert "schema validation" in failures["generated-source-path"]
    assert "not in ACTION_SPECS" in failures["unregistered-public-command"]
    assert "authoritative_data missing" in failures["missing-umbrella-authoritative-data"]


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
