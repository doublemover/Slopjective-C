#!/usr/bin/env python3
"""Checker for M318-B002 sustainable planning hygiene policy."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_sustainable_planning_hygiene_policy_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_b002_sustainable_planning_hygiene_policy_core_feature_implementation_packet.md"
POLICY_JSON = ROOT / "spec" / "governance" / "objc3c_planning_hygiene_policy.json"
RUNBOOK_MD = ROOT / "docs" / "runbooks" / "objc3c_planning_hygiene.md"
ROADMAP_TEMPLATE = ROOT / ".github" / "ISSUE_TEMPLATE" / "roadmap_execution.yml"
CONFORMANCE_TEMPLATE = ROOT / ".github" / "ISSUE_TEMPLATE" / "conformance_execution.yml"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-B002" / "planning_hygiene_policy_summary.json"


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


def main(argv: Sequence[str]) -> int:
    del argv
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    policy = read_json(POLICY_JSON)
    runbook = read_text(RUNBOOK_MD)
    roadmap_template = read_text(ROADMAP_TEMPLATE)
    conformance_template = read_text(CONFORMANCE_TEMPLATE)
    package = read_json(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-planning-hygiene-policy/m318-b002-v1" in expectations, str(EXPECTATIONS_DOC), "M318-B002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("budget-impact classification" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-B002-EXP-02", "expectations missing budget-impact note", failures)
    checks_passed += require("Implemented rules" in packet and "requires_exception_record" in packet, str(PACKET_DOC), "M318-B002-PKT-01", "packet missing implemented rules or exception option", failures)
    checks_passed += require("Next issue: `M318-B003`." in packet, str(PACKET_DOC), "M318-B002-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(policy.get("mode") == "m318-b002-planning-hygiene-policy-v1", str(POLICY_JSON), "M318-B002-POL-01", "policy mode drifted", failures)
    checks_passed += require(policy.get("contract_id") == "objc3c-governance-planning-hygiene-policy/m318-b002-v1", str(POLICY_JSON), "M318-B002-POL-02", "policy contract id drifted", failures)
    checks_passed += require(policy.get("budget_policy_contract_id") == "objc3c-governance-anti-noise-budget-policy/m318-b001-v1", str(POLICY_JSON), "M318-B002-POL-03", "budget-policy contract link drifted", failures)
    checks_passed += require(policy.get("governance_owner_issue") == "M318-B002", str(POLICY_JSON), "M318-B002-POL-04", "governance owner drifted", failures)
    checks_passed += require(policy.get("budget_impact_options") == ["no_budget_growth", "within_existing_budget", "requires_exception_record"], str(POLICY_JSON), "M318-B002-POL-05", "budget-impact options drifted", failures)
    checks_passed += require(policy.get("next_issue") == "M318-B003", str(POLICY_JSON), "M318-B002-POL-06", "next issue drifted", failures)

    required_fields = [
        "outcome_summary",
        "why_it_matters",
        "acceptance_criteria",
        "implementation_surfaces",
        "dependencies",
        "validation_posture",
        "budget_impact",
    ]
    checks_total += 4
    checks_passed += require(policy.get("required_issue_fields") == required_fields, str(POLICY_JSON), "M318-B002-POL-07", "required issue field set drifted", failures)
    checks_passed += require(policy.get("authoring_rules", {}).get("prefer_shared_validation_surfaces") is True, str(POLICY_JSON), "M318-B002-POL-08", "shared-validation preference drifted", failures)
    checks_passed += require(policy.get("authoring_rules", {}).get("avoid_required_deliverables_boilerplate") is True, str(POLICY_JSON), "M318-B002-POL-09", "boilerplate-avoidance rule drifted", failures)
    checks_passed += require(policy.get("template_paths") == [".github/ISSUE_TEMPLATE/roadmap_execution.yml", ".github/ISSUE_TEMPLATE/conformance_execution.yml"], str(POLICY_JSON), "M318-B002-POL-10", "template paths drifted", failures)

    checks_total += 4
    checks_passed += require("Budget-impact classes" in runbook, str(RUNBOOK_MD), "M318-B002-RUN-01", "runbook missing budget-impact section", failures)
    checks_passed += require("requires_exception_record" in runbook, str(RUNBOOK_MD), "M318-B002-RUN-02", "runbook missing exception class", failures)
    checks_passed += require("Do not add broad `Required deliverables` boilerplate" in runbook, str(RUNBOOK_MD), "M318-B002-RUN-03", "runbook missing boilerplate rule", failures)
    checks_passed += require("Amend old issues rather than layering parallel scopes" in runbook, str(RUNBOOK_MD), "M318-B002-RUN-04", "runbook missing amendment rule", failures)

    for template_path, template_text, prefix in [
        (ROADMAP_TEMPLATE, roadmap_template, "ROAD"),
        (CONFORMANCE_TEMPLATE, conformance_template, "CONF"),
    ]:
        checks_total += 4
        checks_passed += require("id: budget_impact" in template_text, str(template_path), f"M318-B002-{prefix}-01", "template missing budget_impact field", failures)
        checks_passed += require("requires_exception_record" in template_text, str(template_path), f"M318-B002-{prefix}-02", "template missing requires_exception_record option", failures)
        checks_passed += require("id: exception_record" in template_text, str(template_path), f"M318-B002-{prefix}-03", "template missing exception_record field", failures)
        checks_passed += require("Planning hygiene:" in template_text, str(template_path), f"M318-B002-{prefix}-04", "template missing planning hygiene note", failures)

    governance = package.get("objc3cGovernance", {})
    checks_total += 3
    checks_passed += require(governance.get("planningHygienePolicyPath") == "spec/governance/objc3c_planning_hygiene_policy.json", str(PACKAGE_JSON), "M318-B002-PKG-01", "package planning hygiene policy path drifted", failures)
    checks_passed += require(governance.get("planningHygieneRunbookPath") == "docs/runbooks/objc3c_planning_hygiene.md", str(PACKAGE_JSON), "M318-B002-PKG-02", "package planning hygiene runbook path drifted", failures)
    checks_passed += require(governance.get("planningHygieneOwnerIssue") == "M318-B002", str(PACKAGE_JSON), "M318-B002-PKG-03", "package planning hygiene owner drifted", failures)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": policy["contract_id"],
        "required_issue_fields": policy["required_issue_fields"],
        "budget_impact_options": policy["budget_impact_options"],
        "template_paths": policy["template_paths"],
        "package_governance": governance,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [f.__dict__ for f in failures],
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M318-B002 planning hygiene policy checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
