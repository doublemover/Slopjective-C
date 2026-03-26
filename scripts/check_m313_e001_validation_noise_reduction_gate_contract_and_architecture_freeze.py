#!/usr/bin/env python3
"""Checker for M313-E001 validation-noise reduction gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-E001" / "validation_noise_reduction_gate_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_validation_noise_reduction_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_e001_validation_noise_reduction_gate_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_e001_validation_noise_reduction_gate_contract_and_architecture_freeze_contract.json"
RUNNER = ROOT / "scripts" / "m313_acceptance_first_ci_runner.py"
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
    contract = json.loads(read_text(CONTRACT_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-validation-noise-reduction-gate/m313-e001-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-E001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("closeout maximums" in expectations, str(EXPECTATIONS_DOC), "M313-E001-EXP-02", "expectations missing maximums rule", failures)
    checks_passed += require("topology summary" in packet, str(PACKET_DOC), "M313-E001-PKT-01", "packet missing topology-summary focus", failures)
    checks_passed += require("Next issue: `M313-E002`." in packet, str(PACKET_DOC), "M313-E001-PKT-02", "packet missing next issue", failures)

    checks_total += 7
    checks_passed += require(contract.get("mode") == "m313-e001-validation-noise-reduction-gate-v1", str(CONTRACT_JSON), "M313-E001-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-cleanup-validation-noise-reduction-gate/m313-e001-v1", str(CONTRACT_JSON), "M313-E001-CON-02", "contract id drifted", failures)
    checks_passed += require(len(contract.get("required_predecessor_summaries", {})) == 13, str(CONTRACT_JSON), "M313-E001-CON-03", "predecessor summary count drifted", failures)
    checks_passed += require(contract.get("gate_threshold_maximums") == {"check_scripts": 558, "readiness_runners": 179, "pytest_check_files": 555}, str(CONTRACT_JSON), "M313-E001-CON-04", "gate threshold maximums drifted", failures)
    checks_passed += require(len(contract.get("required_suite_ids", [])) == 4, str(CONTRACT_JSON), "M313-E001-CON-05", "required suite ids drifted", failures)
    checks_passed += require(len(contract.get("required_bridge_ids", [])) == 4, str(CONTRACT_JSON), "M313-E001-CON-06", "required bridge ids drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M313-E002", str(CONTRACT_JSON), "M313-E001-CON-07", "next issue drifted", failures)

    observational_codes = set(contract.get("observational_predecessor_summaries", []))
    predecessor_statuses: dict[str, dict[str, object]] = {}
    for code, rel_path in contract["required_predecessor_summaries"].items():
        path = ROOT / rel_path
        checks_total += 2
        checks_passed += require(path.exists(), rel_path, f"M313-E001-SUM-{code}-EXISTS", f"missing predecessor summary for {code}", failures)
        if path.exists():
            payload = json.loads(path.read_text(encoding="utf-8"))
            predecessor_statuses[code] = {
                "ok": payload.get("ok"),
                "summary_path": rel_path,
                "observational": code in observational_codes,
            }
            if code in observational_codes:
                checks_passed += require(True, rel_path, f"M313-E001-SUM-{code}-OK", "observational predecessor summary accepted", failures)
            else:
                checks_passed += require(payload.get("ok") is True, rel_path, f"M313-E001-SUM-{code}-OK", f"predecessor summary for {code} must report ok=true", failures)

    topo_path = ROOT / contract["required_topology_summary"]
    rc, topo, error = run_json([sys.executable, str(RUNNER), "--stage", "topology", "--summary-out", str(topo_path)])
    checks_total += 8
    checks_passed += require(rc == 0, str(RUNNER), "M313-E001-TOPO-01", f"topology rerun failed: {error}", failures)
    checks_passed += require(isinstance(topo, dict) and topo.get("ok") is True, str(RUNNER), "M313-E001-TOPO-02", "topology summary not ok", failures)
    counts = topo.get("budget_snapshot", {}).get("non_quarantined_counts", {}) if isinstance(topo, dict) else {}
    maxes = contract["gate_threshold_maximums"]
    checks_passed += require(counts.get("check_scripts", 10**9) <= maxes["check_scripts"], str(RUNNER), "M313-E001-TOPO-03", "check_scripts threshold exceeded", failures)
    checks_passed += require(counts.get("readiness_runners", 10**9) <= maxes["readiness_runners"], str(RUNNER), "M313-E001-TOPO-04", "readiness threshold exceeded", failures)
    checks_passed += require(counts.get("pytest_check_files", 10**9) <= maxes["pytest_check_files"], str(RUNNER), "M313-E001-TOPO-05", "pytest threshold exceeded", failures)
    checks_passed += require(topo.get("budget_snapshot", {}).get("active_exception_count") == 0, str(RUNNER), "M313-E001-TOPO-06", "active exceptions must be zero", failures)
    checks_passed += require(sorted(entry["suite_id"] for entry in topo.get("suite_results", [])) == sorted(contract["required_suite_ids"]), str(RUNNER), "M313-E001-TOPO-07", "required suite ids drifted in topology summary", failures)
    bridge_owners = {entry["bridge_id"]: entry.get("deprecation_owner_issue") for entry in topo.get("bridge_results", [])}
    checks_passed += require(bridge_owners == contract["required_bridge_deprecation_owners"], str(RUNNER), "M313-E001-TOPO-08", "bridge deprecation owners drifted", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-e001-validation-noise-reduction-gate-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-E001-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-e001-validation-noise-reduction-gate-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-E001-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-e001-lane-e-readiness"' in package, str(PACKAGE_JSON), "M313-E001-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "predecessor_statuses": predecessor_statuses,
        "gate_threshold_maximums": contract["gate_threshold_maximums"],
        "observed_non_quarantined_counts": counts,
        "next_issue": "M313-E002",
        "failures": [finding.__dict__ for finding in failures]
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-E001 validation-noise gate checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
