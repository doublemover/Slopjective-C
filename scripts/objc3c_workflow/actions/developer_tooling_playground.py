"""Playground and editor-facing developer-tooling actions."""

from __future__ import annotations

import hashlib
import json
import os
import re
import subprocess
import sys
from pathlib import Path

from objc3c_tooling.json_io import load_json_object as load_json

from ..commands import extract_output_line, run
from ..environment import ROOT, WORKFLOW_COMMAND_TEXT
from .developer_tooling_paths import (
    DEFAULT_PLAYGROUND_SOURCE,
    EDITOR_TOOLING_SURFACE_PY,
    FORMAT_OBJC3C_SOURCE_PY,
    FRONTEND_C_API_RUNNER_EXE,
    PLAYGROUND_ARTIFACT_ROOT,
    PLAYGROUND_REPORT_ROOT,
    PLAYGROUND_WORKSPACE_CONTRACT_ID,
)


def _execute_registered_action(action: str, rest: list[str]) -> int:
    from scripts.objc3c_workflow.action_dispatch import execute_registered_action

    return execute_registered_action(action, rest)


def _parse_playground_invocation(rest: list[str]) -> tuple[str, list[str]]:
    if rest and not rest[0].startswith("--"):
        source_text = rest[0]
        passthrough = rest[1:]
    else:
        source_text = str(DEFAULT_PLAYGROUND_SOURCE.relative_to(ROOT).as_posix())
        passthrough = rest
    for forbidden in (
        "--out-dir",
        "--emit-prefix",
        "--summary-out",
        "--dump-summary-json",
        "--dump-observability-json",
        "--dump-playground-repro-json",
        "--dump-runtime-inspector-json",
        "--dump-stage-trace-json",
    ):
        if forbidden in passthrough:
            raise ValueError(f"{forbidden} is managed by the public playground action")
    return source_text, passthrough


def _resolve_source_path(source_text: str) -> tuple[Path, str]:
    candidate = Path(source_text)
    resolved = candidate if candidate.is_absolute() else ROOT / candidate
    resolved = resolved.resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"playground source not found: {source_text}")
    try:
        display = resolved.relative_to(ROOT).as_posix()
    except ValueError:
        display = resolved.as_posix()
    return resolved, display


def _slugify_playground_workspace(source_display: str) -> str:
    safe_stem = re.sub(r"[^a-z0-9]+", "-", Path(source_display).stem.lower()).strip("-")
    if not safe_stem:
        safe_stem = "source"
    digest = hashlib.sha256(source_display.encode("utf-8")).hexdigest()[:12]
    return f"{safe_stem}-{digest}"


def ensure_frontend_runner_ready() -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if not native_main.is_file() and FRONTEND_C_API_RUNNER_EXE.is_file():
        return 0
    return _execute_registered_action("build-native-binaries", [])


def _run_playground_workspace(
    rest: list[str],
    *,
    emit_payload: bool,
) -> int:
    try:
        source_text, passthrough = _parse_playground_invocation(rest)
        _, source_display = _resolve_source_path(source_text)
    except (ValueError, FileNotFoundError) as exc:
        print(str(exc), file=sys.stderr)
        return 2

    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc

    workspace_id = _slugify_playground_workspace(source_display)
    workspace_root = PLAYGROUND_ARTIFACT_ROOT / workspace_id
    artifact_root = workspace_root / "build"
    report_root = PLAYGROUND_REPORT_ROOT / workspace_id
    workspace_manifest_path = workspace_root / "workspace.json"
    summary_path = report_root / "compile-summary.json"
    dump_path = report_root / "playground-repro.json"

    artifact_root.mkdir(parents=True, exist_ok=True)
    report_root.mkdir(parents=True, exist_ok=True)

    artifact_root_rel = artifact_root.relative_to(ROOT).as_posix()
    summary_path_rel = summary_path.relative_to(ROOT).as_posix()

    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(
        [
            str(FRONTEND_C_API_RUNNER_EXE),
            source_display,
            "--out-dir",
            artifact_root_rel,
            "--emit-prefix",
            "module",
            "--summary-out",
            summary_path_rel,
            "--dump-playground-repro-json",
            *passthrough,
        ],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        if result.stdout:
            sys.stdout.write(result.stdout)
        return result.returncode

    try:
        playground_payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        print(
            f"playground-workspace: invalid JSON from frontend runner: {exc}",
            file=sys.stderr,
        )
        return 1

    dump_path.write_text(json.dumps(playground_payload, indent=2) + "\n", encoding="utf-8")

    editor_result = subprocess.run(
        [sys.executable, str(EDITOR_TOOLING_SURFACE_PY), source_display],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if editor_result.stdout:
        sys.stdout.write(editor_result.stdout)
    if editor_result.stderr:
        sys.stderr.write(editor_result.stderr)
    if editor_result.returncode != 0:
        return editor_result.returncode

    editor_surface_path_text = extract_output_line(editor_result.stdout, "dump_path:")
    capabilities_path_text = extract_output_line(editor_result.stdout, "capabilities_path:")
    navigation_path_text = extract_output_line(editor_result.stdout, "navigation_path:")
    formatter_path_text = extract_output_line(editor_result.stdout, "formatter_path:")
    debug_path_text = extract_output_line(editor_result.stdout, "debug_path:")
    if not editor_surface_path_text:
        print(
            "playground-workspace: editor tooling action did not publish dump_path",
            file=sys.stderr,
        )
        return 1
    editor_surface_path = ROOT / editor_surface_path_text
    if not editor_surface_path.is_file():
        print(
            f"playground-workspace: missing editor tooling surface {editor_surface_path_text}",
            file=sys.stderr,
        )
        return 1
    editor_surface_payload = load_json(editor_surface_path)
    formatter_payload = editor_surface_payload.get("formatter", {})
    debug_payload = editor_surface_payload.get("debug", {})
    workspace_drill_commands = {
        "inspect_editor_tooling": f"{WORKFLOW_COMMAND_TEXT} inspect-editor-tooling {source_display}",
        "format_preview": f"{WORKFLOW_COMMAND_TEXT} format-objc3c {source_display}",
        "object_symbol_inventory": str(debug_payload.get("object_symbol_inventory_command", "")),
    }

    workspace_payload = {
        "contract_id": PLAYGROUND_WORKSPACE_CONTRACT_ID,
        "schema_version": 1,
        "workspace_id": workspace_id,
        "source_path": source_display,
        "workspace_root": workspace_root.relative_to(ROOT).as_posix(),
        "artifact_root": artifact_root.relative_to(ROOT).as_posix(),
        "report_root": report_root.relative_to(ROOT).as_posix(),
        "emit_prefix": "module",
        "summary_path": summary_path.relative_to(ROOT).as_posix(),
        "playground_payload_path": dump_path.relative_to(ROOT).as_posix(),
        "playground_payload_contract_id": playground_payload.get("contract_id"),
        "public_actions": [
            "materialize-playground-workspace",
            "compile-objc3c",
            "inspect-playground-repro",
            "inspect-compile-observability",
            "inspect-editor-tooling",
            "format-objc3c",
            "trace-compile-stages",
            "validate-developer-tooling",
        ],
        "compile_profile": playground_payload.get("compile_profile", {}),
        "artifact_paths": playground_payload.get("artifact_paths", {}),
        "showcase_examples": playground_payload.get("showcase_examples", []),
        "repro_command": playground_payload.get("dump_commands", {}).get("repro_runner", ""),
        "editor_tooling": {
            "editor_surface_path": editor_surface_path_text,
            "language_server_capabilities_path": capabilities_path_text,
            "navigation_path": navigation_path_text,
            "formatter_path": formatter_path_text,
            "debug_path": debug_path_text,
            "formatted_output_path": formatter_payload.get("formatted_output_path"),
            "format_preview_supported": formatter_payload.get("supported"),
            "debugger_model": debug_payload.get("debugger_model", ""),
            "declaration_breakpoint_anchor_count": debug_payload.get(
                "declaration_breakpoint_anchor_count",
                0,
            ),
            "statement_level_stepping": debug_payload.get("statement_level_stepping"),
        },
        "workspace_drill_commands": workspace_drill_commands,
    }
    workspace_manifest_path.write_text(
        json.dumps(workspace_payload, indent=2) + "\n",
        encoding="utf-8",
    )

    if emit_payload:
        sys.stdout.write(json.dumps(playground_payload, indent=2) + "\n")
    print(f"workspace_path: {workspace_manifest_path.relative_to(ROOT).as_posix()}")
    print(f"summary_path: {summary_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"artifact_root: {artifact_root.relative_to(ROOT).as_posix()}")
    return 0


def action_inspect_editor_tooling(rest: list[str]) -> int:
    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return run([sys.executable, str(EDITOR_TOOLING_SURFACE_PY), *rest])


def action_format_objc3c(rest: list[str]) -> int:
    return run([sys.executable, str(FORMAT_OBJC3C_SOURCE_PY), *rest])


def action_inspect_playground_repro(rest: list[str]) -> int:
    return _run_playground_workspace(rest, emit_payload=True)


def action_materialize_playground_workspace(rest: list[str]) -> int:
    return _run_playground_workspace(rest, emit_payload=False)
