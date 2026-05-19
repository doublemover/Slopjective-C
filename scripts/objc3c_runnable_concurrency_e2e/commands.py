"""Command execution helpers for runnable concurrency end-to-end validation."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import compile_probe
from objc3c_tooling.subprocesses import run_capture

from .assertions import expect
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


def compile_fixture(
    compile_script: Path,
    fixture_path: Path,
    compile_out_dir: Path,
    *,
    cwd: Path,
) -> dict[str, Path]:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(fixture_path),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(f"packaged compile wrapper failed for {repo_rel(fixture_path)}")

    artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_path in artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")
    return artifacts


def compile_packaged_probe(
    *,
    clangxx: str,
    probe_source: Path,
    probe_exe: Path,
    cwd: Path,
    runtime_library: Path,
    object_input: Path,
) -> None:
    compile_probe(
        clangxx,
        probe_source,
        probe_exe,
        cwd=cwd,
        runtime_library=runtime_library,
        object_inputs=(object_input,),
    )


def run_packaged_probe(probe_exe: Path, *, cwd: Path) -> object:
    return run_capture([str(probe_exe)], cwd=cwd)


def run_packaged_execution_smoke(smoke_script: Path, *, cwd: Path) -> object:
    return run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(smoke_script),
            "-Limit",
            "12",
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
    "compile_fixture",
    "compile_packaged_probe",
    "package_runnable_toolchain",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
    "run_packaged_probe",
]
