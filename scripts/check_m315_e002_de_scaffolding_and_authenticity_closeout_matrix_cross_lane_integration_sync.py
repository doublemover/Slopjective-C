#!/usr/bin/env python3
"""Checker for M315-E002 de-scaffolding and authenticity closeout matrix."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence


ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-E002" / "de_scaffolding_authenticity_closeout_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_de_scaffolding_and_authenticity_closeout_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_e002_de_scaffolding_and_authenticity_closeout_matrix_cross_lane_integration_sync_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_e002_de_scaffolding_and_authenticity_closeout_matrix_cross_lane_integration_sync_contract.json"
B005_SUMMARY = ROOT / "tmp" / "reports" / "m315" / "M315-B005" / "comment_constexpr_contract_string_decontamination_summary.json"
D001_SUMMARY = ROOT / "tmp" / "reports" / "m315" / "M315-D001" / "source_hygiene_proof_policy_enforcement_contract_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m315" / "M315-D002" / "anti_noise_enforcement_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m315" / "M315-E001" / "source_hygiene_proof_hygiene_gate_summary.json"


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


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise ValueError(f"expected JSON object at {path}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_checker(path: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run([sys.executable, str(path)], cwd=ROOT, capture_output=True, text=True, check=False)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = read_json(CONTRACT_JSON)

    predecessor_results = {}
    for rel_path in contract["predecessor_checkers"]:
        predecessor_results[rel_path] = run_checker(ROOT / rel_path)

    b005 = read_json(B005_SUMMARY)
    d001 = read_json(D001_SUMMARY)
    d002 = read_json(D002_SUMMARY)
    e001 = read_json(E001_SUMMARY)

    checks_total += 6
    checks_passed += require("objc3c.cleanup.de-scaffolding.authenticity.closeout.matrix/m315-e002-v1" in expectations, str(EXPECTATIONS_DOC), "M315-E002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("every predecessor `M315-A001` through `M315-E001` checker still passes" in expectations, str(EXPECTATIONS_DOC), "M315-E002-EXP-02", "expectations missing predecessor checker truth", failures)
    checks_passed += require("rerun every predecessor `M315` checker from lane A through lane E" in packet, str(PACKET_DOC), "M315-E002-PKT-01", "packet missing full rerun requirement", failures)
    checks_passed += require(contract.get("required_b005_match_count") == 57, str(CONTRACT_JSON), "M315-E002-CON-01", "required B005 match count drifted", failures)
    checks_passed += require(contract.get("required_stable_identifier_family") == "advanced_integration_closeout_signoff", str(CONTRACT_JSON), "M315-E002-CON-02", "required stable identifier family drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M318-A001", str(CONTRACT_JSON), "M315-E002-CON-03", "next issue drifted", failures)

    checks_total += len(predecessor_results) + 7
    for rel_path, result in predecessor_results.items():
        checks_passed += require(result.returncode == 0, rel_path, "M315-E002-PRD-01", f"predecessor checker failed: {result.stderr.strip()}", failures)
    checks_passed += require(b005.get("match_count") == contract["required_b005_match_count"], str(B005_SUMMARY), "M315-E002-MAT-01", "B005 match count drifted", failures)
    checks_passed += require(b005.get("residual_class_counts") == contract["required_remaining_residual_classes"], str(B005_SUMMARY), "M315-E002-MAT-02", "B005 residual class set drifted", failures)
    checks_passed += require(d001.get("current_b005_zero_target_residuals") == contract["required_current_zero_target_residuals"], str(D001_SUMMARY), "M315-E002-MAT-03", "D001 current zero-target residuals drifted", failures)
    checks_passed += require(d002.get("stable_identifier_family") == contract["required_stable_identifier_family"], str(D002_SUMMARY), "M315-E002-MAT-04", "D002 stable identifier family drifted", failures)
    checks_passed += require(all(not hits for hits in d002.get("target_file_zero_target_hits", {}).values()), str(D002_SUMMARY), "M315-E002-MAT-05", "D002 zero-target hits drifted", failures)
    checks_passed += require(e001.get("remaining_residual_classes") == contract["required_remaining_residual_classes"], str(E001_SUMMARY), "M315-E002-MAT-06", "E001 residual class set drifted", failures)
    checks_passed += require(e001.get("stable_identifier_family") == contract["required_stable_identifier_family"], str(E001_SUMMARY), "M315-E002-MAT-07", "E001 stable identifier family drifted", failures)

    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "predecessor_checker_returncodes": {key: result.returncode for key, result in predecessor_results.items()},
        "final_b005_match_count": b005.get("match_count"),
        "final_remaining_residual_classes": b005.get("residual_class_counts"),
        "final_current_zero_target_residuals": d001.get("current_b005_zero_target_residuals"),
        "stable_identifier_family": d002.get("stable_identifier_family"),
        "next_milestone": contract.get("next_milestone"),
        "next_issue": contract.get("next_issue"),
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-E002 de-scaffolding/authenticity closeout checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
