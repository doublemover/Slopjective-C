"""Packaged public-tooling drills for runnable developer-tooling E2E validation."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import normalize_rel_path
from objc3c_tooling.public_workflow_output import extract_output_value

from .assertions import expect
from .commands import run_format_objc3c
from .commands import run_inspect_editor_tooling
from .commands import run_materialize_playground_workspace
from .commands import run_validate_developer_tooling
from .outputs import extract_last_output_value


def _assert_runtime_debug_trace_payload(
    payload: dict[str, Any],
    *,
    contract: dict[str, Any],
    label: str,
) -> None:
    expect(
        payload.get("runtime_debug_trace_command")
        == contract["expected_runtime_debug_trace_command"],
        f"{label} runtime debug trace command drifted",
    )
    expect(
        payload.get("runtime_debug_trace_path")
        == contract["expected_runtime_debug_trace_path"],
        f"{label} runtime debug trace path drifted",
    )
    expect(
        payload.get("runtime_debug_trace_schema")
        == contract["expected_runtime_debug_trace_schema"],
        f"{label} runtime debug trace schema drifted",
    )
    expect(
        payload.get("runtime_debug_trace_model")
        == contract["expected_runtime_debug_trace_model"],
        f"{label} runtime debug trace model drifted",
    )


def run_inspect_editor_tooling_check(
    *,
    package_root: Path,
    hello_source: Path,
    contract: dict[str, Any],
) -> tuple[Any, str]:
    inspect_result = run_inspect_editor_tooling(hello_source, cwd=package_root)
    if inspect_result.returncode != 0:
        raise RuntimeError("packaged inspect-editor-tooling failed")
    dump_path_text = extract_output_value(inspect_result.stdout, "dump_path")
    expect(
        bool(dump_path_text),
        "packaged inspect-editor-tooling did not publish dump_path",
    )
    editor_surface = load_json(package_root / normalize_rel_path(str(dump_path_text)))
    debug_payload = editor_surface.get("debug", {})
    navigation_payload = editor_surface.get("navigation", {})
    workspace_index = navigation_payload.get("workspace_index", {})
    formatter_payload = editor_surface.get("formatter", {})
    expect(
        formatter_payload.get("supported") is True,
        "packaged editor surface did not report formatter support",
    )
    expect(
        debug_payload.get("supported") is True,
        "packaged editor surface did not report debug support",
    )
    expect(
        debug_payload.get("debugger_model") == contract["expected_debugger_model"],
        "packaged editor surface debugger model drifted",
    )
    expect(
        debug_payload.get("statement_level_stepping")
        is (not contract["expected_fail_closed_statement_stepping"]),
        "packaged editor surface stepping availability drifted",
    )
    expect(
        navigation_payload.get("available") is True,
        "packaged editor surface did not publish navigation availability",
    )
    expect(
        workspace_index.get("available") is True,
        "packaged editor surface did not publish workspace index availability",
    )
    expect(
        int(workspace_index.get("package_count", 0)) >= 9,
        "packaged editor surface workspace index did not include stdlib/showcase packages",
    )
    expect(
        workspace_index.get("guardrails", {}).get("ok") is True,
        "packaged editor surface workspace package guardrails failed",
    )
    expect(
        int(debug_payload.get("declaration_breakpoint_anchor_count", 0)) >= 3,
        "packaged editor surface did not publish enough breakpoint anchors",
    )
    _assert_runtime_debug_trace_payload(
        debug_payload,
        contract=contract,
        label="packaged editor surface",
    )
    return inspect_result, str(dump_path_text)


def run_format_check(
    *,
    package_root: Path,
    format_source: Path,
    expected_formatted_source: Path,
) -> tuple[Any, str]:
    format_result = run_format_objc3c(format_source, cwd=package_root)
    if format_result.returncode != 0:
        raise RuntimeError("packaged format-objc3c failed")
    format_summary_path_text = extract_output_value(format_result.stdout, "summary_path")
    expect(
        bool(format_summary_path_text),
        "packaged format-objc3c did not publish summary_path",
    )
    format_summary = load_json(package_root / normalize_rel_path(str(format_summary_path_text)))
    expected_formatted_text = expected_formatted_source.read_text(encoding="utf-8")
    packaged_formatted_output = package_root / normalize_rel_path(
        str(format_summary.get("formatted_output_path", ""))
    )
    expect(
        packaged_formatted_output.is_file(),
        "packaged formatter did not publish formatted_output_path",
    )
    expect(
        packaged_formatted_output.read_text(encoding="utf-8") == expected_formatted_text,
        "packaged formatter output drifted from the checked-in canonical formatted fixture",
    )
    return format_result, str(format_summary_path_text)


def run_workspace_check(
    *,
    package_root: Path,
    hello_source: Path,
    contract: dict[str, Any],
) -> tuple[Any, str]:
    workspace_result = run_materialize_playground_workspace(
        hello_source,
        cwd=package_root,
    )
    if workspace_result.returncode != 0:
        raise RuntimeError("packaged materialize-playground-workspace failed")
    workspace_path_text = extract_output_value(workspace_result.stdout, "workspace_path")
    expect(
        bool(workspace_path_text),
        "packaged materialize-playground-workspace did not publish workspace_path",
    )
    workspace = load_json(package_root / normalize_rel_path(str(workspace_path_text)))
    workspace_editor_tooling = workspace.get("editor_tooling", {})
    expect(isinstance(workspace_editor_tooling, dict), "packaged workspace did not publish editor_tooling")
    expect(
        workspace_editor_tooling.get("debugger_model") == contract["expected_debugger_model"],
        "packaged workspace debugger model drifted",
    )
    expect(
        workspace_editor_tooling.get("statement_level_stepping")
        is (not contract["expected_fail_closed_statement_stepping"]),
        "packaged workspace stepping availability drifted",
    )
    expect(
        workspace_editor_tooling.get("workspace_index_guardrails_ok") is True,
        "packaged workspace semantic index guardrail status drifted",
    )
    expect(
        int(workspace_editor_tooling.get("workspace_package_count", 0)) >= 9,
        "packaged workspace semantic index package count drifted",
    )
    _assert_runtime_debug_trace_payload(
        workspace_editor_tooling,
        contract=contract,
        label="packaged workspace",
    )
    for action in (
        "inspect-editor-tooling",
        "format-objc3c",
        "trace-runtime-debug",
        "validate-developer-tooling",
    ):
        expect(
            action in workspace.get("public_actions", []),
            f"packaged workspace missing public action {action}",
        )
    return workspace_result, str(workspace_path_text)


def run_integrated_validation_check(
    *,
    package_root: Path,
    contract: dict[str, Any],
) -> tuple[Any, str]:
    integrated_result = run_validate_developer_tooling(cwd=package_root)
    if integrated_result.returncode != 0:
        raise RuntimeError("packaged validate-developer-tooling failed")
    integrated_summary_path_text = extract_last_output_value(
        integrated_result.stdout,
        "summary_path",
    )
    expect(
        bool(integrated_summary_path_text),
        "packaged validate-developer-tooling did not publish summary_path",
    )
    integrated_summary = load_json(package_root / normalize_rel_path(str(integrated_summary_path_text)))
    expect(
        integrated_summary.get("ok") is True,
        "packaged developer-tooling integration summary did not report ok=true",
    )
    step_names = {
        str(step.get("name", ""))
        for step in integrated_summary.get("steps", [])
        if isinstance(step, dict)
    }
    expect(
        "trace-runtime-debug" in step_names,
        "packaged developer-tooling integration did not run trace-runtime-debug",
    )
    reports = integrated_summary.get("reports", {})
    expect(
        isinstance(reports, dict),
        "packaged developer-tooling integration summary did not publish reports",
    )
    expect(
        reports.get("runtime_debug_trace") == contract["expected_runtime_debug_trace_path"],
        "packaged developer-tooling integration runtime debug trace path drifted",
    )
    runtime_debug_trace_path = package_root / normalize_rel_path(
        contract["expected_runtime_debug_trace_path"]
    )
    expect(
        runtime_debug_trace_path.is_file(),
        "packaged developer-tooling integration did not publish runtime debug trace report",
    )
    runtime_debug_trace = load_json(runtime_debug_trace_path)
    expect(
        runtime_debug_trace.get("contract_id") == "objc3c.runtime.debug.trace.v1",
        "packaged runtime debug trace report contract id drifted",
    )
    expect(
        runtime_debug_trace.get("ok") is True,
        "packaged runtime debug trace report did not report ok=true",
    )
    return integrated_result, str(integrated_summary_path_text)


__all__ = [
    "run_format_check",
    "run_inspect_editor_tooling_check",
    "run_integrated_validation_check",
    "run_workspace_check",
]
