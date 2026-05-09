"""Usage text for the objc3c workflow CLI."""

from __future__ import annotations

from .argument_option_contracts import (
    ARGUMENT_OPTION_CONTRACT_ID,
    ARGUMENT_OPTION_OWNER_SURFACE,
    WORKFLOW_ARGUMENT_OPTIONS,
)
from .environment import WORKFLOW_COMMAND_TEXT

ARGUMENT_USAGE_CONTRACT_ID = "objc3c-workflow-argument-usage-v1"
ARGUMENT_USAGE_OWNER_SURFACE = "scripts/objc3c_workflow/argument_usage.py"


def usage_text() -> str:
    lines = [f"usage: {WORKFLOW_COMMAND_TEXT} <action> [args...]"]
    lines.extend(
        option.usage.replace("usage: ", "       ", 1)
        for option in WORKFLOW_ARGUMENT_OPTIONS
    )
    return "\n".join(lines)


def usage_contract_payload() -> dict[str, object]:
    return {
        "contract_id": ARGUMENT_USAGE_CONTRACT_ID,
        "owner_surface": ARGUMENT_USAGE_OWNER_SURFACE,
        "option_contract_id": ARGUMENT_OPTION_CONTRACT_ID,
        "option_owner_surface": ARGUMENT_OPTION_OWNER_SURFACE,
        "canonical_command": WORKFLOW_COMMAND_TEXT,
        "public_usage_text": usage_text(),
    }


__all__ = [
    "ARGUMENT_USAGE_CONTRACT_ID",
    "ARGUMENT_USAGE_OWNER_SURFACE",
    "usage_contract_payload",
    "usage_text",
]
