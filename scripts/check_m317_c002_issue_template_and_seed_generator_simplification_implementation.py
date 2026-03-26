#!/usr/bin/env python3
"""Checker for M317-C002 issue template and seed-generator simplification implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-C002" / "issue_template_seed_generator_simplification_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_issue_template_and_seed_generator_simplification_implementation_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_c002_issue_template_and_seed_generator_simplification_implementation_core_feature_implementation_packet.md"
ROADMAP_TEMPLATE = ROOT / ".github" / "ISSUE_TEMPLATE" / "roadmap_execution.yml"
CONFORMANCE_TEMPLATE = ROOT / ".github" / "ISSUE_TEMPLATE" / "conformance_execution.yml"
GENERATOR = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "generate_cleanup_acceleration_program.py"
SEED = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "cleanup_acceleration_program_seed.json"
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
    roadmap = read_text(ROADMAP_TEMPLATE)
    conformance = read_text(CONFORMANCE_TEMPLATE)
    generator = read_text(GENERATOR)
    seed = json.loads(read_text(SEED))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-issue-template-seed-generator-simplification/m317-c002-v1`" in expectations, str(EXPECTATIONS_DOC), "M317-C002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("`M317-C001`" in packet, str(PACKET_DOC), "M317-C002-PKT-01", "packet missing C001 reference", failures)
    checks_passed += require("Next issue: `M317-D001`." in packet, str(PACKET_DOC), "M317-C002-PKT-02", "packet missing next issue", failures)
    checks_passed += require("Regenerated cleanup-program seed output demonstrates the new shape." in expectations, str(EXPECTATIONS_DOC), "M317-C002-EXP-02", "expectations missing seed outcome", failures)

    checks_total += 8
    checks_passed += require("type:roadmap" in roadmap, str(ROADMAP_TEMPLATE), "M317-C002-TPL-01", "roadmap template missing type:roadmap", failures)
    checks_passed += require("id: validation_posture" in roadmap, str(ROADMAP_TEMPLATE), "M317-C002-TPL-02", "roadmap template missing validation posture field", failures)
    checks_passed += require("label: Acceptance Criteria" in roadmap, str(ROADMAP_TEMPLATE), "M317-C002-TPL-03", "roadmap template missing acceptance criteria", failures)
    checks_passed += require("label: Primary Implementation Surfaces" in roadmap, str(ROADMAP_TEMPLATE), "M317-C002-TPL-04", "roadmap template missing implementation surfaces", failures)
    checks_passed += require("type:roadmap" in conformance, str(CONFORMANCE_TEMPLATE), "M317-C002-TPL-05", "conformance template missing type:roadmap", failures)
    checks_passed += require("id: validation_posture" in conformance, str(CONFORMANCE_TEMPLATE), "M317-C002-TPL-06", "conformance template missing validation posture field", failures)
    checks_passed += require("label: Acceptance Criteria" in conformance, str(CONFORMANCE_TEMPLATE), "M317-C002-TPL-07", "conformance template missing acceptance criteria", failures)
    checks_passed += require("Prefer shared validation surfaces over new per-issue scaffolds." in conformance, str(CONFORMANCE_TEMPLATE), "M317-C002-TPL-08", "conformance template missing shared-surface guidance", failures)

    checks_total += 6
    checks_passed += require("## Validation posture" in generator, str(GENERATOR), "M317-C002-GEN-01", "generator missing validation posture section", failures)
    checks_passed += require("validation_posture_by_lane" in generator, str(GENERATOR), "M317-C002-GEN-02", "generator missing validation posture defaults", failures)
    checks_passed += require("validation_evidence" in generator, str(GENERATOR), "M317-C002-GEN-03", "generator missing validation evidence map", failures)
    checks_passed += require("## Required deliverables" not in generator, str(GENERATOR), "M317-C002-GEN-04", "generator still emits required deliverables boilerplate", failures)
    checks_passed += require("validation_posture=issue_def.get(\"validation_posture\")" in generator, str(GENERATOR), "M317-C002-GEN-05", "generator does not pass validation posture through issue defs", failures)
    checks_passed += require("generator_contract_only" in generator, str(GENERATOR), "M317-C002-GEN-06", "generator missing generator_contract_only posture", failures)

    issues = [issue for milestone in seed["milestones"] for issue in milestone["issues"]]
    checks_total += 5
    checks_passed += require(bool(issues), str(SEED), "M317-C002-SEED-01", "regenerated seed has no issues", failures)
    checks_passed += require(all("## Validation posture" in issue["body"] for issue in issues), str(SEED), "M317-C002-SEED-02", "seed issues missing validation posture section", failures)
    checks_passed += require(all("## Required deliverables" not in issue["body"] for issue in issues), str(SEED), "M317-C002-SEED-03", "seed issues still contain required deliverables boilerplate", failures)
    checks_passed += require(any("- Class: `generator_contract_only`" in issue["body"] for issue in issues), str(SEED), "M317-C002-SEED-04", "seed missing generator_contract_only example", failures)
    checks_passed += require(any("- Class: `shared_acceptance_harness`" in issue["body"] for issue in issues), str(SEED), "M317-C002-SEED-05", "seed missing shared_acceptance_harness example", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m317-c002-issue-template-and-seed-generator-simplification-implementation"' in package, str(PACKAGE_JSON), "M317-C002-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m317-c002-issue-template-and-seed-generator-simplification-implementation"' in package, str(PACKAGE_JSON), "M317-C002-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m317-c002-lane-c-readiness"' in package, str(PACKAGE_JSON), "M317-C002-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "issue_count": len(issues),
        "next_issue": "M317-D001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-C002 issue template/seed generator checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
