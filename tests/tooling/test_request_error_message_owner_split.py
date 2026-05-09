from __future__ import annotations

from scripts.objc3c_workflow.request_error_messages import (
    unknown_action_message,
    unknown_package_script_message,
)


def test_request_error_messages_are_owned_helpers() -> None:
    assert unknown_action_message("missing") == "unknown action: missing"
    assert unknown_package_script_message("legacy") == (
        "unknown package script: legacy"
    )
