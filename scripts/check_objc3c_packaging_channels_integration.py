#!/usr/bin/env python3
"""Validate the integrated objc3c packaging-channels workflow report."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file

ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "workflow_surface.json"
PUBLIC_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-packaging-channels.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "integration-summary.json"
EXPECTED_STEP_ORDER = [
    "validate-release-foundation",
    "check-packaging-channels-surface",
    "check-packaging-channels-schema-surface",
    "build-package-channels",
]




def main() -> int:
    workflow_surface = load_json(WORKFLOW_SURFACE)
    owner_policy = workflow_surface.get("owner_policy")
    if not isinstance(owner_policy, dict) or owner_policy.get("evidence_log_allowed") is not False:
        raise RuntimeError("packaging-channels workflow surface missing source-owned owner_policy")
    blocker_metadata = workflow_surface.get("blocker_metadata")
    if not isinstance(blocker_metadata, dict) or blocker_metadata.get("blocker_owner") != "packaging-channels-blockers":
        raise RuntimeError("packaging-channels workflow surface missing blocker metadata")
    report = load_json(PUBLIC_REPORT)
    if report.get("action") != workflow_surface.get("validate_action"):
        raise RuntimeError("public packaging-channels report drifted from workflow_surface validate_action")
    steps = report.get("steps")
    if not isinstance(steps, list):
        raise RuntimeError("public packaging-channels report did not publish steps")
    step_actions = [step.get("action") for step in steps if isinstance(step, dict)]
    if step_actions != EXPECTED_STEP_ORDER:
        raise RuntimeError(f"packaging-channels step order drifted: {step_actions!r}")

    summary = {
        "contract_id": "objc3c.packaging.channels.integration.summary.v1",
        "status": "PASS",
        "public_workflow_report": repo_rel(PUBLIC_REPORT),
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "step_order": step_actions,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-packaging-channels-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
