#!/usr/bin/env python3
"""Checker for M314-B001 public-versus-internal command model."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-B001" / "public_internal_command_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_public_versus_internal_command_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b001_public_versus_internal_command_model_contract_and_architecture_freeze_packet.md"
MODEL_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b001_public_versus_internal_command_model_contract_and_architecture_freeze_model.json"
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
    model = json.loads(read_text(MODEL_JSON))
    package_text = read_text(PACKAGE_JSON)
    scripts = json.loads(package_text).get("scripts", {})
    readme = read_text(README)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-public-internal-command-model/m314-b001-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-B001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Legacy milestone-local `check:*` and `test:tooling:*` aliases are compatibility surfaces only" in expectations, str(EXPECTATIONS_DOC), "M314-B001-EXP-02", "expectations missing compatibility statement", failures)
    checks_passed += require("Public operator layer:" in packet, str(PACKET_DOC), "M314-B001-PKT-01", "packet missing public operator layer", failures)
    checks_passed += require("Next issue: `M314-B002`." in packet, str(PACKET_DOC), "M314-B001-PKT-02", "packet missing next issue", failures)

    checks_total += 9
    checks_passed += require(model.get("mode") == "m314-b001-public-internal-command-model-v1", str(MODEL_JSON), "M314-B001-MOD-01", "mode drifted", failures)
    checks_passed += require(model.get("contract_id") == "objc3c-cleanup-public-internal-command-model/m314-b001-v1", str(MODEL_JSON), "M314-B001-MOD-02", "contract id drifted", failures)
    checks_passed += require(model.get("depends_on") == "M314-A002", str(MODEL_JSON), "M314-B001-MOD-03", "dependency drifted", failures)
    checks_passed += require(model.get("public_operator_layer", {}).get("budget_maximum") == 25, str(MODEL_JSON), "M314-B001-MOD-04", "budget maximum drifted", failures)
    checks_passed += require(model.get("public_operator_layer", {}).get("families") == ["build", "compile", "lint", "test", "package", "tool", "proof"], str(MODEL_JSON), "M314-B001-MOD-05", "allowed public families drifted", failures)
    checks_passed += require(model.get("ci_only_layer", {}).get("workflow_count") == 9, str(MODEL_JSON), "M314-B001-MOD-06", "workflow count drifted", failures)
    checks_passed += require(len(model.get("temporary_compatibility_surface", {}).get("categories", [])) == 3, str(MODEL_JSON), "M314-B001-MOD-07", "compatibility categories drifted", failures)
    checks_passed += require("new public dev:* commands" in model.get("prohibited_patterns", []), str(MODEL_JSON), "M314-B001-MOD-08", "prohibited pattern drifted", failures)
    checks_passed += require(model.get("next_issue") == "M314-B002", str(MODEL_JSON), "M314-B001-MOD-09", "next issue drifted", failures)

    public_entrypoints = model["public_operator_layer"]["current_documented_entrypoints"]
    for idx, command in enumerate(public_entrypoints, start=1):
        checks_total += 1
        checks_passed += require(command in scripts, str(PACKAGE_JSON), f"M314-B001-PKG-{idx:02d}", f"missing public entrypoint {command}", failures)

    checks_total += 3
    checks_passed += require("python scripts/build_site_index.py" in readme, str(README), "M314-B001-RD-01", "README direct python leak missing", failures)
    checks_passed += require("pwsh -NoProfile -ExecutionPolicy Bypass -File .\\scripts\\check_objc3c_native_execution_smoke.ps1" in readme, str(README), "M314-B001-RD-02", "README direct PowerShell leak missing", failures)
    checks_passed += require('"check:objc3c:m314-b001-public-versus-internal-command-model-contract-and-architecture-freeze"' in package_text, str(PACKAGE_JSON), "M314-B001-PKG-LAST-01", "package missing checker script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": model["mode"],
        "contract_id": model["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "public_entrypoints": public_entrypoints,
        "temporary_compatibility_categories": model["temporary_compatibility_surface"]["categories"],
        "next_issue": "M314-B002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-B001 public/internal command model checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
