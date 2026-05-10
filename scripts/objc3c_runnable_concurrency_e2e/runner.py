"""Runnable concurrency end-to-end validation runner."""

from __future__ import annotations

from datetime import datetime
from typing import Sequence

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx

from .catalog import CONCURRENCY_SCENARIOS
from .cli import parse_args
from .commands import compile_fixture
from .commands import compile_packaged_probe
from .commands import package_runnable_toolchain
from .commands import run_packaged_execution_replay
from .commands import run_packaged_execution_smoke
from .commands import run_packaged_probe
from .manifest import load_and_validate_manifest
from .manifest import manifest_path
from .paths import REPORT_PATH
from .paths import ROOT
from .report import build_summary_payload
from .report import write_summary_report
from .results import assert_probe_payload
from .results import parse_probe_payload


def main(argv: Sequence[str] | None = None) -> int:
    parse_args(argv)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-cc-e2e" / run_id
    manifest_json_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "runnable-concurrency-e2e"

    package_result = package_runnable_toolchain(package_root)
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_and_validate_manifest(manifest_json_path, package_root=package_root)
    compile_script = manifest_path(manifest, "compile_wrapper", package_root=package_root)
    runtime_library = manifest_path(manifest, "runtime_library", package_root=package_root)
    smoke_script = manifest_path(manifest, "execution_smoke_script", package_root=package_root)
    replay_script = manifest_path(manifest, "execution_replay_script", package_root=package_root)

    scenario_fixtures = {
        scenario.scenario_id: manifest_path(
            manifest,
            scenario.fixture_manifest_key,
            package_root=package_root,
        )
        for scenario in CONCURRENCY_SCENARIOS
    }
    scenario_probes = {
        scenario.scenario_id: manifest_path(
            manifest,
            scenario.probe_manifest_key,
            package_root=package_root,
        )
        for scenario in CONCURRENCY_SCENARIOS
    }

    compile_artifacts = {
        scenario.scenario_id: compile_fixture(
            compile_script,
            scenario_fixtures[scenario.scenario_id],
            artifacts_root / scenario.scenario_id / "compile",
            cwd=package_root,
        )
        for scenario in CONCURRENCY_SCENARIOS
    }

    clangxx = find_clangxx()
    probe_executables = {
        scenario.scenario_id: artifacts_root / scenario.scenario_id / scenario.probe_executable_name
        for scenario in CONCURRENCY_SCENARIOS
    }
    for scenario in CONCURRENCY_SCENARIOS:
        compile_packaged_probe(
            clangxx=clangxx,
            probe_source=scenario_probes[scenario.scenario_id],
            probe_exe=probe_executables[scenario.scenario_id],
            cwd=package_root,
            runtime_library=runtime_library,
            object_input=compile_artifacts[scenario.scenario_id]["object"],
        )

    probe_payloads: dict[str, dict[str, object]] = {}
    for scenario in CONCURRENCY_SCENARIOS:
        payload = parse_probe_payload(
            run_packaged_probe(probe_executables[scenario.scenario_id], cwd=package_root),
            scenario,
        )
        assert_probe_payload(scenario, payload)
        probe_payloads[scenario.scenario_id] = payload

    smoke_result = run_packaged_execution_smoke(smoke_script, cwd=package_root)
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke failed")

    replay_result = run_packaged_execution_replay(replay_script, cwd=package_root)
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")

    payload = build_summary_payload(
        package_result=package_result,
        smoke_result=smoke_result,
        replay_result=replay_result,
        manifest_json_path=manifest_json_path,
        package_root=package_root,
        probe_executables=probe_executables,
        compile_artifacts=compile_artifacts,
        probe_payloads=probe_payloads,
        clangxx=clangxx,
    )

    write_summary_report(payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = ["main"]
