"""Owned public option contracts for the workflow argument parser."""

from __future__ import annotations

from dataclasses import asdict, dataclass

from .environment import WORKFLOW_COMMAND_TEXT

ARGUMENT_OPTION_CONTRACT_ID = "objc3c-workflow-argument-options-v1"
ARGUMENT_OPTION_OWNER_SURFACE = (
    "scripts/objc3c_workflow/argument_option_contracts.py"
)
LIST_ACTIONS_OPTION = "--list-json"
DESCRIBE_ACTION_OPTION = "--describe"
DESCRIBE_PACKAGE_SCRIPT_OPTION = "--describe-script"


@dataclass(frozen=True)
class WorkflowArgumentOption:
    token: str
    request_kind: str
    required_argument_count: int
    usage: str
    owner_surface: str = ARGUMENT_OPTION_OWNER_SURFACE


WORKFLOW_ARGUMENT_OPTIONS: tuple[WorkflowArgumentOption, ...] = (
    WorkflowArgumentOption(
        token=LIST_ACTIONS_OPTION,
        request_kind="list-actions",
        required_argument_count=0,
        usage=f"usage: {WORKFLOW_COMMAND_TEXT} {LIST_ACTIONS_OPTION}",
    ),
    WorkflowArgumentOption(
        token=DESCRIBE_ACTION_OPTION,
        request_kind="describe-action",
        required_argument_count=1,
        usage=f"usage: {WORKFLOW_COMMAND_TEXT} {DESCRIBE_ACTION_OPTION} <action>",
    ),
    WorkflowArgumentOption(
        token=DESCRIBE_PACKAGE_SCRIPT_OPTION,
        request_kind="describe-package-script",
        required_argument_count=1,
        usage=(
            f"usage: {WORKFLOW_COMMAND_TEXT} {DESCRIBE_PACKAGE_SCRIPT_OPTION} "
            "<package-script>"
        ),
    ),
)

WORKFLOW_ARGUMENT_OPTION_BY_TOKEN = {
    option.token: option for option in WORKFLOW_ARGUMENT_OPTIONS
}


def workflow_argument_option(token: str) -> WorkflowArgumentOption | None:
    return WORKFLOW_ARGUMENT_OPTION_BY_TOKEN.get(token)


def workflow_argument_option_usage(token: str) -> str:
    option = workflow_argument_option(token)
    if option is None:
        return f"usage: {WORKFLOW_COMMAND_TEXT} <action> [args...]"
    return option.usage


def workflow_argument_options_payload() -> dict[str, object]:
    return {
        "contract_id": ARGUMENT_OPTION_CONTRACT_ID,
        "owner_surface": ARGUMENT_OPTION_OWNER_SURFACE,
        "options": [asdict(option) for option in WORKFLOW_ARGUMENT_OPTIONS],
        "execute_action_passthrough": True,
    }


__all__ = [
    "ARGUMENT_OPTION_CONTRACT_ID",
    "ARGUMENT_OPTION_OWNER_SURFACE",
    "DESCRIBE_ACTION_OPTION",
    "DESCRIBE_PACKAGE_SCRIPT_OPTION",
    "LIST_ACTIONS_OPTION",
    "WORKFLOW_ARGUMENT_OPTIONS",
    "WorkflowArgumentOption",
    "workflow_argument_option",
    "workflow_argument_option_usage",
    "workflow_argument_options_payload",
]
