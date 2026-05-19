"""CLI dispatch owner for objc3c workflow entrypoints."""

from __future__ import annotations

from collections.abc import Sequence

from .cli_argv import CLI_ARGV_NORMALIZATION_CONTRACT_ID, workflow_argv
from .request_dispatch import parse_and_dispatch_workflow_request

CLI_DISPATCH_CONTRACT_ID = "objc3c-workflow-cli-dispatch-v1"
CLI_DISPATCH_OWNER_SURFACE = "scripts/objc3c_workflow/cli_dispatch.py"


def cli_dispatch_contract_payload() -> dict[str, object]:
    return {
        "contract_id": CLI_DISPATCH_CONTRACT_ID,
        "owner_surface": CLI_DISPATCH_OWNER_SURFACE,
        "argv_contract_id": CLI_ARGV_NORMALIZATION_CONTRACT_ID,
        "request_dispatch_owner_surface": "scripts/objc3c_workflow/request_dispatch.py",
        "dispatches_normalized_argv_only": True,
        "retired_direct_action_dispatch_allowed": False,
    }


def dispatch_cli(argv: Sequence[str] | None = None) -> int:
    return parse_and_dispatch_workflow_request(workflow_argv(argv))


__all__ = [
    "CLI_DISPATCH_CONTRACT_ID",
    "CLI_DISPATCH_OWNER_SURFACE",
    "cli_dispatch_contract_payload",
    "dispatch_cli",
]
