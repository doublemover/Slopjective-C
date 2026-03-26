#!/usr/bin/env python3
"""Checker for M318-E002 governance hardening closeout matrix."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_governance_hardening_closeout_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_e002_governance_hardening_closeout_matrix_cross_lane_integration_sync_packet.md"
MATRIX_JSON = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_e002_governance_hardening_closeout_matrix_cross_lane_integration_sync_matrix.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-E002" / "governance_hardening_closeout_matrix_summary.json"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> dict:
    return json.loads(read_text(path))


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=False)


def main(argv: Sequence[str]) -> int:
    del argv
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    matrix = read_json(MATRIX_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-hardening-closeout-matrix/m318-e002-v1" in expectations, str(EXPECTATIONS_DOC), "M318-E002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("open issues are exactly `M318-E002`" in expectations, str(EXPECTATIONS_DOC), "M318-E002-EXP-02", "expectations missing open-issue condition", failures)
    checks_passed += require("Next issue: `M316-A001`." in packet, str(PACKET_DOC), "M318-E002-PKT-01", "packet missing next issue", failures)
    checks_passed += require(matrix.get("contract_id") == "objc3c-governance-hardening-closeout-matrix/m318-e002-v1", str(MATRIX_JSON), "M318-E002-MAT-01", "matrix contract id drifted", failures)

    gate_summary = read_json(ROOT / "tmp" / "reports" / "m318" / "M318-E001" / "sustainable_progress_gate_summary.json")
    topology_summary = read_json(ROOT / "tmp" / "reports" / "m318" / "governance" / "topology.json")
    proposal_summary = read_json(ROOT / "tmp" / "reports" / "m318" / "governance" / "new_work_proposal" / "publication_summary.json")

    checks_total += 5
    checks_passed += require(gate_summary.get("topology_ok") is True, str(ROOT / "tmp" / "reports" / "m318" / "M318-E001" / "sustainable_progress_gate_summary.json"), "M318-E002-GATE-01", "E001 gate topology evidence is not green", failures)
    checks_passed += require(gate_summary.get("expired_exception_ids") == [], str(ROOT / "tmp" / "reports" / "m318" / "M318-E001" / "sustainable_progress_gate_summary.json"), "M318-E002-GATE-02", "E001 gate retained expired exception ids", failures)
    checks_passed += require(topology_summary.get("ok") is True, str(ROOT / "tmp" / "reports" / "m318" / "governance" / "topology.json"), "M318-E002-TOP-01", "topology summary is not green", failures)
    checks_passed += require(topology_summary.get("alarms") == [], str(ROOT / "tmp" / "reports" / "m318" / "governance" / "topology.json"), "M318-E002-TOP-02", "topology alarms are not empty", failures)
    checks_passed += require(proposal_summary.get("publication_mode") == matrix["closeout_conditions"]["proposal_publication_mode"], str(ROOT / "tmp" / "reports" / "m318" / "governance" / "new_work_proposal" / "publication_summary.json"), "M318-E002-PROP-01", "proposal publication mode drifted", failures)

    milestone_state = run(["gh", "issue", "list", "--state", "open", "--milestone", str(matrix["milestone_number"]), "--limit", "20", "--json", "title,number"])
    open_titles = []
    if milestone_state.returncode == 0:
        open_titles = [item["title"] for item in json.loads(milestone_state.stdout)]
    checks_total += 1
    checks_passed += require(milestone_state.returncode == 0 and sorted(open_titles) == sorted(matrix["required_open_issue_titles"]), f"milestone:{matrix['milestone_number']}", "M318-E002-GH-01", f"open issue titles drifted: {open_titles}", failures)

    summary = {
        "contract_id": matrix["contract_id"],
        "milestone_number": matrix["milestone_number"],
        "open_issue_titles": open_titles,
        "gate_topology_ok": gate_summary.get("topology_ok"),
        "gate_expired_exception_ids": gate_summary.get("expired_exception_ids", []),
        "topology_ok": topology_summary.get("ok"),
        "proposal_publication_mode": proposal_summary.get("publication_mode"),
        "next_issue": matrix["next_issue"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [f.__dict__ for f in failures]
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M318-E002 governance closeout matrix checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
