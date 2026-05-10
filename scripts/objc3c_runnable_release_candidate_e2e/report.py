"""Report rendering for runnable release-candidate end-to-end validation."""

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


def build_summary_payload(
    *,
    package_result: object,
    smoke_result: object,
    replay_result: object,
    manifest_json_path: Path,
    package_root: Path,
    release_candidate_fixture: Path,
    validate_out_dir: Path,
    compile_artifacts: dict[str, Path],
    validate_artifacts: list[str],
    claim_payload: dict[str, object],
    evidence_payload: dict[str, object],
    compile_result: object,
    validation_result: object,
    clangxx: str,
) -> dict[str, object]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": RUNNER_PATH,
        "package_manifest_path": repo_rel(manifest_json_path),
        "package_root": repo_rel(package_root),
        "packaged_compile_fixture": repo_rel(release_candidate_fixture),
        "packaged_validate_out_dir": repo_rel(validate_out_dir),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifacts.items()
        },
        "packaged_validate_artifacts": validate_artifacts,
        "release_candidate_claim_probe_payload": claim_payload,
        "release_candidate_evidence_probe_payload": evidence_payload,
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
                "action": "compile-release-candidate-fixture",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "validate-release-candidate-fixture",
                "exit_code": validation_result.returncode,
            },
            {
                "action": "compile-release-candidate-claim-probe",
                "exit_code": 0,
                "clangxx": clangxx,
            },
            {
                "action": "compile-release-candidate-evidence-probe",
                "exit_code": 0,
                "clangxx": clangxx,
            },
            {
                "action": "run-release-candidate-claim-probe",
                "exit_code": 0,
            },
            {
                "action": "run-release-candidate-evidence-probe",
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
