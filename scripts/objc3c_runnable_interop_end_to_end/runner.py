"""Runnable mixed-module interop end-to-end validation runner."""

from __future__ import annotations

from datetime import datetime

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .commands import package_runnable_toolchain
from .commands import run_packaged_execution_replay
from .commands import run_packaged_execution_smoke
from .manifest import manifest_path
from .manifest import validate_package_manifest
from .paths import REPORT_PATH
from .paths import ROOT
from .report import build_summary_payload
from .report import write_summary_report
from .results import assert_header_bridge_link_plan
from .results import assert_runtime_interop_link_plan
from .scenarios import run_header_bridge_scenario
from .scenarios import run_probe_scenarios
from .scenarios import run_runtime_interop_scenario


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-interop-e2e" / run_id
    manifest_json_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "runnable-interop-e2e"

    package_result = package_runnable_toolchain(package_root)
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_json(manifest_json_path)
    validate_package_manifest(manifest, package_root=package_root)

    compile_script = manifest_path(manifest, "compile_wrapper", package_root=package_root)
    runtime_library = manifest_path(manifest, "runtime_library", package_root=package_root)
    smoke_script = manifest_path(manifest, "execution_smoke_script", package_root=package_root)
    replay_script = manifest_path(manifest, "execution_replay_script", package_root=package_root)
    provider_fixture = manifest_path(manifest, "interop_runtime_fixture", package_root=package_root)
    consumer_fixture = manifest_path(manifest, "interop_runtime_consumer_fixture", package_root=package_root)
    header_provider_fixture = manifest_path(manifest, "interop_header_bridge_fixture", package_root=package_root)
    header_consumer_fixture = manifest_path(manifest, "interop_header_bridge_consumer_fixture", package_root=package_root)
    packaging_probe = manifest_path(manifest, "interop_packaging_probe", package_root=package_root)
    bridge_probe = manifest_path(manifest, "interop_bridge_generation_probe", package_root=package_root)

    runtime_interop = run_runtime_interop_scenario(
        compile_script=compile_script,
        provider_fixture=provider_fixture,
        consumer_fixture=consumer_fixture,
        artifacts_root=artifacts_root,
        package_root=package_root,
    )
    header_bridge = run_header_bridge_scenario(
        compile_script=compile_script,
        provider_fixture=header_provider_fixture,
        consumer_fixture=header_consumer_fixture,
        artifacts_root=artifacts_root,
        package_root=package_root,
    )
    probes = run_probe_scenarios(
        runtime_library=runtime_library,
        packaging_probe=packaging_probe,
        bridge_probe=bridge_probe,
        artifacts_root=artifacts_root,
        package_root=package_root,
        consumer_link_plan=runtime_interop.consumer_link_plan,
        provider_bridge_json=runtime_interop.provider_bridge_json,
    )

    smoke_result = run_packaged_execution_smoke(smoke_script, cwd=package_root)
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke validation failed")

    replay_result = run_packaged_execution_replay(replay_script, cwd=package_root)
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")

    payload = build_summary_payload(
        package_result=package_result,
        smoke_result=smoke_result,
        replay_result=replay_result,
        manifest_json_path=manifest_json_path,
        package_root=package_root,
        runtime_interop=runtime_interop,
        header_bridge=header_bridge,
        probes=probes,
    )

    write_summary_report(payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = [
    "assert_header_bridge_link_plan",
    "assert_runtime_interop_link_plan",
    "main",
]
