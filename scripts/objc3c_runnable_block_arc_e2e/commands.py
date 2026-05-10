"""Command execution helpers for runnable block/ARC end-to-end validation."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.subprocesses import run_capture

from .assertions import expect
from .catalog import BLOCK_ARC_SMOKE_FIXTURES
from .paths import PACKAGE_PS1
from .paths import PWSH
from .paths import ROOT


def package_runnable_toolchain(package_root: Path) -> object:
    return run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(package_root),
        ],
        cwd=ROOT,
    )


def compile_block_arc_fixture(
    compile_script: Path,
    block_arc_fixture: Path,
    compile_out_dir: Path,
    *,
    cwd: Path,
) -> tuple[object, dict[str, Path]]:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(block_arc_fixture),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the block/ARC fixture")

    compile_artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_path in compile_artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")
    return compile_result, compile_artifacts


def link_packaged_block_arc_fixture(
    *,
    clangxx: str,
    object_path: Path,
    runtime_library: Path,
    linked_fixture_exe: Path,
    cwd: Path,
) -> object:
    linked_fixture_exe.parent.mkdir(parents=True, exist_ok=True)
    return run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            str(object_path),
            str(runtime_library),
            "-o",
            str(linked_fixture_exe),
        ],
        cwd=cwd,
    )


def compile_runtime_abi_probe(
    *,
    clangxx: str,
    runtime_abi_probe: Path,
    runtime_library: Path,
    abi_probe_exe: Path,
    cwd: Path,
) -> object:
    return run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((cwd / "native/objc3c/src").resolve()),
            str(runtime_abi_probe),
            str(runtime_library),
            "-o",
            str(abi_probe_exe),
        ],
        cwd=cwd,
    )


def compile_byref_forwarding_probe(
    *,
    clangxx: str,
    byref_forwarding_probe: Path,
    runtime_library: Path,
    byref_probe_exe: Path,
    cwd: Path,
) -> object:
    return run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((cwd / "native/objc3c/src").resolve()),
            str(byref_forwarding_probe),
            str(runtime_library),
            "-o",
            str(byref_probe_exe),
        ],
        cwd=cwd,
    )


def run_packaged_executable(executable: Path, *, cwd: Path) -> object:
    return run_capture([str(executable)], cwd=cwd)


def write_smoke_fixture_list(
    smoke_fixture_list: Path,
    *,
    execution_fixture_root: str,
) -> None:
    smoke_fixture_list.parent.mkdir(parents=True, exist_ok=True)
    smoke_fixture_list.write_text(
        "\n".join(
            f"{execution_fixture_root}/{fixture_path}"
            for fixture_path in BLOCK_ARC_SMOKE_FIXTURES
        )
        + "\n",
        encoding="utf-8",
    )


def run_packaged_execution_smoke(
    smoke_script: Path,
    *,
    smoke_fixture_list: Path,
    cwd: Path,
) -> object:
    return run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(smoke_script),
            "-FixtureList",
            str(smoke_fixture_list),
        ],
        cwd=cwd,
    )


def run_packaged_execution_replay(replay_script: Path, *, cwd: Path) -> object:
    return run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(replay_script),
            "-CaseId",
            "canonical-runnable",
        ],
        cwd=cwd,
    )


__all__ = [
    "compile_block_arc_fixture",
    "compile_byref_forwarding_probe",
    "compile_runtime_abi_probe",
    "link_packaged_block_arc_fixture",
    "package_runnable_toolchain",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
    "run_packaged_executable",
    "write_smoke_fixture_list",
]
