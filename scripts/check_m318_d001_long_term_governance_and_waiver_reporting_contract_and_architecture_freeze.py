#!/usr/bin/env python3
"""Checker for M318-D001 long-term governance reporting contract."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_long_term_governance_and_waiver_reporting_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_d001_long_term_governance_and_waiver_reporting_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "governance" / "objc3c_long_term_governance_reporting_contract.json"
PACKAGE_JSON = ROOT / "package.json"
GUARD = ROOT / "scripts" / "m318_governance_guard.py"
PROPOSAL_TOOL = ROOT / "tmp" / "github-publish" / "m318_governance" / "publish_new_work_proposal.py"
PROPOSAL_TEMPLATE = ROOT / "tmp" / "planning" / "m318_governance" / "new_work_proposal_template.json"
PROPOSAL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "m318_governance" / "new_work_proposal_sample.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-D001" / "long_term_governance_reporting_summary.json"


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


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=False)


def main(argv: Sequence[str]) -> int:
    del argv
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = read_json(CONTRACT_JSON)
    package = read_json(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-long-term-reporting-contract/m318-d001-v1" in expectations, str(EXPECTATIONS_DOC), "M318-D001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("expired` exception records are blocking" in expectations, str(EXPECTATIONS_DOC), "M318-D001-EXP-02", "expectations missing blocking-status policy", failures)
    checks_passed += require("objc3c_long_term_governance_reporting_contract.json" in packet, str(PACKET_DOC), "M318-D001-PKT-01", "packet missing contract path", failures)
    checks_passed += require("Next issue: `M318-E001`." in packet, str(PACKET_DOC), "M318-D001-PKT-02", "packet missing next issue", failures)

    checks_total += 7
    checks_passed += require(contract.get("contract_id") == "objc3c-governance-long-term-reporting-contract/m318-d001-v1", str(CONTRACT_JSON), "M318-D001-CON-01", "contract id drifted", failures)
    checks_passed += require(contract.get("source_of_truth", {}).get("exception_registry_path") == "spec/governance/objc3c_anti_noise_exception_registry.json", str(CONTRACT_JSON), "M318-D001-CON-02", "exception registry path drifted", failures)
    checks_passed += require(contract.get("source_of_truth", {}).get("blocking_statuses") == ["expired"], str(CONTRACT_JSON), "M318-D001-CON-03", "blocking statuses drifted", failures)
    checks_passed += require(contract.get("proposal_publication_policy", {}).get("default_mode") == "render-only", str(CONTRACT_JSON), "M318-D001-CON-04", "default proposal mode drifted", failures)
    checks_passed += require(contract.get("operator_surfaces", {}).get("guard_runner_path") == "scripts/m318_governance_guard.py", str(CONTRACT_JSON), "M318-D001-CON-05", "guard path drifted", failures)
    checks_passed += require(contract.get("operator_surfaces", {}).get("proposal_tool_path") == "tmp/github-publish/m318_governance/publish_new_work_proposal.py", str(CONTRACT_JSON), "M318-D001-CON-06", "proposal tool path drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M318-E001", str(CONTRACT_JSON), "M318-D001-CON-07", "next issue drifted", failures)

    checks_total += 2
    governance = package.get("objc3cGovernance", {})
    checks_passed += require(governance.get("longTermGovernanceReportingContractPath") == "spec/governance/objc3c_long_term_governance_reporting_contract.json", str(PACKAGE_JSON), "M318-D001-PKG-01", "package missing reporting contract path", failures)
    checks_passed += require(governance.get("longTermGovernanceReportingOwnerIssue") == "M318-D001", str(PACKAGE_JSON), "M318-D001-PKG-02", "package missing reporting owner issue", failures)

    for stage_key, stage_name in (("budget_snapshot", "budget-snapshot"), ("exception_registry", "exception-registry"), ("residue_proof", "residue-proof"), ("topology", "topology")):
        completed = run_command([sys.executable, str(GUARD), "--stage", stage_name])
        checks_total += 1
        checks_passed += require(completed.returncode == 0, str(GUARD), f"M318-D001-RUN-{stage_name}", f"guard stage {stage_name} failed: {(completed.stderr or completed.stdout).strip()}", failures)

    proposal_completed = run_command([sys.executable, str(PROPOSAL_TOOL), "--proposal", str(PROPOSAL_FIXTURE), "--template", str(PROPOSAL_TEMPLATE)])
    checks_total += 1
    checks_passed += require(proposal_completed.returncode == 0, str(PROPOSAL_TOOL), "M318-D001-RUN-PROP", f"proposal tool failed: {(proposal_completed.stderr or proposal_completed.stdout).strip()}", failures)

    report_paths = {key: ROOT / value for key, value in contract["report_paths"].items()}
    checks_total += len(report_paths)
    for key, path in report_paths.items():
        payload = read_json(path) if path.exists() else {}
        required_keys = contract["required_report_keys"][key]
        checks_passed += require(path.exists() and all(item in payload for item in required_keys), str(path), f"M318-D001-REP-{key}", f"report missing or drifted required keys for {key}", failures)

    summary = {
        "contract_id": contract["contract_id"],
        "report_paths": contract["report_paths"],
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
    print(f"[ok] M318-D001 governance reporting contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
