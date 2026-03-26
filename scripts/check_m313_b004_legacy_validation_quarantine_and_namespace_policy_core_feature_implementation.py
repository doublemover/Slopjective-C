#!/usr/bin/env python3
"""Checker for M313-B004 legacy validation quarantine and namespace policy."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-B004" / "legacy_validation_quarantine_namespace_policy_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_b004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_packet.md"
POLICY_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_policy.json"
A002_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"
PACKAGE_JSON = ROOT / "package.json"

CLASS_TO_GLOB = {
    "issue_local_checker": "scripts/check_*.py",
    "issue_local_readiness_runner": "scripts/run_*_readiness.py",
    "issue_local_pytest_wrapper": "tests/tooling/test_check_*.py",
}


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


def extract_milestone_code(name: str) -> int | None:
    match = re.search(r"(?:^|_)m(\d{2,3})(?:_|-)", name)
    if not match:
        return None
    return int(match.group(1))


def classify(glob: str, active_codes: set[int]) -> dict[str, object]:
    active: list[str] = []
    quarantine: list[str] = []
    retained_non_milestone: list[str] = []
    for path in sorted(ROOT.glob(glob)):
        rel = str(path.relative_to(ROOT)).replace("\\", "/")
        code = extract_milestone_code(path.name)
        if code is None:
            retained_non_milestone.append(rel)
        elif code in active_codes:
            active.append(rel)
        else:
            quarantine.append(rel)
    return {
        "active": active,
        "quarantine": quarantine,
        "retained_non_milestone": retained_non_milestone,
        "non_quarantined_count": len(active) + len(retained_non_milestone),
        "total_count": len(active) + len(quarantine) + len(retained_non_milestone),
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    policy = json.loads(read_text(POLICY_JSON))
    a002 = json.loads(read_text(A002_JSON))
    package = read_text(PACKAGE_JSON)
    active_codes = set(policy["active_milestone_codes"])

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-legacy-validation-quarantine-namespace-policy/m313-b004-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-B004-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Count only non-quarantined namespaces against the `M313-A002` namespace-quarantine ratchet stage." in expectations, str(EXPECTATIONS_DOC), "M313-B004-EXP-02", "expectations missing ratchet-stage counting rule", failures)
    checks_passed += require("active cleanup-window validation namespaces" in packet, str(PACKET_DOC), "M313-B004-PKT-01", "packet missing active-window focus", failures)
    checks_passed += require("Next issue: `M313-B005`." in packet, str(PACKET_DOC), "M313-B004-PKT-02", "packet missing next issue", failures)

    checks_total += 8
    checks_passed += require(policy.get("mode") == "m313-b004-legacy-validation-quarantine-namespace-policy-v1", str(POLICY_JSON), "M313-B004-CON-01", "mode drifted", failures)
    checks_passed += require(policy.get("contract_id") == "objc3c-cleanup-legacy-validation-quarantine-namespace-policy/m313-b004-v1", str(POLICY_JSON), "M313-B004-CON-02", "contract id drifted", failures)
    checks_passed += require(policy.get("active_milestone_codes") == [313, 314, 315, 316, 317, 318], str(POLICY_JSON), "M313-B004-CON-03", "active milestone window drifted", failures)
    checks_passed += require([entry.get("validation_class") for entry in policy.get("namespace_rules", [])] == ["issue_local_checker", "issue_local_readiness_runner", "issue_local_pytest_wrapper"], str(POLICY_JSON), "M313-B004-CON-04", "namespace rules drifted", failures)
    checks_passed += require(policy.get("ratchet_stage", {}).get("owner_issue") == "M313-B004" and policy.get("ratchet_stage", {}).get("stage") == "namespace_quarantine", str(POLICY_JSON), "M313-B004-CON-05", "ratchet stage owner drifted", failures)
    checks_passed += require(policy.get("ratchet_stage", {}).get("maximums") == a002["ratchet_stages"][1]["maximums"], str(POLICY_JSON), "M313-B004-CON-06", "ratchet stage maximums drifted from A002", failures)
    checks_passed += require(policy.get("classification_requirements") == {
        "must_exhaustively_classify": True,
        "quarantined_surfaces_are_not_default_truth": True,
        "no_new_active_namespace_outside_allowlist": True,
    }, str(POLICY_JSON), "M313-B004-CON-07", "classification requirements drifted", failures)
    checks_passed += require(policy.get("next_issue") == "M313-B005", str(POLICY_JSON), "M313-B004-CON-08", "next issue drifted", failures)

    measurements: dict[str, dict[str, object]] = {}
    checks_total += 6
    for index, rule in enumerate(policy["namespace_rules"], start=1):
        validation_class = rule["validation_class"]
        measurement = classify(CLASS_TO_GLOB[validation_class], active_codes)
        measurements[validation_class] = measurement
        checks_passed += require(measurement["total_count"] == len(measurement["active"]) + len(measurement["quarantine"]) + len(measurement["retained_non_milestone"]), str(POLICY_JSON), f"M313-B004-MEAS-{index:02d}", f"{validation_class} classification is not exhaustive", failures)
        checks_passed += require(len(measurement["quarantine"]) > 0, str(POLICY_JSON), f"M313-B004-MEAS-{index + 3:02d}", f"{validation_class} did not quarantine any legacy surfaces", failures)

    checks_total += 3
    maximums = policy["ratchet_stage"]["maximums"]
    checks_passed += require(measurements["issue_local_checker"]["non_quarantined_count"] <= maximums["check_scripts"], str(POLICY_JSON), "M313-B004-RATCHET-01", "non-quarantined checker count exceeds stage maximum", failures)
    checks_passed += require(measurements["issue_local_readiness_runner"]["non_quarantined_count"] <= maximums["readiness_runners"], str(POLICY_JSON), "M313-B004-RATCHET-02", "non-quarantined readiness count exceeds stage maximum", failures)
    checks_passed += require(measurements["issue_local_pytest_wrapper"]["non_quarantined_count"] <= maximums["pytest_check_files"], str(POLICY_JSON), "M313-B004-RATCHET-03", "non-quarantined pytest-wrapper count exceeds stage maximum", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-b004-legacy-validation-quarantine-and-namespace-policy-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-B004-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-b004-legacy-validation-quarantine-and-namespace-policy-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-B004-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-b004-lane-b-readiness"' in package, str(PACKAGE_JSON), "M313-B004-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": policy["mode"],
        "contract_id": policy["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "active_milestone_codes": policy["active_milestone_codes"],
        "ratchet_stage": policy["ratchet_stage"],
        "measurements": {
            key: {
                "active_count": len(value["active"]),
                "retained_non_milestone_count": len(value["retained_non_milestone"]),
                "non_quarantined_count": value["non_quarantined_count"],
                "quarantined_count": len(value["quarantine"]),
                "total_count": value["total_count"],
                "active_samples": value["active"][:10],
                "quarantined_samples": value["quarantine"][:10],
                "retained_samples": value["retained_non_milestone"][:10],
            }
            for key, value in measurements.items()
        },
        "next_issue": "M313-B005",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-B004 legacy validation quarantine checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
