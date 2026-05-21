#!/usr/bin/env python3
"""Validate packaged platform-hardening publication and smoke behavior end to end."""

from __future__ import annotations

import shutil
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence
from objc3c_tooling.paths import normalize_rel_path, repo_rel
from objc3c_tooling.json_io import require_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_completed
from platform_hardening_contracts import (
    PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH,
    ROOT,
    RUNNABLE_END_TO_END_SUMMARY_PATH,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    SUPPORT_MATRIX_SUMMARY_PATH,
    PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH,
    platform_hardening_owner_payload,
    require_platform_hardening_blocker_metadata,
    require_platform_hardening_owner_policy,
)

PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.platform.hardening.runnable.end-to-end.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)




def run_step(command: Sequence[str], *, cwd: Path) -> int:
    return run_completed(command, cwd=cwd, capture_output=False).returncode

def package_path(package_root: Path, relative_path: str) -> Path:
    return package_root / normalize_rel_path(relative_path)


def main() -> int:
    contract = load_json(PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH)
    owner_policy = require_platform_hardening_owner_policy(contract, surface_name="packaged platform hardening smoke contract")
    blocker_metadata = require_platform_hardening_blocker_metadata(
        contract,
        surface_name="packaged platform hardening smoke contract",
        required_blockers=("packaged platform hardening command surface missing",),
    )
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
        "platform_toolchain_support_evidence_schema",
        "platform_toolchain_support_evidence",
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
    package_bridge = str(contract["package_bridge"])
    manifest_package_bridge = manifest.get("package_bridge")
    for action in contract["public_actions"]:
        expect(action in public_actions, f"package manifest missing platform-hardening public action: {action}")
    expect(manifest_package_bridge == package_bridge, f"package manifest missing package bridge {package_bridge}")

    matrix_exit_code = run_step(
        public_workflow_command("build-platform-support-matrix"),
        cwd=package_root,
    )
    if matrix_exit_code != 0:
        raise RuntimeError("packaged build-platform-support-matrix failed")
    matrix_artifact_path_text = repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH)
    matrix_summary_path_text = repo_rel(SUPPORT_MATRIX_SUMMARY_PATH)
    support_matrix = load_json(package_root / normalize_rel_path(str(matrix_artifact_path_text)))
    publication_surface = support_matrix.get("publication_surface", {})
    for field_name in contract["required_publication_surface_fields"]:
        expect(field_name in publication_surface, f"packaged support matrix missing publication surface field {field_name}")
    expect(support_matrix.get("default_platform_id") == "windows-x64", "packaged support matrix default platform drifted")

    integration_exit_code = run_step(
        public_workflow_command("validate-platform-hardening"),
        cwd=package_root,
    )
    if integration_exit_code != 0:
        raise RuntimeError("packaged validate-platform-hardening failed")
    integration_summary_path_text = repo_rel(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH)
    integration_summary = load_json(package_root / normalize_rel_path(str(integration_summary_path_text)))
    expect(integration_summary.get("ok") is True, "packaged platform-hardening integration summary did not report ok=true")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "owner_policy": owner_policy or platform_hardening_owner_payload(),
        "blocker_metadata": blocker_metadata,
        "runner_path": "scripts/check_objc3c_runnable_platform_hardening_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "platform_support_matrix_path": repo_rel(package_root / normalize_rel_path(str(matrix_artifact_path_text))),
        "platform_support_matrix_summary_path": repo_rel(package_root / normalize_rel_path(str(matrix_summary_path_text))),
        "platform_hardening_integration_summary_path": repo_rel(package_root / normalize_rel_path(str(integration_summary_path_text))),
        "packaged_public_actions": public_actions,
        "package_bridge": package_bridge,
        "packaged_package_bridge": manifest_package_bridge,
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
    RUNNABLE_END_TO_END_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(RUNNABLE_END_TO_END_SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(RUNNABLE_END_TO_END_SUMMARY_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
