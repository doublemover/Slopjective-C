#!/usr/bin/env python3
"""Validate objc3c release-operations metadata end to end."""

from __future__ import annotations

import json
import os
import hashlib
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_capture

UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
RELEASE_CHANNEL_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "channel-manifest" / "objc3c-release-channel-manifest.json"
UPGRADE_SUPPORT_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-upgrade-report.json"
RELEASE_NOTES = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-release-notes.json"
PUBLIC_CHANGELOG = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-public-changelog.json"
CHANNEL_OPERATIONS_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "channel_operations_model.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "end-to-end-summary.json"
PACKAGE_CHANNEL_FRESHNESS_TIMESTAMP_SOURCES = [
    "package_channels_summary.generated_at_utc",
    "package_channels_manifest.generated_at_utc",
    "platform_support_matrix.generated_at_utc",
]
GENERATED_OUTPUT_PREFIXES = ("tmp/", "artifacts/")






def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def normalize_repo_path(raw_path: Any, field_name: str) -> str:
    if not isinstance(raw_path, str) or not raw_path:
        raise RuntimeError(f"{field_name} must be a non-empty repo-relative path")
    normalized = raw_path.replace("\\", "/")
    if normalized != raw_path:
        raise RuntimeError(f"{field_name} must use slash-separated repo paths: {raw_path}")
    path = Path(normalized)
    if path.is_absolute() or ".." in path.parts:
        raise RuntimeError(f"{field_name} must be a repo-relative path: {raw_path}")
    return normalized


def is_generated_output_path(raw_path: str) -> bool:
    return raw_path.startswith(GENERATED_OUTPUT_PREFIXES)


def validate_checked_source_paths(paths: Any, field_name: str) -> list[str]:
    expect(isinstance(paths, list) and paths, f"{field_name} must be a non-empty source list")
    checked_paths: list[str] = []
    for index, raw_path in enumerate(paths):
        normalized = normalize_repo_path(raw_path, f"{field_name}[{index}]")
        expect(
            not is_generated_output_path(normalized),
            f"{field_name}[{index}] used generated output as source truth: {normalized}",
        )
        expect((ROOT / normalized).is_file(), f"{field_name}[{index}] missing checked source: {normalized}")
        checked_paths.append(normalized)
    return checked_paths


def validate_generated_evidence_paths(paths: Any, field_name: str) -> list[str]:
    expect(isinstance(paths, list) and paths, f"{field_name} must be a non-empty generated evidence list")
    generated_paths: list[str] = []
    for index, raw_path in enumerate(paths):
        normalized = normalize_repo_path(raw_path, f"{field_name}[{index}]")
        expect(
            is_generated_output_path(normalized),
            f"{field_name}[{index}] must be generated release evidence: {normalized}",
        )
        generated_paths.append(normalized)
    return generated_paths


def validate_release_evidence_source_truth(release_evidence: dict[str, Any]) -> dict[str, Any]:
    release_note_sources = validate_checked_source_paths(
        release_evidence.get("release_note_sources"),
        "release_evidence.release_note_sources",
    )
    public_changelog_sources = validate_checked_source_paths(
        release_evidence.get("public_changelog_sources"),
        "release_evidence.public_changelog_sources",
    )
    evidence_artifacts = validate_generated_evidence_paths(
        release_evidence.get("evidence_artifacts"),
        "release_evidence.evidence_artifacts",
    )
    return {
        "release_note_sources": release_note_sources,
        "public_changelog_sources": public_changelog_sources,
        "evidence_artifacts": evidence_artifacts,
    }


def validate_clean_install_prerequisites(channel_operations_model: dict[str, Any]) -> list[str]:
    channels = channel_operations_model.get("channels", [])
    expect(isinstance(channels, list) and channels, "channel operations model missing channels")
    channel_ids: list[str] = []
    for channel in channels:
        expect(isinstance(channel, dict), "channel operations model channel must be an object")
        channel_id = str(channel.get("channel_id", ""))
        expect(channel_id, "channel operations model channel missing channel_id")
        prerequisite = channel.get("clean_install_prerequisite")
        expect(isinstance(prerequisite, dict), f"{channel_id} channel missing clean install prerequisite")
        expect(
            prerequisite.get("required_action") == "validate-package-install-distribution",
            f"{channel_id} clean install prerequisite action drifted",
        )
        expect(
            prerequisite.get("required_flag") == "--from-nothing",
            f"{channel_id} clean install prerequisite flag drifted",
        )
        expect(
            prerequisite.get("required_summary")
            == "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json",
            f"{channel_id} clean install prerequisite summary drifted",
        )
        expect(
            prerequisite.get("blocks_publication_on_failure") is True,
            f"{channel_id} clean install prerequisite must block publication",
        )
        channel_ids.append(channel_id)
    return channel_ids


def validate_package_channel_freshness(
    release_channel_manifest: dict[str, Any],
) -> dict[str, Any]:
    channel_manifests = release_channel_manifest.get("channel_manifests", [])
    expect(isinstance(channel_manifests, list) and channel_manifests, "channel manifests missing")
    freshness_by_channel: dict[str, Any] = {}
    for channel in channel_manifests:
        expect(isinstance(channel, dict), "channel manifest must be an object")
        channel_id = str(channel.get("channel_id", ""))
        freshness = channel.get("package_channel_freshness")
        expect(isinstance(freshness, dict), f"{channel_id} missing package channel freshness")
        expect(
            freshness.get("timestamp_sources") == PACKAGE_CHANNEL_FRESHNESS_TIMESTAMP_SOURCES,
            f"{channel_id} freshness timestamp sources drifted",
        )
        expect(
            freshness.get("stale_behavior") == "fail-closed",
            f"{channel_id} freshness must fail closed",
        )
        expect(
            freshness.get("refresh_command") == "npm run objc3c -- build-package-channels",
            f"{channel_id} freshness refresh command drifted",
        )
        expect(
            freshness.get("blocks_publication_on_stale") is True,
            f"{channel_id} freshness must block stale publication",
        )
        skew = freshness.get("artifact_skew_hours")
        limits = freshness.get("channel_max_artifact_skew_hours")
        expect(isinstance(skew, (int, float)) and skew >= 0, f"{channel_id} freshness skew missing")
        expect(isinstance(limits, dict), f"{channel_id} freshness channel limits missing")
        expect(
            all(skew <= limit for limit in limits.values() if isinstance(limit, int)),
            f"{channel_id} package-channel freshness exceeded a channel limit",
        )
        freshness_by_channel[channel_id] = freshness
    return freshness_by_channel


def write_summary() -> None:
    update_manifest = load_json(UPDATE_MANIFEST)
    release_channel_manifest = load_json(RELEASE_CHANNEL_MANIFEST)
    upgrade_support_report = load_json(UPGRADE_SUPPORT_REPORT)
    release_notes = load_json(RELEASE_NOTES)
    public_changelog = load_json(PUBLIC_CHANGELOG)
    channel_operations_model = load_json(CHANNEL_OPERATIONS_MODEL)
    clean_install_prerequisite_channels = validate_clean_install_prerequisites(channel_operations_model)
    package_channel_freshness = validate_package_channel_freshness(release_channel_manifest)

    channel_ids = [entry.get("channel_id") for entry in update_manifest.get("channels", [])]
    expect(channel_ids == ["stable", "candidate", "nightly", "preview"], f"channel ids drifted: {channel_ids}")
    expect(update_manifest.get("default_channel") == "stable", "default channel drifted")
    expect(
        update_manifest.get("release_channel_manifest") == repo_rel(RELEASE_CHANNEL_MANIFEST),
        "update manifest release channel manifest link drifted",
    )
    expect(
        update_manifest.get("upgrade_support_report") == repo_rel(UPGRADE_SUPPORT_REPORT),
        "update manifest upgrade support report link drifted",
    )
    expect(
        release_channel_manifest.get("contract_id") == "objc3c.release.operations.channel-manifest.v1",
        "release channel manifest contract drifted",
    )
    expect(
        release_channel_manifest.get("required_channels") == ["stable", "nightly"],
        "release channel manifest required channels drifted",
    )
    expect(
        upgrade_support_report.get("contract_id") == "objc3c.release.operations.upgrade-support-report.v1",
        "upgrade support report contract drifted",
    )
    expect(
        upgrade_support_report.get("release_channel_manifest") == repo_rel(RELEASE_CHANNEL_MANIFEST),
        "upgrade support report release channel manifest link drifted",
    )
    expect(
        release_notes.get("contract_id") == "objc3c.release.operations.release-notes.v1",
        "release notes contract drifted",
    )
    expect(release_notes.get("source_mode") == "source-derived", "release notes source mode drifted")
    expect(
        release_notes.get("update_manifest") == repo_rel(UPDATE_MANIFEST),
        "release notes update manifest link drifted",
    )
    expect(
        release_notes.get("release_channel_manifest") == repo_rel(RELEASE_CHANNEL_MANIFEST),
        "release notes channel manifest link drifted",
    )
    expect(
        release_notes.get("source_model") == repo_rel(CHANNEL_OPERATIONS_MODEL),
        "release notes source model drifted",
    )
    expect(
        release_notes.get("release_note_sources")
        == release_channel_manifest["release_evidence"]["release_note_sources"],
        "release notes source list drifted from release evidence",
    )
    expect(
        not set(release_notes.get("forbidden_sources", []))
        & set(release_notes.get("release_note_sources", [])),
        "release notes included a forbidden source",
    )
    expect(
        len(release_notes.get("channels", [])) == len(update_manifest["channels"]),
        "release notes channel count drifted",
    )
    expect(
        public_changelog.get("contract_id") == "objc3c.release.operations.public-changelog.v1",
        "public changelog contract drifted",
    )
    expect(
        public_changelog.get("source_mode") == "source-derived",
        "public changelog source mode drifted",
    )
    expect(
        public_changelog.get("release_notes") == repo_rel(RELEASE_NOTES),
        "public changelog release notes link drifted",
    )
    expect(
        public_changelog.get("public_changelog_sources")
        == release_channel_manifest["release_evidence"]["public_changelog_sources"],
        "public changelog source list drifted from release evidence",
    )
    expect(
        len(public_changelog.get("entries", [])) == len(release_channel_manifest["channel_manifests"]),
        "public changelog entry count drifted",
    )
    expect(len(upgrade_support_report.get("revert_guidance", [])) >= 4, "revert guidance drifted")
    rollback_diagnostics = upgrade_support_report.get("rollback_diagnostics", [])
    expect(
        any(entry.get("user_facing_message") for entry in rollback_diagnostics if isinstance(entry, dict)),
        "rollback diagnostics omitted user-facing messages",
    )
    expect(len(upgrade_support_report.get("warnings", [])) >= 3, "upgrade warnings drifted")
    fail_closed = upgrade_support_report.get("fail_closed_diagnostics", [])
    expect(
        any(entry.get("blocks_publication") is True for entry in fail_closed if isinstance(entry, dict)),
        "fail-closed diagnostics omitted blocking publication rule",
    )

    stable = next(entry for entry in update_manifest["channels"] if entry["channel_id"] == "stable")
    channel_manifests = release_channel_manifest.get("channel_manifests", [])
    stable_channel_manifest = next(
        entry for entry in channel_manifests if entry["channel_id"] == "stable"
    )
    nightly_channel_manifest = next(
        entry for entry in channel_manifests if entry["channel_id"] == "nightly"
    )
    stable_gates = stable_channel_manifest["release_gate_actions"]
    nightly_gates = nightly_channel_manifest["release_gate_actions"]
    expect("validate-release-candidate-conformance" in stable_gates, "stable channel omitted release-candidate gate")
    expect("test-nightly" in nightly_gates, "nightly channel omitted nightly gate")
    expect(set(stable_gates) != set(nightly_gates), "stable and nightly gates collapsed")
    expect(
        nightly_channel_manifest["rollback_safety"]["rollback_channel"] == "offline-bundle",
        "nightly rollback channel drifted",
    )
    expect(
        stable_channel_manifest["rollback_safety"]["rollback_channel"] == "local-installer",
        "stable rollback channel drifted",
    )
    provenance = release_channel_manifest.get("local_provenance", {})
    expect(
        provenance.get("release_manifest_sha256")
        == sha256_file(ROOT / provenance["release_manifest"].replace("/", os.sep)),
        "release manifest provenance digest drifted",
    )
    release_evidence = release_channel_manifest.get("release_evidence", {})
    release_source_boundary = validate_release_evidence_source_truth(release_evidence)
    expect(
        repo_rel(RELEASE_CHANNEL_MANIFEST) in release_evidence.get("evidence_artifacts", []),
        "release evidence omitted channel manifest artifact",
    )
    for artifact_key in ("portable_archive", "installer_archive", "offline_archive"):
        artifact_rel = stable["artifacts"][artifact_key]
        artifact_path = ROOT / artifact_rel.replace("/", os.sep)
        expect(artifact_path.is_file(), f"missing stable artifact {artifact_rel}")
    installer_signature = stable["artifacts"].get("installer_signature", {})
    installer_path = ROOT / stable["artifacts"]["installer_archive"].replace("/", os.sep)
    expect(installer_signature.get("signature_format") == "objc3c-local-sha256-v1", "stable installer signature format drifted")
    expect(installer_signature.get("artifact") == stable["artifacts"]["installer_archive"], "stable installer signature artifact drifted")
    expect(installer_signature.get("sha256") == sha256_file(installer_path), "stable installer signature digest drifted")

    summary = {
        "contract_id": "objc3c.release.operations.end-to-end.summary.v1",
        "status": "PASS",
        "update_manifest": repo_rel(UPDATE_MANIFEST),
        "release_channel_manifest": repo_rel(RELEASE_CHANNEL_MANIFEST),
        "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT),
        "release_notes": repo_rel(RELEASE_NOTES),
        "public_changelog": repo_rel(PUBLIC_CHANGELOG),
        "channels": channel_ids,
        "stable_gate_actions": stable_gates,
        "nightly_gate_actions": nightly_gates,
        "stable_artifacts": stable["artifacts"],
        "clean_install_prerequisite_channels": clean_install_prerequisite_channels,
        "package_channel_freshness_channels": sorted(package_channel_freshness),
        "package_channel_freshness": next(iter(package_channel_freshness.values())),
        "fail_closed_diagnostic_count": len(fail_closed),
        "rollback_diagnostic_count": len(rollback_diagnostics),
        "release_note_channel_count": len(release_notes.get("channels", [])),
        "public_changelog_entry_count": len(public_changelog.get("entries", [])),
        "release_evidence_artifact_count": len(release_evidence.get("evidence_artifacts", [])),
        "release_source_truth_paths": sorted(
            set(release_source_boundary["release_note_sources"])
            | set(release_source_boundary["public_changelog_sources"])
        ),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")


def main(argv: Sequence[str] | None = None) -> int:
    args = list(sys.argv[1:] if argv is None else argv)
    skip_upstream = False
    if args == ["--skip-upstream"]:
        skip_upstream = True
    elif args:
        raise RuntimeError(f"unexpected arguments: {args}")

    if not skip_upstream:
        result = run_capture(public_workflow_command("validate-release-operations"), capture_output=False)
        if result.returncode != 0:
            raise RuntimeError("validate-release-operations failed")

    write_summary()
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
