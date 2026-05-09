#!/usr/bin/env python3
"""Validate the checked-in objc3c packaging-channels source surface."""

from __future__ import annotations

from pathlib import Path
from objc3c_tooling.paths import repo_rel, resolve_repo_path
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "source-surface-summary.json"
SURFACE_CONTRACT_ID = "objc3c.packaging.channels.source.surface.v1"
SUMMARY_CONTRACT_ID = "objc3c.packaging.channels.source.surface.summary.v1"



def main() -> int:
    payload = load_json(SOURCE_SURFACE)
    if payload.get("contract_id") != SURFACE_CONTRACT_ID:
        raise RuntimeError("source surface contract_id drifted")
    required_keys = (
        "runbook",
        "channel_architecture",
        "supported_platforms",
        "installer_policy",
        "metadata_surface",
        "schema_surface",
    )
    for key in required_keys:
        raw_path = payload.get(key)
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError(f"source surface did not publish {key}")
        candidate = resolve_repo_path(raw_path)
        if not candidate.is_file():
            raise RuntimeError(f"missing source-surface path {raw_path}")

    checked_in_sources = payload.get("checked_in_sources")
    if not isinstance(checked_in_sources, list) or not checked_in_sources:
        raise RuntimeError("source surface did not publish checked_in_sources")
    for raw_path in checked_in_sources:
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError("checked_in_sources contained an invalid path")
        candidate = resolve_repo_path(raw_path)
        if not candidate.is_file():
            raise RuntimeError(f"missing checked-in packaging-channels source {raw_path}")

    output_roots = payload.get("machine_owned_output_roots")
    if not isinstance(output_roots, list) or not output_roots:
        raise RuntimeError("source surface did not publish machine_owned_output_roots")
    owner_policy = payload.get("owner_policy")
    if not isinstance(owner_policy, dict) or owner_policy.get("report_only_allowed") is not False:
        raise RuntimeError("source surface did not publish source-owned non-report-only owner_policy")
    blocker_metadata = payload.get("blocker_metadata")
    if not isinstance(blocker_metadata, dict) or blocker_metadata.get("blocker_owner") != "packaging-channels-blockers":
        raise RuntimeError("source surface did not publish packaging-channel blocker metadata")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "checked_in_source_count": len(checked_in_sources),
        "machine_owned_output_roots": output_roots,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("packaging-channels-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
