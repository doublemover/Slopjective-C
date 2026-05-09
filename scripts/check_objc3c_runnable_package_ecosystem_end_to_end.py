#!/usr/bin/env python3
"""Validate package ecosystem workflows from the staged runnable toolchain bundle."""

from __future__ import annotations

import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_capture
from package_ecosystem_contracts import package_ecosystem_owner_payload


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "runnable-package-ecosystem-summary.json"






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
    manifest_package_bridge = manifest.get("package_bridge")
    expect(isinstance(package_surface, dict), "package manifest missing package_ecosystem_surface", failures)
    for action in ("build-package-lock", "validate-package-authoring", "validate-package-mirror", "validate-runnable-package-ecosystem"):
        expect(action in public_actions, f"package manifest missing public action {action}", failures)
    expect(manifest_package_bridge == "objc3c", "package manifest missing objc3c package bridge", failures)

    packaged_runner = package_root / "scripts" / "objc3c_workflow" / "runner.py"
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
        mirror_summary.get("hosted_registry_support") == "unsupported-fail-closed-if-claimed",
        "packaged hosted registry support claim drifted",
        failures,
    )

    payload = {
        "contract_id": "objc3c.package_ecosystem.runnable.end_to_end.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "owner_policy": package_ecosystem_owner_payload(),
        "blocker_metadata": {
            "blocker_owner": "package-ecosystem-blockers",
            "blocking_conditions": [
                "packaged package authoring workflow failed",
                "packaged mirror workflow failed",
                "runnable package manifest missing package ecosystem owner surface",
                "packaged hosted registry claim did not fail closed",
            ],
        },
        "package_root": repo_rel(package_root),
        "manifest_path": repo_rel(manifest_path),
        "package_ecosystem_surface": package_surface,
        "package_ecosystem_public_actions": public_actions,
        "package_bridge": "objc3c",
        "packaged_package_bridge": manifest_package_bridge,
        "failures": failures,
        "packaged_reports": {
            "authoring_summary": repo_rel(package_root / "tmp" / "reports" / "package-ecosystem" / "package-authoring-workflow-summary.json"),
            "mirror_reproducibility_summary": repo_rel(package_root / "tmp" / "reports" / "package-ecosystem" / "registry-mirror-reproducibility-summary.json"),
        },
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
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
