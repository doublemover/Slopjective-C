#!/usr/bin/env python3
"""Validate developer/product workflow source truth for OBJ3-NEXT issues."""

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
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/product_workflow_source_truth.json"
CAPABILITY_MATRIX_PATH = ROOT / "docs/support/capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs/support/evidence_map.json"
SUMMARY_PATH = ROOT / "tmp/reports/developer-tooling/product-workflow-source-truth-summary.json"
OWNED_ISSUES = {"#8157", "#8169", "#8170", "#8171", "#8172", "#8178"}


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def action_name(command: str) -> str:
    prefix = "npm run objc3c -- "
    if not command.startswith(prefix):
        return ""
    return command.removeprefix(prefix).split(" ", 1)[0]


def capability_rows(matrix: dict[str, Any]) -> dict[str, dict[str, Any]]:
    rows = matrix.get("capabilities", [])
    if not isinstance(rows, list):
        return {}
    return {
        str(row.get("id")): row
        for row in rows
        if isinstance(row, dict) and isinstance(row.get("id"), str)
    }


def evidence_rows(evidence_map: dict[str, Any]) -> list[dict[str, Any]]:
    rows = evidence_map.get("rows", [])
    return [row for row in rows if isinstance(row, dict)] if isinstance(rows, list) else []


def row_has_evidence(
    rows: list[dict[str, Any]],
    *,
    capability_id: str,
    support_claim: str,
    command: str,
) -> bool:
    return any(
        row.get("capability_id") == capability_id
        and row.get("support_claim") == support_claim
        and row.get("command") == command
        for row in rows
    )


def validate_contract(contract: dict[str, Any]) -> dict[str, Any]:
    matrix = load_json_object(CAPABILITY_MATRIX_PATH)
    evidence_map = load_json_object(EVIDENCE_MAP_PATH)
    capabilities = capability_rows(matrix)
    evidence = evidence_rows(evidence_map)
    registered_actions = set(public_workflow_action_names())
    failures: list[str] = []

    expect(
        set(contract.get("owned_issues", [])) == OWNED_ISSUES,
        "owned issue set drifted",
        failures,
    )
    expect(contract.get("index_issue") == "#8153", "index issue route drifted", failures)

    workflow_rows = contract.get("workflow_rows", [])
    if not isinstance(workflow_rows, list):
        workflow_rows = []
        failures.append("workflow_rows must be a list")

    issue_counts: dict[str, int] = {issue: 0 for issue in OWNED_ISSUES}
    row_summaries: list[dict[str, Any]] = []
    for index, row in enumerate(workflow_rows):
        if not isinstance(row, dict):
            failures.append(f"workflow_rows[{index}] must be an object")
            continue

        workflow_id = str(row.get("workflow_id", f"row-{index}"))
        issue = str(row.get("issue", ""))
        capability_id = str(row.get("capability_id", ""))
        support_claim = str(row.get("support_claim", ""))
        expected_state = str(row.get("expected_state", ""))
        public_commands = [str(command) for command in row.get("public_commands", [])]
        source_paths = [str(path) for path in row.get("source_paths", [])]
        generated_outputs = [str(path) for path in row.get("generated_outputs", [])]

        expect(issue in OWNED_ISSUES, f"{workflow_id}: issue {issue} is outside owned set", failures)
        if issue in issue_counts:
            issue_counts[issue] += 1

        capability = capabilities.get(capability_id)
        expect(capability is not None, f"{workflow_id}: missing capability row {capability_id}", failures)
        if capability is not None:
            expect(
                capability.get("state") == expected_state,
                f"{workflow_id}: capability state drifted for {capability_id}",
                failures,
            )
            claims = capability.get("support_claims", [])
            expect(
                support_claim in claims if isinstance(claims, list) else False,
                f"{workflow_id}: support claim missing from capability row",
                failures,
            )

        for raw_path in source_paths:
            expect(not raw_path.startswith("tmp/"), f"{workflow_id}: source path uses generated tmp output {raw_path}", failures)
            expect((ROOT / raw_path).exists(), f"{workflow_id}: missing source path {raw_path}", failures)
        for raw_path in generated_outputs:
            expect(raw_path.startswith("tmp/"), f"{workflow_id}: generated output must stay under tmp: {raw_path}", failures)

        evidence_backed_command_count = 0
        for command in public_commands:
            name = action_name(command)
            expect(bool(name), f"{workflow_id}: command is not a public objc3c invocation: {command}", failures)
            expect(name in registered_actions, f"{workflow_id}: public action is not registered: {name}", failures)
            if row_has_evidence(
                evidence,
                capability_id=capability_id,
                support_claim=support_claim,
                command=command,
            ):
                evidence_backed_command_count += 1

        expect(
            evidence_backed_command_count > 0,
            f"{workflow_id}: no public command is backed by an evidence-map row",
            failures,
        )

        row_summaries.append(
            {
                "workflow_id": workflow_id,
                "issue": issue,
                "capability_id": capability_id,
                "public_command_count": len(public_commands),
                "evidence_backed_command_count": evidence_backed_command_count,
                "source_path_count": len(source_paths),
                "generated_output_count": len(generated_outputs),
            }
        )

    missing_issues = sorted(issue for issue, count in issue_counts.items() if count == 0)
    if missing_issues:
        failures.append("missing workflow row(s) for issue(s): " + ", ".join(missing_issues))

    return {
        "contract_id": contract.get("contract_id"),
        "surface_kind": contract.get("surface_kind"),
        "status": "PASS" if not failures else "FAIL",
        "ok": not failures,
        "contract": repo_rel(CONTRACT_PATH),
        "capability_matrix": repo_rel(CAPABILITY_MATRIX_PATH),
        "evidence_map": repo_rel(EVIDENCE_MAP_PATH),
        "owned_issues": sorted(OWNED_ISSUES),
        "index_issue": contract.get("index_issue"),
        "workflow_row_count": len(row_summaries),
        "issue_counts": dict(sorted(issue_counts.items())),
        "rows": row_summaries,
        "failures": failures,
    }


def main() -> int:
    contract = load_json_object(CONTRACT_PATH)
    summary = validate_contract(contract)
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if summary["ok"]:
        print("developer-product-workflow-source-truth: PASS")
        return 0
    print("developer-product-workflow-source-truth: FAIL", file=sys.stderr)
    for failure in summary["failures"]:
        print(f"- {failure}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
