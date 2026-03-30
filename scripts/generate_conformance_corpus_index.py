#!/usr/bin/env python3
"""Generate a deterministic checked-in conformance corpus index."""

from __future__ import annotations

import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CORPUS_SURFACE_PATH = ROOT / "tests" / "conformance" / "corpus_surface.json"
LONGITUDINAL_SUITES_PATH = ROOT / "tests" / "conformance" / "longitudinal_suites.json"
INDEX_PATH = ROOT / "tmp" / "reports" / "conformance" / "corpus-index.json"
INDEX_CONTRACT_ID = "objc3c.conformance.corpus.index.v1"
FAMILY_ROW_RE = re.compile(r"^\|\s*`(?P<family>[^`]+)`\s*\|\s*`(?P<lane>[^`]+)`\s*\|\s*`(?P<buckets>[^`]+)`\s*\|$")


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_family_rows(coverage_map_path: Path) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    for line in coverage_map_path.read_text(encoding="utf-8").splitlines():
        match = FAMILY_ROW_RE.match(line.strip())
        if not match:
            continue
        bucket_values = [
            part.strip().strip("`")
            for part in match.group("buckets").split(",")
            if part.strip()
        ]
        rows.append(
            {
                "family": match.group("family"),
                "issue_lane": match.group("lane"),
                "buckets": bucket_values,
            }
        )
    return rows


def summarize_manifest(bucket: str, manifest_path: Path) -> dict[str, Any]:
    payload = load_json(manifest_path)
    groups = payload.get("groups", [])
    records: list[dict[str, Any]] = []
    total_files = 0
    for group in groups:
        if not isinstance(group, dict):
            continue
        files = [
            file_name
            for file_name in group.get("files", [])
            if isinstance(file_name, str) and file_name
        ]
        total_files += len(files)
        issues: list[int] = []
        if isinstance(group.get("issue"), int):
            issues.append(group["issue"])
        if isinstance(group.get("issues"), list):
            issues.extend([value for value in group["issues"] if isinstance(value, int)])
        records.append(
            {
                "name": group.get("name"),
                "feature": group.get("feature"),
                "issues": sorted(set(issues)),
                "files": sorted(files),
            }
        )
    records.sort(key=lambda entry: str(entry.get("name") or ""))
    return {
        "bucket": bucket,
        "manifest_path": repo_rel(manifest_path),
        "suite": payload.get("suite"),
        "description": payload.get("description"),
        "group_count": len(records),
        "fixture_count": total_files,
        "groups": records,
    }


def main() -> int:
    if not CORPUS_SURFACE_PATH.is_file():
        print(
            f"conformance-corpus-index: missing corpus surface contract {repo_rel(CORPUS_SURFACE_PATH)}",
            file=sys.stderr,
        )
        return 1

    surface = load_json(CORPUS_SURFACE_PATH)
    longitudinal_payload = load_json(LONGITUDINAL_SUITES_PATH)
    taxonomy = surface.get("taxonomy", {})
    manifest_inventory = taxonomy.get("manifest_inventory", {})
    if not isinstance(manifest_inventory, dict):
        print("conformance-corpus-index: manifest inventory missing", file=sys.stderr)
        return 1

    coverage_map_path = ROOT / str(surface["coverage_map"])
    family_rows = parse_family_rows(coverage_map_path)

    manifest_summaries: list[dict[str, Any]] = []
    for bucket, manifest_relative in sorted(manifest_inventory.items()):
        manifest_path = ROOT / str(manifest_relative)
        manifest_summaries.append(summarize_manifest(bucket, manifest_path))

    retained_suites = longitudinal_payload.get("retained_suites", [])
    retained_partition: list[dict[str, Any]] = []
    for entry in retained_suites:
        if not isinstance(entry, dict):
            continue
        retained_partition.append(
            {
                "suite_id": entry.get("suite_id"),
                "suite_class": entry.get("suite_class"),
                "bucket": entry.get("bucket"),
                "manifest": entry.get("manifest"),
                "traceability_targets": entry.get("traceability_targets"),
            }
        )
    retained_partition.sort(key=lambda entry: str(entry.get("suite_id") or ""))

    payload = {
        "contract_id": INDEX_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "corpus_surface_contract": repo_rel(CORPUS_SURFACE_PATH),
        "coverage_map_source": str(surface["coverage_map"]),
        "runbook": surface.get("runbook"),
        "claim_policy": surface.get("claim_policy"),
        "gap_priority_model": surface.get("gap_priority_model"),
        "suite_partitions": surface.get("suite_partitions"),
        "longitudinal_policy": surface.get("longitudinal_policy"),
        "workflow_surface": surface.get("workflow_surface"),
        "retained_partition": retained_partition,
        "audit_surface": surface.get("audit_surface"),
        "family_rows": family_rows,
        "manifest_summaries": manifest_summaries,
    }
    INDEX_PATH.parent.mkdir(parents=True, exist_ok=True)
    INDEX_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"wrote index: {repo_rel(INDEX_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
