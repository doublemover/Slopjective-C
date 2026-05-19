"""Report rendering for runnable metaprogramming end-to-end validation."""

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


def child_report_paths(
    *,
    manifest_json_path: Path,
    package_result: object,
    smoke_result: object,
    replay_result: object,
) -> list[str]:
    paths = [repo_rel(manifest_json_path)]
    for stdout in (package_result.stdout, smoke_result.stdout, replay_result.stdout):
        paths.extend(extract_report_paths(stdout))
    return sorted(dict.fromkeys(paths))


def build_summary_payload(
    *,
    package_result: object,
    smoke_result: object,
    replay_result: object,
    manifest_json_path: Path,
    package_root: Path,
    provider_artifacts: dict[str, Path],
    consumer_artifacts: dict[str, Path],
    consumer_link_plan_path: Path,
    runtime_probe: Path,
    probe_exe: Path,
    provider_module_name: str,
    provider_host_cache: dict[str, object],
    provider_host_cache_second: dict[str, object],
) -> dict[str, object]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": RUNNER_PATH,
        "package_root": repo_rel(package_root),
        "package_manifest_path": repo_rel(manifest_json_path),
        "provider_compile_manifest_path": repo_rel(provider_artifacts["manifest"]),
        "provider_host_cache_artifact_path": repo_rel(provider_artifacts["host_cache"]),
        "provider_runtime_import_surface_path": repo_rel(provider_artifacts["runtime_import_surface"]),
        "consumer_compile_manifest_path": repo_rel(consumer_artifacts["manifest"]),
        "consumer_link_plan_path": repo_rel(consumer_link_plan_path),
        "probe_path": repo_rel(runtime_probe),
        "probe_executable_path": repo_rel(probe_exe),
        "provider_module_name": provider_module_name,
        "provider_cache_key": provider_host_cache.get("cache_key"),
        "provider_first_materialization_state": provider_host_cache.get("cache_materialization_state"),
        "provider_second_materialization_state": provider_host_cache_second.get("cache_materialization_state"),
        "smoke_report_path": extract_output_value(smoke_result.stdout, "summary_path"),
        "replay_report_path": extract_output_value(replay_result.stdout, "summary_path"),
        "child_report_paths": child_report_paths(
            manifest_json_path=manifest_json_path,
            package_result=package_result,
            smoke_result=smoke_result,
            replay_result=replay_result,
        ),
    }


def write_summary_report(payload: dict[str, object]) -> None:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)


__all__ = ["build_summary_payload", "child_report_paths", "write_summary_report"]
