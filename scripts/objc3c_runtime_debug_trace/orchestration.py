"""Command orchestration for runtime debug trace generation."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object, write_json_file
from objc3c_tooling.paths import ROOT, display_path
from objc3c_tooling.public_workflow_output import extract_line_value
from objc3c_tooling.subprocesses import run_timed
from scripts.objc3c_editor_tooling.paths import paths_for_source, resolve_source
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .contracts import (
    COMPILE_STAGE_TRACE_REPORT_PATH,
    RUNTIME_DEBUG_TRACE_SUMMARY_PATH,
    RUNTIME_INSPECTOR_REPORT_PATH,
)
from .payload import build_runtime_debug_trace_payload
from .validation import validate_runtime_debug_trace_payload


def _step_payload(name: str, command: list[str]) -> dict[str, Any]:
    completed = run_timed(command, cwd=ROOT, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": completed.returncode,
        "duration_ms": completed.duration_ms,
        "_stdout": completed.stdout,
        "_stderr": completed.stderr,
        "stdout_snippet": completed.stdout[-2000:],
        "stderr_snippet": completed.stderr[-2000:],
    }


def _load_report(path: Path, label: str, failures: list[str]) -> dict[str, Any]:
    if not path.is_file():
        failures.append(f"missing {label} report: {display_path(path)}")
        return {}
    return load_json_object(path)


def _path_from_output_or_model(path_text: str, modeled_path: Path) -> Path:
    if not path_text:
        return modeled_path
    candidate = Path(path_text)
    return candidate if candidate.is_absolute() else ROOT / candidate


def build_runtime_debug_trace_from_live_actions(
    *,
    source_path: str,
    trace_out: Path = RUNTIME_DEBUG_TRACE_SUMMARY_PATH,
) -> tuple[int, dict[str, Any]]:
    raw_steps = [
        _step_payload(
            "inspect-runtime-inspector",
            public_workflow_command("inspect-runtime-inspector", source_path),
        ),
        _step_payload(
            "trace-compile-stages",
            public_workflow_command("trace-compile-stages", source_path),
        ),
        _step_payload(
            "inspect-editor-tooling",
            public_workflow_command("inspect-editor-tooling", source_path),
        ),
    ]
    steps = [
        {
            key: value
            for key, value in step.items()
            if not key.startswith("_")
        }
        for step in raw_steps
    ]
    failures = [
        f"{step['name']} failed"
        for step in raw_steps
        if int(step.get("exit_code", 1)) != 0
    ]

    editor_stdout = str(raw_steps[-1].get("_stdout", ""))
    editor_surface_path_text = extract_line_value(editor_stdout, "dump_path:")
    debug_map_path_text = extract_line_value(editor_stdout, "debug_path:")

    editor_paths = paths_for_source(resolve_source(source_path))
    editor_surface_path = _path_from_output_or_model(
        editor_surface_path_text,
        editor_paths.editor_surface,
    )
    debug_map_path = _path_from_output_or_model(
        debug_map_path_text,
        editor_paths.debug_map,
    )
    runtime_inspector = _load_report(
        RUNTIME_INSPECTOR_REPORT_PATH,
        "runtime inspector",
        failures,
    )
    stage_trace = _load_report(
        COMPILE_STAGE_TRACE_REPORT_PATH,
        "compile stage trace",
        failures,
    )
    editor_surface = _load_report(editor_surface_path, "editor surface", failures)
    debug_map = _load_report(debug_map_path, "debug map", failures)

    payload = build_runtime_debug_trace_payload(
        source_path=source_path,
        runtime_inspector=runtime_inspector,
        stage_trace=stage_trace,
        editor_surface=editor_surface,
        debug_map=debug_map,
        input_paths={
            "runtime_inspector": RUNTIME_INSPECTOR_REPORT_PATH,
            "compile_stage_trace": COMPILE_STAGE_TRACE_REPORT_PATH,
            "editor_surface": editor_surface_path,
            "debug_map": debug_map_path,
        },
        steps=steps,
        initial_failures=failures,
    )
    payload_failures = validate_runtime_debug_trace_payload(payload)
    if payload_failures:
        payload = {**payload, "ok": False, "failures": [*payload["failures"], *payload_failures]}
    trace_out.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(trace_out, payload)
    return (0 if payload.get("ok") is True else 1), payload
