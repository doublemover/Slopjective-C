#!/usr/bin/env python3
"""Checker for M318-E001 sustainable progress gate."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_sustainable_progress_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_e001_sustainable_progress_gate_contract_and_architecture_freeze_packet.md"
GATE_JSON = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_e001_sustainable_progress_gate_contract_and_architecture_freeze_gate.json"
GUARD = ROOT / "scripts" / "m318_governance_guard.py"
PROPOSAL_TOOL = ROOT / "tmp" / "github-publish" / "m318_governance" / "publish_new_work_proposal.py"
PROPOSAL_TEMPLATE = ROOT / "tmp" / "planning" / "m318_governance" / "new_work_proposal_template.json"
PROPOSAL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "m318_governance" / "new_work_proposal_sample.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-E001" / "sustainable_progress_gate_summary.json"


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
    gate = read_json(GATE_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-sustainable-progress-gate/m318-e001-v1" in expectations, str(EXPECTATIONS_DOC), "M318-E001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("exactly `M318-E001` and `M318-E002`" in expectations, str(EXPECTATIONS_DOC), "M318-E001-EXP-02", "expectations missing open-issue gate", failures)
    checks_passed += require("M318-E002" in packet, str(PACKET_DOC), "M318-E001-PKT-01", "packet missing next issue", failures)
    checks_passed += require(gate.get("contract_id") == "objc3c-governance-sustainable-progress-gate/m318-e001-v1", str(GATE_JSON), "M318-E001-GATE-01", "gate contract id drifted", failures)

    topology = run([sys.executable, str(GUARD), "--stage", "topology"])
    proposal = run([sys.executable, str(PROPOSAL_TOOL), "--proposal", str(PROPOSAL_FIXTURE), "--template", str(PROPOSAL_TEMPLATE)])
    checks_total += 2
    checks_passed += require(topology.returncode == 0, str(GUARD), "M318-E001-RUN-01", f"topology stage failed: {(topology.stderr or topology.stdout).strip()}", failures)
    checks_passed += require(proposal.returncode == 0, str(PROPOSAL_TOOL), "M318-E001-RUN-02", f"proposal tool failed: {(proposal.stderr or proposal.stdout).strip()}", failures)

    topology_payload = read_json(ROOT / "tmp" / "reports" / "m318" / "governance" / "topology.json")
    exception_payload = read_json(ROOT / "tmp" / "reports" / "m318" / "governance" / "exception_registry.json")
    proposal_payload = read_json(ROOT / "tmp" / "reports" / "m318" / "governance" / "new_work_proposal" / "publication_summary.json")

    checks_total += 4
    checks_passed += require(topology_payload.get("ok") is True, "tmp/reports/m318/governance/topology.json", "M318-E001-TOP-01", "topology is not green", failures)
    checks_passed += require(topology_payload.get("alarms") == [], "tmp/reports/m318/governance/topology.json", "M318-E001-TOP-02", "topology alarms are not empty", failures)
    checks_passed += require(exception_payload.get("expired_exception_ids") == [], "tmp/reports/m318/governance/exception_registry.json", "M318-E001-EXC-01", "expired exception ids are not empty", failures)
    checks_passed += require(proposal_payload.get("publication_mode") == gate["gate_conditions"]["proposal_publication_mode"], "tmp/reports/m318/governance/new_work_proposal/publication_summary.json", "M318-E001-PROP-01", "proposal publication mode drifted", failures)

    milestone_state = run(["gh", "issue", "list", "--state", "open", "--milestone", str(gate["milestone_number"]), "--limit", "20", "--json", "title"])
    titles = []
    if milestone_state.returncode == 0:
        titles = [item["title"] for item in json.loads(milestone_state.stdout)]
    checks_total += 1
    checks_passed += require(milestone_state.returncode == 0 and sorted(titles) == sorted(gate["required_open_issue_titles"]), f"milestone:{gate['milestone_number']}", "M318-E001-GH-01", f"open issue titles drifted: {titles}", failures)

    summary = {
        "contract_id": gate["contract_id"],
        "milestone_number": gate["milestone_number"],
        "open_issue_titles": titles,
        "topology_ok": topology_payload.get("ok"),
        "expired_exception_ids": exception_payload.get("expired_exception_ids", []),
        "proposal_publication_mode": proposal_payload.get("publication_mode"),
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
    print(f"[ok] M318-E001 sustainable progress gate checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
