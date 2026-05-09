#!/usr/bin/env python3
"""Build the machine-owned objc3c update manifest from live release artifacts."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "source_surface.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPGRADE_PATH_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_path_surface.json"
UPDATE_CHANNEL_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "update_channel_policy.json"
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "metadata_surface.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "release-operations" / "update-manifest-summary.json"
MANIFEST_PATH = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
PACKAGE_CHANNELS_SUMMARY = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
RELEASE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
PLATFORM_SUPPORT_MATRIX = ROOT / "tmp" / "artifacts" / "platform-hardening" / "objc3c-platform-support-matrix.json"
PLATFORM_SUPPORT_SUMMARY = ROOT / "tmp" / "reports" / "platform-hardening" / "platform-support-matrix-summary.json"
UPGRADE_SUPPORT_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-upgrade-support-report.json"


def require_file(path: Path, owner_action: str) -> None:
    if not path.is_file():
        raise RuntimeError(
            f"required upstream artifact missing: {repo_rel(path)}; run owner action {owner_action}"
        )


def main() -> int:
    load_json(SOURCE_SURFACE)
    versioning_model = load_json(VERSIONING_MODEL)
    upgrade_surface = load_json(UPGRADE_PATH_SURFACE)
    update_channel_policy = load_json(UPDATE_CHANNEL_POLICY)
    metadata_surface = load_json(METADATA_SURFACE)

    require_file(PACKAGE_CHANNELS_SUMMARY, "build-package-channels")
    require_file(RELEASE_MANIFEST, "build-release-manifest")
    require_file(PLATFORM_SUPPORT_MATRIX, "build-platform-support-matrix")
    require_file(PLATFORM_SUPPORT_SUMMARY, "build-platform-support-matrix")

    package_channels_summary = load_json(PACKAGE_CHANNELS_SUMMARY)
    release_manifest = load_json(RELEASE_MANIFEST)
    platform_support_matrix = load_json(PLATFORM_SUPPORT_MATRIX)

    artifacts = {
        "portable_archive": package_channels_summary["portable_archive"],
        "installer_archive": package_channels_summary["installer_archive"],
        "offline_archive": package_channels_summary["offline_archive"],
        "package_channels_manifest": package_channels_summary["manifest_path"],
        "release_manifest": repo_rel(RELEASE_MANIFEST),
        "platform_support_matrix": repo_rel(PLATFORM_SUPPORT_MATRIX),
    }

    channel_order = update_channel_policy["channel_order"]
    channel_by_id = {
        entry["channel_id"]: entry
        for entry in update_channel_policy["channels"]
    }
    if sorted(channel_order) != sorted(channel_by_id):
        raise RuntimeError("update channel policy channel_order drifted from channel definitions")
    if update_channel_policy["default_channel"] not in channel_by_id:
        raise RuntimeError("default update channel is not declared in channel policy")

    channels = []
    for channel_id in channel_order:
        channel_policy = channel_by_id[channel_id]
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
        "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT),
    }
    for field_name in metadata_surface["required_update_manifest_fields"]:
        if field_name not in payload:
            raise RuntimeError(f"update manifest missing required field {field_name}")

    MANIFEST_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(MANIFEST_PATH, payload)

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.update-manifest.summary.v1",
        "status": "PASS",
        "update_manifest_path": repo_rel(MANIFEST_PATH),
        "channel_count": len(channels),
        "default_channel": payload["default_channel"],
        "default_platform_id": payload["default_platform_id"],
        "platform_support_matrix": payload["platform_support_matrix"],
        "platform_support_summary": payload["platform_support_summary"],
        "package_channels_manifest": package_channels_summary["manifest_path"],
        "release_manifest": repo_rel(RELEASE_MANIFEST),
        "upgrade_support_report": payload["upgrade_support_report"],
        "upstream_artifacts": [
            repo_rel(PACKAGE_CHANNELS_SUMMARY),
            repo_rel(RELEASE_MANIFEST),
            repo_rel(PLATFORM_SUPPORT_MATRIX),
            repo_rel(PLATFORM_SUPPORT_SUMMARY),
        ],
    }
    write_json_file(REPORT_PATH, summary)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-update-manifest: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
