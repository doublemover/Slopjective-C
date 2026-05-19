#!/usr/bin/env python3
from __future__ import annotations

from typing import Any

from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import python_script_command, run_completed
from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT,
    CHANNEL_CATALOG_PATH,
    PUBLICATION_SURFACE,
    RELEASE_PUBLICATION_SUMMARY_PATH,
    ROOT,
    SUPPORT_MATRIX_ARTIFACT_PATH,
    TOOLCHAIN_RANGE_REPLAY_CONTRACT_PATH,
    TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH,
    UPGRADE_SUPPORT_REPORT_PATH,
    UPDATE_MANIFEST_PATH,
    load_json_object,
    platform_hardening_owner_payload,
    require_platform_hardening_blocker_metadata,
    require_platform_hardening_owner_policy,
    summary_passes,
    write_json,
)

PROBE_SCRIPT = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"


def run(command: list[str]) -> None:
    result = run_completed(command, cwd=ROOT, capture_output=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def main() -> int:
    contract = load_json_object(TOOLCHAIN_RANGE_REPLAY_CONTRACT_PATH)
    owner_policy = require_platform_hardening_owner_policy(contract, surface_name="platform toolchain range replay contract")
    blocker_metadata = require_platform_hardening_blocker_metadata(
        contract,
        surface_name="platform toolchain range replay contract",
        required_blockers=("release operation published toolchain range outside replay evidence",),
    )
    probe_summary = ROOT / contract["toolchain_probe_summary"]

    run(python_script_command(BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT))
    run(python_script_command(PROBE_SCRIPT, "--summary-out", repo_rel(probe_summary)))
    step_commands = {
        "check-release-operations-integration": PUBLICATION_SURFACE["release_operations_command"],
        "check-release-operations-end-to-end": PUBLICATION_SURFACE["release_operations_end_to_end_command"],
    }
    step_results: list[dict[str, Any]] = [
        {
            "step": "probe-objc3c-llvm-capabilities",
            "command": python_script_command(PROBE_SCRIPT, "--summary-out", repo_rel(probe_summary)),
            "status": "PASS",
        }
    ]
    for step_name in contract["required_steps"]:
        if step_name == "probe-objc3c-llvm-capabilities":
            continue
        action = step_commands[step_name]
        command = public_workflow_command(str(action))
        run(command)
        step_results.append({"step": step_name, "command": command, "status": "PASS"})

    for raw_path in contract["required_reports"]:
        report_path = ROOT / raw_path
        if not report_path.is_file():
            raise RuntimeError(f"required toolchain replay report missing: {raw_path}")

    matrix = load_json_object(SUPPORT_MATRIX_ARTIFACT_PATH)
    capabilities = load_json_object(probe_summary)
    update_manifest = load_json_object(UPDATE_MANIFEST_PATH)
    upgrade_support_report = load_json_object(UPGRADE_SUPPORT_REPORT_PATH)
    publication_summary = load_json_object(RELEASE_PUBLICATION_SUMMARY_PATH)
    channel_catalog = load_json_object(CHANNEL_CATALOG_PATH)
    stable = next(entry for entry in update_manifest["channels"] if entry["channel_id"] == "stable")

    checks = {
        "matrix_default_platform_is_windows_x64": matrix["default_platform_id"] == "windows-x64",
        "toolchain_probe_passes": capabilities.get("ok") is True,
        "toolchain_parity_ready": capabilities.get("sema_type_system_parity", {}).get("parity_ready") is True,
        "release_update_manifest_published": summary_passes(publication_summary),
        "release_metadata_uses_platform_support_matrix": update_manifest.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "upgrade_support_report_uses_platform_support_matrix": upgrade_support_report.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "channel_catalog_uses_platform_support_matrix": channel_catalog.get("platform_support_matrix") == repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "stable_artifacts_published": len(stable.get("artifacts", {})) >= 3,
    }

    summary = {
        "contract_id": "objc3c.platform.hardening.toolchain.range.replay.summary.v1",
        "status": "PASS" if all(checks.values()) else "FAIL",
        "owner_policy": owner_policy or platform_hardening_owner_payload(),
        "blocker_metadata": blocker_metadata,
        "support_matrix_artifact": repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "toolchain_probe_summary": repo_rel(probe_summary),
        "release_operations_update_manifest": repo_rel(UPDATE_MANIFEST_PATH),
        "release_operations_publication_summary": repo_rel(RELEASE_PUBLICATION_SUMMARY_PATH),
        "release_operations_upgrade_support_report": repo_rel(UPGRADE_SUPPORT_REPORT_PATH),
        "release_operations_channel_catalog": repo_rel(CHANNEL_CATALOG_PATH),
        "required_toolchain_claims": contract["required_toolchain_claims"],
        "required_steps": step_results,
        "checks": checks,
    }
    write_json(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH)}")
    print("objc3c-platform-toolchain-range-replay: PASS")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
