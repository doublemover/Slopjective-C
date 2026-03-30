#!/usr/bin/env python3
"""Validate the integrated distribution-credibility workflow and publication outputs."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-distribution-credibility.json"
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "workflow_surface.json"
DASHBOARD_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "dashboard-summary.json"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "publication-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "integration-summary.json"

REQUIRED_STEPS = [
    "validate-release-operations",
    "check-distribution-credibility-surface",
    "check-distribution-credibility-schema-surface",
    "build-distribution-credibility-dashboard",
    "publish-distribution-credibility",
]


def fail(message: str) -> int:
    print(f"objc3c-distribution-credibility-integration: {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{path} did not contain a JSON object")
    return payload


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def main() -> int:
    for path in (WORKFLOW_REPORT, WORKFLOW_SURFACE, DASHBOARD_SUMMARY, PUBLICATION_SUMMARY):
        if not path.is_file():
            return fail(f"missing required artifact {repo_rel(path)}")

    workflow_report = load_json(WORKFLOW_REPORT)
    workflow_surface = load_json(WORKFLOW_SURFACE)
    dashboard_summary = load_json(DASHBOARD_SUMMARY)
    publication_summary = load_json(PUBLICATION_SUMMARY)

    if workflow_report.get("status") != "PASS":
        return fail("workflow report did not pass")
    steps = workflow_report.get("steps")
    if not isinstance(steps, list):
        return fail("workflow report was missing steps")
    step_actions = [step.get("action") for step in steps if isinstance(step, dict)]
    if step_actions != REQUIRED_STEPS:
        return fail(f"workflow steps drifted: {step_actions}")
    if workflow_surface.get("validate_action") != "validate-distribution-credibility":
        return fail("workflow surface drifted from validate-distribution-credibility")
    if dashboard_summary.get("status") != "PASS":
        return fail("dashboard summary did not pass")
    if publication_summary.get("status") != "PASS":
        return fail("distribution-credibility publication summary did not pass")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.distribution.credibility.integration.summary.v1",
        "status": "PASS",
        "workflow_report": repo_rel(WORKFLOW_REPORT),
        "validated_steps": REQUIRED_STEPS,
        "dashboard_summary": repo_rel(DASHBOARD_SUMMARY),
        "publication_summary": repo_rel(PUBLICATION_SUMMARY),
        "trust_state": dashboard_summary.get("trust_state"),
        "trust_report_json": publication_summary.get("trust_report_json"),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-distribution-credibility-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
