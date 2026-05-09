from __future__ import annotations

from scripts.objc3c_workflow.request_error_messages import (
    UNKNOWN_ACTION_MESSAGE_CONTRACT_ID,
    UNKNOWN_PACKAGE_SCRIPT_MESSAGE_CONTRACT_ID,
    request_error_message_contract_fields,
    unknown_action_message,
    unknown_package_script_message,
)


def test_request_error_messages_are_owned_helpers() -> None:
    assert UNKNOWN_ACTION_MESSAGE_CONTRACT_ID == (
        "objc3c-workflow-unknown-action-message-v1"
    )
    assert UNKNOWN_PACKAGE_SCRIPT_MESSAGE_CONTRACT_ID == (
        "objc3c-workflow-unknown-package-script-message-v1"
    )
    contract = request_error_message_contract_fields()
    assert contract["canonical_command_template"] == "npm run objc3c -- <action>"
    assert contract["retired_metadata_allowed"] is False
    assert unknown_action_message("missing") == (
        "unknown action: missing; expected command shape: "
        "npm run objc3c -- <action>"
    )
    assert unknown_package_script_message("retired-script") == (
        "unknown package script: retired-script; expected package script: objc3c"
    )
