from __future__ import annotations

import json
from pathlib import Path

import pytest

from scripts.objc3c_workflow.actions.external_validation_owner_contracts import (
    EXTERNAL_VALIDATION_OWNER_CONTRACT,
    external_validation_action_owner_map,
    external_validation_owner_contract,
    require_external_validation_action,
)
from scripts.objc3c_workflow.actions.external_validation_targets import (
    EXTERNAL_VALIDATION_TARGETS,
    VALIDATE_EXTERNAL_VALIDATION_CHILD_ACTIONS,
)


ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "external_validation"


def _load_fixture(name: str) -> dict[str, object]:
    return json.loads((FIXTURE_ROOT / name).read_text(encoding="utf-8"))


def test_external_validation_owner_contract_matches_checked_in_payload() -> None:
    assert external_validation_owner_contract() == _load_fixture("owner_contracts.json")


def test_external_validation_owner_contract_blocks_non_truthy_evidence() -> None:
    contract = external_validation_owner_contract()
    trust_policy = _load_fixture("trust_policy.json")
    guardrails = contract["hard_cutover_guardrails"]

    assert contract["accepted_trust_states"] == trust_policy["capability_truth_trust_states"]
    assert trust_policy["publishable_trust_states"] == ["accepted"]
    assert trust_policy["forbidden_capability_truth_states"] == [
        "candidate",
        "quarantined",
        "rejected",
    ]
    assert guardrails["fallback_trust_route_allowed"] is False
    assert guardrails["local_only_validation_claim_allowed"] is False
    assert guardrails["evidence_log_validation_claim_allowed"] is False
    assert guardrails["wrapper_only_action_surface_allowed"] is False
    assert guardrails["unaccepted_fixture_publication_allowed"] is False
    assert guardrails["quarantined_fixture_capability_truth_allowed"] is False


def test_external_validation_owner_contract_covers_workflow_actions() -> None:
    contract = external_validation_owner_contract()
    workflow_surface = _load_fixture("workflow_surface.json")
    owner_map = external_validation_action_owner_map()

    assert workflow_surface["owner_contracts"] == (
        "tests/tooling/fixtures/external_validation/owner_contracts.json"
    )
    assert contract["owned_actions"] == workflow_surface["required_actions"]
    assert contract["validate_child_actions"] == workflow_surface["validate_child_actions"]
    assert tuple(contract["validate_child_actions"]) == VALIDATE_EXTERNAL_VALIDATION_CHILD_ACTIONS
    assert set(owner_map) == set(workflow_surface["required_actions"])


def test_external_validation_targets_have_artifact_truth_contracts() -> None:
    contract = external_validation_owner_contract()
    report_contracts = contract["required_report_contracts"]

    for action_name, target in EXTERNAL_VALIDATION_TARGETS.items():
        assert action_name in contract["owned_actions"]
        assert target.owner_contract_id == EXTERNAL_VALIDATION_OWNER_CONTRACT.contract_id
        assert target.artifact_report in report_contracts
        assert target.capability_truth_source
        require_external_validation_action(action_name)


def test_external_validation_rejects_unknown_trust_route() -> None:
    with pytest.raises(ValueError, match="not an external-validation owned action"):
        require_external_validation_action("trust-local-external-validation-report")
