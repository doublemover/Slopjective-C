#!/usr/bin/env python3
"""Checker for M313-B005 new-checker and temporary-validation exception policy."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-B005" / "new_checker_temporary_validation_exception_policy_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_b005_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_packet.md"
POLICY_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_policy.json"
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_exception_registry.json"
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
    policy = json.loads(read_text(POLICY_JSON))
    registry = json.loads(read_text(REGISTRY_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-new-checker-temporary-validation-exception-policy/m313-b005-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-B005-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Default to no new milestone-local checker, readiness, or pytest-wrapper surface without a recorded exception." in expectations, str(EXPECTATIONS_DOC), "M313-B005-EXP-02", "expectations missing default-no-growth rule", failures)
    checks_passed += require("empty-by-default registry for active waivers" in packet, str(PACKET_DOC), "M313-B005-PKT-01", "packet missing registry focus", failures)
    checks_passed += require("Next issue: `M313-C001`." in packet, str(PACKET_DOC), "M313-B005-PKT-02", "packet missing next issue", failures)

    checks_total += 8
    checks_passed += require(policy.get("mode") == "m313-b005-new-checker-temporary-validation-exception-policy-v1", str(POLICY_JSON), "M313-B005-CON-01", "mode drifted", failures)
    checks_passed += require(policy.get("contract_id") == "objc3c-cleanup-new-checker-temporary-validation-exception-policy/m313-b005-v1", str(POLICY_JSON), "M313-B005-CON-02", "contract id drifted", failures)
    checks_passed += require(policy.get("exception_record_required_fields") == ["id", "reason", "owner_issue", "created_by_issue", "expiry_issue", "replacement_target", "approved_by", "validation_class", "status"], str(POLICY_JSON), "M313-B005-CON-03", "required exception fields drifted", failures)
    checks_passed += require(policy.get("allowed_statuses") == ["active", "expired", "retired"], str(POLICY_JSON), "M313-B005-CON-04", "allowed statuses drifted", failures)
    checks_passed += require(policy.get("zero_active_exceptions_expected_at_closeout") is True, str(POLICY_JSON), "M313-B005-CON-05", "closeout expectation drifted", failures)
    checks_passed += require(policy.get("exception_registry_path") == "spec/planning/compiler/m313/m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_exception_registry.json", str(POLICY_JSON), "M313-B005-CON-06", "exception registry path drifted", failures)
    checks_passed += require(policy.get("next_issue") == "M313-C001", str(POLICY_JSON), "M313-B005-CON-07", "next issue drifted", failures)
    checks_passed += require(policy.get("prohibited_without_exception") == [
        "new scripts/check_m*_*.py surfaces",
        "new scripts/run_*_readiness.py surfaces that do not delegate through the shared harness",
        "new tests/tooling/test_check_*.py wrappers that duplicate executable suite truth",
    ], str(POLICY_JSON), "M313-B005-CON-08", "prohibited-without-exception set drifted", failures)

    checks_total += 4
    checks_passed += require(registry.get("mode") == "m313-b005-validation-exception-registry-v1", str(REGISTRY_JSON), "M313-B005-REG-01", "registry mode drifted", failures)
    checks_passed += require(registry.get("contract_id") == "objc3c-cleanup-new-checker-temporary-validation-exception-registry/m313-b005-v1", str(REGISTRY_JSON), "M313-B005-REG-02", "registry contract id drifted", failures)
    checks_passed += require(registry.get("policy_contract_id") == policy.get("contract_id"), str(REGISTRY_JSON), "M313-B005-REG-03", "registry policy link drifted", failures)
    checks_passed += require(registry.get("exceptions") == [], str(REGISTRY_JSON), "M313-B005-REG-04", "exception registry is no longer empty by default", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-b005-new-checker-and-temporary-validation-exception-policy-edge-case-and-compatibility-completion"' in package, str(PACKAGE_JSON), "M313-B005-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-b005-new-checker-and-temporary-validation-exception-policy-edge-case-and-compatibility-completion"' in package, str(PACKAGE_JSON), "M313-B005-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-b005-lane-b-readiness"' in package, str(PACKAGE_JSON), "M313-B005-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": policy["mode"],
        "contract_id": policy["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "required_fields": policy["exception_record_required_fields"],
        "allowed_statuses": policy["allowed_statuses"],
        "exception_count": len(registry["exceptions"]),
        "next_issue": "M313-C001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-B005 validation exception policy checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
