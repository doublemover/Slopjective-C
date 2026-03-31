#!/usr/bin/env python3
"""Validate the checked-in security-hardening source surface."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "source-surface-summary.json"

EXPECTED_CONTRACT_IDS = {
    "boundary_inventory": "objc3c.security.hardening.boundary.inventory.v1",
    "response_policy": "objc3c.security.hardening.response.disclosure.policy.v1",
    "macro_trust_policy": "objc3c.security.hardening.macro.package.provenance.trust.policy.v1",
    "release_key_policy": "objc3c.security.hardening.installer.update.release-key.policy.v1",
    "artifact_reporting_contract": "objc3c.security.hardening.artifact.reporting.contract.v1",
    "supply_chain_audit_contract": "objc3c.security.hardening.supply.chain.audit.contract.v1",
    "response_drill_contract": "objc3c.security.hardening.response.drill.contract.v1",
    "schema_surface": "objc3c.security.hardening.schema.surface.v1",
    "workflow_surface": "objc3c.security.hardening.workflow.surface.v1",
}


def fail(message: str) -> int:
    print(f"security-hardening-source-surface: {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError(f"{path} did not contain a JSON object")
    return payload


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface {repo_rel(SOURCE_SURFACE)}")
    source_surface = load_json(SOURCE_SURFACE)
    if source_surface.get("contract_id") != "objc3c.security.hardening.source.surface.v1":
        return fail("unexpected source surface contract_id")
    if source_surface.get("surface_kind") != "security-hardening-source-surface":
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
        if list_name != "checked_in_sources":
            continue
        for raw_path in items:
            if not isinstance(raw_path, str) or not raw_path:
                return fail(f"{list_name} contained an invalid path entry")
            target = ROOT / raw_path
            if not target.exists():
                return fail(f"{list_name} referenced missing path {raw_path}")
            checked_paths.append(raw_path)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.security.hardening.source.surface.summary.v1",
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("security-hardening-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
