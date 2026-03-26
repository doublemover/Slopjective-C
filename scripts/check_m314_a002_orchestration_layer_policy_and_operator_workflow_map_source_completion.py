#!/usr/bin/env python3
"""Checker for M314-A002 orchestration-layer policy and operator workflow map."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-A002" / "orchestration_layer_workflow_map_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_orchestration_layer_policy_and_operator_workflow_map_source_completion_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_a002_orchestration_layer_policy_and_operator_workflow_map_source_completion_packet.md"
MAP_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_a002_orchestration_layer_policy_and_operator_workflow_map_source_completion_map.json"
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
    payload = json.loads(read_text(MAP_JSON))
    package_text = read_text(PACKAGE_JSON)
    package = json.loads(package_text)
    readme = read_text(README)
    scripts = package.get("scripts", {})

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-orchestration-layer-workflow-map/m314-a002-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-A002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("primary public operator-facing entrypoint layer" in expectations, str(EXPECTATIONS_DOC), "M314-A002-EXP-02", "expectations missing public-layer rule", failures)
    checks_passed += require("Public operator layer: `package.json` scripts." in packet, str(PACKET_DOC), "M314-A002-PKT-01", "packet missing public-layer summary", failures)
    checks_passed += require("Next issue: `M314-B001`." in packet, str(PACKET_DOC), "M314-A002-PKT-02", "packet missing next issue", failures)

    checks_total += 10
    checks_passed += require(payload.get("mode") == "m314-a002-orchestration-layer-workflow-map-v1", str(MAP_JSON), "M314-A002-MAP-01", "mode drifted", failures)
    checks_passed += require(payload.get("contract_id") == "objc3c-cleanup-orchestration-layer-workflow-map/m314-a002-v1", str(MAP_JSON), "M314-A002-MAP-02", "contract id drifted", failures)
    checks_passed += require(payload.get("depends_on") == "M314-A001", str(MAP_JSON), "M314-A002-MAP-03", "dependency drifted", failures)
    checks_passed += require(payload.get("primary_public_layer") == "package.json scripts", str(MAP_JSON), "M314-A002-MAP-04", "primary public layer drifted", failures)
    checks_passed += require(len(payload.get("workflow_map", [])) == 4, str(MAP_JSON), "M314-A002-MAP-05", "workflow count drifted", failures)
    checks_passed += require(len(payload.get("documented_transition_leaks", [])) == 3, str(MAP_JSON), "M314-A002-MAP-06", "transition leak count drifted", failures)
    checks_passed += require(payload.get("prototype_surface_owner_issue") == "M314-B005", str(MAP_JSON), "M314-A002-MAP-07", "prototype owner drifted", failures)
    checks_passed += require(payload.get("next_issue") == "M314-B001", str(MAP_JSON), "M314-A002-MAP-08", "next issue drifted", failures)
    checks_passed += require(".github/workflows/task-hygiene.yml" in payload.get("layer_policy", {}).get("ci_only_layer", {}).get("workflow_files", []), str(MAP_JSON), "M314-A002-MAP-09", "workflow map missing task-hygiene", failures)
    checks_passed += require("test:objc3c:execution-smoke" in payload.get("workflow_map", [])[3].get("public_entrypoints", []), str(MAP_JSON), "M314-A002-MAP-10", "execution-smoke public entrypoint drifted", failures)

    checks_total += 7
    checks_passed += require("build:objc3c-native" in scripts, str(PACKAGE_JSON), "M314-A002-PKG-01", "package missing native build public entrypoint", failures)
    checks_passed += require("compile:objc3c" in scripts, str(PACKAGE_JSON), "M314-A002-PKG-02", "package missing compile entrypoint", failures)
    checks_passed += require("build:spec" in scripts, str(PACKAGE_JSON), "M314-A002-PKG-03", "package missing spec build entrypoint", failures)
    checks_passed += require("lint:spec" in scripts, str(PACKAGE_JSON), "M314-A002-PKG-04", "package missing spec lint entrypoint", failures)
    checks_passed += require("test:objc3c:execution-smoke" in scripts, str(PACKAGE_JSON), "M314-A002-PKG-05", "package missing execution smoke entrypoint", failures)
    checks_passed += require('"check:objc3c:m314-a002-orchestration-layer-policy-and-operator-workflow-map-source-completion"' in package_text, str(PACKAGE_JSON), "M314-A002-PKG-06", "package missing checker script", failures)
    checks_passed += require('"check:objc3c:m314-a002-lane-a-readiness"' in package_text, str(PACKAGE_JSON), "M314-A002-PKG-07", "package missing readiness script", failures)

    checks_total += 3
    checks_passed += require("python scripts/build_site_index.py" in readme, str(README), "M314-A002-RD-01", "README missing current direct Python leak", failures)
    checks_passed += require("pwsh -NoProfile -ExecutionPolicy Bypass -File .\\scripts\\check_objc3c_native_execution_smoke.ps1" in readme, str(README), "M314-A002-RD-02", "README missing current direct PowerShell leak", failures)
    checks_passed += require("npm run build:objc3c-native" in readme, str(README), "M314-A002-RD-03", "README missing public build workflow", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": payload["mode"],
        "contract_id": payload["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "primary_public_layer": payload["primary_public_layer"],
        "workflow_ids": [entry["workflow_id"] for entry in payload["workflow_map"]],
        "transition_leak_commands": [entry["command"] for entry in payload["documented_transition_leaks"]],
        "next_issue": "M314-B001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-A002 orchestration workflow map checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
