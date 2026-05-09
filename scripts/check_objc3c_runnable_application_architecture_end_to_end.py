#!/usr/bin/env python3
"""Validate application architecture surfaces from the staged runnable toolchain bundle."""

from __future__ import annotations

import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "package-integration-summary.json"






def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-application-architecture-e2e" / run_id
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
    app_surface = manifest.get("application_architecture_surface", {})
    command_surfaces = manifest.get("command_surfaces", {})
    public_actions = manifest.get("application_architecture_public_actions", [])
    package_bridge = manifest.get("package_bridge")
    failures: list[str] = []

    expect(isinstance(app_surface, dict), "package manifest missing application_architecture_surface", failures)
    expect(
        "materialize-canonical-application-workspace" in public_actions,
        "package manifest missing application architecture public action",
        failures,
    )
    expect(
        "validate-runnable-application-architecture" in public_actions,
        "package manifest missing runnable application architecture public action",
        failures,
    )
    expect(
        package_bridge == "objc3c",
        "package manifest missing objc3c package bridge",
        failures,
    )
    expect(
        command_surfaces.get("build_application_workspace") == "npm run objc3c -- materialize-canonical-application-workspace",
        "package manifest missing build_application_workspace command surface",
        failures,
    )
    expect(
        command_surfaces.get("application_architecture") == "npm run objc3c -- validate-application-architecture",
        "package manifest missing application_architecture command surface",
        failures,
    )
    expect(
        command_surfaces.get("application_architecture_e2e") == "npm run objc3c -- validate-runnable-application-architecture",
        "package manifest missing application_architecture_e2e command surface",
        failures,
    )

    packaged_materializer = package_root / str(app_surface.get("canonical_workspace_materializer", ""))
    packaged_template_checker = package_root / str(app_surface.get("template_harness_checker", ""))
    packaged_e2e_checker = package_root / str(app_surface.get("runnable_end_to_end_validation", ""))
    expect(packaged_materializer.is_file(), "packaged canonical workspace materializer missing", failures)
    expect(packaged_template_checker.is_file(), "packaged template harness checker missing", failures)
    expect(packaged_e2e_checker.is_file(), "packaged runnable application architecture validator missing", failures)

    packaged_materialize_result = run_capture(
        [sys.executable, str(packaged_materializer)],
        cwd=package_root,
    )
    expect(packaged_materialize_result.returncode == 0, "packaged canonical workspace materializer failed", failures)

    packaged_template_result = run_capture(
        [sys.executable, str(packaged_template_checker)],
        cwd=package_root,
    )
    expect(packaged_template_result.returncode == 0, "packaged template harness checker failed", failures)

    canonical_summary = load_json(
        package_root / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"
    )
    template_summary = load_json(
        package_root / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json"
    )
    expect(canonical_summary.get("status") == "PASS", "packaged canonical workspace summary did not report PASS", failures)
    expect(canonical_summary.get("example_count") == 3, "packaged canonical workspace example count drifted", failures)
    expect(template_summary.get("status") == "PASS", "packaged template harness summary did not report PASS", failures)

    payload = {
        "contract_id": "objc3c.application.architecture.testing.runnable.end.to.end.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "package_root": repo_rel(package_root),
        "manifest_path": repo_rel(manifest_path),
        "application_architecture_surface": app_surface,
        "application_architecture_public_actions": public_actions,
        "package_bridge": "objc3c",
        "packaged_package_bridge": package_bridge,
        "failures": failures,
        "packaged_reports": {
            "canonical_workspace_summary": repo_rel(
                package_root / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"
            ),
            "template_harness_summary": repo_rel(
                package_root / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json"
            ),
        },
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("runnable-application-architecture: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("runnable-application-architecture: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
