"""Summary report rendering for runnable error end-to-end validation."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths

from .paths import REPORT_PATH, SUMMARY_CONTRACT_ID


def build_summary_payload(
    *,
    package_result: object,
    compile_result: object,
    probe_compile_result: object,
    smoke_result: object,
    replay_result: object,
    clangxx: str,
    manifest_json_path: Path,
    package_root: Path,
    error_fixture: Path,
    error_probe: Path,
    probe_exe: Path,
    compile_artifacts: dict[str, Path],
    probe_payload: dict[str, object],
) -> dict[str, object]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_error_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_json_path),
        "package_root": repo_rel(package_root),
        "packaged_compile_fixture": repo_rel(error_fixture),
        "packaged_probe": repo_rel(error_probe),
        "packaged_probe_executable": repo_rel(probe_exe),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifacts.items()
        },
        "probe_payload": probe_payload,
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
                "action": "compile-error-fixture",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "compile-packaged-error-probe",
                "exit_code": probe_compile_result.returncode,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-error-probe",
                "exit_code": 0,
            },
            {
                "action": "packaged-execution-smoke",
                "exit_code": smoke_result.returncode,
                "report_paths": extract_report_paths(smoke_result.stdout),
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
