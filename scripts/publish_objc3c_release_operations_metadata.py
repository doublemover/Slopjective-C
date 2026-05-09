#!/usr/bin/env python3
"""Publish release-operations metadata from the live update manifest and policies."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file

ROOT = Path(__file__).resolve().parents[1]
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPGRADE_PATH_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_path_surface.json"
UPGRADE_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "compatibility_claim_policy.json"
UPDATE_CHANNEL_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "update_channel_policy.json"
FAIL_CLOSED_DIAGNOSTICS_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "fallback_diagnostics_policy.json"
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "metadata_surface.json"
UPGRADE_SUPPORT_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-upgrade-support-report.json"
CHANNEL_CATALOG = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-release-channel-catalog.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "publication-summary.json"




def main() -> int:
    update_manifest = load_json(UPDATE_MANIFEST)
    versioning_model = load_json(VERSIONING_MODEL)
    upgrade_surface = load_json(UPGRADE_PATH_SURFACE)
    claim_policy = load_json(UPGRADE_CLAIM_POLICY)
    update_channel_policy = load_json(UPDATE_CHANNEL_POLICY)
    fail_closed_policy = load_json(FAIL_CLOSED_DIAGNOSTICS_POLICY)
    metadata_surface = load_json(METADATA_SURFACE)

    warning_index = {
        entry["warning_id"]: entry["severity"]
        for entry in update_channel_policy["warning_classes"]
    }
    warnings = []
    for channel in update_manifest["channels"]:
        for warning_id in channel.get("warning_classes", []):
            warnings.append({
                "channel_id": channel["channel_id"],
                "warning_id": warning_id,
                "severity": warning_index.get(warning_id, "warn"),
                "message": f"{channel['channel_id']} channel emitted release-operations warning {warning_id}",
            })
    for diagnostic in fail_closed_policy["diagnostic_classes"]:
        warnings.append({
            "channel_id": "policy",
            "warning_id": diagnostic["diagnostic_id"],
            "severity": diagnostic["severity"],
            "message": diagnostic["trigger"],
            "required_action": diagnostic["required_action"],
            "blocks_publication": diagnostic["blocks_publication"],
        })

    revert_guidance = [
        {
            "channel_id": "stable",
            "instruction": "Reinstall the current same-major stable toolchain with the local installer.",
            "requires_backup": false,
        },
        {
            "channel_id": "candidate",
            "instruction": "Revert candidate drills through the local installer receipt path.",
            "requires_backup": false,
        },
        {
            "channel_id": "preview",
            "instruction": "Revert preview drills from the offline bundle and preserve pre-upgrade inputs.",
            "requires_backup": true,
        },
    ]

    upgrade_support_report = {
        "contract_id": "objc3c.release.operations.upgrade-support-report.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "current_version": update_manifest["current_version"],
        "default_platform_id": update_manifest["default_platform_id"],
        "platform_support_matrix": update_manifest["platform_support_matrix"],
        "supported_platform_ids": update_manifest["supported_platform_ids"],
        "support_tiers": update_manifest["support_tiers"],
        "support_windows": versioning_model["support_windows"],
        "upgrade_paths": upgrade_surface["upgrade_path_classes"],
        "warnings": warnings,
        "revert_guidance": revert_guidance,
        "fail_closed_diagnostics": fail_closed_policy["diagnostic_classes"],
        "forbidden_claims": claim_policy["forbidden_claims"],
    }
    for field_name in metadata_surface["required_upgrade_support_report_fields"]:
        if field_name not in upgrade_support_report:
            raise RuntimeError(f"upgrade support report missing required field {field_name}")

    UPGRADE_SUPPORT_REPORT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(UPGRADE_SUPPORT_REPORT, upgrade_support_report)

    channel_catalog = {
        "contract_id": "objc3c.release.operations.channel-catalog.v1",
        "generated_at_utc": upgrade_support_report["generated_at_utc"],
        "default_channel": update_manifest["default_channel"],
        "default_platform_id": update_manifest["default_platform_id"],
        "platform_support_matrix": update_manifest["platform_support_matrix"],
        "supported_platform_ids": update_manifest["supported_platform_ids"],
        "support_tiers": update_manifest["support_tiers"],
        "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT),
        "channels": update_manifest["channels"],
    }
    write_json_file(CHANNEL_CATALOG, channel_catalog)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.publication.summary.v1",
        "status": "PASS",
        "update_manifest": repo_rel(UPDATE_MANIFEST),
        "upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT),
        "channel_catalog": repo_rel(CHANNEL_CATALOG),
        "warning_count": len(warnings),
        "claim_class_count": len(claim_policy["upgrade_claim_classes"]),
        "platform_support_matrix": update_manifest["platform_support_matrix"],
    }
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
