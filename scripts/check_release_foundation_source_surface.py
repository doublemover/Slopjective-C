#!/usr/bin/env python3
"""Validate the checked-in release-foundation source surface."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "source-surface-summary.json"

EXPECTED_CONTRACT_IDS = {
    "artifact_taxonomy": "objc3c.release.foundation.artifact.taxonomy.v1",
    "distribution_trust_model": "objc3c.release.foundation.distribution.trust.model.v1",
    "distribution_audit": "objc3c.release.foundation.distribution.audit.v1",
    "reproducibility_policy": "objc3c.release.foundation.reproducibility.policy.v1",
    "release_payload_policy": "objc3c.release.foundation.payload.policy.v1",
    "provenance_policy": "objc3c.release.foundation.provenance.policy.v1",
    "workflow_surface": "objc3c.release.foundation.workflow.surface.v1",
    "schema_surface": "objc3c.release.foundation.schema.surface.v1",
}


def fail(message: str) -> int:
    print(f"release-foundation-source-surface: {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError(f"{path} did not contain a JSON object")
    return payload


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace('\\', '/')


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface {repo_rel(SOURCE_SURFACE)}")
    source_surface = load_json(SOURCE_SURFACE)
    if source_surface.get("contract_id") != "objc3c.release.foundation.source.surface.v1":
        return fail("unexpected source surface contract_id")
    if source_surface.get("surface_kind") != "release-foundation-source-surface":
        return fail("unexpected source surface kind")

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

    for field_name in ("runbook",):
        raw_path = source_surface.get(field_name)
        if not isinstance(raw_path, str) or not raw_path:
            return fail(f"{field_name} was missing from the source surface")
        target = ROOT / raw_path
        if not target.is_file():
            return fail(f"{field_name} referenced missing file {raw_path}")
        checked_paths.append(raw_path)

    for list_name in ("checked_in_sources", "upstream_surfaces", "build_scripts", "machine_owned_output_roots", "explicit_non_goals"):
        items = source_surface.get(list_name)
        if not isinstance(items, list) or not items:
            return fail(f"{list_name} must be a non-empty list")
        if list_name == "machine_owned_output_roots":
            continue
        for raw_path in items:
            if not isinstance(raw_path, str) or not raw_path:
                return fail(f"{list_name} contained an invalid path entry")
            if list_name == "explicit_non_goals":
                continue
            target = ROOT / raw_path
            if not target.exists():
                return fail(f"{list_name} referenced missing path {raw_path}")
            checked_paths.append(raw_path)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.foundation.source.surface.summary.v1",
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-foundation-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
