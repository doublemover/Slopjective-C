"""Retired workflow entrypoint rejection contract."""

from __future__ import annotations

from typing import NoReturn

WORKFLOW_RETIRED_RUNNER_OWNER = "objc3c-workflow-retired-runner-entrypoint"
WORKFLOW_RETIRED_RUNNER_CONTRACT_ID = "objc3c-workflow-retired-runner-v1"
WORKFLOW_RETIRED_RUNNER_SURFACE = "scripts/objc3c_workflow/runner.py"
WORKFLOW_RETIRED_RUNNER_ERROR = (
    "error: scripts/objc3c_workflow/runner.py is not a public command surface; "
    "use `npm run objc3c -- <action>`."
)


def entrypoint_rejection_contract_payload() -> dict[str, object]:
    return {
        "contract_id": WORKFLOW_RETIRED_RUNNER_CONTRACT_ID,
        "owner": WORKFLOW_RETIRED_RUNNER_OWNER,
        "retired_surface": WORKFLOW_RETIRED_RUNNER_SURFACE,
        "public_surface": "npm run objc3c -- <action>",
        "retired_entrypoint_alternates_allowed": False,
    }


def reject_direct_runner() -> NoReturn:
    raise SystemExit(WORKFLOW_RETIRED_RUNNER_ERROR)


__all__ = [
    "WORKFLOW_RETIRED_RUNNER_CONTRACT_ID",
    "WORKFLOW_RETIRED_RUNNER_ERROR",
    "WORKFLOW_RETIRED_RUNNER_OWNER",
    "WORKFLOW_RETIRED_RUNNER_SURFACE",
    "entrypoint_rejection_contract_payload",
    "reject_direct_runner",
]
