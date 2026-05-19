from __future__ import annotations

from pathlib import Path
from typing import Any

from .blocker_contract import evaluate_contract_checks, parse_projection
from .config import (
    DASHBOARD_SCRIPT,
    RELEASE_BLOCKER_SCRIPT,
    RUNBOOK,
    SUMMARY_CONTRACT_ID,
)
from .input_loading import load_source_truth_inputs, read_json
from .tooling import repo_rel


def build_summary(policy_path: Path) -> dict[str, Any]:
    policy = read_json(policy_path)
    projection = parse_projection(policy)
    inputs = load_source_truth_inputs(
        policy_path=policy_path,
        release_blocker_script=RELEASE_BLOCKER_SCRIPT,
        dashboard_script=DASHBOARD_SCRIPT,
        runbook=RUNBOOK,
    )
    checks = evaluate_contract_checks(
        projection,
        release_blocker_text=inputs.release_blocker_text,
        dashboard_text=inputs.dashboard_text,
        runbook_text=inputs.runbook_text,
        tmp_source_truth_paths=inputs.tmp_source_truth_paths,
    )
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "source_policy_contract_id": policy.get("contract_id"),
        "source_policy_path": repo_rel(policy_path),
        "release_blocker_script": repo_rel(RELEASE_BLOCKER_SCRIPT),
        "dashboard_script": repo_rel(DASHBOARD_SCRIPT),
        "runbook": repo_rel(RUNBOOK),
        "dashboard_release_blocker_projection": projection.to_payload(),
        "source_truth_paths": inputs.source_truth_paths,
        "tmp_source_truth_paths": inputs.tmp_source_truth_paths,
        "checks": checks,
    }
