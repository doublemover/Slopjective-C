from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_POLICY = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "full_envelope_claimability"
    / "release_blocker_rollout_policy.json"
)
DEFAULT_JSON_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "dashboard-release-blockers"
    / "dashboard_release_blocker_contract_summary.json"
)
DEFAULT_MD_OUT = (
    ROOT
    / "reports"
    / "claimability"
    / "dashboard-release-blockers"
    / "dashboard_release_blocker_contract_summary.md"
)
RUNBOOK = ROOT / "docs" / "runbooks" / "objc3c_full_envelope_claimability.md"
RELEASE_BLOCKER_SCRIPT = (
    ROOT / "scripts" / "build_full_envelope_claimability_release_blocker_summary.py"
)
DASHBOARD_SCRIPT = ROOT / "scripts" / "build_full_envelope_claimability_dashboard.py"
SUMMARY_CONTRACT_ID = "objc3c.claimability.dashboard.release_blocker.contract.summary.v1"


def resolve_repo_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path
