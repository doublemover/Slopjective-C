from __future__ import annotations

from scripts.objc3c_workflow.action_registry_bridge_payload import (
    registry_bridge_fields,
)
from scripts.objc3c_workflow.action_registry_capability_truth import (
    registry_capability_truth_fields,
)
from scripts.objc3c_workflow.action_registry_payload import build_registry_payload
from scripts.objc3c_workflow.action_spec import ActionSpec
from scripts.objc3c_workflow.registry_schema_index import ACTION_PAYLOAD_SCHEMA_REF


def test_registry_bridge_fields_keep_single_public_bridge_contract() -> None:
    fields = registry_bridge_fields()

    assert fields["package_bridge_count"] == 1
    assert fields["single_package_bridge_only"] is True
    assert fields["package_bridges"] == ["npm run objc3c -- <action>"]


def test_registry_capability_truth_fields_use_schema_owner_ref() -> None:
    fields = registry_capability_truth_fields()["capability_truth"]

    assert fields["scope"] == "workflow-action-registry"
    assert fields["machine_readable"] is True
    assert fields["action_payload_schema_ref"] == ACTION_PAYLOAD_SCHEMA_REF


def test_registry_payload_composes_owner_fields() -> None:
    payload = build_registry_payload(
        [
            ActionSpec(
                "sample-action",
                "sample action",
                "runner-internal sample",
            )
        ]
    )

    assert payload["package_bridge_count"] == registry_bridge_fields()[
        "package_bridge_count"
    ]
    assert payload["capability_truth"] == registry_capability_truth_fields()[
        "capability_truth"
    ]
    assert payload["actions"][0]["action"] == "sample-action"
