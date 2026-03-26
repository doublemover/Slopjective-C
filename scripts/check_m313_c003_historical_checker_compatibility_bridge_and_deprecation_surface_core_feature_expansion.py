#!/usr/bin/env python3
"""Checker for M313-C003 historical checker compatibility bridge."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-C003" / "historical_checker_compatibility_bridge_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_packet.md"
PLAN_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_plan.json"
SCHEMA_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_schema.json"
BRIDGE_TOOL = ROOT / "scripts" / "m313_historical_checker_compatibility_bridge.py"
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


def run_json(command: list[str]) -> tuple[int, dict[str, object] | None, str]:
    completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)
    if completed.returncode != 0:
        return completed.returncode, None, completed.stderr or completed.stdout
    try:
        return completed.returncode, json.loads(completed.stdout), ""
    except json.JSONDecodeError as exc:
        return 1, None, str(exc)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    plan = json.loads(read_text(PLAN_JSON))
    schema = json.loads(read_text(SCHEMA_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-historical-checker-compatibility-bridge/m313-c003-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-C003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Emit `M313-C001` compatibility-bridge summaries" in expectations, str(EXPECTATIONS_DOC), "M313-C003-EXP-02", "expectations missing bridge-summary requirement", failures)
    checks_passed += require("schema-compliant compatibility summaries" in packet, str(PACKET_DOC), "M313-C003-PKT-01", "packet missing compatibility-summary focus", failures)
    checks_passed += require("Next issue: `M313-D001`." in packet, str(PACKET_DOC), "M313-C003-PKT-02", "packet missing next issue", failures)

    checks_total += 4
    checks_passed += require(plan.get("mode") == "m313-c003-historical-checker-compatibility-bridge-v1", str(PLAN_JSON), "M313-C003-CON-01", "mode drifted", failures)
    checks_passed += require(plan.get("contract_id") == "objc3c-cleanup-historical-checker-compatibility-bridge/m313-c003-v1", str(PLAN_JSON), "M313-C003-CON-02", "contract id drifted", failures)
    checks_passed += require(plan.get("default_report_root") == "tmp/reports/m313/compatibility/<bridge_id>/summary.json", str(PLAN_JSON), "M313-C003-CON-03", "default report root drifted", failures)
    checks_passed += require(plan.get("next_issue") == "M313-D001", str(PLAN_JSON), "M313-C003-CON-04", "next issue drifted", failures)

    bridge_artifacts: list[dict[str, object]] = []
    for index, bridge in enumerate(plan["bridge_families"], start=1):
        bridge_id = bridge["bridge_id"]
        summary_path = ROOT / "tmp" / "reports" / "m313" / "compatibility" / bridge_id / "summary.json"
        rc, payload, error = run_json([sys.executable, str(BRIDGE_TOOL), "--run-bridge", bridge_id, "--summary-out", str(summary_path)])
        checks_total += 7
        checks_passed += require(rc == 0, str(BRIDGE_TOOL), f"M313-C003-BRG-{index:02d}", f"run-bridge failed for {bridge_id}: {error}", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("schema_version") == schema.get("schema_version"), str(BRIDGE_TOOL), f"M313-C003-BRG-{index+10:02d}", f"bridge {bridge_id} missing schema version", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("contract_id") == schema.get("contract_id"), str(BRIDGE_TOOL), f"M313-C003-BRG-{index+20:02d}", f"bridge {bridge_id} contract id drifted", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("suite_id") == bridge.get("suite_id"), str(BRIDGE_TOOL), f"M313-C003-BRG-{index+30:02d}", f"bridge {bridge_id} suite id drifted", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("artifact_class") == "compatibility_bridge_summary", str(BRIDGE_TOOL), f"M313-C003-BRG-{index+40:02d}", f"bridge {bridge_id} artifact class drifted", failures)
        measures = payload.get("measurements", {}) if isinstance(payload, dict) else {}
        checks_passed += require(measures.get("legacy_wrappers_observed", 0) > 0 and measures.get("legacy_wrappers_remaining", 0) > 0, str(BRIDGE_TOOL), f"M313-C003-BRG-{index+50:02d}", f"bridge {bridge_id} did not observe legacy wrappers", failures)
        checks_passed += require(measures.get("deprecation_owner_issue") == bridge.get("deprecation_owner_issue"), str(BRIDGE_TOOL), f"M313-C003-BRG-{index+60:02d}", f"bridge {bridge_id} deprecation owner drifted", failures)
        bridge_artifacts.append(payload or {})

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-c003-historical-checker-compatibility-bridge-and-deprecation-surface-core-feature-expansion"' in package, str(PACKAGE_JSON), "M313-C003-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-c003-historical-checker-compatibility-bridge-and-deprecation-surface-core-feature-expansion"' in package, str(PACKAGE_JSON), "M313-C003-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-c003-lane-c-readiness"' in package, str(PACKAGE_JSON), "M313-C003-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": plan["mode"],
        "contract_id": plan["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "bridge_ids": [entry["bridge_id"] for entry in plan["bridge_families"]],
        "next_issue": "M313-D001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-C003 historical checker compatibility bridge checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
