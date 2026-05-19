"""Report rendering for runnable interop end-to-end validation."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths

from .paths import REPORT_PATH
from .paths import SUMMARY_CONTRACT_ID
from .scenarios import HeaderBridgeScenarioResult
from .scenarios import ProbeScenarioResult
from .scenarios import RuntimeInteropScenarioResult


def build_summary_payload(
    *,
    package_result: object,
    smoke_result: object,
    replay_result: object,
    manifest_json_path: Path,
    package_root: Path,
    runtime_interop: RuntimeInteropScenarioResult,
    header_bridge: HeaderBridgeScenarioResult,
    probes: ProbeScenarioResult,
) -> dict[str, object]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_interop_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_json_path),
        "package_root": repo_rel(package_root),
        "packaged_provider_fixture": repo_rel(runtime_interop.provider_fixture),
        "packaged_consumer_fixture": repo_rel(runtime_interop.consumer_fixture),
        "packaged_packaging_probe": repo_rel(probes.packaging_probe),
        "packaged_bridge_probe": repo_rel(probes.bridge_probe),
        "packaged_probe_executables": {
            "packaging_probe": repo_rel(probes.packaging_probe_exe),
            "bridge_probe": repo_rel(probes.bridge_probe_exe),
        },
        "provider_bridge_json_path": repo_rel(runtime_interop.provider_artifacts["bridge_json"]),
        "consumer_cross_module_link_plan_path": repo_rel(
            runtime_interop.consumer_artifacts["cross_module_link_plan"]
        ),
        "packaging_probe_payload": probes.packaging_probe_payload,
        "bridge_probe_payload": probes.bridge_probe_payload,
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
                "action": "compile-interop-provider-fixture",
                "exit_code": 0,
            },
            {
                "action": "compile-interop-consumer-fixture",
                "exit_code": runtime_interop.consumer_compile_result.returncode,
            },
            {
                "action": "compile-header-bridge-provider-fixture",
                "exit_code": 0,
            },
            {
                "action": "compile-header-bridge-consumer-fixture",
                "exit_code": header_bridge.consumer_compile_result.returncode,
            },
            {
                "action": "compile-packaged-interop-probes",
                "exit_code": 0,
                "clangxx": probes.clangxx,
            },
            {
                "action": "run-packaged-interop-probes",
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
