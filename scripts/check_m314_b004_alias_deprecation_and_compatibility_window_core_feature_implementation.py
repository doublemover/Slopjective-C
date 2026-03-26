#!/usr/bin/env python3
"""Checker for M314-B004 alias deprecation and compatibility window."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-B004" / "alias_deprecation_compatibility_window_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_alias_deprecation_and_compatibility_window_core_feature_implementation_b004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b004_alias_deprecation_and_compatibility_window_core_feature_implementation_packet.md"
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b004_alias_deprecation_and_compatibility_window_core_feature_implementation_registry.json"
PACKAGE_JSON = ROOT / "package.json"
README = ROOT / "README.md"


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
    registry = json.loads(read_text(REGISTRY_JSON))
    package_text = read_text(PACKAGE_JSON)
    package = json.loads(package_text)
    compatibility = package.get("objc3cCommandCompatibility", {})
    readme = read_text(README)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-alias-deprecation-compatibility-window/m314-b004-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-B004-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("machine-readable in `package.json`" in expectations, str(EXPECTATIONS_DOC), "M314-B004-EXP-02", "expectations missing package rule", failures)
    checks_passed += require("machine-readable compatibility registry" in packet, str(PACKET_DOC), "M314-B004-PKT-01", "packet missing compatibility registry", failures)
    checks_passed += require("Next issue: `M314-B005`." in packet, str(PACKET_DOC), "M314-B004-PKT-02", "packet missing next issue", failures)

    checks_total += 9
    checks_passed += require(registry.get("mode") == "m314-b004-alias-deprecation-compatibility-window-v1", str(REGISTRY_JSON), "M314-B004-REG-01", "mode drifted", failures)
    checks_passed += require(registry.get("contract_id") == "objc3c-cleanup-alias-deprecation-compatibility-window/m314-b004-v1", str(REGISTRY_JSON), "M314-B004-REG-02", "contract id drifted", failures)
    checks_passed += require(registry.get("depends_on") == "M314-B003", str(REGISTRY_JSON), "M314-B004-REG-03", "dependency drifted", failures)
    checks_passed += require(registry.get("no_new_legacy_alias_families") is True, str(REGISTRY_JSON), "M314-B004-REG-04", "no-new-growth rule drifted", failures)
    checks_passed += require(len(registry.get("compatibility_families", [])) == 4, str(REGISTRY_JSON), "M314-B004-REG-05", "family count drifted", failures)
    checks_passed += require(registry.get("prototype_surface_owner_issue") == "M314-B005", str(REGISTRY_JSON), "M314-B004-REG-06", "prototype owner drifted", failures)
    checks_passed += require(registry.get("gate_owner_issues") == ["M314-E001", "M314-E002"], str(REGISTRY_JSON), "M314-B004-REG-07", "gate owner set drifted", failures)
    checks_passed += require(registry.get("next_issue") == "M314-B005", str(REGISTRY_JSON), "M314-B004-REG-08", "next issue drifted", failures)
    checks_passed += require("must not promote compatibility aliases" in registry.get("public_documentation_rule", ""), str(REGISTRY_JSON), "M314-B004-REG-09", "documentation rule drifted", failures)

    checks_total += 5
    checks_passed += require(compatibility.get("mode") == registry.get("mode"), str(PACKAGE_JSON), "M314-B004-PKG-01", "package compatibility mode drifted", failures)
    checks_passed += require(compatibility.get("contractId") == registry.get("contract_id"), str(PACKAGE_JSON), "M314-B004-PKG-02", "package compatibility contract drifted", failures)
    checks_passed += require(compatibility.get("noNewLegacyAliasFamilies") is True, str(PACKAGE_JSON), "M314-B004-PKG-03", "package no-new-growth drifted", failures)
    checks_passed += require(len(compatibility.get("families", [])) == 4, str(PACKAGE_JSON), "M314-B004-PKG-04", "package compatibility family count drifted", failures)
    checks_passed += require(compatibility.get("prototypeSurfaceOwnerIssue") == "M314-B005", str(PACKAGE_JSON), "M314-B004-PKG-05", "package prototype owner drifted", failures)

    checks_total += 3
    checks_passed += require("All other package scripts are compatibility or internal surfaces" in readme, str(README), "M314-B004-RD-01", "README missing compatibility note", failures)
    checks_passed += require('"check:objc3c:m314-b004-alias-deprecation-and-compatibility-window-core-feature-implementation"' in package_text, str(PACKAGE_JSON), "M314-B004-PKG-LAST-01", "package missing checker script", failures)
    checks_passed += require('"check:objc3c:m314-b004-lane-b-readiness"' in package_text, str(PACKAGE_JSON), "M314-B004-PKG-LAST-02", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": registry["mode"],
        "contract_id": registry["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "compatibility_family_ids": [entry["family_id"] for entry in registry["compatibility_families"]],
        "next_issue": "M314-B005",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-B004 alias compatibility checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
