#!/usr/bin/env python3
"""Publish release-operations metadata from the live update manifest and policies."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPGRADE_PATH_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_path_surface.json"
COMPATIBILITY_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "compatibility_claim_policy.json"
UPDATE_CHANNEL_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "update_channel_policy.json"
FALLBACK_DIAGNOSTICS_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "fallback_diagnostics_policy.json"
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "metadata_surface.json"
COMPATIBILITY_REPORT = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-compatibility-report.json"
CHANNEL_CATALOG = ROOT / "tmp" / "artifacts" / "release-operations" / "publication" / "objc3c-release-channel-catalog.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "publication-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def main() -> int:
    update_manifest = load_json(UPDATE_MANIFEST)
    versioning_model = load_json(VERSIONING_MODEL)
    upgrade_surface = load_json(UPGRADE_PATH_SURFACE)
    claim_policy = load_json(COMPATIBILITY_CLAIM_POLICY)
    update_channel_policy = load_json(UPDATE_CHANNEL_POLICY)
    fallback_policy = load_json(FALLBACK_DIAGNOSTICS_POLICY)
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
            })
    for diagnostic in fallback_policy["diagnostic_classes"]:
        warnings.append({
            "channel_id": "policy",
            "warning_id": diagnostic["diagnostic_id"],
            "severity": diagnostic["severity"],
            "required_action": diagnostic["required_action"],
        })

    rollback_guidance = [
        {
            "channel_id": "stable",
            "recommended_transport": "local-installer",
            "reason": "same-major stable rollback",
        },
        {
            "channel_id": "candidate",
            "recommended_transport": "local-installer",
            "reason": "candidate rollback stays on the installer receipt path",
        },
        {
            "channel_id": "preview",
            "recommended_transport": "offline-bundle",
            "reason": "preview rollback uses the bundled channel set",
        },
    ]

    compatibility_report = {
        "contract_id": "objc3c.release.operations.compatibility-report.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "current_version": update_manifest["current_version"],
        "support_windows": versioning_model["support_windows"],
        "upgrade_paths": upgrade_surface["upgrade_path_classes"],
        "warnings": warnings,
        "rollback_guidance": rollback_guidance,
        "forbidden_claims": claim_policy["forbidden_claims"],
    }
    for field_name in metadata_surface["required_compatibility_report_fields"]:
        if field_name not in compatibility_report:
            raise RuntimeError(f"compatibility report missing required field {field_name}")

    COMPATIBILITY_REPORT.parent.mkdir(parents=True, exist_ok=True)
    COMPATIBILITY_REPORT.write_text(json.dumps(compatibility_report, indent=2) + "\n", encoding="utf-8")

    channel_catalog = {
        "contract_id": "objc3c.release.operations.channel-catalog.v1",
        "generated_at_utc": compatibility_report["generated_at_utc"],
        "default_channel": update_manifest["default_channel"],
        "channels": update_manifest["channels"],
    }
    CHANNEL_CATALOG.write_text(json.dumps(channel_catalog, indent=2) + "\n", encoding="utf-8")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.publication.summary.v1",
        "status": "PASS",
        "update_manifest": repo_rel(UPDATE_MANIFEST),
        "compatibility_report": repo_rel(COMPATIBILITY_REPORT),
        "channel_catalog": repo_rel(CHANNEL_CATALOG),
        "warning_count": len(warnings),
        "claim_class_count": len(claim_policy["claim_classes"]),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
