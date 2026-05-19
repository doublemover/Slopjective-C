from __future__ import annotations

from scripts.objc3c_workflow.actions.command_facades_inventory import (
    COMMAND_FACADE_INVENTORY_CONTRACT_ID,
    COMMAND_FACADE_INVENTORY_OWNER_SURFACE,
    command_facade_inventory_contract,
    package_bridge_inventory_fields,
    package_bridge_names_from_scripts,
    package_bridge_payloads_from_scripts,
)

WORKFLOW_ENTRYPOINT = "python -m scripts." + "objc3c_workflow"
WORKFLOW_LINT_ENTRYPOINT = f"{WORKFLOW_ENTRYPOINT} lint"


def test_command_facade_inventory_owns_package_bridge_discovery() -> None:
    scripts = {"objc3c": WORKFLOW_ENTRYPOINT}

    assert package_bridge_names_from_scripts(scripts) == ["objc3c"]

    fields = package_bridge_inventory_fields(scripts)
    assert fields["contract_id"] == COMMAND_FACADE_INVENTORY_CONTRACT_ID
    assert fields["owner_surface"] == COMMAND_FACADE_INVENTORY_OWNER_SURFACE
    assert fields["package_bridge_count"] == 1
    assert fields["canonical_package_bridge"] == "objc3c"
    assert fields["package_bridges"] == ["objc3c"]
    assert fields["missing_package_bridge"] == []
    assert fields["unexpected_package_bridges"] == []
    assert fields["single_package_bridge_only"] is True
    assert fields["public_command_template"] == "npm run objc3c -- <action>"


def test_command_facade_inventory_reports_package_bridge_drift() -> None:
    fields = package_bridge_inventory_fields(
        {
            "objc3c": WORKFLOW_ENTRYPOINT,
            "lint": WORKFLOW_LINT_ENTRYPOINT,
        }
    )
    assert fields["package_bridges"] == ["objc3c"]
    assert fields["missing_package_bridge"] == []
    assert fields["unexpected_package_bridges"] == ["lint"]

    missing = package_bridge_inventory_fields({})
    assert missing["package_bridges"] == []
    assert missing["missing_package_bridge"] == ["objc3c"]


def test_command_facade_inventory_publishes_orchestration_contract() -> None:
    payload = command_facade_inventory_contract(
        {"objc3c": WORKFLOW_ENTRYPOINT},
        workflow_action_count=12,
        public_action_count=12,
        internal_action_count=0,
    )

    assert payload["workflow_action_count"] == 12
    assert payload["public_action_count"] == 12
    assert payload["internal_action_count"] == 0
    assert payload["orchestration_model"] == {
        "package_bridge_owner": "package.json scripts.objc3c -> scripts.objc3c_workflow",
        "package_bridge_registry_owner": "scripts/objc3c_workflow/public_bridge_registry.py",
        "package_bridge_payload_owner": "scripts/objc3c_workflow/public_bridge_payloads.py",
        "internal_action_owner": (
            "ACTION_SPECS actions are reached through the objc3c package bridge"
        ),
        "appendix_generator": "scripts/render_objc3c_public_command_surface.py",
        "inventory_owner": "scripts/objc3c_workflow/actions/command_facades_inventory.py",
    }


def test_command_facade_inventory_builds_public_bridge_payloads() -> None:
    payloads = package_bridge_payloads_from_scripts(
        {"objc3c": WORKFLOW_ENTRYPOINT}
    )
    assert len(payloads) == 1
    assert payloads[0]["script_name"] == "objc3c"
    assert payloads[0]["package_bridge"] == "objc3c"
    assert payloads[0]["backend"] == "npm run objc3c -- <action>"
    assert payloads[0]["package_script_lookup"]["contract_id"] == (
        "objc3c-workflow-package-script-lookup-v1"
    )
    assert payloads[0]["capability_truth"]["owner_surface"] == (
        "scripts/objc3c_workflow/public_bridge_payloads.py"
    )
