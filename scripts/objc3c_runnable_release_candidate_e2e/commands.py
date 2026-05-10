"""Command execution helpers for runnable release-candidate validation."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.probe_compile import compile_probe
from objc3c_tooling.subprocesses import run_capture

from .artifacts import assert_compile_artifacts
from .artifacts import assert_validate_artifacts
from .artifacts import collect_validate_artifacts
from .artifacts import compile_artifact_paths
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


def compile_release_candidate_fixture(
    compile_script: Path,
    release_candidate_fixture: Path,
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
            str(release_candidate_fixture),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the release-candidate fixture")

    compile_artifacts = compile_artifact_paths(compile_out_dir)
    assert_compile_artifacts(compile_artifacts)
    return compile_result, compile_artifacts


def validate_release_candidate_fixture(
    native_executable: Path,
    conformance_report: Path,
    validate_out_dir: Path,
    *,
    cwd: Path,
) -> tuple[object, list[str]]:
    validation_result = run_capture(
        [
            str(native_executable),
            "--validate-objc3-conformance",
            str(conformance_report),
            "--out-dir",
            str(validate_out_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ],
        cwd=cwd,
    )
    if validation_result.returncode != 0:
        raise RuntimeError("packaged release-candidate validation failed")

    validate_artifacts = collect_validate_artifacts(validate_out_dir)
    assert_validate_artifacts(validate_artifacts)
    return validation_result, validate_artifacts


def compile_release_candidate_probe(
    *,
    clangxx: str,
    probe_source: Path,
    output_exe: Path,
    cwd: Path,
    runtime_library: Path,
) -> object:
    return compile_probe(
        clangxx,
        probe_source,
        output_exe,
        cwd=cwd,
        runtime_library=runtime_library,
    )


def run_packaged_executable(executable: Path, *, cwd: Path) -> object:
    return run_capture([str(executable)], cwd=cwd)


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
    "compile_release_candidate_fixture",
    "compile_release_candidate_probe",
    "package_runnable_toolchain",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
    "run_packaged_executable",
    "validate_release_candidate_fixture",
]
