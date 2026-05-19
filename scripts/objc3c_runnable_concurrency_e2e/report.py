"""Report rendering for runnable concurrency end-to-end validation."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths

from .catalog import CONCURRENCY_SCENARIOS
from .paths import REPORT_PATH
from .paths import SUMMARY_CONTRACT_ID


def build_summary_payload(
    *,
    package_result: object,
    smoke_result: object,
    replay_result: object,
    manifest_json_path: Path,
    package_root: Path,
    probe_executables: dict[str, Path],
    compile_artifacts: dict[str, dict[str, Path]],
    probe_payloads: dict[str, dict[str, object]],
    clangxx: str,
) -> dict[str, object]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_concurrency_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_json_path),
        "package_root": repo_rel(package_root),
        "packaged_probe_executables": {
            scenario.scenario_id: repo_rel(probe_executables[scenario.scenario_id])
            for scenario in CONCURRENCY_SCENARIOS
        },
        "packaged_compile_artifacts": {
            scenario.scenario_id: {
                key: repo_rel(path)
                for key, path in compile_artifacts[scenario.scenario_id].items()
            }
            for scenario in CONCURRENCY_SCENARIOS
        },
        "probe_payloads": {
            scenario.scenario_id: probe_payloads[scenario.scenario_id]
            for scenario in CONCURRENCY_SCENARIOS
        },
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
            *[
                {"action": scenario.compile_fixture_action, "exit_code": 0}
                for scenario in CONCURRENCY_SCENARIOS
            ],
            *[
                {
                    "action": scenario.compile_probe_action,
                    "exit_code": 0,
                    "clangxx": clangxx,
                }
                for scenario in CONCURRENCY_SCENARIOS
            ],
            *[
                {"action": scenario.run_probe_action, "exit_code": 0}
                for scenario in CONCURRENCY_SCENARIOS
            ],
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
