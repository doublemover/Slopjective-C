"""Owned entrypoint dispatch policy for objc3c workflow CLIs."""

from __future__ import annotations

from collections.abc import Callable, Sequence

WORKFLOW_ENTRYPOINT_POLICY_OWNER = "objc3c-workflow-entrypoint-policy"
WORKFLOW_MODULE_ENTRYPOINT_OWNER = "objc3c-workflow-module-entrypoint"
WORKFLOW_SCRIPT_ENTRYPOINT_OWNER = "objc3c-workflow-script-entrypoint"
WORKFLOW_ENTRYPOINT_POLICY_CONTRACT_ID = "objc3c-workflow-entrypoint-policy-v1"


def entrypoint_policy_contract_payload() -> dict[str, object]:
    return {
        "contract_id": WORKFLOW_ENTRYPOINT_POLICY_CONTRACT_ID,
        "policy_owner": WORKFLOW_ENTRYPOINT_POLICY_OWNER,
        "module_entrypoint_owner": WORKFLOW_MODULE_ENTRYPOINT_OWNER,
        "script_entrypoint_owner": WORKFLOW_SCRIPT_ENTRYPOINT_OWNER,
        "dispatch_owner_surface": "scripts/objc3c_workflow/cli_dispatch.py",
        "argv_policy": "entrypoints forward explicit argv to the shared CLI dispatcher",
        "retired_entrypoint_alternates_allowed": False,
    }


def dispatch_entrypoint(
    dispatcher: Callable[[Sequence[str] | None], int],
    argv: Sequence[str] | None,
) -> int:
    return dispatcher(argv)


__all__ = [
    "WORKFLOW_ENTRYPOINT_POLICY_CONTRACT_ID",
    "WORKFLOW_ENTRYPOINT_POLICY_OWNER",
    "WORKFLOW_MODULE_ENTRYPOINT_OWNER",
    "WORKFLOW_SCRIPT_ENTRYPOINT_OWNER",
    "dispatch_entrypoint",
    "entrypoint_policy_contract_payload",
]
