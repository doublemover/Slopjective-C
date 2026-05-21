#!/usr/bin/env python3
"""Validate the integrated release-operations workflow and publication outputs."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file

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
    if workflow_surface.get("workflow_child_actions") != REQUIRED_STEPS:
        return fail("workflow child action contract drifted")
    if workflow_surface.get("integration_summary_contract") != "objc3c.release.operations.integration.summary.v1":
        return fail("integration summary contract drifted")
    if manifest_summary.get("status") != "PASS":
        return fail("update manifest summary did not pass")
    if publication_summary.get("status") != "PASS":
        return fail("release-operations publication summary did not pass")
    if manifest_summary.get("upgrade_support_report") != publication_summary.get("upgrade_support_report"):
        return fail("upgrade support report path drifted between manifest and publication")
    if manifest_summary.get("release_channel_manifest") != publication_summary.get("release_channel_manifest"):
        return fail("release channel manifest path drifted between manifest and publication")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.integration.summary.v1",
        "status": "PASS",
        "workflow_report": repo_rel(WORKFLOW_REPORT),
        "validated_steps": REQUIRED_STEPS,
        "update_manifest_path": manifest_summary.get("update_manifest_path"),
        "release_channel_manifest": publication_summary.get("release_channel_manifest"),
        "upgrade_support_report": publication_summary.get("upgrade_support_report"),
        "channel_catalog": publication_summary.get("channel_catalog"),
        "release_operations_owned_actions": workflow_surface.get("release_operations_owned_actions"),
    }
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
