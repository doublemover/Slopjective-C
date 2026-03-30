#!/usr/bin/env python3
"""Validate the checked-in conformance corpus surface and emit a summary."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CORPUS_SURFACE_PATH = ROOT / "tests" / "conformance" / "corpus_surface.json"
LONGITUDINAL_SUITES_PATH = ROOT / "tests" / "conformance" / "longitudinal_suites.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "conformance" / "corpus-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.conformance.corpus.surface.summary.v1"
EXPECTED_PRIMARY_BUCKETS = [
    "parser",
    "semantic",
    "lowering_abi",
    "module_roundtrip",
    "diagnostics",
]
EXPECTED_SUPPLEMENTAL_BUCKETS = [
    "examples",
    "spec_open_issues",
    "workpacks",
]


def fail(message: str) -> int:
    print(f"conformance-corpus-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def summarize_manifest(bucket: str, manifest_path: Path) -> dict[str, Any]:
    payload = load_json(manifest_path)
    groups = payload.get("groups")
    if not isinstance(groups, list) or not groups:
        raise RuntimeError(f"{repo_rel(manifest_path)} is missing non-empty groups")

    referenced_files: list[str] = []
    issue_refs: set[int] = set()
    for group in groups:
        if not isinstance(group, dict):
            raise RuntimeError(f"{repo_rel(manifest_path)} contains a non-object group")
        files = group.get("files")
        if not isinstance(files, list) or not files:
            raise RuntimeError(f"{repo_rel(manifest_path)} contains a group without files")
        for file_name in files:
            if not isinstance(file_name, str) or not file_name:
                raise RuntimeError(f"{repo_rel(manifest_path)} contains a non-string file entry")
            fixture_path = manifest_path.parent / file_name
            if not fixture_path.is_file():
                raise RuntimeError(f"{repo_rel(manifest_path)} references missing fixture {repo_rel(fixture_path)}")
            referenced_files.append(file_name)
        issue = group.get("issue")
        if isinstance(issue, int):
            issue_refs.add(issue)
        issues = group.get("issues")
        if isinstance(issues, list):
            for value in issues:
                if isinstance(value, int):
                    issue_refs.add(value)

    return {
        "bucket": bucket,
        "manifest_path": repo_rel(manifest_path),
        "suite": payload.get("suite"),
        "group_count": len(groups),
        "fixture_count": len(referenced_files),
        "issue_count": len(issue_refs),
        "issue_refs": sorted(issue_refs),
    }


def main() -> int:
    if not CORPUS_SURFACE_PATH.is_file():
        return fail(f"missing corpus surface contract: {repo_rel(CORPUS_SURFACE_PATH)}")

    surface = load_json(CORPUS_SURFACE_PATH)
    if surface.get("contract_id") != "objc3c.conformance.corpus.surface.v1":
        return fail("corpus surface contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("corpus surface schema_version drifted")
    if surface.get("corpus_root") != "tests/conformance":
        return fail("corpus_root drifted")
    if surface.get("suite_readme") != "tests/conformance/README.md":
        return fail("suite_readme drifted")
    if surface.get("coverage_map") != "tests/conformance/COVERAGE_MAP.md":
        return fail("coverage_map drifted")
    if surface.get("runbook") != "docs/runbooks/objc3c_conformance_corpus.md":
        return fail("runbook drifted")
    if surface.get("primary_buckets") != EXPECTED_PRIMARY_BUCKETS:
        return fail("primary_buckets drifted")
    if surface.get("supplemental_buckets") != EXPECTED_SUPPLEMENTAL_BUCKETS:
        return fail("supplemental_buckets drifted")

    require_path("tests/conformance/README.md", kind="suite readme")
    require_path("tests/conformance/COVERAGE_MAP.md", kind="coverage map")
    require_path("docs/runbooks/objc3c_conformance_corpus.md", kind="runbook")
    require_path("tests/conformance/longitudinal_suites.json", kind="longitudinal suite manifest")

    taxonomy = surface.get("taxonomy")
    if not isinstance(taxonomy, dict):
        return fail("taxonomy is missing")
    manifest_inventory = taxonomy.get("manifest_inventory")
    if not isinstance(manifest_inventory, dict):
        return fail("taxonomy.manifest_inventory is missing")

    required_manifest_keys = EXPECTED_PRIMARY_BUCKETS + ["examples", "spec_open_issues"]
    if sorted(manifest_inventory.keys()) != sorted(required_manifest_keys):
        return fail("manifest inventory keys drifted")

    bucket_summaries: list[dict[str, Any]] = []
    for bucket in required_manifest_keys:
        manifest_relative = manifest_inventory[bucket]
        if not isinstance(manifest_relative, str) or not manifest_relative:
            return fail(f"manifest inventory entry for {bucket} is missing")
        manifest_path = require_path(manifest_relative, kind=f"{bucket} manifest")
        bucket_summaries.append(summarize_manifest(bucket, manifest_path))

    artifact_surface = surface.get("artifact_surface")
    if not isinstance(artifact_surface, dict):
        return fail("artifact_surface is missing")
    if artifact_surface.get("surface_summary") != "tmp/reports/conformance/corpus-surface-summary.json":
        return fail("artifact_surface.surface_summary drifted")
    if artifact_surface.get("coverage_index") != "tmp/reports/conformance/corpus-index.json":
        return fail("artifact_surface.coverage_index drifted")

    longitudinal_policy = surface.get("longitudinal_policy")
    if not isinstance(longitudinal_policy, dict):
        return fail("longitudinal_policy is missing")
    if longitudinal_policy.get("suite_manifest") != "tests/conformance/longitudinal_suites.json":
        return fail("longitudinal_policy.suite_manifest drifted")

    longitudinal_payload = load_json(LONGITUDINAL_SUITES_PATH)
    if longitudinal_payload.get("contract_id") != "objc3c.conformance.longitudinal_suites.v1":
        return fail("longitudinal_suites contract_id drifted")
    if longitudinal_payload.get("schema_version") != 1:
        return fail("longitudinal_suites schema_version drifted")
    retained_suites = longitudinal_payload.get("retained_suites")
    if not isinstance(retained_suites, list) or not retained_suites:
        return fail("longitudinal_suites retained_suites is missing")

    retained_summary: list[dict[str, Any]] = []
    known_buckets = set(required_manifest_keys)
    known_suite_classes = set(longitudinal_policy.get("retained_suite_classes", []))
    for entry in retained_suites:
        if not isinstance(entry, dict):
            return fail("longitudinal_suites contains a non-object entry")
        suite_id = entry.get("suite_id")
        suite_class = entry.get("suite_class")
        bucket = entry.get("bucket")
        manifest = entry.get("manifest")
        traceability_targets = entry.get("traceability_targets")
        if not all(isinstance(value, str) and value for value in (suite_id, suite_class, bucket, manifest)):
            return fail("longitudinal_suites entry is missing suite_id/suite_class/bucket/manifest")
        if bucket not in known_buckets:
            return fail(f"longitudinal_suites references unknown bucket {bucket}")
        if suite_class not in known_suite_classes:
            return fail(f"longitudinal_suites references unknown suite_class {suite_class}")
        if not isinstance(traceability_targets, list) or not traceability_targets:
            return fail(f"longitudinal_suites entry {suite_id} is missing traceability_targets")
        require_path(manifest, kind=f"longitudinal manifest reference for {suite_id}")
        retained_summary.append(
            {
                "suite_id": suite_id,
                "suite_class": suite_class,
                "bucket": bucket,
                "manifest": manifest,
                "traceability_targets": traceability_targets,
            }
        )

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "corpus_surface_contract": repo_rel(CORPUS_SURFACE_PATH),
        "coverage_map": surface["coverage_map"],
        "runbook": surface["runbook"],
        "primary_buckets": EXPECTED_PRIMARY_BUCKETS,
        "supplemental_buckets": EXPECTED_SUPPLEMENTAL_BUCKETS,
        "bucket_summaries": bucket_summaries,
        "claim_policy": surface.get("claim_policy"),
        "gap_priority_model": surface.get("gap_priority_model"),
        "suite_partitions": surface.get("suite_partitions"),
        "longitudinal_policy": surface.get("longitudinal_policy"),
        "retained_suite_summary": retained_summary,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("conformance-corpus-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
