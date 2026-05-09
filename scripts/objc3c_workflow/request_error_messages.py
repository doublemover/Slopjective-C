"""Workflow request error message construction."""

from __future__ import annotations

from .public_bridge_constants import (
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
)

REQUEST_ERROR_MESSAGE_OWNER = "scripts/objc3c_workflow/request_error_messages.py"
UNKNOWN_ACTION_MESSAGE_CONTRACT_ID = "objc3c-workflow-unknown-action-message-v1"
UNKNOWN_PACKAGE_SCRIPT_MESSAGE_CONTRACT_ID = (
    "objc3c-workflow-unknown-package-script-message-v1"
)


def request_error_message_contract_fields() -> dict[str, object]:
    return {
        "owner_surface": REQUEST_ERROR_MESSAGE_OWNER,
        "unknown_action_contract_id": UNKNOWN_ACTION_MESSAGE_CONTRACT_ID,
        "unknown_package_script_contract_id": (
            UNKNOWN_PACKAGE_SCRIPT_MESSAGE_CONTRACT_ID
        ),
        "canonical_command_template": WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
        "canonical_package_script": WORKFLOW_BRIDGE_SCRIPT,
    }


def unknown_action_message(action: str) -> str:
    return (
        f"unknown action: {action}; expected command shape: "
        f"{WORKFLOW_PUBLIC_COMMAND_TEMPLATE}"
    )


def unknown_package_script_message(package_script: str) -> str:
    return (
        f"unknown package script: {package_script}; expected package script: "
        f"{WORKFLOW_BRIDGE_SCRIPT}"
    )


__all__ = [
    "REQUEST_ERROR_MESSAGE_OWNER",
    "UNKNOWN_ACTION_MESSAGE_CONTRACT_ID",
    "UNKNOWN_PACKAGE_SCRIPT_MESSAGE_CONTRACT_ID",
    "request_error_message_contract_fields",
    "unknown_action_message",
    "unknown_package_script_message",
]
