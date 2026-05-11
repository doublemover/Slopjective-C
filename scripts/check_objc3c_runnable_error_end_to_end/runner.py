"""Runnable error end-to-end validation runner."""

from __future__ import annotations

from datetime import datetime

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx

from .artifacts import assert_compile_artifacts_exist, compile_artifact_paths
from .commands import (
    compile_error_fixture,
    compile_packaged_error_probe,
    package_runnable_toolchain,
    run_packaged_error_probe,
    run_packaged_execution_replay,
    run_packaged_execution_smoke,
)
from .manifest import manifest_path, validate_package_manifest
from .paths import REPORT_PATH, ROOT
from .probes import assert_probe_payload
from .report import build_summary_payload, write_summary_report


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-error-e2e" / run_id
    manifest_json_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    compile_out_dir = package_root / "tmp" / "artifacts" / "runnable-error-e2e" / "compile"
    probe_exe = package_root / "tmp" / "artifacts" / "runnable-error-e2e" / "probe.exe"

    package_result = package_runnable_toolchain(package_root)
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_json(manifest_json_path)
    validate_package_manifest(manifest, package_root=package_root)

    compile_script = manifest_path(manifest, "compile_wrapper", package_root=package_root)
    error_fixture = manifest_path(manifest, "error_runtime_fixture", package_root=package_root)
    runtime_library = manifest_path(manifest, "runtime_library", package_root=package_root)
    error_probe = manifest_path(manifest, "error_runtime_probe", package_root=package_root)
    smoke_script = manifest_path(manifest, "execution_smoke_script", package_root=package_root)
    replay_script = manifest_path(manifest, "execution_replay_script", package_root=package_root)

    compile_result = compile_error_fixture(
        compile_script=compile_script,
        error_fixture=error_fixture,
        compile_out_dir=compile_out_dir,
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the error fixture")

    compile_artifacts = compile_artifact_paths(compile_out_dir)
    assert_compile_artifacts_exist(compile_artifacts)

    clangxx = find_clangxx()
    probe_compile_result = compile_packaged_error_probe(
        clangxx=clangxx,
        package_root=package_root,
        error_probe=error_probe,
        module_object=compile_artifacts["object"],
        runtime_library=runtime_library,
        probe_exe=probe_exe,
    )
    if probe_compile_result.returncode != 0:
        raise RuntimeError("packaged error probe compile failed")

    probe_payload = run_packaged_error_probe(probe_exe, cwd=package_root)
    assert_probe_payload(probe_payload)

    smoke_result = run_packaged_execution_smoke(smoke_script, cwd=package_root)
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke failed")

    replay_result = run_packaged_execution_replay(replay_script, cwd=package_root)
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")

    payload = build_summary_payload(
        package_result=package_result,
        compile_result=compile_result,
        probe_compile_result=probe_compile_result,
        smoke_result=smoke_result,
        replay_result=replay_result,
        clangxx=clangxx,
        manifest_json_path=manifest_json_path,
        package_root=package_root,
        error_fixture=error_fixture,
        error_probe=error_probe,
        probe_exe=probe_exe,
        compile_artifacts=compile_artifacts,
        probe_payload=probe_payload,
    )

    write_summary_report(payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = ["main"]
