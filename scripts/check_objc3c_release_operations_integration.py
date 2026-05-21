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
CHANNEL_OPERATIONS_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "channel_operations_model.json"
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
PACKAGE_CHANNEL_FRESHNESS = {
    "timestamp_sources": [
        "package_channels_summary.generated_at_utc",
        "package_channels_manifest.generated_at_utc",
        "platform_support_matrix.generated_at_utc",
    ],
    "max_artifact_skew_hours": 6,
    "stale_behavior": "fail-closed",
    "refresh_command": "npm run objc3c -- build-package-channels",
    "blocks_publication_on_stale": True,
}


def fail(message: str) -> int:
    print(f"objc3c-release-operations-integration: {message}", file=sys.stderr)
    return 1


def clean_install_prerequisite_channels(channel_operations_model: dict[str, Any]) -> list[str]:
    channels = channel_operations_model.get("channels", [])
    if not isinstance(channels, list) or not channels:
        raise RuntimeError("channel operations model missing channels")
    channel_ids: list[str] = []
    for channel in channels:
        if not isinstance(channel, dict):
            raise RuntimeError("channel operations model channel must be an object")
        channel_id = str(channel.get("channel_id", ""))
        prerequisite = channel.get("clean_install_prerequisite")
        if not isinstance(prerequisite, dict):
            raise RuntimeError(f"{channel_id} channel missing clean install prerequisite")
        expected = {
            "required_action": "validate-package-install-distribution",
            "required_flag": "--from-nothing",
            "required_summary": "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json",
            "blocks_publication_on_failure": True,
        }
        if prerequisite != expected:
            raise RuntimeError(f"{channel_id} clean install prerequisite drifted")
        if channel.get("package_channel_freshness") != PACKAGE_CHANNEL_FRESHNESS:
            raise RuntimeError(f"{channel_id} package freshness policy drifted")
        channel_ids.append(channel_id)
    return channel_ids




def main(argv: list[str] | None = None) -> int:
    args = list(sys.argv[1:] if argv is None else argv)
    skip_workflow_report = False
    if args == ["--skip-workflow-report"]:
        skip_workflow_report = True
    elif args:
        raise RuntimeError(f"unexpected arguments: {args}")

    required_paths = [WORKFLOW_SURFACE, CHANNEL_OPERATIONS_MODEL, MANIFEST_SUMMARY, PUBLICATION_SUMMARY]
    if not skip_workflow_report:
        required_paths.insert(0, WORKFLOW_REPORT)
    for path in required_paths:
        if not path.is_file():
            return fail(f"missing required artifact {repo_rel(path)}")

    workflow_report = load_json(WORKFLOW_REPORT) if not skip_workflow_report else {}
    workflow_surface = load_json(WORKFLOW_SURFACE)
    channel_operations_model = load_json(CHANNEL_OPERATIONS_MODEL)
    manifest_summary = load_json(MANIFEST_SUMMARY)
    publication_summary = load_json(PUBLICATION_SUMMARY)

    if not skip_workflow_report and workflow_report.get("status") != "PASS":
        return fail("workflow report did not pass")
    if not skip_workflow_report:
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
    try:
        clean_prerequisite_channels = clean_install_prerequisite_channels(channel_operations_model)
    except RuntimeError as exc:
        return fail(str(exc))

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.integration.summary.v1",
        "status": "PASS",
        "workflow_report": repo_rel(WORKFLOW_REPORT) if not skip_workflow_report else None,
        "workflow_report_required": not skip_workflow_report,
        "validated_steps": REQUIRED_STEPS if not skip_workflow_report else [],
        "validated_source_artifacts": [repo_rel(path) for path in required_paths],
        "update_manifest_path": manifest_summary.get("update_manifest_path"),
        "release_channel_manifest": publication_summary.get("release_channel_manifest"),
        "upgrade_support_report": publication_summary.get("upgrade_support_report"),
        "channel_catalog": publication_summary.get("channel_catalog"),
        "clean_install_prerequisite_channels": clean_prerequisite_channels,
        "package_channel_freshness": PACKAGE_CHANNEL_FRESHNESS,
        "release_operations_owned_actions": workflow_surface.get("release_operations_owned_actions"),
    }
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
