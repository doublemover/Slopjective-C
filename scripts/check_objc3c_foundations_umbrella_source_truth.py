#!/usr/bin/env python3
"""Validate foundations umbrella source-truth contracts without generated evidence."""

from __future__ import annotations

import argparse
import json
import sys
from collections.abc import Sequence
from copy import deepcopy
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPTS_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from check_objc3c_standalone_textual_interface_payload import (  # noqa: E402
    FIXTURE as STANDALONE_TEXTUAL_INTERFACE_FIXTURE,
    REQUIRED_NEGATIVE_CASE_IDS,
    validate_payload as validate_standalone_textual_interface_payload,
)
from objc3c_shared.schema_registry import (  # noqa: E402
    schema_registry_summary,
    validate_registered_schema,
)
from scripts.objc3c_workflow.action_catalog import ACTION_SPECS  # noqa: E402

SCHEMA_ID = "objc3c-foundations-umbrella-source-truth-v1"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "foundations_umbrella_readiness"
    / "source_truth_contract.json"
)
CAPABILITY_MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs" / "support" / "evidence_map.json"
UMBRELLA_READINESS_PATH = ROOT / "docs" / "support" / "umbrella_readiness.json"
ADVANCED_RUNTIME_EXECUTABLE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "advanced_runtime_closure"
    / "executable_runtime_contract_surfaces.json"
)
FORBIDDEN_SOURCE_PREFIXES = (
    "tmp/",
    "tmp\\",
    "temp/",
    "temp\\",
    "generated/",
    "generated\\",
    "build/",
    "build\\",
    "dist/",
    "dist\\",
)
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "


def _load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{path} must contain a JSON object")
    return payload


def _repo_path(raw_path: str, *, label: str) -> Path:
    repo_path = Path(raw_path)
    if (
        raw_path.startswith(FORBIDDEN_SOURCE_PREFIXES)
        or repo_path.is_absolute()
        or ".." in repo_path.parts
    ):
        raise RuntimeError(f"{label} uses non-source path as source truth: {raw_path}")
    path = ROOT / raw_path
    if not path.exists():
        raise RuntimeError(f"{label} references missing path: {raw_path}")
    return path


def _action_from_public_command(command: str) -> str:
    if not command.startswith(PUBLIC_COMMAND_PREFIX):
        raise RuntimeError(f"unsupported public command surface: {command}")
    return command[len(PUBLIC_COMMAND_PREFIX) :].split(maxsplit=1)[0]


def _capability_rows_by_id(matrix: dict[str, Any]) -> dict[str, dict[str, Any]]:
    rows = matrix.get("capabilities", [])
    if not isinstance(rows, list):
        raise RuntimeError("capability matrix capabilities must be an array")
    return {
        str(row["id"]): row
        for row in rows
        if isinstance(row, dict) and isinstance(row.get("id"), str)
    }


def _evidence_capability_ids(evidence_map: dict[str, Any]) -> set[str]:
    rows = evidence_map.get("rows", [])
    if not isinstance(rows, list):
        raise RuntimeError("evidence map rows must be an array")
    return {
        str(row["capability_id"])
        for row in rows
        if isinstance(row, dict) and isinstance(row.get("capability_id"), str)
    }


def _validate_schema_contracts(contract: dict[str, Any]) -> None:
    registry = schema_registry_summary()
    for row in contract["schema_contracts"]:
        schema_id = str(row["schema_id"])
        path = str(row["path"])
        _repo_path(path, label=f"schema_contracts.{schema_id}.path")
        if registry.get(schema_id) != path:
            raise RuntimeError(
                f"schema contract {schema_id} expected registry path {path}, "
                f"saw {registry.get(schema_id)!r}"
            )


def _validate_capability_truth(contract: dict[str, Any]) -> None:
    matrix = _load_json(CAPABILITY_MATRIX_PATH)
    evidence_map = _load_json(EVIDENCE_MAP_PATH)
    rows_by_id = _capability_rows_by_id(matrix)
    evidence_ids = _evidence_capability_ids(evidence_map)

    for row in contract["capability_truth_rows"]:
        capability_id = str(row["capability_id"])
        matrix_row = rows_by_id.get(capability_id)
        if matrix_row is None:
            raise RuntimeError(f"unknown capability truth row: {capability_id}")
        if matrix_row.get("state") != row["expected_state"]:
            raise RuntimeError(
                f"{capability_id} expected_state {row['expected_state']} "
                f"does not match matrix state {matrix_row.get('state')}"
            )
        if capability_id not in evidence_ids:
            raise RuntimeError(f"{capability_id} has no evidence-map rows")
        for path in row["evidence_paths"]:
            _repo_path(str(path), label=f"{capability_id}.evidence_paths")
        for command in row["required_public_commands"]:
            action = _action_from_public_command(str(command))
            if action not in ACTION_SPECS:
                raise RuntimeError(f"{capability_id} public action not in ACTION_SPECS: {action}")


def _validate_canonical_fixtures(contract: dict[str, Any]) -> None:
    for fixture in contract["canonical_fixtures"]:
        _repo_path(str(fixture["path"]), label=f"canonical_fixtures.{fixture['fixture_id']}.path")


def _validate_workflow_actions(contract: dict[str, Any]) -> None:
    for row in contract["workflow_action_contracts"]:
        action = str(row["action"])
        command_action = _action_from_public_command(str(row["command"]))
        if action != command_action:
            raise RuntimeError(f"{action} does not match public command action {command_action}")
        spec = ACTION_SPECS.get(action)
        if spec is None:
            raise RuntimeError(f"workflow action not in ACTION_SPECS: {action}")
        if spec.backend != row["backend"]:
            raise RuntimeError(
                f"{action} backend drifted: expected {row['backend']}, saw {spec.backend}"
            )
        if row.get("wrapper_only") is not False:
            raise RuntimeError(f"{action} is marked wrapper-only")
        _repo_path(str(row["owner_surface"]), label=f"workflow_action_contracts.{action}.owner_surface")


def _validate_umbrella_readiness(contract: dict[str, Any]) -> None:
    readiness = _load_json(UMBRELLA_READINESS_PATH)
    umbrella_contract = contract["umbrella_readiness_contract"]
    projection_policy = readiness.get("projection_policy", {})
    authoritative_data = set(projection_policy.get("authoritative_data", []))
    for path in umbrella_contract["projection_authoritative_data_must_include"]:
        if path not in authoritative_data:
            raise RuntimeError(f"umbrella readiness authoritative_data missing {path}")

    entries = {
        str(entry["umbrella_capability_id"]): entry
        for entry in readiness.get("entries", [])
        if isinstance(entry, dict) and isinstance(entry.get("umbrella_capability_id"), str)
    }
    for capability_id in umbrella_contract["required_entry_capability_ids"]:
        if capability_id not in entries:
            raise RuntimeError(f"umbrella readiness missing entry {capability_id}")
    for capability_id in umbrella_contract["ready_entry_capability_ids"]:
        entry = entries.get(str(capability_id))
        if entry is None or entry.get("readiness_state") != "ready":
            raise RuntimeError(f"umbrella readiness entry is not ready: {capability_id}")
        if entry.get("promotion_blockers"):
            raise RuntimeError(f"umbrella readiness entry has blockers: {capability_id}")
    for capability_id in umbrella_contract["blocked_entry_capability_ids"]:
        entry = entries.get(str(capability_id))
        if entry is None or entry.get("readiness_state") != "blocked":
            raise RuntimeError(f"umbrella readiness entry is not blocked: {capability_id}")


def _validate_dependent_contracts(contract: dict[str, Any]) -> None:
    standalone_payload = _load_json(STANDALONE_TEXTUAL_INTERFACE_FIXTURE)
    validate_standalone_textual_interface_payload(standalone_payload)
    negative_ids = {str(row["case_id"]) for row in standalone_payload["negative_cases"]}
    if negative_ids != REQUIRED_NEGATIVE_CASE_IDS:
        raise RuntimeError(
            "standalone textual interface declared negative cases drifted: "
            f"{sorted(negative_ids)}"
        )

    executable_contract = _load_json(ADVANCED_RUNTIME_EXECUTABLE_FIXTURE)
    if executable_contract.get("dependency_issue_ref") != 8213:
        raise RuntimeError("advanced runtime executable contract is not tied to #8213")
    command = str(executable_contract.get("public_command", ""))
    if _action_from_public_command(command) not in ACTION_SPECS:
        raise RuntimeError("advanced runtime executable public command is not registered")

    declared_foundation_negative_cases = {
        str(row["case_id"]) for row in contract["negative_case_contracts"]
    }
    missing_negative_cases = REQUIRED_NEGATIVE_CASE_IDS - declared_foundation_negative_cases
    if missing_negative_cases:
        raise RuntimeError(
            "foundation source-truth contract omits standalone negative cases: "
            f"{sorted(missing_negative_cases)}"
        )
    for row in contract["negative_case_contracts"]:
        _repo_path(str(row["source"]), label=f"negative_case_contracts.{row['case_id']}.source")


def validate_foundations_source_truth_payload(contract: dict[str, Any]) -> dict[str, Any]:
    validate_registered_schema(contract, SCHEMA_ID)
    _validate_schema_contracts(contract)
    _validate_capability_truth(contract)
    _validate_canonical_fixtures(contract)
    _validate_workflow_actions(contract)
    _validate_umbrella_readiness(contract)
    _validate_dependent_contracts(contract)
    return {
        "status": "PASS",
        "issue": contract["issue"],
        "roadmap_issue_refs": sorted(contract["roadmap_issue_refs"]),
        "capability_rows": sorted(
            row["capability_id"] for row in contract["capability_truth_rows"]
        ),
        "workflow_actions": sorted(row["action"] for row in contract["workflow_action_contracts"]),
        "negative_cases": sorted(row["case_id"] for row in contract["negative_case_contracts"]),
    }


def validate_foundations_source_truth(path: Path = CONTRACT_PATH) -> dict[str, Any]:
    contract = _load_json(path)
    if path == CONTRACT_PATH and contract.get("source_path") != CONTRACT_PATH.relative_to(ROOT).as_posix():
        raise RuntimeError("foundation source-truth fixture source_path drifted")
    return validate_foundations_source_truth_payload(contract)


def negative_foundations_source_truth_cases() -> dict[str, str]:
    contract = _load_json(CONTRACT_PATH)
    cases: dict[str, str] = {}

    temp_source = deepcopy(contract)
    temp_source["canonical_fixtures"][0]["path"] = "temp/generated/standalone.json"
    cases["generated-source-path"] = _failure(temp_source)

    unregistered_action = deepcopy(contract)
    unregistered_action["workflow_action_contracts"][0]["action"] = "validate-invented-foundation"
    unregistered_action["workflow_action_contracts"][0][
        "command"
    ] = "npm run objc3c -- validate-invented-foundation"
    cases["unregistered-public-command"] = _failure(unregistered_action)

    state_drift = deepcopy(contract)
    state_drift["capability_truth_rows"][2]["expected_state"] = "reserved"
    cases["capability-state-drift"] = _failure(state_drift)

    missing_projection_contract = deepcopy(contract)
    missing_projection_contract["umbrella_readiness_contract"][
        "projection_authoritative_data_must_include"
    ] = ["tests/tooling/fixtures/foundations_umbrella_readiness/missing.json"]
    cases["missing-umbrella-authoritative-data"] = _failure(missing_projection_contract)
    return cases


def _failure(contract: dict[str, Any]) -> str:
    try:
        validate_foundations_source_truth_payload(contract)
    except Exception as exc:  # noqa: BLE001 - negative fixture summarization
        return str(exc)
    raise RuntimeError("negative foundations source-truth contract unexpectedly passed")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--negative-cases", action="store_true", help="include negative case summaries")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    summary = validate_foundations_source_truth()
    if args.negative_cases:
        summary["negative_case_failures"] = negative_foundations_source_truth_cases()
    print(json.dumps(summary, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
