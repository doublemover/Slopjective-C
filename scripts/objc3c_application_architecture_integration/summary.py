"""Summary payload construction for application architecture integration."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .contracts import (
    CHILD_SUMMARY_PATHS,
    INTEGRATION_CONTRACT_ID,
    RUNNER_PATH,
    WORKFLOW_ACTIONS,
)
from .execution import summary_passes


def build_step_result(
    *,
    name: str,
    command: list[str],
    summary_path: Path,
    summary: dict[str, Any],
) -> dict[str, Any]:
    return {
        "name": name,
        "command": command,
        "summary_path": repo_rel(summary_path),
        "mode": "executed",
        "summary_ok": summary_passes(summary) if isinstance(summary, dict) else False,
        "summary_status": (
            summary.get("status", summary.get("ok")) if isinstance(summary, dict) else None
        ),
    }


def build_integration_payload(
    *,
    failures: list[str],
    step_results: list[dict[str, Any]],
) -> dict[str, Any]:
    return {
        "contract_id": INTEGRATION_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": RUNNER_PATH,
        "workflow_actions": list(WORKFLOW_ACTIONS),
        "child_report_paths": [repo_rel(summary_path) for summary_path in CHILD_SUMMARY_PATHS],
        "steps": step_results,
        "failures": failures,
    }
