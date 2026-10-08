"""Summary payload construction for the source-surface checker."""

from __future__ import annotations

from collections.abc import Mapping, Sequence
from pathlib import Path

from objc3c_tooling.paths import repo_rel


def build_summary_payload(
    *,
    summary_contract_id: str,
    source_surface: Path,
    expected_runbook: str,
    expected_required_paths: Mapping[str, str],
    checked_in_sources: Sequence[str],
    expected_checked_in_roots: Sequence[str],
    owner_split: dict[str, list[str]],
    build_scripts: Sequence[str],
    upstream_reports: Sequence[str],
    machine_owned_output_roots: Sequence[str],
    explicit_non_goals: Sequence[str],
    checked_paths: Sequence[str],
) -> dict[str, object]:
    unique_checked_paths = sorted(set(checked_paths))
    return {
        "contract_id": summary_contract_id,
        "status": "PASS",
        "source_surface": repo_rel(source_surface),
        "runbook": expected_runbook,
        "budget_model": expected_required_paths["budget_model"],
        "claim_policy": expected_required_paths["claim_policy"],
        "breach_triage_policy": expected_required_paths["breach_triage_policy"],
        "lab_policy": expected_required_paths["lab_policy"],
        "waiver_registry": expected_required_paths["waiver_registry"],
        "workflow_surface": expected_required_paths["workflow_surface"],
        "schema_surface": expected_required_paths["schema_surface"],
        "optimization_runtime_debug_safety_contract": expected_required_paths[
            "optimization_runtime_debug_safety_contract"
        ],
        "checked_in_sources": list(checked_in_sources),
        "checked_in_roots": list(expected_checked_in_roots),
        "owner_split": owner_split,
        "build_scripts": list(build_scripts),
        "upstream_reports": list(upstream_reports),
        "machine_owned_output_roots": list(machine_owned_output_roots),
        "explicit_non_goals": list(explicit_non_goals),
        "checked_path_count": len(unique_checked_paths),
        "checked_paths": unique_checked_paths,
    }
