#!/usr/bin/env python3
"""Checker for M314-B002 public package script collapse."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-B002" / "public_package_script_collapse_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_public_package_script_collapse_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b002_public_package_script_collapse_core_feature_implementation_packet.md"
SURFACE_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b002_public_package_script_collapse_core_feature_implementation_surface.json"
PACKAGE_JSON = ROOT / "package.json"
README = ROOT / "README.md"

REMOVED_PREFIXES = (
    "check:objc3c:m",
    "test:tooling:m",
    "check:compiler-closeout:m",
    "run:objc3c:",
    "plan:compiler-dispatch:",
    "refresh:compiler-dispatch:",
    "dev:objc3c:",
)


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
    surface = json.loads(read_text(SURFACE_JSON))
    package_text = read_text(PACKAGE_JSON)
    package = json.loads(package_text)
    scripts = package.get("scripts", {})
    readme = read_text(README)
    package_surface = package.get("objc3cCommandSurface", {})

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-public-package-script-collapse/m314-b002-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-B002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("remove the broader compatibility alias mass" in expectations, str(EXPECTATIONS_DOC), "M314-B002-EXP-02", "expectations missing alias-removal rule", failures)
    checks_passed += require("public command surface block" in packet, str(PACKET_DOC), "M314-B002-PKT-01", "packet missing package-surface block", failures)
    checks_passed += require("Next issue: `M314-B003`." in packet, str(PACKET_DOC), "M314-B002-PKT-02", "packet missing next issue", failures)

    checks_total += 10
    checks_passed += require(surface.get("mode") == "m314-b002-public-package-script-collapse-v1", str(SURFACE_JSON), "M314-B002-SUR-01", "mode drifted", failures)
    checks_passed += require(surface.get("contract_id") == "objc3c-cleanup-public-package-script-collapse/m314-b002-v1", str(SURFACE_JSON), "M314-B002-SUR-02", "contract id drifted", failures)
    checks_passed += require(surface.get("depends_on") == "M314-B001", str(SURFACE_JSON), "M314-B002-SUR-03", "dependency drifted", failures)
    checks_passed += require(surface.get("package_json_public_surface_key") == "objc3cCommandSurface", str(SURFACE_JSON), "M314-B002-SUR-04", "package surface key drifted", failures)
    checks_passed += require(len(surface.get("public_scripts", [])) <= 25, str(SURFACE_JSON), "M314-B002-SUR-05", "public script count exceeds budget", failures)
    checks_passed += require(surface.get("public_script_budget_maximum") == 25, str(SURFACE_JSON), "M314-B002-SUR-06", "budget maximum drifted", failures)
    checks_passed += require(surface.get("compatibility_policy", {}).get("legacy_alias_mass_retained") is False, str(SURFACE_JSON), "M314-B002-SUR-07", "alias-retirement state drifted", failures)
    checks_passed += require(surface.get("compatibility_policy", {}).get("package_surface_fully_trimmed") is True, str(SURFACE_JSON), "M314-B002-SUR-08", "trimmed-surface flag drifted", failures)
    checks_passed += require(any(entry.get("status") == "implemented" for entry in surface.get("readme_migrations", [])), str(SURFACE_JSON), "M314-B002-SUR-09", "implemented README migration missing", failures)
    checks_passed += require(surface.get("next_issue") == "M314-B003", str(SURFACE_JSON), "M314-B002-SUR-10", "next issue drifted", failures)

    checks_total += 7
    checks_passed += require(package_surface.get("mode") == surface.get("mode"), str(PACKAGE_JSON), "M314-B002-PKG-01", "package public-surface mode drifted", failures)
    checks_passed += require(package_surface.get("contractId") == surface.get("contract_id"), str(PACKAGE_JSON), "M314-B002-PKG-02", "package public-surface contract drifted", failures)
    checks_passed += require(package_surface.get("publicScripts") == surface.get("public_scripts"), str(PACKAGE_JSON), "M314-B002-PKG-03", "package publicScripts drifted", failures)
    checks_passed += require(package_surface.get("budgetMaximum") == 25, str(PACKAGE_JSON), "M314-B002-PKG-04", "package budget maximum drifted", failures)
    checks_passed += require(package_surface.get("totalScriptCount") == len(scripts), str(PACKAGE_JSON), "M314-B002-PKG-05", "package total script count drifted", failures)
    checks_passed += require("objc3cCommandCompatibility" not in package, str(PACKAGE_JSON), "M314-B002-PKG-06", "compatibility block should be removed", failures)
    checks_passed += require(len(scripts) <= 25, str(PACKAGE_JSON), "M314-B002-PKG-07", "trimmed package script surface exceeds budget", failures)

    for idx, script_name in enumerate(surface.get("public_scripts", []), start=1):
        checks_total += 1
        checks_passed += require(script_name in scripts, str(PACKAGE_JSON), f"M314-B002-PKG-{idx+20:02d}", f"missing public script {script_name}", failures)

    removed_hits = sorted(name for name in scripts if name.startswith(REMOVED_PREFIXES))
    checks_total += 2
    checks_passed += require(not removed_hits, str(PACKAGE_JSON), "M314-B002-PKG-REM-01", f"removed alias families still present: {removed_hits}", failures)
    checks_passed += require("No additional package-script compatibility aliases remain supported." in readme, str(README), "M314-B002-RD-01", "README missing alias-removal note", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": surface["mode"],
        "contract_id": surface["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "public_scripts": surface["public_scripts"],
        "total_scripts": len(scripts),
        "next_issue": "M314-B003",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-B002 public package script collapse checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
