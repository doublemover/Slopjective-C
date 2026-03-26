#!/usr/bin/env python3
"""Checker for M318-B003 maintainer review checklist and regression handling model."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_maintainer_review_checklist_and_regression_handling_model_edge_case_and_compatibility_completion_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_b003_maintainer_review_checklist_and_regression_handling_model_edge_case_and_compatibility_completion_packet.md"
MODEL_JSON = ROOT / "spec" / "governance" / "objc3c_review_and_regression_model.json"
RUNBOOK_MD = ROOT / "docs" / "runbooks" / "objc3c_review_checklist.md"
PR_TEMPLATE = ROOT / ".github" / "PULL_REQUEST_TEMPLATE.md"
CONTRIBUTING = ROOT / "CONTRIBUTING.md"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-B003" / "review_regression_model_summary.json"


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
    model = read_json(MODEL_JSON)
    runbook = read_text(RUNBOOK_MD)
    pr_template = read_text(PR_TEMPLATE)
    contributing = read_text(CONTRIBUTING)
    governance = read_json(PACKAGE_JSON).get("objc3cGovernance", {})

    checks_total += 4
    checks_passed += require("objc3c-governance-review-and-regression-model/m318-b003-v1" in expectations, str(EXPECTATIONS_DOC), "M318-B003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("rollback or regression path" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-B003-EXP-02", "expectations missing rollback note", failures)
    checks_passed += require("Implemented review questions" in packet, str(PACKET_DOC), "M318-B003-PKT-01", "packet missing review questions section", failures)
    checks_passed += require("Next issue: `M318-C001`." in packet, str(PACKET_DOC), "M318-B003-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(model.get("mode") == "m318-b003-review-and-regression-model-v1", str(MODEL_JSON), "M318-B003-MOD-01", "model mode drifted", failures)
    checks_passed += require(model.get("contract_id") == "objc3c-governance-review-and-regression-model/m318-b003-v1", str(MODEL_JSON), "M318-B003-MOD-02", "model contract id drifted", failures)
    checks_passed += require(model.get("planning_hygiene_contract_id") == "objc3c-governance-planning-hygiene-policy/m318-b002-v1", str(MODEL_JSON), "M318-B003-MOD-03", "planning hygiene contract link drifted", failures)
    checks_passed += require(model.get("required_review_questions") == ["budget_family_changed", "exception_record_if_required", "validation_posture_and_evidence", "rollback_or_regression_path"], str(MODEL_JSON), "M318-B003-MOD-04", "required review questions drifted", failures)
    checks_passed += require(model.get("regression_handling", {}).get("expired_exception_blocks_merge") is True, str(MODEL_JSON), "M318-B003-MOD-05", "expired exception merge rule drifted", failures)
    checks_passed += require(model.get("next_issue") == "M318-C001", str(MODEL_JSON), "M318-B003-MOD-06", "next issue drifted", failures)

    checks_total += 4
    checks_passed += require("Required review questions" in runbook, str(RUNBOOK_MD), "M318-B003-RUN-01", "runbook missing review question section", failures)
    checks_passed += require("Expired exceptions block merge." in runbook, str(RUNBOOK_MD), "M318-B003-RUN-02", "runbook missing expired exception rule", failures)
    checks_passed += require("Missing exception references block merge" in runbook, str(RUNBOOK_MD), "M318-B003-RUN-03", "runbook missing missing-reference rule", failures)
    checks_passed += require("rollback or regression path" in runbook.lower(), str(RUNBOOK_MD), "M318-B003-RUN-04", "runbook missing rollback guidance", failures)

    checks_total += 4
    checks_passed += require("Budget impact:" in pr_template, str(PR_TEMPLATE), "M318-B003-PR-01", "PR template missing budget impact", failures)
    checks_passed += require("Exception record:" in pr_template, str(PR_TEMPLATE), "M318-B003-PR-02", "PR template missing exception record", failures)
    checks_passed += require("Validation posture:" in pr_template, str(PR_TEMPLATE), "M318-B003-PR-03", "PR template missing validation posture", failures)
    checks_passed += require("Rollback or regression path" in pr_template, str(PR_TEMPLATE), "M318-B003-PR-04", "PR template missing rollback checklist item", failures)

    checks_total += 3
    checks_passed += require("docs/runbooks/objc3c_planning_hygiene.md" in contributing, str(CONTRIBUTING), "M318-B003-CON-01", "CONTRIBUTING missing planning hygiene link", failures)
    checks_passed += require("docs/runbooks/objc3c_review_checklist.md" in contributing, str(CONTRIBUTING), "M318-B003-CON-02", "CONTRIBUTING missing review checklist link", failures)
    checks_passed += require(".github/PULL_REQUEST_TEMPLATE.md" in contributing, str(CONTRIBUTING), "M318-B003-CON-03", "CONTRIBUTING missing PR template link", failures)

    checks_total += 3
    checks_passed += require(governance.get("reviewRegressionModelPath") == "spec/governance/objc3c_review_and_regression_model.json", str(PACKAGE_JSON), "M318-B003-PKG-01", "package review model path drifted", failures)
    checks_passed += require(governance.get("reviewChecklistRunbookPath") == "docs/runbooks/objc3c_review_checklist.md", str(PACKAGE_JSON), "M318-B003-PKG-02", "package review checklist path drifted", failures)
    checks_passed += require(governance.get("reviewChecklistOwnerIssue") == "M318-B003", str(PACKAGE_JSON), "M318-B003-PKG-03", "package review checklist owner drifted", failures)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": model["contract_id"],
        "required_review_questions": model["required_review_questions"],
        "regression_handling": model["regression_handling"],
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
    print(f"[ok] M318-B003 review/regression model checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
