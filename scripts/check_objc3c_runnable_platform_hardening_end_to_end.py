#!/usr/bin/env python3
"""Validate packaged platform-hardening publication and smoke behavior end to end."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "platform_hardening" / "packaged_smoke_integration_contract.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "platform-hardening" / "runnable-end-to-end-summary.json"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.platform.hardening.runnable.end-to-end.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def normalize_rel_path(raw_path: str) -> str:
    return raw_path.replace("\\", "/")


def run_step(command: Sequence[str], *, cwd: Path) -> int:
    return subprocess.run(
        list(command),
        cwd=cwd,
        check=False,
        text=True,
    ).returncode


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def package_path(package_root: Path, relative_path: str) -> Path:
    return package_root / normalize_rel_path(relative_path)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-platform-hardening-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"

    package_exit_code = run_step(
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
    if package_exit_code != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_json(manifest_path)
    expect(manifest.get("contract_id") == PACKAGE_CONTRACT_ID, "runnable toolchain package manifest published the wrong contract id")

    for field in contract["manifest_fields"]:
        value = manifest.get(field)
        expect(value not in (None, "", []), f"package manifest did not publish {field}")

    for field in (
        "platform_hardening_runbook",
        "platform_hardening_boundary_inventory",
        "platform_support_tier_policy",
        "platform_unsupported_host_policy",
        "platform_toolchain_archive_policy",
        "platform_support_matrix_schema",
        "platform_support_matrix_contract",
        "platform_build_package_validation_contract",
        "platform_toolchain_range_replay_contract",
        "platform_install_matrix_integration_contract",
        "platform_packaged_smoke_contract",
    ):
        candidate = package_path(package_root, str(manifest[field]))
        expect(candidate.is_file(), f"packaged runnable toolchain missing {field} at {manifest[field]}")

    scripts = manifest.get("platform_hardening_scripts", {})
    expect(isinstance(scripts, dict), "package manifest did not publish platform_hardening_scripts")
    for script_name, relative_path in scripts.items():
        expect(isinstance(relative_path, str) and relative_path, f"platform_hardening_scripts entry {script_name} is empty")
        expect(package_path(package_root, relative_path).is_file(), f"packaged runnable toolchain missing platform-hardening script {script_name} at {relative_path}")

    command_surfaces = manifest.get("command_surfaces", {})
    expect(isinstance(command_surfaces, dict), "package manifest did not publish command_surfaces")
    for command_name in contract["required_command_surfaces"]:
        expect(command_name in command_surfaces, f"package manifest missing platform-hardening command surface: {command_name}")

    public_actions = manifest.get("platform_hardening_public_actions", [])
    public_scripts = manifest.get("platform_hardening_public_scripts", [])
    for action in contract["public_actions"]:
        expect(action in public_actions, f"package manifest missing platform-hardening public action: {action}")
    for script in contract["public_scripts"]:
        expect(script in public_scripts, f"package manifest missing platform-hardening public script: {script}")

    packaged_runner = package_root / "scripts" / "objc3c_public_workflow_runner.py"

    matrix_exit_code = run_step(
        [sys.executable, str(packaged_runner), "build-platform-support-matrix"],
        cwd=package_root,
    )
    if matrix_exit_code != 0:
        raise RuntimeError("packaged build-platform-support-matrix failed")
    matrix_artifact_path_text = "tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json"
    matrix_summary_path_text = "tmp/reports/platform-hardening/platform-support-matrix-summary.json"
    support_matrix = load_json(package_root / normalize_rel_path(str(matrix_artifact_path_text)))
    publication_surface = support_matrix.get("publication_surface", {})
    for field_name in contract["required_publication_surface_fields"]:
        expect(field_name in publication_surface, f"packaged support matrix missing publication surface field {field_name}")
    expect(support_matrix.get("default_platform_id") == "windows-x64", "packaged support matrix default platform drifted")

    integration_exit_code = run_step(
        [sys.executable, str(packaged_runner), "validate-platform-hardening"],
        cwd=package_root,
    )
    if integration_exit_code != 0:
        raise RuntimeError("packaged validate-platform-hardening failed")
    integration_summary_path_text = "tmp/reports/platform-hardening/integration-summary.json"
    integration_summary = load_json(package_root / normalize_rel_path(str(integration_summary_path_text)))
    expect(integration_summary.get("ok") is True, "packaged platform-hardening integration summary did not report ok=true")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_platform_hardening_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "platform_support_matrix_path": repo_rel(package_root / normalize_rel_path(str(matrix_artifact_path_text))),
        "platform_support_matrix_summary_path": repo_rel(package_root / normalize_rel_path(str(matrix_summary_path_text))),
        "platform_hardening_integration_summary_path": repo_rel(package_root / normalize_rel_path(str(integration_summary_path_text))),
        "packaged_public_actions": public_actions,
        "packaged_public_scripts": public_scripts,
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_exit_code,
                "package_root": repo_rel(package_root),
                "manifest": repo_rel(manifest_path),
            },
            {
                "action": "build-platform-support-matrix",
                "exit_code": matrix_exit_code,
                "artifact_path": matrix_artifact_path_text,
                "summary_path": matrix_summary_path_text,
            },
            {
                "action": "validate-platform-hardening",
                "exit_code": integration_exit_code,
                "summary_path": integration_summary_path_text,
            },
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
