#!/usr/bin/env python3
"""Validate packaged developer-tooling behavior end to end from the staged package root."""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling" / "packaged_cli_to_editor_contract.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-developer-tooling-e2e" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.developer.tooling.e2e.summary.v1"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def normalize_rel_path(raw_path: str) -> str:
    return raw_path.replace("\\", "/")


def run_capture(command: Sequence[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=cwd,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def extract_output_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return None


def extract_last_output_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    value: str | None = None
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            value = line.split(":", 1)[1].strip()
    return value


def extract_report_paths(stdout: str) -> list[str]:
    report_paths: list[str] = []
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith("summary_path:"):
            report_paths.append(line.split(":", 1)[1].strip().replace("\\", "/"))
            continue
        match = re.search(r"runtime-acceptance:\s+PASS\s+\((.+)\)", line)
        if match:
            candidate = Path(match.group(1).strip())
            try:
                report_paths.append(repo_rel(candidate))
            except ValueError:
                report_paths.append(match.group(1).strip().replace("\\", "/"))
            continue
        if line.startswith("public-workflow-report:"):
            report_paths.append(line.split(":", 1)[1].strip().replace("\\", "/"))
    return report_paths


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def package_path(package_root: Path, relative_path: str) -> Path:
    return package_root / normalize_rel_path(relative_path)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-developer-tooling-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"

    package_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(package_root),
        ],
        cwd=ROOT,
    )
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_json(manifest_path)
    expect(
        manifest.get("contract_id") == PACKAGE_CONTRACT_ID,
        "runnable toolchain package manifest published the wrong contract id",
    )

    for field in contract["manifest_fields"]:
        value = manifest.get(field)
        expect(value not in (None, "", []), f"package manifest did not publish {field}")

    for field in (
        "developer_tooling_runbook",
        "developer_tooling_boundary_inventory",
        "developer_tooling_editor_surface_schema",
        "developer_tooling_navigation_contract",
        "developer_tooling_formatter_debug_contract",
        "developer_tooling_workspace_contract",
        "developer_tooling_packaged_contract",
        "developer_tooling_example_source",
        "developer_tooling_negative_source",
        "developer_tooling_formatter_source",
        "developer_tooling_expected_formatted_source",
    ):
        candidate = package_path(package_root, str(manifest[field]))
        expect(candidate.is_file(), f"packaged runnable toolchain missing {field} at {manifest[field]}")

    scripts = manifest.get("developer_tooling_scripts", {})
    expect(isinstance(scripts, dict), "package manifest did not publish developer_tooling_scripts")
    for script_name, relative_path in scripts.items():
        expect(isinstance(relative_path, str) and relative_path, f"package manifest script entry {script_name} is empty")
        expect(package_path(package_root, relative_path).is_file(), f"packaged runnable toolchain missing developer tooling script {script_name} at {relative_path}")

    command_surfaces = manifest.get("command_surfaces", {})
    expect(isinstance(command_surfaces, dict), "package manifest did not publish command_surfaces")
    for command_name in contract["required_command_surfaces"]:
        expect(command_name in command_surfaces, f"package manifest missing developer tooling command surface: {command_name}")

    public_actions = manifest.get("developer_tooling_public_actions", [])
    public_scripts = manifest.get("developer_tooling_public_scripts", [])
    for action in contract["public_actions"]:
        expect(action in public_actions, f"package manifest missing developer tooling public action: {action}")
    for script in contract["public_scripts"]:
        expect(script in public_scripts, f"package manifest missing developer tooling public script: {script}")

    packaged_runner = package_root / "scripts" / "objc3c_public_workflow_runner.py"
    hello_source = package_path(package_root, str(manifest["developer_tooling_example_source"]))
    format_source = package_path(package_root, str(manifest["developer_tooling_formatter_source"]))
    expected_formatted_source = package_path(package_root, str(manifest["developer_tooling_expected_formatted_source"]))

    inspect_result = run_capture(
        [sys.executable, str(packaged_runner), "inspect-editor-tooling", str(hello_source)],
        cwd=package_root,
    )
    if inspect_result.returncode != 0:
        raise RuntimeError("packaged inspect-editor-tooling failed")
    dump_path_text = extract_output_value(inspect_result.stdout, "dump_path")
    expect(bool(dump_path_text), "packaged inspect-editor-tooling did not publish dump_path")
    editor_surface = load_json(package_root / normalize_rel_path(str(dump_path_text)))
    debug_payload = editor_surface.get("debug", {})
    navigation_payload = editor_surface.get("navigation", {})
    formatter_payload = editor_surface.get("formatter", {})
    expect(formatter_payload.get("supported") is True, "packaged editor surface did not report formatter support")
    expect(debug_payload.get("supported") is True, "packaged editor surface did not report debug support")
    expect(
        debug_payload.get("debugger_model") == contract["expected_debugger_model"],
        "packaged editor surface debugger model drifted",
    )
    expect(
        debug_payload.get("statement_level_stepping") is (not contract["expected_fail_closed_statement_stepping"]),
        "packaged editor surface stepping availability drifted",
    )
    expect(navigation_payload.get("available") is True, "packaged editor surface did not publish navigation availability")
    expect(int(debug_payload.get("declaration_breakpoint_anchor_count", 0)) >= 3, "packaged editor surface did not publish enough breakpoint anchors")

    format_result = run_capture(
        [sys.executable, str(packaged_runner), "format-objc3c", str(format_source)],
        cwd=package_root,
    )
    if format_result.returncode != 0:
        raise RuntimeError("packaged format-objc3c failed")
    format_summary_path_text = extract_output_value(format_result.stdout, "summary_path")
    expect(bool(format_summary_path_text), "packaged format-objc3c did not publish summary_path")
    format_summary = load_json(package_root / normalize_rel_path(str(format_summary_path_text)))
    expected_formatted_text = expected_formatted_source.read_text(encoding="utf-8")
    packaged_formatted_output = package_root / normalize_rel_path(str(format_summary.get("formatted_output_path", "")))
    expect(packaged_formatted_output.is_file(), "packaged formatter did not publish formatted_output_path")
    expect(
        packaged_formatted_output.read_text(encoding="utf-8") == expected_formatted_text,
        "packaged formatter output drifted from the checked-in canonical formatted fixture",
    )

    workspace_result = run_capture(
        [sys.executable, str(packaged_runner), "materialize-playground-workspace", str(hello_source)],
        cwd=package_root,
    )
    if workspace_result.returncode != 0:
        raise RuntimeError("packaged materialize-playground-workspace failed")
    workspace_path_text = extract_output_value(workspace_result.stdout, "workspace_path")
    expect(bool(workspace_path_text), "packaged materialize-playground-workspace did not publish workspace_path")
    workspace = load_json(package_root / normalize_rel_path(str(workspace_path_text)))
    workspace_editor_tooling = workspace.get("editor_tooling", {})
    expect(isinstance(workspace_editor_tooling, dict), "packaged workspace did not publish editor_tooling")
    expect(
        workspace_editor_tooling.get("debugger_model") == contract["expected_debugger_model"],
        "packaged workspace debugger model drifted",
    )
    expect(
        workspace_editor_tooling.get("statement_level_stepping") is (not contract["expected_fail_closed_statement_stepping"]),
        "packaged workspace stepping availability drifted",
    )
    for action in ("inspect-editor-tooling", "format-objc3c", "validate-developer-tooling"):
        expect(action in workspace.get("public_actions", []), f"packaged workspace missing public action {action}")

    integrated_result = run_capture(
        [sys.executable, str(packaged_runner), "validate-developer-tooling"],
        cwd=package_root,
    )
    if integrated_result.returncode != 0:
        raise RuntimeError("packaged validate-developer-tooling failed")
    integrated_summary_path_text = extract_last_output_value(integrated_result.stdout, "summary_path")
    expect(bool(integrated_summary_path_text), "packaged validate-developer-tooling did not publish summary_path")
    integrated_summary = load_json(package_root / normalize_rel_path(str(integrated_summary_path_text)))
    expect(integrated_summary.get("ok") is True, "packaged developer-tooling integration summary did not report ok=true")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_editor_surface_path": repo_rel(package_root / normalize_rel_path(str(dump_path_text))),
        "packaged_formatter_summary_path": repo_rel(package_root / normalize_rel_path(str(format_summary_path_text))),
        "packaged_workspace_path": repo_rel(package_root / normalize_rel_path(str(workspace_path_text))),
        "packaged_integration_summary_path": repo_rel(package_root / normalize_rel_path(str(integrated_summary_path_text))),
        "packaged_public_actions": public_actions,
        "packaged_public_scripts": public_scripts,
        "child_report_paths": [
            *extract_report_paths(package_result.stdout),
            *extract_report_paths(inspect_result.stdout),
            *extract_report_paths(format_result.stdout),
            *extract_report_paths(workspace_result.stdout),
            *extract_report_paths(integrated_result.stdout),
        ],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "inspect-editor-tooling",
                "exit_code": inspect_result.returncode,
                "dump_path": dump_path_text,
            },
            {
                "action": "format-objc3c",
                "exit_code": format_result.returncode,
                "summary_path": format_summary_path_text,
            },
            {
                "action": "materialize-playground-workspace",
                "exit_code": workspace_result.returncode,
                "workspace_path": workspace_path_text,
            },
            {
                "action": "validate-developer-tooling",
                "exit_code": integrated_result.returncode,
                "summary_path": integrated_summary_path_text,
            },
        ],
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
