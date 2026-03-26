#!/usr/bin/env python3
"""Checker for M318-C003 new-work proposal tooling."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_new_work_proposal_and_publication_workflow_tooling_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_c003_new_work_proposal_and_publication_workflow_tooling_core_feature_expansion_packet.md"
RESULT_JSON = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_c003_new_work_proposal_and_publication_workflow_tooling_core_feature_expansion_result.json"
TEMPLATE_JSON = ROOT / "tmp" / "planning" / "m318_governance" / "new_work_proposal_template.json"
TOOL = ROOT / "tmp" / "github-publish" / "m318_governance" / "publish_new_work_proposal.py"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "m318_governance" / "new_work_proposal_sample.json"
SUMMARY_DIR = ROOT / "tmp" / "reports" / "m318" / "governance" / "new_work_proposal"
SUMMARY_JSON = SUMMARY_DIR / "publication_summary.json"
BODY_MD = SUMMARY_DIR / "issue_body.md"
PAYLOAD_JSON = SUMMARY_DIR / "issue_payload.json"
PACKAGE_JSON = ROOT / "package.json"


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
    result = read_json(RESULT_JSON)
    template = read_json(TEMPLATE_JSON)
    package = read_json(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-new-work-proposal-tooling/m318-c003-v1" in expectations, str(EXPECTATIONS_DOC), "M318-C003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("render-only publication" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-C003-EXP-02", "expectations missing render-only boundary", failures)
    checks_passed += require("tmp/planning/m318_governance/new_work_proposal_template.json" in packet, str(PACKET_DOC), "M318-C003-PKT-01", "packet missing template path", failures)
    checks_passed += require("Next issue: `M318-D001`." in packet, str(PACKET_DOC), "M318-C003-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(result.get("contract_id") == "objc3c-governance-new-work-proposal-tooling/m318-c003-v1", str(RESULT_JSON), "M318-C003-RES-01", "result contract id drifted", failures)
    checks_passed += require(result.get("template_path") == "tmp/planning/m318_governance/new_work_proposal_template.json", str(RESULT_JSON), "M318-C003-RES-02", "result template path drifted", failures)
    checks_passed += require(result.get("publication_tool_path") == "tmp/github-publish/m318_governance/publish_new_work_proposal.py", str(RESULT_JSON), "M318-C003-RES-03", "result tool path drifted", failures)
    checks_passed += require(result.get("default_mode") == "render-only", str(RESULT_JSON), "M318-C003-RES-04", "default mode drifted", failures)
    checks_passed += require(result.get("sample_output_dir") == "tmp/reports/m318/governance/new_work_proposal", str(RESULT_JSON), "M318-C003-RES-05", "sample output dir drifted", failures)
    checks_passed += require(result.get("next_issue") == "M318-D001", str(RESULT_JSON), "M318-C003-RES-06", "next issue drifted", failures)

    checks_total += 6
    checks_passed += require(template.get("contract_id") == result["contract_id"], str(TEMPLATE_JSON), "M318-C003-TPL-01", "template contract id drifted", failures)
    checks_passed += require(template.get("render_defaults", {}).get("publish_mode") == "render-only", str(TEMPLATE_JSON), "M318-C003-TPL-02", "template default mode drifted", failures)
    checks_passed += require("validation_posture" in template.get("required_issue_fields", []), str(TEMPLATE_JSON), "M318-C003-TPL-03", "template missing validation posture field", failures)
    checks_passed += require("budget_impact" in template.get("required_issue_fields", []), str(TEMPLATE_JSON), "M318-C003-TPL-04", "template missing budget impact field", failures)
    checks_passed += require(template.get("execution_order_fields") == ["cleanup_program_slot", "cleanup_program_phase", "global_sequence", "blocked_by_prior_milestones", "direct_issue_blockers", "directly_unblocks", "instruction"], str(TEMPLATE_JSON), "M318-C003-TPL-05", "execution order schema drifted", failures)
    checks_passed += require(template.get("next_issue") == "M318-D001", str(TEMPLATE_JSON), "M318-C003-TPL-06", "template next issue drifted", failures)

    checks_total += 3
    governance = package.get("objc3cGovernance", {})
    checks_passed += require(governance.get("newWorkProposalTemplatePath") == "tmp/planning/m318_governance/new_work_proposal_template.json", str(PACKAGE_JSON), "M318-C003-PKG-01", "package missing template path wiring", failures)
    checks_passed += require(governance.get("newWorkProposalPublicationToolPath") == "tmp/github-publish/m318_governance/publish_new_work_proposal.py", str(PACKAGE_JSON), "M318-C003-PKG-02", "package missing tool path wiring", failures)
    checks_passed += require(governance.get("newWorkProposalToolingOwnerIssue") == "M318-C003", str(PACKAGE_JSON), "M318-C003-PKG-03", "package missing owner issue wiring", failures)

    completed = subprocess.run([sys.executable, str(TOOL), "--proposal", str(FIXTURE), "--template", str(TEMPLATE_JSON), "--output-dir", str(SUMMARY_DIR)], cwd=ROOT, capture_output=True, text=True, check=False)
    checks_total += 4
    checks_passed += require(completed.returncode == 0, str(TOOL), "M318-C003-RUN-01", f"proposal tool failed: {(completed.stderr or completed.stdout).strip()}", failures)
    summary = read_json(SUMMARY_JSON) if SUMMARY_JSON.exists() else {}
    payload = read_json(PAYLOAD_JSON) if PAYLOAD_JSON.exists() else {}
    body = read_text(BODY_MD) if BODY_MD.exists() else ""
    checks_passed += require(summary.get("ok") is True and summary.get("publication_mode") == "render-only", str(SUMMARY_JSON), "M318-C003-RUN-02", "summary is not green render-only output", failures)
    checks_passed += require(payload.get("title") == "[M400][Lane-A][A001] Example proposal publication boundary - Contract and architecture freeze", str(PAYLOAD_JSON), "M318-C003-RUN-03", "payload title drifted", failures)
    checks_passed += require("## Validation posture" in body and "<!-- EXECUTION-ORDER-START -->" in body, str(BODY_MD), "M318-C003-RUN-04", "rendered issue body missing expected sections", failures)

    checker_summary = {
        "contract_id": result["contract_id"],
        "template_path": result["template_path"],
        "publication_tool_path": result["publication_tool_path"],
        "sample_fixture_path": result["sample_fixture_path"],
        "rendered_output_paths": {
            "issue_body": str(BODY_MD.relative_to(ROOT)).replace("\\", "/"),
            "issue_payload": str(PAYLOAD_JSON.relative_to(ROOT)).replace("\\", "/"),
            "summary": str(SUMMARY_JSON.relative_to(ROOT)).replace("\\", "/")
        },
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [f.__dict__ for f in failures]
    }
    SUMMARY_DIR.mkdir(parents=True, exist_ok=True)
    (SUMMARY_DIR / "checker_summary.json").write_text(json.dumps(checker_summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M318-C003 proposal tooling checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
