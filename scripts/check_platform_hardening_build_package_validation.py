#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import hashlib
from typing import Any

from objc3c_tooling.paths import repo_rel
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import python_script_command, run_capture
from platform_hardening_contracts import (
    BUILD_PACKAGE_VALIDATION_CONTRACT_PATH,
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT,
    ROOT,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    host_matches_supported_platform,
    load_json_object,
    platform_hardening_owner_payload,
    require_platform_hardening_blocker_metadata,
    require_platform_hardening_owner_policy,
    require_required_fields,
    write_json,
)

PLATFORM_PACKAGE_VALIDATION_BUILD_ENV = {
    "CMAKE_BUILD_PARALLEL_LEVEL": "2",
    "CL_MPCount": "2",
    "LLVM_PARALLEL_COMPILE_JOBS": "2",
    "OBJC3C_NATIVE_BUILD_PARALLELISM": "2",
}

VALIDATION_PACKAGE_ROOT = (
    ROOT
    / "tmp"
    / "pkg"
    / "objc3c-native-runnable-toolchain"
    / "platform-hardening-build-package-validation"
)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def package_manifest_path(package_root: Path) -> Path:
    return package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"


def main() -> int:
    run_capture(python_script_command(BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT))
    contract = load_json_object(BUILD_PACKAGE_VALIDATION_CONTRACT_PATH)
    owner_policy = require_platform_hardening_owner_policy(contract, surface_name="platform build package validation contract")
    blocker_metadata = require_platform_hardening_blocker_metadata(
        contract,
        surface_name="platform build package validation contract",
        required_blockers=("required package validation step failed",),
    )
    matrix = load_json_object(SUPPORT_MATRIX_ARTIFACT_PATH)

    require_required_fields(matrix, contract["required_matrix_fields"], "platform support matrix")
    if not host_matches_supported_platform(str(matrix["default_platform_id"])):
        raise RuntimeError("current host fell outside the checked-in supported platform matrix")
    acceptance_rows = matrix["packaged_runtime_acceptance"]
    if not isinstance(acceptance_rows, list) or not acceptance_rows:
        raise RuntimeError("platform support matrix omitted packaged runtime acceptance rows")
    for acceptance in acceptance_rows:
        if acceptance.get("platform_id") == matrix["default_platform_id"]:
            for action in acceptance["required_public_actions"]:
                public_workflow_command(action)
            break
    else:
        raise RuntimeError("platform support matrix omitted default-platform packaged runtime acceptance")
    for tool_name, probe in matrix["required_tool_probes"].items():
        if not probe.get("available"):
            raise RuntimeError(f"required tool probe failed for {tool_name}")

    steps: list[dict[str, Any]] = []
    reusable_package_root: Path | None = None
    reusable_package_manifest: Path | None = None
    for step_name in contract["required_steps"]:
        if step_name == "build-native-binaries":
            command = public_workflow_command("build-native-binaries")
        elif step_name == "package-runnable-toolchain":
            command = public_workflow_command(
                "package-runnable-toolchain",
                "--",
                "-PackageRoot",
                repo_rel(VALIDATION_PACKAGE_ROOT),
                "-Parallelism",
                "2",
            )
        elif step_name == "build-package-channels":
            if reusable_package_root is None:
                raise RuntimeError(
                    "build-package-channels requires the package-runnable-toolchain "
                    "step to report a reusable package root"
                )
            command = public_workflow_command(
                "build-package-channels",
                "--",
                "--reuse-runnable-package-root",
                repo_rel(reusable_package_root),
            )
        elif step_name == "validate-packaging-channels-end-to-end":
            command = [
                *python_script_command(ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"),
                "--use-existing-build-report",
            ]
        else:
            raise RuntimeError(f"unknown platform build/package validation step: {step_name}")
        result = run_capture(
            command,
            env_overlay=PLATFORM_PACKAGE_VALIDATION_BUILD_ENV,
        )
        if step_name == "package-runnable-toolchain" and result.returncode == 0:
            reusable_package_root = VALIDATION_PACKAGE_ROOT
            reusable_package_manifest = package_manifest_path(reusable_package_root)
        steps.append({
            "step": step_name,
            "command": command,
            "exit_code": result.returncode,
            "ok": result.returncode == 0,
        })
        if result.returncode != 0:
            raise RuntimeError(f"platform build/package step failed: {step_name}")

    if reusable_package_root is None or reusable_package_manifest is None:
        raise RuntimeError("platform build/package validation missed reusable package output")
    if not reusable_package_manifest.is_file():
        raise RuntimeError(
            "platform build/package validation reusable package manifest missing: "
            f"{repo_rel(reusable_package_manifest)}"
        )

    for raw_path in contract["required_reports"]:
        report_path = ROOT / raw_path
        if not report_path.is_file():
            raise RuntimeError(f"required package report missing: {raw_path}")
    package_summary = load_json_object(ROOT / contract["required_reports"][0])
    default_acceptance = next(
        acceptance
        for acceptance in acceptance_rows
        if acceptance.get("platform_id") == matrix["default_platform_id"]
    )
    for artifact_key in default_acceptance["required_artifacts"]:
        if artifact_key not in package_summary:
            raise RuntimeError(f"package summary missing packaged runtime artifact {artifact_key}")

    summary = {
        "contract_id": "objc3c.platform.hardening.build.package.validation.summary.v1",
        "status": "PASS",
        "owner_policy": owner_policy or platform_hardening_owner_payload(),
        "blocker_metadata": blocker_metadata,
        "support_matrix_artifact": repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "default_platform_id": matrix["default_platform_id"],
        "current_host": matrix["current_host"],
        "reusable_package_root": repo_rel(reusable_package_root),
        "reusable_package_manifest": repo_rel(reusable_package_manifest),
        "reusable_package_manifest_sha256": sha256_file(reusable_package_manifest),
        "steps": steps,
        "required_reports": contract["required_reports"],
        "packaged_runtime_acceptance": acceptance_rows,
    }
    write_json(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH)}")
    print("objc3c-platform-build-package-validation: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
