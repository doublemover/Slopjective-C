"""Report rendering for runnable block/ARC end-to-end validation."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths

from .paths import REPORT_PATH
from .paths import RUNNER_PATH
from .paths import SUMMARY_CONTRACT_ID
from .results import byref_forwarding_probe_summary


def build_summary_payload(
    *,
    package_result: object,
    smoke_result: object,
    replay_result: object,
    manifest_json_path: Path,
    package_root: Path,
    block_arc_fixture: Path,
    runtime_abi_probe: Path,
    byref_forwarding_probe: Path,
    linked_fixture_exe: Path,
    abi_probe_exe: Path,
    byref_probe_exe: Path,
    compile_artifacts: dict[str, Path],
    runtime_abi_probe_payload: dict[str, object],
    byref_forwarding_probe_payload: dict[str, object],
    compile_result: object,
    link_fixture_result: object,
    linked_fixture_run: object,
    abi_probe_compile_result: object,
    byref_probe_compile_result: object,
    smoke_fixture_list: Path,
    clangxx: str,
) -> dict[str, object]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": RUNNER_PATH,
        "package_manifest_path": repo_rel(manifest_json_path),
        "package_root": repo_rel(package_root),
        "packaged_compile_fixture": repo_rel(block_arc_fixture),
        "packaged_runtime_abi_probe": repo_rel(runtime_abi_probe),
        "packaged_byref_forwarding_probe": repo_rel(byref_forwarding_probe),
        "packaged_fixture_executable": repo_rel(linked_fixture_exe),
        "packaged_runtime_abi_probe_executable": repo_rel(abi_probe_exe),
        "packaged_byref_forwarding_probe_executable": repo_rel(byref_probe_exe),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifacts.items()
        },
        "runtime_abi_probe_payload": runtime_abi_probe_payload,
        "byref_forwarding_probe_payload": byref_forwarding_probe_payload,
        "byref_forwarding_probe_summary": byref_forwarding_probe_summary(
            byref_forwarding_probe_payload
        ),
        "child_report_paths": [
            *extract_report_paths(package_result.stdout),
            *extract_report_paths(smoke_result.stdout),
            *extract_report_paths(replay_result.stdout),
        ],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "compile-block-arc-fixture",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "link-packaged-block-arc-fixture",
                "exit_code": link_fixture_result.returncode,
                "expected_exit_code": 14,
                "actual_exit_code": linked_fixture_run.returncode,
            },
            {
                "action": "compile-packaged-block-arc-runtime-abi-probe",
                "exit_code": abi_probe_compile_result.returncode,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-block-arc-runtime-abi-probe",
                "exit_code": 0,
            },
            {
                "action": "compile-packaged-block-byref-forwarding-probe",
                "exit_code": byref_probe_compile_result.returncode,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-block-byref-forwarding-probe",
                "exit_code": 0,
            },
            {
                "action": "packaged-block-arc-execution-smoke",
                "exit_code": smoke_result.returncode,
                "report_paths": extract_report_paths(smoke_result.stdout),
                "fixture_list": repo_rel(smoke_fixture_list),
            },
            {
                "action": "packaged-execution-replay",
                "exit_code": replay_result.returncode,
                "report_paths": extract_report_paths(replay_result.stdout),
            },
        ],
    }


def write_summary_report(payload: dict[str, object]) -> None:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)


__all__ = ["build_summary_payload", "write_summary_report"]
