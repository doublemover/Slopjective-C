#!/usr/bin/env python3
"""Publish release-operations metadata from the live update manifest and policies."""

from __future__ import annotations

from pathlib import Path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file

try:
    from release_operations_publication_contracts import (
        build_release_operations_publication_payloads,
    )
except ModuleNotFoundError:
    from scripts.release_operations_publication_contracts import (
        build_release_operations_publication_payloads,
    )

ROOT = Path(__file__).resolve().parents[1]
UPDATE_MANIFEST = ROOT / "tmp" / "artifacts" / "release-operations" / "update-manifest" / "objc3c-update-manifest.json"
VERSIONING_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "versioning_model.json"
UPGRADE_PATH_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_path_surface.json"
UPGRADE_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "upgrade_support_claim_policy.json"
UPDATE_CHANNEL_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "update_channel_policy.json"
FAIL_CLOSED_DIAGNOSTICS_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "fail_closed_diagnostics_policy.json"
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

    payloads = build_release_operations_publication_payloads(
        update_manifest=update_manifest,
        versioning_model=versioning_model,
        upgrade_surface=upgrade_surface,
        claim_policy=claim_policy,
        update_channel_policy=update_channel_policy,
        fail_closed_policy=fail_closed_policy,
        metadata_surface=metadata_surface,
        update_manifest_path=repo_rel(UPDATE_MANIFEST),
        upgrade_support_report_path=repo_rel(UPGRADE_SUPPORT_REPORT),
        channel_catalog_path=repo_rel(CHANNEL_CATALOG),
    )

    UPGRADE_SUPPORT_REPORT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(UPGRADE_SUPPORT_REPORT, payloads.upgrade_support_report)
    write_json_file(CHANNEL_CATALOG, payloads.channel_catalog)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payloads.summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-release-operations-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
