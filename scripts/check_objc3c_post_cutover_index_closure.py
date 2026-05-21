#!/usr/bin/env python3
"""Validate the post-cutover roadmap index closure contract."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object, write_json_file
from objc3c_tooling.paths import repo_rel
from scripts import check_objc3c_post_cutover_issue_evidence as evidence
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


SUMMARY_PATH = ROOT / "tmp/reports/post-cutover/post-cutover-index-closure-summary.json"
CONTRACT_ID = "objc3c.post_cutover.index_closure.v1"
INDEX_ISSUE = "#8153"
INDEX_CONTRACT_SCRIPT = "scripts/check_objc3c_post_cutover_index_closure.py"


def _expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _issue_rows_by_id(markdown: str) -> dict[str, evidence.IssueEvidenceRow]:
    rows = evidence.parse_issue_rows(markdown)
    return {row.issue: row for row in rows}


def _implementation_issue_contract_coverage() -> set[str]:
    covered: set[str] = set()
    for spec in evidence.COMPLETION_CONTRACT_SPECS:
        if spec.covered_issues == (INDEX_ISSUE,):
            continue
        covered.update(spec.covered_issues)
    return covered


def validate_index_closure() -> dict[str, Any]:
    failures: list[str] = []
    markdown = evidence.ISSUE_EVIDENCE_PATH.read_text(encoding="utf-8")
    matrix = load_json_object(evidence.CAPABILITY_MATRIX_PATH)
    evidence_map = load_json_object(evidence.EVIDENCE_MAP_PATH)
    product_workflow = load_json_object(
        ROOT / "tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json"
    )

    markdown_summary = evidence.validate_markdown_text(
        markdown,
        matrix=matrix,
        evidence_map=evidence_map,
        registered_actions=set(public_workflow_action_names()),
    )
    failures.extend(str(failure) for failure in markdown_summary["failures"])

    rows_by_id = _issue_rows_by_id(markdown)
    index_row = rows_by_id.get(INDEX_ISSUE)
    _expect(index_row is not None, "#8153 evidence row missing", failures)
    if index_row is not None:
        for required_path in (
            "docs/support/capability_matrix.json",
            "docs/support/evidence_map.json",
            "tests/conformance/support_claim_runnable_evidence_catalog.json",
        ):
            _expect(
                f"`{required_path}`" in index_row.local_evidence,
                f"#8153 evidence row missing {required_path}",
                failures,
            )
        _expect(
            "`npm run objc3c -- validate-post-cutover-issue-evidence`"
            in index_row.public_replay_surface,
            "#8153 evidence row must point at aggregate validation",
            failures,
        )
        _expect(
            "source-backed support truth" in index_row.boundary,
            "#8153 boundary must reject prose-only closure",
            failures,
        )

    expected_implementation_issues = set(evidence.REQUIRED_ISSUES) - {INDEX_ISSUE}
    covered_implementation_issues = _implementation_issue_contract_coverage()
    _expect(
        covered_implementation_issues == expected_implementation_issues,
        (
            "post-cutover implementation contract coverage drifted: "
            f"expected {sorted(expected_implementation_issues)}, "
            f"observed {sorted(covered_implementation_issues)}"
        ),
        failures,
    )

    index_specs = [
        spec
        for spec in evidence.COMPLETION_CONTRACT_SPECS
        if spec.covered_issues == (INDEX_ISSUE,)
    ]
    _expect(len(index_specs) == 1, "#8153 must have exactly one index closure contract", failures)
    if index_specs:
        _expect(
            index_specs[0].script == INDEX_CONTRACT_SCRIPT,
            "#8153 index closure contract script drifted",
            failures,
        )

    _expect(
        product_workflow.get("index_issue") == INDEX_ISSUE,
        "developer product workflow index issue drifted",
        failures,
    )
    _expect(
        product_workflow.get("contract_id")
        == "objc3c.developer_tooling.product_workflow_source_truth.v1",
        "developer product workflow source truth contract drifted",
        failures,
    )

    return {
        "contract_id": CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "index_issue": INDEX_ISSUE,
        "issue_evidence_path": repo_rel(evidence.ISSUE_EVIDENCE_PATH),
        "implementation_contract_issues": sorted(covered_implementation_issues),
        "implementation_contract_count": len(evidence.COMPLETION_CONTRACT_SPECS) - 1,
        "failures": failures,
    }


def main() -> int:
    summary = validate_index_closure()
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if summary["failures"]:
        print(
            f"objc3c-post-cutover-index-closure: FAIL ({len(summary['failures'])} failures)",
            file=sys.stderr,
        )
        for failure in summary["failures"]:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-post-cutover-index-closure: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
