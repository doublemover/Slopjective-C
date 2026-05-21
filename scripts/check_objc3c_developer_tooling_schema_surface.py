#!/usr/bin/env python3
"""Validate the developer-tooling editor surface schema and generated payloads."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    validate_json_schema,
    write_report_json,
)
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_workflow_output import extract_line_value
from objc3c_tooling.subprocesses import run_capture


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "tooling_schema_surface_contract.json"
)
SUMMARY_OUT = (
    ROOT
    / "tmp"
    / "reports"
    / "developer-tooling"
    / "schema-surface"
    / "tooling_schema_surface_summary.json"
)

SUBPAYLOAD_PATH_FIELDS = {
    "language_server": "capabilities_path",
    "navigation": "navigation_path",
    "workspace_index": "workspace_index_path",
    "artifact_inspector": "artifact_inspector_path",
    "formatter": "formatter_path",
    "debug": "debug_path",
}


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def repo_path(path_text: str) -> Path:
    return ROOT / Path(path_text)


def run_inspect_editor_tooling(source_path: str) -> Any:
    return run_capture(
        [
            sys.executable,
            "-m",
            "scripts.objc3c_workflow",
            "inspect-editor-tooling",
            source_path,
        ],
        cwd=ROOT,
        capture_output=True,
        echo=False,
    )


def output_paths(stdout: str, required_labels: list[str]) -> dict[str, str]:
    paths: dict[str, str] = {}
    for label in required_labels:
        value = extract_line_value(stdout, f"{label}:")
        if value:
            paths[label] = value
    return paths


def load_payload(path_text: str, failures: list[str]) -> dict[str, Any]:
    path = repo_path(path_text)
    if not path.is_file():
        failures.append(f"missing generated payload: {path_text}")
        return {}
    return load_json_object(path)


def validate_schema_payload(
    *,
    payload: dict[str, Any],
    schema: dict[str, Any],
    label: str,
    failures: list[str],
) -> None:
    try:
        validate_json_schema(payload, schema, label=label)
    except JsonSchemaValidationError as exc:
        failures.append(str(exc))


def validate_embedded_payload_consistency(
    editor_surface: dict[str, Any],
    subpayloads: dict[str, dict[str, Any]],
    failures: list[str],
) -> None:
    for subpayload_name, payload in subpayloads.items():
        if subpayload_name == "workspace_index":
            embedded = editor_surface.get("navigation", {}).get("workspace_index")
        else:
            embedded = editor_surface.get(subpayload_name)
        expect(
            embedded == payload,
            f"{subpayload_name} generated file drifted from embedded editor surface payload",
            failures,
        )


def validate_capability_boundaries(
    language_server: dict[str, Any],
    contract: dict[str, Any],
    failures: list[str],
) -> None:
    statuses = language_server.get("capability_statuses", {})
    supported_ids = set(language_server.get("supported_capability_ids", []))
    unpublished_ids = set(language_server.get("unpublished_capability_ids", []))

    for capability_id in contract["expected_supported_capabilities"]:
        status = statuses.get(capability_id, {})
        expect(capability_id in supported_ids, f"missing supported capability: {capability_id}", failures)
        expect(status.get("supported") is True, f"{capability_id} status did not publish supported=true", failures)
        expect(status.get("fail_closed") is False, f"{capability_id} supported row should not be fail-closed", failures)
        expect(bool(status.get("evidence_ids")), f"{capability_id} supported row lacks evidence_ids", failures)

    for capability_id in contract["expected_fail_closed_capabilities"]:
        status = statuses.get(capability_id, {})
        expect(capability_id in unpublished_ids, f"missing unpublished capability: {capability_id}", failures)
        expect(capability_id not in supported_ids, f"fail-closed capability was also published: {capability_id}", failures)
        expect(status.get("supported") is False, f"{capability_id} status did not publish supported=false", failures)
        expect(status.get("fail_closed") is True, f"{capability_id} status did not fail closed", failures)
        expect(not status.get("evidence_ids"), f"{capability_id} fail-closed row should not publish evidence_ids", failures)
        expect(bool(status.get("unpublished_reason")), f"{capability_id} missing unpublished_reason", failures)


def validate_workspace_surface(
    workspace_index: dict[str, Any],
    contract: dict[str, Any],
    failures: list[str],
) -> None:
    expect(workspace_index.get("available") is True, "workspace index should be available for the contract source", failures)
    expect(workspace_index.get("fail_closed") is False, "workspace index should not fail closed for the contract source", failures)
    expect(len(str(workspace_index.get("workspace_index_digest", ""))) == 64, "workspace index digest must be SHA-256 sized", failures)
    expect(
        workspace_index.get("unsupported_surfaces") == contract["expected_workspace_unsupported_surfaces"],
        "workspace unsupported surface inventory drifted",
        failures,
    )
    evidence_roots = set(workspace_index.get("evidence_roots", []))
    for input_path in contract["expected_workspace_source_truth_inputs"]:
        expect(input_path in evidence_roots, f"workspace evidence root missing: {input_path}", failures)
    guardrails = workspace_index.get("guardrails", {})
    expect(guardrails.get("ok") is True, "workspace guardrails should pass", failures)
    for check_name, check_value in guardrails.get("checks", {}).items():
        expect(check_value is True, f"workspace guardrail failed: {check_name}", failures)


def validate_artifact_inspector_surface(
    artifact_inspector: dict[str, Any],
    contract: dict[str, Any],
    failures: list[str],
) -> None:
    record_kinds = sorted(record.get("kind") for record in artifact_inspector.get("artifact_records", []))
    expect(
        record_kinds == contract["expected_artifact_record_kinds"],
        "artifact inspector record kind inventory drifted",
        failures,
    )
    for record in artifact_inspector.get("artifact_records", []):
        kind = record.get("kind", "<unknown>")
        if record.get("available") is True:
            expect(len(str(record.get("sha256", ""))) == 64, f"{kind} record missing SHA-256 digest", failures)
            expect(int(record.get("size_bytes", 0) or 0) > 0, f"{kind} record should have nonzero size", failures)
            expect(not record.get("retired_route_reason"), f"{kind} available record should not publish retired_route_reason", failures)
        else:
            expect(bool(record.get("retired_route_reason")), f"{kind} unavailable record missing retired_route_reason", failures)
    inspection_commands = sorted(artifact_inspector.get("inspection_commands", {}))
    expect(
        inspection_commands == contract["expected_inspection_command_keys"],
        "artifact inspector command inventory drifted",
        failures,
    )
    object_payload = artifact_inspector.get("object", {})
    if object_payload.get("available") is True:
        expect(object_payload.get("inspection_ready") is True, "available object artifact should be inspection_ready", failures)


def validate_debug_surface(
    debug_surface: dict[str, Any],
    contract: dict[str, Any],
    failures: list[str],
) -> None:
    reserved = {
        row.get("capability_id"): row
        for row in debug_surface.get("reserved_capability_rows", [])
        if isinstance(row, dict)
    }
    for capability_id in contract["reserved_debug_capabilities"]:
        row = reserved.get(capability_id, {})
        expect(row.get("status") == "reserved", f"debug row is not reserved: {capability_id}", failures)
        expect(row.get("fail_closed") is True, f"debug row does not fail closed: {capability_id}", failures)
        expect(bool(row.get("unpublished_reason")), f"debug row missing unpublished_reason: {capability_id}", failures)
    expect(debug_surface.get("statement_level_stepping") is False, "debug surface overpublished statement-level stepping", failures)
    expect(debug_surface.get("source_map_supported") is False, "debug surface overpublished source maps", failures)


def build_summary(
    *,
    contract: dict[str, Any],
    editor_surface_path: str,
    checked_payload_paths: dict[str, str],
    failures: list[str],
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.schema.surface.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "ok": not failures,
        "generated_by": "scripts/check_objc3c_developer_tooling_schema_surface.py",
        "schema_path": contract["editor_surface_schema"],
        "schema_summary_path": contract["summary_schema"],
        "source_path": contract["source"],
        "editor_surface_path": editor_surface_path,
        "checked_payload_paths": checked_payload_paths,
        "checked_subpayloads": list(contract["checked_subpayloads"]),
        "fail_closed_capabilities": list(contract["expected_fail_closed_capabilities"]),
        "reserved_debug_capabilities": list(contract["reserved_debug_capabilities"]),
        "failures": failures,
    }


def main() -> int:
    contract = load_json_object(CONTRACT_PATH)
    failures: list[str] = []

    result = run_inspect_editor_tooling(str(contract["source"]))
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    expect(result.returncode == 0, "inspect-editor-tooling failed", failures)

    labels = list(contract["required_output_labels"])
    paths = output_paths(result.stdout, labels)
    for label in labels:
        expect(label in paths, f"inspect-editor-tooling did not publish {label}", failures)

    editor_surface_path = paths.get("dump_path", "")
    editor_surface = load_payload(editor_surface_path, failures) if editor_surface_path else {}
    subpayloads = {
        name: load_payload(paths[field], failures)
        for name, field in SUBPAYLOAD_PATH_FIELDS.items()
        if field in paths
    }

    editor_schema = load_json_object(repo_path(str(contract["editor_surface_schema"])))
    summary_schema = load_json_object(repo_path(str(contract["summary_schema"])))
    if editor_surface:
        validate_schema_payload(
            payload=editor_surface,
            schema=editor_schema,
            label=editor_surface_path,
            failures=failures,
        )
    validate_embedded_payload_consistency(editor_surface, subpayloads, failures)
    if "language_server" in subpayloads:
        validate_capability_boundaries(subpayloads["language_server"], contract, failures)
    if "workspace_index" in subpayloads:
        validate_workspace_surface(subpayloads["workspace_index"], contract, failures)
    if "artifact_inspector" in subpayloads:
        validate_artifact_inspector_surface(subpayloads["artifact_inspector"], contract, failures)
    if "debug" in subpayloads:
        validate_debug_surface(subpayloads["debug"], contract, failures)

    checked_payload_paths = {
        field: paths.get(field, "")
        for field in SUBPAYLOAD_PATH_FIELDS.values()
    }
    summary = build_summary(
        contract=contract,
        editor_surface_path=editor_surface_path,
        checked_payload_paths=checked_payload_paths,
        failures=failures,
    )
    write_report_json(SUMMARY_OUT, summary, sort_keys=False, schema=summary_schema)
    print(f"summary_path: {repo_rel(SUMMARY_OUT)}")
    print("developer-tooling-schema-surface: OK" if not failures else "developer-tooling-schema-surface: FAIL")
    for failure in failures:
        print(f"- {failure}", file=sys.stderr)
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
