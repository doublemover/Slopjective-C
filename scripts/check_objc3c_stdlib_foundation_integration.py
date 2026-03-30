#!/usr/bin/env python3
"""Run the integrated stdlib foundation validation path."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.foundation.integration.summary.v1"
WORKSPACE_CONTRACT_PATH = ROOT / "stdlib" / "workspace.json"
LOWERING_IMPORT_SURFACE_PATH = ROOT / "stdlib" / "lowering_import_surface.json"
SURFACE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "surface-summary.json"
SMOKE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "workspace-smoke-summary.json"


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

    smoke_result = run_capture([sys.executable, str(ROOT / "scripts" / "run_objc3c_stdlib_workspace_smoke.py")])
    if smoke_result.returncode != 0:
        raise RuntimeError("stdlib workspace smoke validation failed")

    surface_summary = load_json(SURFACE_SUMMARY_PATH)
    smoke_summary = load_json(SMOKE_SUMMARY_PATH)
    workspace_contract = load_json(WORKSPACE_CONTRACT_PATH)
    lowering_import_surface = load_json(LOWERING_IMPORT_SURFACE_PATH)
    lowering_import_surface_rel = repo_rel(LOWERING_IMPORT_SURFACE_PATH)

    expect(
        workspace_contract.get("lowering_import_surface") == lowering_import_surface_rel,
        "workspace contract lowering/import surface drifted from the integrated stdlib boundary",
    )
    expect(
        surface_summary.get("lowering_import_surface") == lowering_import_surface_rel,
        "stdlib surface summary drifted from the lowering/import contract",
    )
    expect(
        smoke_summary.get("lowering_import_surface") == lowering_import_surface_rel,
        "stdlib smoke summary drifted from the lowering/import contract",
    )

    canonical_modules = surface_summary.get("canonical_modules")
    compile_results = smoke_summary.get("compile_results")
    expect(isinstance(canonical_modules, list) and canonical_modules, "stdlib surface summary missing canonical_modules")
    expect(isinstance(compile_results, list) and compile_results, "stdlib smoke summary missing compile_results")
    expect(
        len(compile_results) == len(canonical_modules),
        "stdlib smoke summary compiled module count drifted from the checked-in module inventory",
    )

    artifact_filenames = lowering_import_surface.get("artifact_filenames")
    expect(isinstance(artifact_filenames, dict), "lowering/import surface missing artifact_filenames")
    for compile_result in compile_results:
        expect(isinstance(compile_result, dict), "stdlib smoke summary published a malformed compile result")
        for key in ("artifact_root", "object", "manifest", "registration_manifest"):
            expect(key in compile_result, f"stdlib smoke summary missing {key}")
            artifact_path = ROOT / str(compile_result[key])
            expect(artifact_path.is_file() or artifact_path.is_dir(), f"stdlib smoke artifact missing at {artifact_path}")

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(
        json.dumps(
            {
                "contract_id": SUMMARY_CONTRACT_ID,
                "schema_version": 1,
                "generated_at_utc": datetime.now(timezone.utc).isoformat(),
                "status": "PASS",
                "child_report_paths": [repo_rel(SURFACE_SUMMARY_PATH), repo_rel(SMOKE_SUMMARY_PATH)],
                "workflow_actions": [
                    "check-stdlib-surface",
                    "materialize-stdlib-workspace",
                    "validate-stdlib-foundation",
                ],
                "lowering_import_surface": lowering_import_surface_rel,
                "artifact_filenames": artifact_filenames,
                "compiled_module_count": len(compile_results),
                "surface_summary": surface_summary,
                "workspace_smoke_summary": smoke_summary,
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
