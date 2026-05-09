"""Workspace materialization for public playground workflow actions."""

from __future__ import annotations

import json
import sys
from pathlib import Path

from objc3c_tooling.json_io import load_json_object as load_json

from ..commands import extract_output_line
from ..environment import ROOT, WORKFLOW_COMMAND_TEXT
from .developer_tooling_paths import (
    PLAYGROUND_ARTIFACT_ROOT,
    PLAYGROUND_REPORT_ROOT,
    PLAYGROUND_WORKSPACE_CONTRACT_ID,
)
from .developer_tooling_playground_inputs import (
    PlaygroundInvocation,
    resolve_playground_invocation,
)
from .developer_tooling_playground_runner import (
    ensure_frontend_runner_ready,
    playground_subprocess_env,
    run_editor_tooling,
    run_frontend_playground,
)


def _playground_workspace_paths(invocation: PlaygroundInvocation) -> tuple[Path, Path, Path, Path, Path]:
    workspace_root = PLAYGROUND_ARTIFACT_ROOT / invocation.workspace_id
    artifact_root = workspace_root / "build"
    report_root = PLAYGROUND_REPORT_ROOT / invocation.workspace_id
    workspace_manifest_path = workspace_root / "workspace.json"
    summary_path = report_root / "compile-summary.json"
    dump_path = report_root / "playground-repro.json"
    return workspace_root, artifact_root, report_root, workspace_manifest_path, summary_path, dump_path


def _load_editor_surface(editor_result_stdout: str) -> tuple[dict[str, object], dict[str, str]]:
    editor_surface_path_text = extract_output_line(editor_result_stdout, "dump_path:")
    published_paths = {
        "editor_surface_path": editor_surface_path_text,
        "language_server_capabilities_path": extract_output_line(
            editor_result_stdout,
            "capabilities_path:",
        ),
        "navigation_path": extract_output_line(editor_result_stdout, "navigation_path:"),
        "formatter_path": extract_output_line(editor_result_stdout, "formatter_path:"),
        "debug_path": extract_output_line(editor_result_stdout, "debug_path:"),
    }
    if not editor_surface_path_text:
        raise RuntimeError("playground-workspace: editor tooling action did not publish dump_path")
    editor_surface_path = ROOT / editor_surface_path_text
    if not editor_surface_path.is_file():
        raise FileNotFoundError(
            f"playground-workspace: missing editor tooling surface {editor_surface_path_text}"
        )
    return load_json(editor_surface_path), published_paths


def _workspace_drill_commands(source_display: str, debug_payload: dict[str, object]) -> dict[str, str]:
    return {
        "inspect_editor_tooling": (
            f"{WORKFLOW_COMMAND_TEXT} inspect-editor-tooling {source_display}"
        ),
        "format_preview": f"{WORKFLOW_COMMAND_TEXT} format-objc3c {source_display}",
        "object_symbol_inventory": str(
            debug_payload.get("object_symbol_inventory_command", "")
        ),
    }


def _workspace_payload(
    invocation: PlaygroundInvocation,
    *,
    workspace_root: Path,
    artifact_root: Path,
    report_root: Path,
    summary_path: Path,
    dump_path: Path,
    playground_payload: dict[str, object],
    editor_surface_payload: dict[str, object],
    published_paths: dict[str, str],
) -> dict[str, object]:
    formatter_payload = editor_surface_payload.get("formatter", {})
    debug_payload = editor_surface_payload.get("debug", {})
    if not isinstance(formatter_payload, dict):
        formatter_payload = {}
    if not isinstance(debug_payload, dict):
        debug_payload = {}

    return {
        "contract_id": PLAYGROUND_WORKSPACE_CONTRACT_ID,
        "schema_version": 1,
        "workspace_id": invocation.workspace_id,
        "source_path": invocation.source_display,
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
        "repro_command": playground_payload.get("dump_commands", {}).get(
            "repro_runner",
            "",
        ),
        "editor_tooling": {
            **published_paths,
            "formatted_output_path": formatter_payload.get("formatted_output_path"),
            "format_preview_supported": formatter_payload.get("supported"),
            "debugger_model": debug_payload.get("debugger_model", ""),
            "declaration_breakpoint_anchor_count": debug_payload.get(
                "declaration_breakpoint_anchor_count",
                0,
            ),
            "statement_level_stepping": debug_payload.get("statement_level_stepping"),
        },
        "workspace_drill_commands": _workspace_drill_commands(
            invocation.source_display,
            debug_payload,
        ),
    }


def run_playground_workspace(rest: list[str], *, emit_payload: bool) -> int:
    try:
        invocation = resolve_playground_invocation(rest)
    except (ValueError, FileNotFoundError) as exc:
        print(str(exc), file=sys.stderr)
        return 2

    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc

    (
        workspace_root,
        artifact_root,
        report_root,
        workspace_manifest_path,
        summary_path,
        dump_path,
    ) = _playground_workspace_paths(invocation)
    artifact_root.mkdir(parents=True, exist_ok=True)
    report_root.mkdir(parents=True, exist_ok=True)

    env = playground_subprocess_env()
    result = run_frontend_playground(
        invocation.source_display,
        artifact_root.relative_to(ROOT).as_posix(),
        summary_path.relative_to(ROOT).as_posix(),
        invocation.passthrough,
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

    editor_result = run_editor_tooling(invocation.source_display, env=env)
    if editor_result.stdout:
        sys.stdout.write(editor_result.stdout)
    if editor_result.stderr:
        sys.stderr.write(editor_result.stderr)
    if editor_result.returncode != 0:
        return editor_result.returncode

    try:
        editor_surface_payload, published_paths = _load_editor_surface(editor_result.stdout)
    except (FileNotFoundError, RuntimeError) as exc:
        print(str(exc), file=sys.stderr)
        return 1

    workspace_payload = _workspace_payload(
        invocation,
        workspace_root=workspace_root,
        artifact_root=artifact_root,
        report_root=report_root,
        summary_path=summary_path,
        dump_path=dump_path,
        playground_payload=playground_payload,
        editor_surface_payload=editor_surface_payload,
        published_paths=published_paths,
    )
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
