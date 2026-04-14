#!/usr/bin/env python3
"""Validate package ecosystem workflows from the staged runnable toolchain bundle."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "runnable-package-ecosystem-summary.json"



def run_capture(command: Sequence[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=cwd,
        text=True,
        capture_output=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result



def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-package-ecosystem-e2e" / run_id
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

    failures: list[str] = []
    manifest = load_json(manifest_path)
    package_surface = manifest.get("package_ecosystem_surface", {})
    public_actions = manifest.get("package_ecosystem_public_actions", [])
    public_scripts = manifest.get("package_ecosystem_public_scripts", [])
    expect(isinstance(package_surface, dict), "package manifest missing package_ecosystem_surface", failures)
    for action in ("build-package-lock", "validate-package-authoring", "validate-package-mirror", "validate-runnable-package-ecosystem"):
        expect(action in public_actions, f"package manifest missing public action {action}", failures)
    for script in ("build:objc3c:package-lock", "test:objc3c:package-authoring", "test:objc3c:package-mirror", "test:objc3c:package-ecosystem:e2e"):
        expect(script in public_scripts, f"package manifest missing public script {script}", failures)

    packaged_runner = package_root / "scripts" / "objc3c_public_workflow_runner.py"
    packaged_authoring = run_capture(
        [sys.executable, str(packaged_runner), "validate-package-authoring"],
        cwd=package_root,
    )
    expect(packaged_authoring.returncode == 0, "packaged package authoring workflow failed", failures)
    packaged_mirror = run_capture(
        [sys.executable, str(packaged_runner), "validate-package-mirror"],
        cwd=package_root,
    )
    expect(packaged_mirror.returncode == 0, "packaged package mirror workflow failed", failures)

    authoring_summary = load_json(package_root / "tmp" / "reports" / "package-ecosystem" / "package-authoring-workflow-summary.json")
    mirror_summary = load_json(package_root / "tmp" / "reports" / "package-ecosystem" / "registry-mirror-reproducibility-summary.json")
    expect(authoring_summary.get("status") == "PASS", "packaged package authoring summary did not report PASS", failures)
    expect(mirror_summary.get("status") == "PASS", "packaged mirror reproducibility summary did not report PASS", failures)
    expect(mirror_summary.get("network_policy") == "no-network-during-validation", "packaged mirror network policy drifted", failures)
    expect(
        mirror_summary.get("hosted_registry_support") == "deferred-release-blocking-if-claimed",
        "packaged hosted registry support claim drifted",
        failures,
    )

    payload = {
        "contract_id": "objc3c.package_ecosystem.runnable.end_to_end.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "package_root": repo_rel(package_root),
        "manifest_path": repo_rel(manifest_path),
        "package_ecosystem_surface": package_surface,
        "package_ecosystem_public_actions": public_actions,
        "package_ecosystem_public_scripts": public_scripts,
        "failures": failures,
        "packaged_reports": {
            "authoring_summary": repo_rel(package_root / "tmp" / "reports" / "package-ecosystem" / "package-authoring-workflow-summary.json"),
            "mirror_reproducibility_summary": repo_rel(package_root / "tmp" / "reports" / "package-ecosystem" / "registry-mirror-reproducibility-summary.json"),
        },
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("runnable-package-ecosystem: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("runnable-package-ecosystem: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
