#!/usr/bin/env python3
"""Validate #8170 editor-tooling source truth and public command wiring."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object, write_json_file
from objc3c_tooling.paths import repo_rel
from scripts.objc3c_workflow.action_handler_lookup import registered_action_handler
from scripts.objc3c_workflow.public_command_api import (
    public_workflow_action_names,
    public_workflow_action_payload,
)


CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/editor_tooling_source_truth_contract.json"
CAPABILITY_MATRIX_PATH = ROOT / "docs/support/capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs/support/evidence_map.json"
SUMMARY_PATH = ROOT / "tmp/reports/developer-tooling/editor-source-truth-summary.json"
INTEGRATION_STEPS_PATH = ROOT / "scripts/objc3c_developer_tooling_integration_check/steps.py"


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


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


def has_evidence_row(
    rows: list[dict[str, Any]],
    *,
    capability_id: str,
    support_claim: str,
    evidence_kind: str,
    path: str,
    command: str | None,
) -> bool:
    for row in rows:
        if row.get("capability_id") != capability_id:
            continue
        if row.get("support_claim") != support_claim:
            continue
        if row.get("evidence_kind") != evidence_kind:
            continue
        if row.get("path") != path:
            continue
        if command is not None and row.get("command") != command:
            continue
        return True
    return False


def validate_capability_rows(
    contract: dict[str, Any],
    capabilities: dict[str, dict[str, Any]],
    failures: list[str],
) -> None:
    for expected in contract.get("expected_capability_rows", []):
        if not isinstance(expected, dict):
            failures.append("expected_capability_rows entries must be objects")
            continue
        capability_id = str(expected.get("capability_id", ""))
        support_claim = str(expected.get("support_claim", ""))
        expected_state = str(expected.get("expected_state", ""))
        row = capabilities.get(capability_id)
        expect(row is not None, f"missing capability row {capability_id}", failures)
        if row is None:
            continue
        expect(
            row.get("state") == expected_state,
            f"{capability_id}: capability state drifted",
            failures,
        )
        claims = row.get("support_claims", [])
        expect(
            isinstance(claims, list) and support_claim in claims,
            f"{capability_id}: support claim missing: {support_claim}",
            failures,
        )


def validate_public_actions(contract: dict[str, Any], failures: list[str]) -> list[dict[str, Any]]:
    registered_actions = set(public_workflow_action_names())
    action_summaries: list[dict[str, Any]] = []
    for expected in contract.get("public_actions", []):
        if not isinstance(expected, dict):
            failures.append("public_actions entries must be objects")
            continue
        action = str(expected.get("action", ""))
        backend = str(expected.get("backend", ""))
        pass_through_args = bool(expected.get("pass_through_args", False))
        expect(action in registered_actions, f"public action is not registered: {action}", failures)
        expect(registered_action_handler(action) is not None, f"public action has no handler: {action}", failures)
        payload = public_workflow_action_payload(action) if action in registered_actions else {}
        expect(payload.get("backend") == backend, f"{action}: backend drifted", failures)
        expect(
            payload.get("public_command") == f"npm run objc3c -- {action}",
            f"{action}: npm bridge command drifted",
            failures,
        )
        expect(
            payload.get("pass_through_args") is pass_through_args,
            f"{action}: pass-through argument policy drifted",
            failures,
        )
        action_summaries.append(
            {
                "action": action,
                "backend": payload.get("backend", ""),
                "public_command": payload.get("public_command", ""),
                "has_handler": registered_action_handler(action) is not None,
            }
        )
    return action_summaries


def validate_source_truth_rows(
    contract: dict[str, Any],
    evidence: list[dict[str, Any]],
    failures: list[str],
) -> list[dict[str, Any]]:
    registered_actions = set(public_workflow_action_names())
    row_summaries: list[dict[str, Any]] = []
    for index, row in enumerate(contract.get("source_truth_rows", [])):
        if not isinstance(row, dict):
            failures.append(f"source_truth_rows[{index}] must be an object")
            continue
        surface_id = str(row.get("surface_id", f"row-{index}"))
        capability_id = str(row.get("capability_id", ""))
        support_claim = str(row.get("support_claim", ""))
        source_paths = [str(path) for path in row.get("required_source_paths", [])]
        public_actions = [str(action) for action in row.get("public_actions", [])]
        generated_outputs = [str(path) for path in row.get("generated_outputs", [])]
        required_evidence = row.get("required_evidence", [])

        for source_path in source_paths:
            expect(
                not source_path.startswith("tmp/"),
                f"{surface_id}: source path cannot be generated tmp output: {source_path}",
                failures,
            )
            expect((ROOT / source_path).exists(), f"{surface_id}: missing source path {source_path}", failures)

        for action in public_actions:
            expect(action in registered_actions, f"{surface_id}: public action is not registered: {action}", failures)
            expect(registered_action_handler(action) is not None, f"{surface_id}: public action has no handler: {action}", failures)

        for output_path in generated_outputs:
            expect(
                output_path.startswith("tmp/"),
                f"{surface_id}: generated output must stay under tmp: {output_path}",
                failures,
            )

        backed_evidence_count = 0
        if not isinstance(required_evidence, list):
            failures.append(f"{surface_id}: required_evidence must be a list")
            required_evidence = []
        for evidence_requirement in required_evidence:
            if not isinstance(evidence_requirement, dict):
                failures.append(f"{surface_id}: required_evidence entries must be objects")
                continue
            evidence_kind = str(evidence_requirement.get("evidence_kind", ""))
            path = str(evidence_requirement.get("path", ""))
            command_value = evidence_requirement.get("command")
            command = str(command_value) if command_value is not None else None
            if has_evidence_row(
                evidence,
                capability_id=capability_id,
                support_claim=support_claim,
                evidence_kind=evidence_kind,
                path=path,
                command=command,
            ):
                backed_evidence_count += 1
            else:
                failures.append(f"{surface_id}: missing evidence-map row for {path}")
        expect(backed_evidence_count > 0, f"{surface_id}: no evidence-map rows backed this surface", failures)

        row_summaries.append(
            {
                "surface_id": surface_id,
                "capability_id": capability_id,
                "public_action_count": len(public_actions),
                "source_path_count": len(source_paths),
                "generated_output_count": len(generated_outputs),
                "evidence_backed_count": backed_evidence_count,
            }
        )
    return row_summaries


def validate_integration_wiring(contract: dict[str, Any], failures: list[str]) -> None:
    text = INTEGRATION_STEPS_PATH.read_text(encoding="utf-8")
    step_name = str(contract.get("integration_step_name", ""))
    expect(step_name in text, f"integration steps missing {step_name}", failures)
    expect(
        "EDITOR_TOOLING_SOURCE_TRUTH_PY" in text,
        "integration steps do not call the editor source-truth validator",
        failures,
    )


def validate_contract(contract: dict[str, Any]) -> dict[str, Any]:
    matrix = load_json_object(CAPABILITY_MATRIX_PATH)
    evidence_map = load_json_object(EVIDENCE_MAP_PATH)
    failures: list[str] = []

    expect(
        contract.get("contract_id") == "objc3c.developer.tooling.editor.source_truth.contract.v1",
        "contract id drifted",
        failures,
    )
    expect(contract.get("issue") == "#8170", "source-truth contract must remain scoped to #8170", failures)

    capabilities = capability_rows(matrix)
    evidence = evidence_rows(evidence_map)
    validate_capability_rows(contract, capabilities, failures)
    action_summaries = validate_public_actions(contract, failures)
    row_summaries = validate_source_truth_rows(contract, evidence, failures)
    validate_integration_wiring(contract, failures)

    return {
        "contract_id": contract.get("contract_id"),
        "surface_kind": contract.get("surface_kind"),
        "issue": contract.get("issue"),
        "status": "PASS" if not failures else "FAIL",
        "ok": not failures,
        "contract": repo_rel(CONTRACT_PATH),
        "capability_matrix": repo_rel(CAPABILITY_MATRIX_PATH),
        "evidence_map": repo_rel(EVIDENCE_MAP_PATH),
        "source_truth_model": contract.get("source_truth_model"),
        "generated_output_boundary": contract.get("generated_output_boundary"),
        "public_action_count": len(action_summaries),
        "source_truth_row_count": len(row_summaries),
        "actions": action_summaries,
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
        print("developer-tooling-editor-source-truth: PASS")
        return 0
    print("developer-tooling-editor-source-truth: FAIL", file=sys.stderr)
    for failure in summary["failures"]:
        print(f"- {failure}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
