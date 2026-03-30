#!/usr/bin/env python3
"""Validate the integrated release-operations workflow and publication outputs."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-release-operations.json"
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "workflow_surface.json"
MANIFEST_SUMMARY = ROOT / "tmp" / "reports" / "release-operations" / "update-manifest-summary.json"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "release-operations" / "publication-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "integration-summary.json"

REQUIRED_STEPS = [
    "validate-packaging-channels",
    "check-release-operations-surface",
    "check-release-operations-schema-surface",
    "build-update-manifest",
    "publish-release-operations",
]


def fail(message: str) -> int:
    print(f"objc3c-release-operations-integration: {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{path} did not contain a JSON object")
    return payload


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def main() -> int:
    for path in (WORKFLOW_REPORT, WORKFLOW_SURFACE, MANIFEST_SUMMARY, PUBLICATION_SUMMARY):
        if not path.is_file():
            return fail(f"missing required artifact {repo_rel(path)}")

    workflow_report = load_json(WORKFLOW_REPORT)
    workflow_surface = load_json(WORKFLOW_SURFACE)
    manifest_summary = load_json(MANIFEST_SUMMARY)
    publication_summary = load_json(PUBLICATION_SUMMARY)

    if workflow_report.get("status") != "PASS":
        return fail("workflow report did not pass")
    steps = workflow_report.get("steps")
    if not isinstance(steps, list):
        return fail("workflow report was missing steps")
    step_actions = [step.get("action") for step in steps if isinstance(step, dict)]
    if step_actions != REQUIRED_STEPS:
        return fail(f"workflow steps drifted: {step_actions}")
    if workflow_surface.get("validate_action") != "validate-release-operations":
        return fail("workflow surface drifted from validate-release-operations")
    if manifest_summary.get("status") != "PASS":
        return fail("update manifest summary did not pass")
    if publication_summary.get("status") != "PASS":
        return fail("release-operations publication summary did not pass")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.integration.summary.v1",
        "status": "PASS",
        "workflow_report": repo_rel(WORKFLOW_REPORT),
        "validated_steps": REQUIRED_STEPS,
        "update_manifest_path": manifest_summary.get("update_manifest_path"),
        "compatibility_report": publication_summary.get("compatibility_report"),
        "channel_catalog": publication_summary.get("channel_catalog"),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
