#!/usr/bin/env python3
"""Checker for M315-A001 repo-wide milestone residue inventory."""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-A001" / "repo_wide_milestone_residue_inventory_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_repo_wide_milestone_residue_inventory_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a001_repo_wide_milestone_residue_inventory_contract_and_architecture_freeze_packet.md"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a001_repo_wide_milestone_residue_inventory_contract_and_architecture_freeze_inventory.json"
PATTERN = re.compile(r"m[0-9]{3}-[a-z][0-9]{3}", re.IGNORECASE)
MILESTONE_PATTERN = re.compile(r"(m[0-9]{3})-[a-z][0-9]{3}", re.IGNORECASE)


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def classify(path: str) -> str:
    if path == "package.json":
        return "package_surface"
    if path == "README.md" or path.startswith("docs/runbooks/"):
        return "operator_docs"
    if path == "docs/objc3c-native.md" or path.startswith("docs/objc3c-native/src/") or path.startswith("site/"):
        return "generated_docs"
    if path.startswith("spec/"):
        return "planning_specs"
    if path.startswith("scripts/"):
        return "tooling_scripts"
    if path.startswith("tests/"):
        return "tests_surface"
    if path.startswith("native/objc3c/src/"):
        return "native_source"
    if path.startswith(".github/"):
        return "github_meta"
    return "other"


def measure() -> dict[str, object]:
    scope_counts: Counter[str] = Counter()
    scope_file_counts: Counter[str] = Counter()
    unique_issue_ids: set[str] = set()
    milestone_counts: Counter[str] = Counter()
    top_files: list[tuple[str, int, str]] = []
    total = 0

    for path in ROOT.rglob("*"):
        if not path.is_file():
            continue
        rel = path.relative_to(ROOT).as_posix()
        if rel.startswith(".git/") or rel.startswith("tmp/"):
            continue
        try:
            text = path.read_text(encoding="utf-8")
        except Exception:
            continue
        matches = PATTERN.findall(text)
        if not matches:
            continue
        scope = classify(rel)
        count = len(matches)
        total += count
        scope_counts[scope] += count
        scope_file_counts[scope] += 1
        unique_issue_ids.update(match.lower() for match in matches)
        for milestone in MILESTONE_PATTERN.findall(text):
            milestone_counts[milestone.lower()] += 1
        top_files.append((rel, count, scope))

    return {
        "total": total,
        "scope_counts": dict(scope_counts),
        "scope_file_counts": dict(scope_file_counts),
        "distinct_issue_ids": len(unique_issue_ids),
        "distinct_milestones": len(milestone_counts),
        "top_files": sorted(top_files, key=lambda item: (-item[1], item[0]))[:10],
        "top_milestones": milestone_counts.most_common(10),
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    inventory = json.loads(read_text(INVENTORY_JSON))
    measured = measure()

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-repo-wide-milestone-residue-inventory/m315-a001-v1`" in expectations, str(EXPECTATIONS_DOC), "M315-A001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("scope, hotspot file, and milestone" in expectations, str(EXPECTATIONS_DOC), "M315-A001-EXP-02", "expectations missing repo-wide baseline note", failures)
    checks_passed += require("repo-wide milestone-residue inventory" in packet, str(PACKET_DOC), "M315-A001-PKT-01", "packet missing summary", failures)
    checks_passed += require("Next issue: `M315-A002`." in packet, str(PACKET_DOC), "M315-A001-PKT-02", "packet missing next issue", failures)

    checks_total += 5
    checks_passed += require(inventory.get("mode") == "m315-a001-repo-wide-milestone-residue-inventory-v1", str(INVENTORY_JSON), "M315-A001-INV-01", "mode drifted", failures)
    checks_passed += require(inventory.get("contract_id") == "objc3c-cleanup-repo-wide-milestone-residue-inventory/m315-a001-v1", str(INVENTORY_JSON), "M315-A001-INV-02", "contract id drifted", failures)
    checks_passed += require(inventory.get("repo_wide_totals", {}).get("match_count") == measured["total"] == 276758, str(INVENTORY_JSON), "M315-A001-INV-03", "repo-wide match count drifted", failures)
    checks_passed += require(inventory.get("repo_wide_totals", {}).get("distinct_milestone_issue_ids") == measured["distinct_issue_ids"] == 3032, str(INVENTORY_JSON), "M315-A001-INV-04", "distinct milestone issue id count drifted", failures)
    checks_passed += require(inventory.get("repo_wide_totals", {}).get("distinct_milestone_families") == measured["distinct_milestones"] == 123, str(INVENTORY_JSON), "M315-A001-INV-05", "distinct milestone family count drifted", failures)
    for key, expected in {
        "package_surface": 13342,
        "operator_docs": 277,
        "generated_docs": 2224,
        "planning_specs": 28801,
        "tooling_scripts": 178482,
        "tests_surface": 27307,
        "native_source": 1809,
        "github_meta": 2,
        "other": 24514,
    }.items():
        checks_total += 1
        checks_passed += require(inventory.get("scope_counts", {}).get(key) == measured["scope_counts"].get(key) == expected, str(INVENTORY_JSON), f"M315-A001-INV-SCOPE-{key}", f"scope count drifted for {key}", failures)

    checks_total += 5
    checks_passed += require(inventory.get("top_hotspot_files", [])[0]["path"] == "package.json", str(INVENTORY_JSON), "M315-A001-HOT-01", "top hotspot file drifted", failures)
    checks_passed += require(inventory.get("top_hotspot_files", [])[3]["path"] == "native/objc3c/src/ARCHITECTURE.md", str(INVENTORY_JSON), "M315-A001-HOT-02", "native hotspot drifted", failures)
    checks_passed += require(inventory.get("top_hotspot_files", [])[3]["match_count"] == 1691, str(INVENTORY_JSON), "M315-A001-HOT-03", "native hotspot count drifted", failures)
    checks_passed += require(inventory.get("top_milestone_families", [])[0]["milestone"] == "m228" and inventory.get("top_milestone_families", [])[0]["match_count"] == 17369, str(INVENTORY_JSON), "M315-A001-HOT-04", "top milestone family drifted", failures)
    checks_passed += require(inventory.get("next_issue") == "M315-A002", str(INVENTORY_JSON), "M315-A001-INV-06", "next issue drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": inventory["mode"],
        "contract_id": inventory["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "repo_wide_match_count": measured["total"],
        "distinct_milestone_issue_ids": measured["distinct_issue_ids"],
        "distinct_milestone_families": measured["distinct_milestones"],
        "scope_counts": measured["scope_counts"],
        "top_files": measured["top_files"],
        "top_milestones": measured["top_milestones"],
        "next_issue": "M315-A002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-A001 repo-wide milestone residue inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
