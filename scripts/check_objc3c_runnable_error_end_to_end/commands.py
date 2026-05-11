"""Command execution helpers for runnable error end-to-end validation."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.probe_output import parse_json_output
from objc3c_tooling.subprocesses import run_capture

from .paths import PACKAGE_PS1, PWSH, ROOT


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


def compile_error_fixture(
    *,
    compile_script: Path,
    error_fixture: Path,
    compile_out_dir: Path,
    cwd: Path,
) -> object:
    return run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(error_fixture),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=cwd,
    )


def compile_packaged_error_probe(
    *,
    clangxx: str,
    package_root: Path,
    error_probe: Path,
    module_object: Path,
    runtime_library: Path,
    probe_exe: Path,
) -> object:
    probe_exe.parent.mkdir(parents=True, exist_ok=True)
    return run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((package_root / "native/objc3c/src").resolve()),
            str(error_probe),
            str(module_object),
            str(runtime_library),
            "-o",
            str(probe_exe),
        ],
        cwd=package_root,
    )


def run_packaged_error_probe(probe_exe: Path, *, cwd: Path) -> dict[str, object]:
    return parse_json_output(
        run_capture([str(probe_exe)], cwd=cwd),
        "packaged error runtime probe",
    )


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
    "compile_error_fixture",
    "compile_packaged_error_probe",
    "package_runnable_toolchain",
    "run_packaged_error_probe",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
]
