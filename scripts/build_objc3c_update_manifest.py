#!/usr/bin/env python3
"""Build the machine-owned objc3c update manifest from live release artifacts."""

from __future__ import annotations

import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "source_surface.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPGRADE_PATH_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_path_surface.json"
UPDATE_CHANNEL_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "update_channel_policy.json"
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "metadata_surface.json"
PACKAGE_CHANNELS_BUILD = ROOT / "scripts" / "build_objc3c_package_channels.py"
PLATFORM_SUPPORT_MATRIX_BUILD = ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
REPORT_PATH = ROOT / "tmp" / "reports" / "release-operations" / "update-manifest-summary.json"
MANIFEST_PATH = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
PACKAGE_CHANNELS_SUMMARY = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
RELEASE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
PLATFORM_SUPPORT_MATRIX = ROOT / "tmp" / "artifacts" / "platform-hardening" / "objc3c-platform-support-matrix.json"
PLATFORM_SUPPORT_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "platform-support-matrix-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run(command: list[str]) -> None:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(command, cwd=ROOT, env=env, text=True, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def main() -> int:
    load_json(SOURCE_SURFACE)
    versioning_model = load_json(VERSIONING_MODEL)
    upgrade_surface = load_json(UPGRADE_PATH_SURFACE)
    update_channel_policy = load_json(UPDATE_CHANNEL_POLICY)
    metadata_surface = load_json(METADATA_SURFACE)

    if not PACKAGE_CHANNELS_SUMMARY.is_file() or not RELEASE_MANIFEST.is_file():
        run([sys.executable, str(PACKAGE_CHANNELS_BUILD)])
    if not PLATFORM_SUPPORT_MATRIX.is_file():
        run([sys.executable, str(PLATFORM_SUPPORT_MATRIX_BUILD)])

    package_channels_summary = load_json(PACKAGE_CHANNELS_SUMMARY)
    release_manifest = load_json(RELEASE_MANIFEST)
    platform_support_matrix = load_json(PLATFORM_SUPPORT_MATRIX)

    compatibility_report_path = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-compatibility-report.json"
    artifacts = {
        "portable_archive": package_channels_summary["portable_archive"],
        "installer_archive": package_channels_summary["installer_archive"],
        "offline_archive": package_channels_summary["offline_archive"],
        "package_channels_manifest": package_channels_summary["manifest_path"],
        "release_manifest": repo_rel(RELEASE_MANIFEST),
        "platform_support_matrix": repo_rel(PLATFORM_SUPPORT_MATRIX),
    }

    channels = []
    for channel_id in update_channel_policy["channel_order"]:
        channel_policy = next(entry for entry in update_channel_policy["channels"] if entry["channel_id"] == channel_id)
        version = versioning_model[f"current_{channel_id}_version"]
        channels.append({
            "channel_id": channel_id,
            "version": version,
            "support_status": channel_policy["support_status"],
            "warning_classes": channel_policy["warning_classes"],
            "upgrade_targets": channel_policy["permitted_upgrade_targets"],
            "artifacts": artifacts,
        })

    payload = {
        "contract_id": "objc3c.release.operations.update-manifest.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "current_version": versioning_model["current_stable_version"],
        "default_channel": update_channel_policy["default_channel"],
        "default_platform_id": platform_support_matrix["default_platform_id"],
        "supported_major_line": versioning_model["supported_major_line"],
        "package_model": release_manifest["package_model"],
        "platform_support_matrix": repo_rel(PLATFORM_SUPPORT_MATRIX),
        "platform_support_summary": repo_rel(PLATFORM_SUPPORT_SUMMARY),
        "supported_platform_ids": platform_support_matrix["claim_boundary"]["supported_platform_ids"],
        "support_tiers": platform_support_matrix["tiers"],
        "channels": channels,
        "upgrade_paths": upgrade_surface["upgrade_path_classes"],
        "compatibility_report": repo_rel(compatibility_report_path),
    }
    for field_name in metadata_surface["required_update_manifest_fields"]:
        if field_name not in payload:
            raise RuntimeError(f"update manifest missing required field {field_name}")

    MANIFEST_PATH.parent.mkdir(parents=True, exist_ok=True)
    MANIFEST_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.update-manifest.summary.v1",
        "status": "PASS",
        "update_manifest_path": repo_rel(MANIFEST_PATH),
        "channel_count": len(channels),
        "default_channel": payload["default_channel"],
        "default_platform_id": payload["default_platform_id"],
        "platform_support_matrix": payload["platform_support_matrix"],
        "package_channels_manifest": package_channels_summary["manifest_path"],
        "release_manifest": repo_rel(RELEASE_MANIFEST),
    }
    REPORT_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-update-manifest: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
