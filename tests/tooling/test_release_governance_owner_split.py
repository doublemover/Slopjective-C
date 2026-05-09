from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_release_channels import (
    RELEASE_CHANNEL_ACTION_OWNER_MAP,
    RELEASE_CHANNEL_GATE_OWNERS,
)
from scripts.objc3c_workflow.action_catalog_reporting_release import (
    STRESS_REPORTING_RELEASE_ACTION_OWNER_MAP,
    STRESS_REPORTING_RELEASE_GATE_OWNERS,
)
from scripts.objc3c_workflow.action_catalog_release_foundation import (
    RELEASE_FOUNDATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_packaging_channels import (
    PACKAGING_CHANNEL_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_release_operations import (
    RELEASE_OPERATIONS_ACTION_SPECS,
)
from scripts.objc3c_workflow.actions.release_governance_owner_contracts import (
    PACKAGING_CHANNEL_ACTION_CONTRACTS,
    RELEASE_FOUNDATION_ACTION_CONTRACTS,
    RELEASE_GATE_OWNERS,
    RELEASE_OPERATIONS_ACTION_CONTRACTS,
    release_action_specs,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"


def _load_fixture(relative_path: str) -> dict[str, object]:
    return json.loads((ROOT / relative_path).read_text(encoding="utf-8"))


def test_release_gate_fixture_owner_policies_match_contract_model() -> None:
    for owner in RELEASE_GATE_OWNERS.values():
        source_surface = _load_fixture(owner.source_surface)
        workflow_surface = _load_fixture(owner.workflow_surface)

        assert source_surface["owner_policy"] == owner.owner_policy()
        assert workflow_surface["owner_policy"] == owner.workflow_owner_policy()
        assert set(owner.owned_actions).issubset(
            _surface_action_names(source_surface, workflow_surface)
        )
        assert workflow_surface["owner_policy"]["report_only_allowed"] is False


def test_release_channel_catalog_exports_gate_and_action_owners() -> None:
    release_channel_gate_ids = {
        "release-foundation",
        "packaging-channels",
        "release-operations",
        "distribution-credibility",
    }
    expected_gate_owners = {
        gate_id: owner.workflow_owner_policy()
        for gate_id, owner in RELEASE_GATE_OWNERS.items()
        if gate_id in release_channel_gate_ids
    }
    assert RELEASE_CHANNEL_GATE_OWNERS == expected_gate_owners

    for gate_id, owner in RELEASE_GATE_OWNERS.items():
        if gate_id not in release_channel_gate_ids:
            continue
        for action_name, action_owner in owner.action_owner_map().items():
            assert RELEASE_CHANNEL_ACTION_OWNER_MAP[action_name] == action_owner


def test_reporting_release_catalog_exports_all_release_gate_owners() -> None:
    expected_gate_owners = {
        gate_id: owner.workflow_owner_policy()
        for gate_id, owner in RELEASE_GATE_OWNERS.items()
    }
    assert STRESS_REPORTING_RELEASE_GATE_OWNERS == expected_gate_owners

    for owner in RELEASE_GATE_OWNERS.values():
        for action_name, action_owner in owner.action_owner_map().items():
            assert STRESS_REPORTING_RELEASE_ACTION_OWNER_MAP[action_name] == action_owner


def test_release_channel_catalogs_are_contract_facades() -> None:
    foundation_text = (
        WORKFLOW_ROOT / "action_catalog_release_foundation.py"
    ).read_text(encoding="utf-8")
    packaging_text = (
        WORKFLOW_ROOT / "action_catalog_packaging_channels.py"
    ).read_text(encoding="utf-8")
    operations_text = (
        WORKFLOW_ROOT / "action_catalog_release_operations.py"
    ).read_text(encoding="utf-8")

    assert "ActionSpec(" not in foundation_text
    assert "ActionSpec(" not in packaging_text
    assert "ActionSpec(" not in operations_text
    assert RELEASE_FOUNDATION_ACTION_SPECS == release_action_specs(
        RELEASE_FOUNDATION_ACTION_CONTRACTS
    )
    assert PACKAGING_CHANNEL_ACTION_SPECS == release_action_specs(
        PACKAGING_CHANNEL_ACTION_CONTRACTS
    )
    assert RELEASE_OPERATIONS_ACTION_SPECS == release_action_specs(
        RELEASE_OPERATIONS_ACTION_CONTRACTS
    )


def test_distribution_credibility_surface_is_split_by_owner_module() -> None:
    facade_text = (
        WORKFLOW_ROOT / "actions" / "release_governance_distribution_credibility.py"
    ).read_text(encoding="utf-8")

    assert "release_governance_distribution_credibility_artifacts" in facade_text
    assert "release_governance_distribution_credibility_validation" in facade_text
    assert "ROOT /" not in facade_text
    assert "run_composite_validation" not in facade_text


def _surface_action_names(
    source_surface: dict[str, object],
    workflow_surface: dict[str, object],
) -> set[str]:
    action_names: set[str] = set()
    for field_name in (
        "public_actions",
        "release_operations_owned_actions",
        "required_actions",
        "actions",
        "validation_child_actions",
    ):
        for payload in (source_surface, workflow_surface):
            values = payload.get(field_name)
            if isinstance(values, list):
                action_names.update(value for value in values if isinstance(value, str))
    return action_names
