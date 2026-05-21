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
SUMMARY_PATH = ROOT / "tmp/reports/developer-tooling/product-workflow-summary.json"
OWNED_ISSUES = {"#8157", "#8169", "#8170", "#8171", "#8172", "#8178"}
NORMAL_DEVELOPER_FROM_NOTHING_CONTRACT_ID = (
    "objc3c.developer_tooling.normal_developer_from_nothing.v1"
)
GENERATED_OUTPUT_PREFIXES = ("tmp/artifacts/", "tmp/reports/")
FORBIDDEN_SOURCE_PREFIXES = GENERATED_OUTPUT_PREFIXES


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def action_name(command: str) -> str:
    prefix = "npm run objc3c -- "
    if not command.startswith(prefix):
        return ""
    return command.removeprefix(prefix).split(" ", 1)[0]


def string_list(value: Any, label: str, failures: list[str]) -> list[str]:
    if not isinstance(value, list):
        failures.append(f"{label} must be a list")
        return []
    result: list[str] = []
    for index, entry in enumerate(value):
        if isinstance(entry, str):
            result.append(entry)
            continue
        failures.append(f"{label}[{index}] must be a string")
    return result


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


def validate_source_paths(
    *,
    paths: list[str],
    label: str,
    failures: list[str],
) -> None:
    for raw_path in paths:
        if raw_path.startswith(FORBIDDEN_SOURCE_PREFIXES):
            failures.append(f"{label}: source truth cannot use generated output {raw_path}")
            continue
        if not (ROOT / raw_path).exists():
            failures.append(f"{label}: missing source truth path {raw_path}")


def validate_generated_outputs(
    *,
    paths: list[str],
    label: str,
    failures: list[str],
) -> None:
    for raw_path in paths:
        if not raw_path.startswith(GENERATED_OUTPUT_PREFIXES):
            failures.append(
                f"{label}: generated output must stay under tmp/artifacts or tmp/reports: {raw_path}"
            )


def validate_runbooks(contract: dict[str, Any], failures: list[str]) -> list[str]:
    runbooks = string_list(contract.get("runbooks"), "runbooks", failures)
    if len(set(runbooks)) != len(runbooks):
        failures.append("runbooks must not contain duplicate paths")
    validate_source_paths(paths=runbooks, label="runbooks", failures=failures)
    return runbooks


def validate_normal_developer_from_nothing(
    *,
    contract: dict[str, Any],
    workflow_rows: list[dict[str, Any]],
    registered_actions: set[str],
    failures: list[str],
) -> dict[str, Any]:
    journey = contract.get("normal_developer_from_nothing")
    if not isinstance(journey, dict):
        failures.append("normal_developer_from_nothing must be an object")
        return {
            "contract_id": None,
            "status": "FAIL",
            "stage_count": 0,
            "stages": [],
        }

    expect(
        journey.get("contract_id") == NORMAL_DEVELOPER_FROM_NOTHING_CONTRACT_ID,
        "normal_developer_from_nothing contract_id drifted",
        failures,
    )
    expect(
        journey.get("source_authority") == "checked-in-contracts-and-public-workflow-actions",
        "normal_developer_from_nothing source_authority drifted",
        failures,
    )

    forbidden_source_prefixes = tuple(
        string_list(
            journey.get("forbidden_source_prefixes"),
            "normal_developer_from_nothing.forbidden_source_prefixes",
            failures,
        )
    )
    expect(
        set(forbidden_source_prefixes) == set(FORBIDDEN_SOURCE_PREFIXES),
        "normal_developer_from_nothing forbidden source prefixes drifted",
        failures,
    )
    generated_output_prefixes = tuple(
        string_list(
            journey.get("generated_output_prefixes"),
            "normal_developer_from_nothing.generated_output_prefixes",
            failures,
        )
    )
    expect(
        set(generated_output_prefixes) == set(GENERATED_OUTPUT_PREFIXES),
        "normal_developer_from_nothing generated output prefixes drifted",
        failures,
    )

    stage_order = string_list(
        journey.get("stage_order"),
        "normal_developer_from_nothing.stage_order",
        failures,
    )
    stages = journey.get("stages")
    if not isinstance(stages, list):
        failures.append("normal_developer_from_nothing.stages must be a list")
        stages = []

    workflow_by_id = {
        str(row.get("workflow_id")): row
        for row in workflow_rows
        if isinstance(row, dict) and isinstance(row.get("workflow_id"), str)
    }
    observed_stage_ids: list[str] = []
    observed_issues: set[str] = set()
    issue_counts: dict[str, int] = {issue: 0 for issue in OWNED_ISSUES}
    stage_summaries: list[dict[str, Any]] = []
    starting_failure_count = len(failures)

    for index, raw_stage in enumerate(stages):
        if not isinstance(raw_stage, dict):
            failures.append(f"normal_developer_from_nothing.stages[{index}] must be an object")
            continue

        stage_id = str(raw_stage.get("stage_id", f"stage-{index}"))
        observed_stage_ids.append(stage_id)
        workflow_id = str(raw_stage.get("workflow_id", ""))
        workflow = workflow_by_id.get(workflow_id)
        issue = str(raw_stage.get("issue", ""))
        capability_id = str(raw_stage.get("capability_id", ""))
        support_claim = str(raw_stage.get("support_claim", ""))
        public_command = str(raw_stage.get("public_command", ""))
        command_action = action_name(public_command)
        source_truth_paths = string_list(
            raw_stage.get("source_truth_paths"),
            f"{stage_id}.source_truth_paths",
            failures,
        )
        generated_outputs = string_list(
            raw_stage.get("generated_outputs"),
            f"{stage_id}.generated_outputs",
            failures,
        )
        unsupported_claims = string_list(
            raw_stage.get("unsupported_claims"),
            f"{stage_id}.unsupported_claims",
            failures,
        )
        required_receipt_paths = string_list(
            raw_stage.get("required_receipt_paths", []),
            f"{stage_id}.required_receipt_paths",
            failures,
        )

        expect(issue in OWNED_ISSUES, f"{stage_id}: issue {issue} is outside owned set", failures)
        if issue in OWNED_ISSUES:
            observed_issues.add(issue)
            issue_counts[issue] += 1
        expect(workflow is not None, f"{stage_id}: missing workflow row {workflow_id}", failures)
        if workflow is not None:
            expect(
                workflow.get("issue") == issue,
                f"{stage_id}: workflow issue drifted from stage issue",
                failures,
            )
            expect(
                workflow.get("capability_id") == capability_id,
                f"{stage_id}: workflow capability_id drifted from stage",
                failures,
            )
            expect(
                workflow.get("support_claim") == support_claim,
                f"{stage_id}: workflow support_claim drifted from stage",
                failures,
            )
            workflow_commands = [
                str(command)
                for command in workflow.get("public_commands", [])
                if isinstance(command, str)
            ]
            expect(
                public_command in workflow_commands,
                f"{stage_id}: stage public command is not listed on workflow row",
                failures,
            )
            workflow_source_paths = {
                str(path)
                for path in workflow.get("source_paths", [])
                if isinstance(path, str)
            }
            extra_source_paths = sorted(set(source_truth_paths) - workflow_source_paths)
            expect(
                not extra_source_paths,
                f"{stage_id}: stage source truth paths are missing from workflow row: {extra_source_paths}",
                failures,
            )

        expect(bool(command_action), f"{stage_id}: invalid public command {public_command}", failures)
        expect(
            command_action in registered_actions,
            f"{stage_id}: unregistered public action {command_action}",
            failures,
        )
        expect(source_truth_paths, f"{stage_id}: source_truth_paths must not be empty", failures)
        validate_source_paths(paths=source_truth_paths, label=stage_id, failures=failures)
        expect(generated_outputs, f"{stage_id}: generated_outputs must not be empty", failures)
        validate_generated_outputs(paths=generated_outputs, label=stage_id, failures=failures)
        expect(unsupported_claims, f"{stage_id}: unsupported_claims must not be empty", failures)

        if raw_stage.get("clean_start_required") is True:
            expect(
                "--from-nothing" in public_command,
                f"{stage_id}: clean-start stage must use --from-nothing public replay",
                failures,
            )
        for receipt_path in required_receipt_paths:
            expect(
                receipt_path in generated_outputs,
                f"{stage_id}: required receipt path is not declared as generated output: {receipt_path}",
                failures,
            )
            expect(
                receipt_path.startswith("tmp/artifacts/"),
                f"{stage_id}: receipt path must stay under tmp/artifacts: {receipt_path}",
                failures,
            )

        developer_contract = raw_stage.get("developer_contract")
        if isinstance(developer_contract, dict):
            first_run_contract = load_json_object(
                ROOT / "tests/tooling/fixtures/developer_tooling/first_run_workflow_contract.json"
            )
            first_compile = first_run_contract.get("first_compile", {})
            if isinstance(first_compile, dict):
                expect(
                    developer_contract.get("first_compile_source") == first_compile.get("source"),
                    f"{stage_id}: first compile source drifted from first-run contract",
                    failures,
                )
                expect(
                    developer_contract.get("workflow_action") == first_compile.get("workflow_action"),
                    f"{stage_id}: first compile action drifted from first-run contract",
                    failures,
                )

        stage_summaries.append(
            {
                "stage_id": stage_id,
                "workflow_id": workflow_id,
                "issue": issue,
                "capability_id": capability_id,
                "public_action": command_action,
                "clean_start_required": raw_stage.get("clean_start_required") is True,
                "source_truth_path_count": len(source_truth_paths),
                "generated_output_count": len(generated_outputs),
                "unsupported_claim_count": len(unsupported_claims),
                "required_receipt_count": len(required_receipt_paths),
            }
        )

    expect(
        observed_stage_ids == stage_order,
        "normal_developer_from_nothing stage order drifted",
        failures,
    )
    expect(
        observed_issues == OWNED_ISSUES,
        "normal_developer_from_nothing stages must cover every owned issue exactly once",
        failures,
    )
    duplicated_or_missing_issues = {
        issue: count for issue, count in issue_counts.items() if count != 1
    }
    expect(
        not duplicated_or_missing_issues,
        f"normal_developer_from_nothing issue counts must be exactly one per owned issue: {duplicated_or_missing_issues}",
        failures,
    )
    expect(
        len(set(observed_stage_ids)) == len(observed_stage_ids),
        "normal_developer_from_nothing stage ids must be unique",
        failures,
    )
    expect(
        any(stage.get("clean_start_required") is True for stage in stages if isinstance(stage, dict)),
        "normal_developer_from_nothing must include at least one clean-start replay stage",
        failures,
    )

    return {
        "contract_id": journey.get("contract_id"),
        "source_authority": journey.get("source_authority"),
        "status": "PASS" if len(failures) == starting_failure_count else "FAIL",
        "stage_count": len(stage_summaries),
        "stage_order": stage_order,
        "covered_issues": sorted(observed_issues),
        "stages": stage_summaries,
    }


def validate_contract(contract: dict[str, Any]) -> dict[str, Any]:
    matrix = load_json_object(CAPABILITY_MATRIX_PATH)
    evidence_map = load_json_object(EVIDENCE_MAP_PATH)
    capabilities = capability_rows(matrix)
    evidence = evidence_rows(evidence_map)
    registered_actions = set(public_workflow_action_names())
    failures: list[str] = []
    runbooks = validate_runbooks(contract, failures)

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

    normal_developer_from_nothing = validate_normal_developer_from_nothing(
        contract=contract,
        workflow_rows=workflow_rows,
        registered_actions=registered_actions,
        failures=failures,
    )

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
        "runbooks": runbooks,
        "workflow_row_count": len(row_summaries),
        "issue_counts": dict(sorted(issue_counts.items())),
        "rows": row_summaries,
        "normal_developer_from_nothing": normal_developer_from_nothing,
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
