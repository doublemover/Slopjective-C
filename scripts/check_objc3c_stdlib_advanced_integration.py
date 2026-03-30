#!/usr/bin/env python3
"""Run the integrated advanced stdlib helper validation path."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
SMOKE_RUNNER = ROOT / "scripts" / "run_objc3c_stdlib_workspace_smoke.py"
ADVANCED_HELPER_PACKAGE_SURFACE_PATH = ROOT / "stdlib" / "advanced_helper_package_surface.json"
SURFACE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "surface-summary.json"
SMOKE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "workspace-smoke-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "advanced-integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.advanced.integration.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: list[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    surface_result = run_capture([sys.executable, str(RUNNER), "check-stdlib-surface"])
    if surface_result.returncode != 0:
        raise RuntimeError("stdlib surface validation failed")

    smoke_result = run_capture([sys.executable, str(SMOKE_RUNNER)])
    if smoke_result.returncode != 0:
        raise RuntimeError("stdlib workspace smoke validation failed")

    advanced_surface = load_json(ADVANCED_HELPER_PACKAGE_SURFACE_PATH)
    surface_summary = load_json(SURFACE_SUMMARY_PATH)
    smoke_summary = load_json(SMOKE_SUMMARY_PATH)

    advanced_surface_rel = repo_rel(ADVANCED_HELPER_PACKAGE_SURFACE_PATH)
    expect(
        surface_summary.get("advanced_helper_package_surface") == advanced_surface_rel,
        "stdlib surface summary drifted from the advanced helper package surface contract",
    )

    advanced_modules = advanced_surface.get("advanced_helper_modules")
    expect(
        isinstance(advanced_modules, list) and advanced_modules,
        "advanced helper package surface missing advanced_helper_modules",
    )
    expected_modules = [entry["canonical_module"] for entry in advanced_modules if isinstance(entry, dict)]
    expect(
        len(expected_modules) == len(advanced_modules),
        "advanced helper package surface published a malformed module entry",
    )

    surfaced_modules = surface_summary.get("advanced_helper_modules")
    expect(
        surfaced_modules == advanced_modules,
        "stdlib surface summary drifted from the advanced helper module inventory",
    )

    compile_results = smoke_summary.get("compile_results")
    expect(isinstance(compile_results, list) and compile_results, "stdlib smoke summary missing compile_results")
    compiled_by_module = {
        str(entry.get("canonical_module")): entry for entry in compile_results if isinstance(entry, dict)
    }
    advanced_compile_results = []
    for canonical_module in expected_modules:
        expect(
            canonical_module in compiled_by_module,
            f"stdlib smoke summary did not compile the advanced helper module {canonical_module}",
        )
        advanced_compile_results.append(compiled_by_module[canonical_module])

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(
        json.dumps(
            {
                "contract_id": SUMMARY_CONTRACT_ID,
                "schema_version": 1,
                "generated_at_utc": datetime.now(timezone.utc).isoformat(),
                "status": "PASS",
                "child_report_paths": [repo_rel(SURFACE_SUMMARY_PATH), repo_rel(SMOKE_SUMMARY_PATH)],
                "workflow_actions": advanced_surface["public_actions"],
                "advanced_helper_package_surface": advanced_surface_rel,
                "advanced_helper_modules": advanced_modules,
                "advanced_helper_command_surfaces": advanced_surface["advanced_helper_command_surfaces"],
                "advanced_helper_profile_gates": advanced_surface["advanced_helper_profile_gates"],
                "advanced_compile_results": advanced_compile_results,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
