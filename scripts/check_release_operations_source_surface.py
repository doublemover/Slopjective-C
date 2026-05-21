#!/usr/bin/env python3
"""Validate the checked-in release-operations source surface."""

from __future__ import annotations

import sys
from pathlib import Path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "source-surface-summary.json"
SURFACE_CONTRACT_ID = "objc3c.release.operations.source.surface.v1"
SUMMARY_CONTRACT_ID = "objc3c.release.operations.source.surface.summary.v1"

EXPECTED_CONTRACT_IDS = {
    "versioning_model": "objc3c.release.operations.versioning.model.v1",
    "upgrade_path_surface": "objc3c.release.operations.upgrade.path.surface.v1",
    "channel_operations_model": "objc3c.release.operations.channel.operations.model.v1",
    "upgrade_claim_policy": "objc3c.release.operations.upgrade.claim.policy.v1",
    "update_channel_policy": "objc3c.release.operations.update.channel.policy.v1",
    "fail_closed_diagnostics_policy": "objc3c.release.operations.fail_closed.diagnostics.policy.v1",
    "metadata_surface": "objc3c.release.operations.metadata.surface.v1",
    "schema_surface": "objc3c.release.operations.schema.surface.v1",
    "workflow_surface": "objc3c.release.operations.workflow.surface.v1",
}

RELEASE_OPERATIONS_ACTIONS = [
    "check-release-operations-surface",
    "check-release-operations-schema-surface",
    "build-update-manifest",
    "publish-release-operations",
    "validate-release-operations",
    "validate-release-operations-end-to-end",
]
CLEAN_INSTALL_PREREQUISITE = {
    "required_action": "validate-package-install-distribution",
    "required_flag": "--from-nothing",
    "required_summary": "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json",
    "blocks_publication_on_failure": True,
}
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
    print(f"release-operations-source-surface: {message}", file=sys.stderr)
    return 1


def clean_install_prerequisite_failures(channel_operations_model: dict) -> list[str]:
    failures: list[str] = []
    channels = channel_operations_model.get("channels")
    if not isinstance(channels, list) or not channels:
        return ["channel operations model must declare channels"]
    for channel in channels:
        if not isinstance(channel, dict):
            failures.append("channel operations model contained a non-object channel")
            continue
        channel_id = str(channel.get("channel_id", ""))
        prerequisite = channel.get("clean_install_prerequisite")
        if prerequisite != CLEAN_INSTALL_PREREQUISITE:
            failures.append(f"{channel_id} channel clean install prerequisite drifted")
        freshness = channel.get("package_channel_freshness")
        if freshness != PACKAGE_CHANNEL_FRESHNESS:
            failures.append(f"{channel_id} channel package freshness policy drifted")
    return failures




def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface {repo_rel(SOURCE_SURFACE)}")
    source_surface = load_json(SOURCE_SURFACE)
    if source_surface.get("contract_id") != SURFACE_CONTRACT_ID:
        return fail("unexpected source surface contract_id")
    if source_surface.get("surface_kind") != "release-operations-source-surface":
        return fail("unexpected source surface kind")
    if source_surface.get("schema_version") != 1:
        return fail("unexpected source surface schema_version")

    checked_paths: list[str] = [repo_rel(SOURCE_SURFACE)]
    for field_name, expected_contract_id in EXPECTED_CONTRACT_IDS.items():
        raw_path = source_surface.get(field_name)
        if not isinstance(raw_path, str) or not raw_path:
            return fail(f"{field_name} was missing from the source surface")
        target = ROOT / raw_path
        if not target.is_file():
            return fail(f"{field_name} referenced missing file {raw_path}")
        payload = load_json(target)
        if payload.get("contract_id") != expected_contract_id:
            return fail(f"{field_name} drifted from expected contract id {expected_contract_id}")
        checked_paths.append(raw_path)

    runbook = source_surface.get("runbook")
    if not isinstance(runbook, str) or not runbook:
        return fail("runbook was missing from the source surface")
    runbook_path = ROOT / runbook
    if not runbook_path.is_file():
        return fail(f"runbook referenced missing file {runbook}")
    checked_paths.append(runbook)

    for list_name in ("checked_in_sources", "machine_owned_output_roots", "explicit_non_goals"):
        items = source_surface.get(list_name)
        if not isinstance(items, list) or not items:
            return fail(f"{list_name} must be a non-empty list")
        if list_name == "machine_owned_output_roots" or list_name == "explicit_non_goals":
            continue
        for raw_path in items:
            if not isinstance(raw_path, str) or not raw_path:
                return fail(f"{list_name} contained an invalid path entry")
            target = ROOT / raw_path
            if not target.exists():
                return fail(f"{list_name} referenced missing path {raw_path}")
            checked_paths.append(raw_path)

    owned_actions = source_surface.get("release_operations_owned_actions")
    if owned_actions != RELEASE_OPERATIONS_ACTIONS:
        return fail("release_operations_owned_actions drifted from public action contract")
    public_actions = source_surface.get("public_actions")
    if not isinstance(public_actions, list) or not set(RELEASE_OPERATIONS_ACTIONS).issubset(public_actions):
        return fail("public_actions omitted a release-operations action")
    workflow_surface = load_json(ROOT / source_surface["workflow_surface"])
    if workflow_surface.get("release_operations_owned_actions") != RELEASE_OPERATIONS_ACTIONS:
        return fail("workflow surface release-operations owner split drifted")
    channel_operations_model = load_json(ROOT / source_surface["channel_operations_model"])
    prerequisite_failures = clean_install_prerequisite_failures(channel_operations_model)
    if prerequisite_failures:
        return fail("; ".join(prerequisite_failures))
    hard_cutover_policy = source_surface.get("hard_cutover_policy")
    if not isinstance(hard_cutover_policy, dict):
        return fail("hard_cutover_policy was missing")
    if hard_cutover_policy.get("missing_artifact_behavior") != "fail-closed":
        return fail("release operations missing-artifact behavior must be fail-closed")
    if hard_cutover_policy.get("public_action_names") != "preserved":
        return fail("public action name preservation policy drifted")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
        "release_operations_owned_actions": RELEASE_OPERATIONS_ACTIONS,
        "clean_install_prerequisite": CLEAN_INSTALL_PREREQUISITE,
        "package_channel_freshness": PACKAGE_CHANNEL_FRESHNESS,
        "missing_artifact_behavior": hard_cutover_policy["missing_artifact_behavior"],
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-operations-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
