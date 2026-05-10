"""Runnable metaprogramming end-to-end validation runner."""

from __future__ import annotations

from datetime import datetime
from typing import Sequence

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx

from .catalog import FIRST_HOST_CACHE_EXPECTATION
from .catalog import SECOND_HOST_CACHE_EXPECTATION
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
from .results import assert_consumer_link_plan
from .results import assert_host_cache_expectation
from .results import assert_preserved_host_cache_fields
from .results import assert_provider_module_name
from .results import assert_runtime_probe_payload
from .results import parse_runtime_probe_payload


def main(argv: Sequence[str] | None = None) -> int:
    parse_args(argv)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-mp-e2e" / run_id
    manifest_json_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "runnable-metaprogramming-e2e"

    package_result = package_runnable_toolchain(package_root)
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_and_validate_manifest(manifest_json_path, package_root=package_root)
    compile_script = manifest_path(manifest, "compile_wrapper", package_root=package_root)
    runtime_library = manifest_path(manifest, "runtime_library", package_root=package_root)
    smoke_script = manifest_path(manifest, "execution_smoke_script", package_root=package_root)
    replay_script = manifest_path(manifest, "execution_replay_script", package_root=package_root)
    provider_fixture = manifest_path(manifest, "metaprogramming_runtime_fixture", package_root=package_root)
    consumer_fixture = manifest_path(manifest, "metaprogramming_runtime_consumer_fixture", package_root=package_root)
    runtime_probe = manifest_path(manifest, "metaprogramming_runtime_probe", package_root=package_root)

    provider_artifacts = compile_fixture(
        compile_script,
        provider_fixture,
        artifacts_root / "provider-first" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    provider_host_cache = load_json(provider_artifacts["host_cache"])
    assert_host_cache_expectation(provider_host_cache, FIRST_HOST_CACHE_EXPECTATION)

    provider_artifacts_second = compile_fixture(
        compile_script,
        provider_fixture,
        artifacts_root / "provider-second" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    provider_host_cache_second = load_json(provider_artifacts_second["host_cache"])
    assert_host_cache_expectation(provider_host_cache_second, SECOND_HOST_CACHE_EXPECTATION)
    assert_preserved_host_cache_fields(provider_host_cache, provider_host_cache_second)

    provider_runtime_import = load_json(provider_artifacts["runtime_import_surface"])
    provider_module_name = assert_provider_module_name(provider_runtime_import.get("module_name"))

    consumer_artifacts = compile_fixture(
        compile_script,
        consumer_fixture,
        artifacts_root / "consumer" / "compile",
        cwd=package_root,
        extra_args=(
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_artifacts["runtime_import_surface"]),
        ),
    )
    consumer_link_plan_path = artifacts_root / "consumer" / "compile" / "module.cross-module-runtime-link-plan.json"
    link_plan = load_json(consumer_link_plan_path)
    assert_consumer_link_plan(link_plan, provider_module_name=provider_module_name)

    clangxx = find_clangxx()
    probe_exe = artifacts_root / "probe.exe"
    compile_packaged_probe(
        clangxx=clangxx,
        runtime_probe=runtime_probe,
        probe_exe=probe_exe,
        cwd=package_root,
        runtime_library=runtime_library,
    )
    probe_payload = parse_runtime_probe_payload(run_packaged_probe(probe_exe, cwd=package_root))
    assert_runtime_probe_payload(probe_payload, provider_host_cache=provider_host_cache)

    smoke_result = run_packaged_execution_smoke(smoke_script, cwd=package_root)
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke validation failed")

    replay_result = run_packaged_execution_replay(replay_script, cwd=package_root)
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay validation failed")

    payload = build_summary_payload(
        package_result=package_result,
        smoke_result=smoke_result,
        replay_result=replay_result,
        manifest_json_path=manifest_json_path,
        package_root=package_root,
        provider_artifacts=provider_artifacts,
        consumer_artifacts=consumer_artifacts,
        consumer_link_plan_path=consumer_link_plan_path,
        runtime_probe=runtime_probe,
        probe_exe=probe_exe,
        provider_module_name=provider_module_name,
        provider_host_cache=provider_host_cache,
        provider_host_cache_second=provider_host_cache_second,
    )

    write_summary_report(payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = ["main"]
