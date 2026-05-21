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
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_capture

ROOT = Path(__file__).resolve().parents[1]
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
RELEASE_CHANNEL_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "channel-manifest" / "objc3c-release-channel-manifest.json"
UPGRADE_SUPPORT_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-upgrade-report.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "end-to-end-summary.json"






def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def write_summary() -> None:
    update_manifest = load_json(UPDATE_MANIFEST)
    release_channel_manifest = load_json(RELEASE_CHANNEL_MANIFEST)
    upgrade_support_report = load_json(UPGRADE_SUPPORT_REPORT)

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
        "channels": channel_ids,
        "stable_gate_actions": stable_gates,
        "nightly_gate_actions": nightly_gates,
        "stable_artifacts": stable["artifacts"],
        "fail_closed_diagnostic_count": len(fail_closed),
        "rollback_diagnostic_count": len(rollback_diagnostics),
        "release_evidence_artifact_count": len(release_evidence.get("evidence_artifacts", [])),
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
