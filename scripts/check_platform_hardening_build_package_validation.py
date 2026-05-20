#!/usr/bin/env python3
from __future__ import annotations

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
    command_map = {
        "build-native-binaries": public_workflow_command("build-native-binaries"),
        "package-runnable-toolchain": public_workflow_command("package-runnable-toolchain"),
        "build-package-channels": python_script_command(ROOT / "scripts" / "build_objc3c_package_channels.py"),
        "validate-packaging-channels-end-to-end": python_script_command(ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"),
    }
    for step_name in contract["required_steps"]:
        command = command_map[step_name]
        result = run_capture(command)
        steps.append({
            "step": step_name,
            "command": command,
            "exit_code": result.returncode,
            "ok": result.returncode == 0,
        })
        if result.returncode != 0:
            raise RuntimeError(f"platform build/package step failed: {step_name}")

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
