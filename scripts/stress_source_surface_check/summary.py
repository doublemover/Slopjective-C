"""Summary payload creation for stress source-surface validation."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .constants import SUMMARY_CONTRACT_ID, SURFACE_PATH


def build_summary(
    *,
    surface: dict[str, Any],
    safety_policy: dict[str, Any],
    artifact_surface: dict[str, Any],
    safety_policy_path: Path,
    artifact_surface_path: Path,
    workflow_surface_path: Path,
    claim_gate_path: Path,
    checked_in_roots: list[str],
    required_actions: list[Any],
    family_summaries: list[dict[str, Any]],
    claim_summaries: list[dict[str, Any]],
) -> dict[str, Any]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_surface_contract": repo_rel(SURFACE_PATH),
        "runbook": surface["runbook"],
        "source_check_script": surface["source_check_script"],
        "safety_policy": repo_rel(safety_policy_path),
        "artifact_surface": repo_rel(artifact_surface_path),
        "workflow_surface": repo_rel(workflow_surface_path),
        "claim_gate": repo_rel(claim_gate_path),
        "checked_in_root_count": len(checked_in_roots),
        "required_guard_count": len(safety_policy["required_guards"]),
        "machine_owned_artifact_root_count": len(artifact_surface["machine_owned_artifact_roots"]),
        "machine_owned_report_root_count": len(artifact_surface["machine_owned_report_roots"]),
        "summary_report_count": len(artifact_surface["summary_reports"]),
        "required_action_count": len(required_actions),
        "family_summaries": family_summaries,
        "claim_summaries": claim_summaries,
    }
