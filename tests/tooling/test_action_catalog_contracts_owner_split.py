from __future__ import annotations

from scripts.objc3c_workflow.action_audience import (
    action_audience,
    action_audience_contract_payload,
)
from scripts.objc3c_workflow.action_catalog_groups import (
    ACTION_CATALOG_SECTION_GROUPS,
    action_catalog_section_owner_contracts,
)
from scripts.objc3c_workflow.action_catalog_sections import (
    action_catalog_section_merge_contract,
    merge_action_catalog_sections,
)
from scripts.objc3c_workflow.action_payload_category import (
    action_category,
    action_category_contract_payload,
)
from scripts.objc3c_workflow.action_payload_field_owners import (
    action_payload_owner_contracts,
    action_payload_owner_fields,
)
from scripts.objc3c_workflow.action_registry_payload import build_registry_payload
from scripts.objc3c_workflow.action_spec import ActionSpec


def test_action_catalog_section_groups_publish_owner_contracts() -> None:
    payload = action_catalog_section_owner_contracts()

    assert payload["contract_id"] == "objc3c-workflow-action-catalog-groups-v1"
    assert payload["hidden_internal_public_split_allowed"] is False
    assert payload["wrapper_only_catalog_grouping_allowed"] is False
    assert [group["group_id"] for group in payload["groups"]] == [
        group.group_id for group in ACTION_CATALOG_SECTION_GROUPS
    ]
    assert all(group["action_count"] > 0 for group in payload["groups"])
    assert all(
        group["wrapper_only_catalog_grouping_allowed"] is False
        for group in payload["groups"]
    )


def test_action_catalog_merge_accepts_owned_section_groups() -> None:
    merged = merge_action_catalog_sections(*ACTION_CATALOG_SECTION_GROUPS)

    assert "lint" in merged
    assert "test-default" in merged
    assert action_catalog_section_merge_contract() == {
        "contract_id": "objc3c-workflow-action-catalog-section-merge-v1",
        "owner_surface": "scripts/objc3c_workflow/action_catalog_sections.py",
        "duplicate_action_policy": "fail-closed",
        "hidden_internal_public_split_allowed": False,
        "wrapper_only_catalog_grouping_allowed": False,
        "public_contract": True,
    }


def test_action_category_contract_has_no_alias_metadata() -> None:
    payload = action_category_contract_payload()

    assert action_category("lint") == "lint"
    assert action_category("check-public-command-budget") == "check"
    assert payload["unknown_category_retired_route_allowed"] is False
    assert payload["public_command_aliases_allowed"] is False
    try:
        action_category("ci-public-command-budget")
    except ValueError as exc:
        assert str(exc) == "unknown workflow action category: ci"
    else:
        raise AssertionError("unknown action categories should fail closed")


def test_action_audience_contract_has_no_default_public_split() -> None:
    payload = action_audience_contract_payload()

    assert action_audience("build-native-full") == "operator"
    assert action_audience("build-site") == "maintainer"
    assert action_audience("lint") == "maintainer"
    assert payload["hidden_internal_public_split_allowed"] is False
    assert payload["unknown_audience_retired_route_allowed"] is False
    assert payload["public_command_aliases_allowed"] is False
    try:
        action_audience("ci-public-command-budget")
    except ValueError as exc:
        assert str(exc) == (
            "unclassified workflow action audience: ci-public-command-budget"
        )
    else:
        raise AssertionError("unknown action audiences should fail closed")


def test_action_payload_publishes_durable_owner_contract_bundle() -> None:
    fields = action_payload_owner_fields("check-public-command-budget")
    contracts = action_payload_owner_contracts()

    assert fields["payload_field_owner_map"]["payload_contracts"] == (
        "scripts/objc3c_workflow/action_payload_field_owners.py"
    )
    assert fields["payload_contracts"] == contracts
    assert contracts["audience"]["unknown_audience_retired_route_allowed"] is False
    assert "public_command_aliases" not in contracts


def test_registry_payload_has_no_internal_public_or_alias_drift() -> None:
    payload = build_registry_payload(
        [
            ActionSpec(
                "check-public-command-budget",
                "check public command budget",
                "runner-internal sample",
            )
        ]
    )
    action = payload["actions"][0]

    assert payload["public_action_count"] == 1
    assert payload["internal_action_count"] == 0
    assert action["category"] == "check"
    assert action["audience"] == "maintainer"
    assert action["public_command"] == (
        "npm run objc3c -- check-public-command-budget"
    )
    assert "public_command_aliases" not in action["payload_contracts"]
