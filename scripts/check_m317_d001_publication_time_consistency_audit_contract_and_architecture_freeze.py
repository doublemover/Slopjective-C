#!/usr/bin/env python3
"""Checker for M317-D001 publication-time consistency audit contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-D001" / "publication_time_consistency_audit_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_publication_time_consistency_audit_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_d001_publication_time_consistency_audit_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_d001_publication_time_consistency_audit_contract_and_architecture_freeze_contract.json"
PACKAGE_JSON = ROOT / "package.json"


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


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = json.loads(read_text(CONTRACT_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 3
    checks_passed += require("Contract ID: `objc3c-cleanup-publication-consistency-audit-contract/m317-d001-v1`" in expectations, str(EXPECTATIONS_DOC), "M317-D001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("required machine-readable report keys" in expectations, str(EXPECTATIONS_DOC), "M317-D001-EXP-02", "expectations missing report-key requirement", failures)
    checks_passed += require("Next issue: `M317-E001`." in packet, str(PACKET_DOC), "M317-D001-PKT-01", "packet missing next issue", failures)

    checks_total += 7
    checks_passed += require(contract.get("mode") == "m317-d001-publication-consistency-audit-contract-v1", str(CONTRACT_JSON), "M317-D001-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-cleanup-publication-consistency-audit-contract/m317-d001-v1", str(CONTRACT_JSON), "M317-D001-CON-02", "contract id drifted", failures)
    checks_passed += require(set(contract.get("audit_facets", [])) == {"open_issue_label_completeness", "execution_order_marker_completeness", "milestone_description_dependency_marker_completeness", "overlap_amendment_coverage", "post_cleanup_dependency_rewrite_coverage", "template_generator_contract_alignment", "idempotent_apply_behavior", "rate_limit_safe_publish_apply_behavior"}, str(CONTRACT_JSON), "M317-D001-CON-03", "audit facet set drifted", failures)
    checks_passed += require(all(key in contract.get("required_report_keys", []) for key in ["checked_milestones", "checked_issues", "updated_entities", "next_issue"]), str(CONTRACT_JSON), "M317-D001-CON-04", "required report keys drifted", failures)
    checks_passed += require(any("unlabeled open roadmap issue" == item for item in contract.get("blocker_failures", [])), str(CONTRACT_JSON), "M317-D001-CON-05", "blocker failure list drifted", failures)
    checks_passed += require(len(contract.get("future_implementation_surfaces", [])) == 5, str(CONTRACT_JSON), "M317-D001-CON-06", "future implementation surface count drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M317-E001", str(CONTRACT_JSON), "M317-D001-CON-07", "next issue drifted", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m317-d001-publication-time-consistency-audit-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M317-D001-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m317-d001-publication-time-consistency-audit-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M317-D001-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m317-d001-lane-d-readiness"' in package, str(PACKAGE_JSON), "M317-D001-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "next_issue": "M317-E001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-D001 publication consistency audit contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
