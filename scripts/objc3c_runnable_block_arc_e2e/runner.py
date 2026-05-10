"""Runnable block/ARC end-to-end validation runner."""

from __future__ import annotations

from datetime import datetime
from typing import Sequence

from objc3c_tooling.paths import normalize_rel_path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx

from .assertions import expect
from .cli import parse_args
from .commands import compile_block_arc_fixture
from .commands import compile_byref_forwarding_probe
from .commands import compile_runtime_abi_probe
from .commands import link_packaged_block_arc_fixture
from .commands import package_runnable_toolchain
from .commands import run_packaged_execution_replay
from .commands import run_packaged_execution_smoke
from .commands import run_packaged_executable
from .commands import write_smoke_fixture_list
from .manifest import load_and_validate_manifest
from .manifest import manifest_path
from .paths import REPORT_PATH
from .paths import ROOT
from .report import build_summary_payload
from .report import write_summary_report
from .results import assert_byref_forwarding_probe_payload
from .results import assert_runtime_abi_probe_payload
from .results import parse_byref_forwarding_probe_payload
from .results import parse_runtime_abi_probe_payload


def main(argv: Sequence[str] | None = None) -> int:
    parse_args(argv)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-ba-e2e" / run_id
    manifest_json_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    compile_out_dir = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "compile"
    linked_fixture_exe = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "block-fixture.exe"
    abi_probe_exe = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "block-arc-runtime-abi-probe.exe"
    byref_probe_exe = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "block-byref-forwarding-probe.exe"
    smoke_fixture_list = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "execution-fixtures.txt"

    package_result = package_runnable_toolchain(package_root)
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_and_validate_manifest(manifest_json_path, package_root=package_root)

    compile_script = manifest_path(manifest, "compile_wrapper", package_root=package_root)
    runtime_library = manifest_path(manifest, "runtime_library", package_root=package_root)
    block_arc_fixture = manifest_path(manifest, "block_arc_fixture", package_root=package_root)
    runtime_abi_probe = manifest_path(manifest, "block_arc_runtime_abi_probe", package_root=package_root)
    byref_forwarding_probe = manifest_path(manifest, "block_arc_byref_forwarding_probe", package_root=package_root)
    smoke_script = manifest_path(manifest, "execution_smoke_script", package_root=package_root)
    replay_script = manifest_path(manifest, "execution_replay_script", package_root=package_root)
    execution_fixture_root = normalize_rel_path(str(manifest["execution_fixture_root"])).rstrip("/")

    compile_result, compile_artifacts = compile_block_arc_fixture(
        compile_script,
        block_arc_fixture,
        compile_out_dir,
        cwd=package_root,
    )

    clangxx = find_clangxx()
    link_fixture_result = link_packaged_block_arc_fixture(
        clangxx=clangxx,
        object_path=compile_artifacts["object"],
        runtime_library=runtime_library,
        linked_fixture_exe=linked_fixture_exe,
        cwd=package_root,
    )
    if link_fixture_result.returncode != 0:
        raise RuntimeError("packaged block/ARC fixture link failed")

    linked_fixture_run = run_packaged_executable(linked_fixture_exe, cwd=package_root)
    expect(
        linked_fixture_run.returncode == 14,
        f"expected packaged block/ARC fixture to exit 14, saw {linked_fixture_run.returncode}",
    )

    abi_probe_compile_result = compile_runtime_abi_probe(
        clangxx=clangxx,
        runtime_abi_probe=runtime_abi_probe,
        runtime_library=runtime_library,
        abi_probe_exe=abi_probe_exe,
        cwd=package_root,
    )
    if abi_probe_compile_result.returncode != 0:
        raise RuntimeError("packaged block ARC runtime ABI probe compile failed")

    abi_probe_payload = parse_runtime_abi_probe_payload(
        run_packaged_executable(abi_probe_exe, cwd=package_root)
    )
    assert_runtime_abi_probe_payload(abi_probe_payload)

    byref_probe_compile_result = compile_byref_forwarding_probe(
        clangxx=clangxx,
        byref_forwarding_probe=byref_forwarding_probe,
        runtime_library=runtime_library,
        byref_probe_exe=byref_probe_exe,
        cwd=package_root,
    )
    if byref_probe_compile_result.returncode != 0:
        raise RuntimeError("packaged block byref forwarding probe compile failed")

    byref_probe_payload = parse_byref_forwarding_probe_payload(
        run_packaged_executable(byref_probe_exe, cwd=package_root)
    )
    assert_byref_forwarding_probe_payload(byref_probe_payload)

    write_smoke_fixture_list(
        smoke_fixture_list,
        execution_fixture_root=execution_fixture_root,
    )
    smoke_result = run_packaged_execution_smoke(
        smoke_script,
        smoke_fixture_list=smoke_fixture_list,
        cwd=package_root,
    )
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged block/ARC execution smoke failed")

    replay_result = run_packaged_execution_replay(replay_script, cwd=package_root)
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")

    payload = build_summary_payload(
        package_result=package_result,
        smoke_result=smoke_result,
        replay_result=replay_result,
        manifest_json_path=manifest_json_path,
        package_root=package_root,
        block_arc_fixture=block_arc_fixture,
        runtime_abi_probe=runtime_abi_probe,
        byref_forwarding_probe=byref_forwarding_probe,
        linked_fixture_exe=linked_fixture_exe,
        abi_probe_exe=abi_probe_exe,
        byref_probe_exe=byref_probe_exe,
        compile_artifacts=compile_artifacts,
        runtime_abi_probe_payload=abi_probe_payload,
        byref_forwarding_probe_payload=byref_probe_payload,
        compile_result=compile_result,
        link_fixture_result=link_fixture_result,
        linked_fixture_run=linked_fixture_run,
        abi_probe_compile_result=abi_probe_compile_result,
        byref_probe_compile_result=byref_probe_compile_result,
        smoke_fixture_list=smoke_fixture_list,
        clangxx=clangxx,
    )

    write_summary_report(payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = ["main"]
