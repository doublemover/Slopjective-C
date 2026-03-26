#!/usr/bin/env python3
"""Checker for M314-B003 build/test/doc workflow runner unification."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-B003" / "build_test_doc_workflow_runner_unification_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_build_test_doc_workflow_runner_unification_core_feature_implementation_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b003_build_test_doc_workflow_runner_unification_core_feature_implementation_packet.md"
PLAN_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b003_build_test_doc_workflow_runner_unification_core_feature_implementation_plan.json"
PACKAGE_JSON = ROOT / "package.json"
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"


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
    plan = json.loads(read_text(PLAN_JSON))
    package_text = read_text(PACKAGE_JSON)
    package = json.loads(package_text)
    scripts = package.get("scripts", {})

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-build-test-doc-runner-unification/m314-b003-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-B003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("shared Python workflow runner" in expectations, str(EXPECTATIONS_DOC), "M314-B003-EXP-02", "expectations missing shared runner requirement", failures)
    checks_passed += require("Add `scripts/objc3c_public_workflow_runner.py`." in packet, str(PACKET_DOC), "M314-B003-PKT-01", "packet missing runner addition", failures)
    checks_passed += require("Next issue: `M314-B004`." in packet, str(PACKET_DOC), "M314-B003-PKT-02", "packet missing next issue", failures)

    checks_total += 8
    checks_passed += require(plan.get("mode") == "m314-b003-build-test-doc-runner-unification-v1", str(PLAN_JSON), "M314-B003-PLN-01", "mode drifted", failures)
    checks_passed += require(plan.get("contract_id") == "objc3c-cleanup-build-test-doc-runner-unification/m314-b003-v1", str(PLAN_JSON), "M314-B003-PLN-02", "contract id drifted", failures)
    checks_passed += require(plan.get("runner_script") == "scripts/objc3c_public_workflow_runner.py", str(PLAN_JSON), "M314-B003-PLN-03", "runner script drifted", failures)
    checks_passed += require(len(plan.get("public_script_routes", {})) == 17, str(PLAN_JSON), "M314-B003-PLN-04", "route count drifted", failures)
    checks_passed += require(len(plan.get("implementation_details", {}).get("powershell_scripts", [])) >= 7, str(PLAN_JSON), "M314-B003-PLN-05", "powershell implementation coverage drifted", failures)
    checks_passed += require("scripts/build_site_index.py" in plan.get("implementation_details", {}).get("python_scripts", []), str(PLAN_JSON), "M314-B003-PLN-06", "python implementation coverage drifted", failures)
    checks_passed += require(plan.get("next_issue") == "M314-B004", str(PLAN_JSON), "M314-B003-PLN-07", "next issue drifted", failures)
    checks_passed += require(RUNNER.exists(), str(RUNNER), "M314-B003-PLN-08", "runner script missing", failures)

    route_map = plan["public_script_routes"]
    for idx, (script_name, action) in enumerate(route_map.items(), start=1):
        checks_total += 1
        command = scripts.get(script_name, "")
        checks_passed += require(command == f"python scripts/objc3c_public_workflow_runner.py {action}" or command == f"python scripts/objc3c_public_workflow_runner.py {action} --", str(PACKAGE_JSON), f"M314-B003-PKG-{idx:02d}", f"public script {script_name} not routed through unified runner", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m314-b003-build-test-doc-workflow-runner-unification-core-feature-implementation"' in package_text, str(PACKAGE_JSON), "M314-B003-PKG-LAST-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m314-b003-build-test-doc-workflow-runner-unification-core-feature-implementation"' in package_text, str(PACKAGE_JSON), "M314-B003-PKG-LAST-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m314-b003-lane-b-readiness"' in package_text, str(PACKAGE_JSON), "M314-B003-PKG-LAST-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": plan["mode"],
        "contract_id": plan["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "runner_script": plan["runner_script"],
        "public_script_routes": route_map,
        "next_issue": "M314-B004",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-B003 workflow runner unification checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
