#!/usr/bin/env python3
from __future__ import annotations

import os

from objc3c_tooling.subprocesses import python_script_command, run_completed
from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    BUILD_PACKAGE_VALIDATION_SCRIPT,
    BUILD_PACKAGE_VALIDATION_SUMMARY_PATH,
    INSTALL_MATRIX_INTEGRATION_CONTRACT_PATH,
    INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH,
    PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
    ROOT,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    TOOLCHAIN_RANGE_REPLAY_SCRIPT,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    load_json_object,
    platform_hardening_owner_payload,
    require_paths_exist,
    require_platform_hardening_blocker_metadata,
    require_platform_hardening_owner_policy,
    summary_passes,
    write_json,
)


def run(command: list[str]) -> None:
    result = run_completed(command, cwd=ROOT, capture_output=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def main() -> int:
    contract = load_json_object(INSTALL_MATRIX_INTEGRATION_CONTRACT_PATH)
    owner_policy = require_platform_hardening_owner_policy(contract, surface_name="platform install matrix integration contract")
    blocker_metadata = require_platform_hardening_blocker_metadata(
        contract,
        surface_name="platform install matrix integration contract",
        required_blockers=("install matrix validation lost rollback evidence",),
    )
    run(python_script_command(BUILD_PACKAGE_VALIDATION_SCRIPT))
    run(python_script_command(TOOLCHAIN_RANGE_REPLAY_SCRIPT))
    require_paths_exist((ROOT / raw_path for raw_path in contract["required_inputs"]), description="install-matrix input")

    matrix = load_json_object(SUPPORT_MATRIX_ARTIFACT_PATH)
    build_package = load_json_object(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH)
    toolchain_replay = load_json_object(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH)
    packaging_e2e = load_json_object(PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH)

    install_root = ROOT / packaging_e2e["install_root"].replace("/", os.sep)
    offline_install_root = ROOT / packaging_e2e["offline_install_root"].replace("/", os.sep)
    offline_native = offline_install_root / "objc3c" / "artifacts" / "bin" / "objc3c-native.exe"

    checks = {
        "matrix_platform_is_windows_x64": matrix["default_platform_id"] == "windows-x64",
        "build_package_validation_passes": summary_passes(build_package),
        "toolchain_range_replay_passes": summary_passes(toolchain_replay),
        "primary_install_root_rolled_back": not (install_root / "objc3c").exists(),
        "offline_install_root_kept_native_executable": offline_native.is_file(),
    }

    summary = {
        "contract_id": "objc3c.platform.hardening.install.matrix.integration.summary.v1",
        "status": "PASS" if all(checks.values()) else "FAIL",
        "owner_policy": owner_policy or platform_hardening_owner_payload(),
        "blocker_metadata": blocker_metadata,
        "support_matrix_artifact": repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "build_package_validation_summary": repo_rel(BUILD_PACKAGE_VALIDATION_SUMMARY_PATH),
        "toolchain_range_replay_summary": repo_rel(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH),
        "packaging_end_to_end_summary": repo_rel(PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH),
        "required_checks": contract["required_checks"],
        "checks": checks,
    }
    write_json(INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH)}")
    print("objc3c-platform-install-matrix-integration: PASS")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
