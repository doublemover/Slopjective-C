#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling" / "workspace_editor_debug_integration_contract.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "developer-tooling" / "workspace-integration-summary.json"


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {path}")
    return payload


def extract_line_value(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return ""


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    result = subprocess.run(
        [sys.executable, str(PUBLIC_RUNNER), "materialize-playground-workspace", contract["workspace_source"]],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)

    failures: list[str] = []
    expect(result.returncode == 0, "materialize-playground-workspace failed", failures)

    workspace_path_text = extract_line_value(result.stdout, "workspace_path:")
    expect(bool(workspace_path_text), "workspace action did not publish workspace_path", failures)
    workspace_path = ROOT / workspace_path_text if workspace_path_text else Path()
    expect(workspace_path.is_file(), f"missing workspace manifest: {workspace_path_text}", failures)

    workspace = load_json(workspace_path) if workspace_path.is_file() else {}
    expect(workspace.get("contract_id") == "objc3c.playground.workspace.v1", "unexpected workspace contract id", failures)
    expect(workspace.get("source_path") == contract["workspace_source"], "workspace source_path drifted", failures)

    public_actions = workspace.get("public_actions", [])
    for action in contract["required_public_actions"]:
        expect(action in public_actions, f"workspace missing public action: {action}", failures)

    editor_tooling = workspace.get("editor_tooling", {})
    expect(isinstance(editor_tooling, dict), "workspace editor_tooling payload missing", failures)
    for field in contract["required_editor_tooling_fields"]:
        expect(field in editor_tooling, f"workspace editor_tooling missing field: {field}", failures)

    workspace_drill_commands = workspace.get("workspace_drill_commands", {})
    expect(isinstance(workspace_drill_commands, dict), "workspace_drill_commands payload missing", failures)
    for field in contract["required_workspace_drill_commands"]:
        expect(field in workspace_drill_commands, f"workspace missing drill command: {field}", failures)
        expect(bool(workspace_drill_commands.get(field)), f"workspace drill command is empty: {field}", failures)

    editor_surface_path = ROOT / str(editor_tooling.get("editor_surface_path", "")) if editor_tooling.get("editor_surface_path") else Path()
    formatter_path = ROOT / str(editor_tooling.get("formatter_path", "")) if editor_tooling.get("formatter_path") else Path()
    debug_path = ROOT / str(editor_tooling.get("debug_path", "")) if editor_tooling.get("debug_path") else Path()
    expect(editor_surface_path.is_file(), "workspace editor surface path missing on disk", failures)
    expect(formatter_path.is_file(), "workspace formatter path missing on disk", failures)
    expect(debug_path.is_file(), "workspace debug path missing on disk", failures)

    editor_surface = load_json(editor_surface_path) if editor_surface_path.is_file() else {}
    expect(editor_surface.get("formatter", {}).get("supported") is True, "workspace editor surface formatter not supported", failures)
    expect(editor_surface.get("debug", {}).get("supported") is True, "workspace editor surface debug not supported", failures)
    expect(editor_surface.get("debug", {}).get("statement_level_stepping") is False, "workspace editor surface must keep statement stepping fail-closed", failures)
    expect(int(editor_tooling.get("declaration_breakpoint_anchor_count", 0)) >= 3, "workspace did not publish enough declaration breakpoint anchors", failures)
    expect(editor_tooling.get("debugger_model") == "declaration-breakpoint-and-object-symbol-inspection", "workspace debugger model drifted", failures)
    expect(editor_tooling.get("format_preview_supported") is True, "workspace formatter preview flag drifted", failures)

    summary = {
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "status": "PASS" if not failures else "FAIL",
        "workspace_path": workspace_path_text,
        "editor_surface_path": editor_tooling.get("editor_surface_path", ""),
        "failures": failures,
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {SUMMARY_OUT.relative_to(ROOT).as_posix()}")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
