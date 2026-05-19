"""Command execution helpers for runnable metaprogramming validation."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

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
    extra_args: Sequence[str] = (),
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
            *extra_args,
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(f"packaged compile wrapper failed for {fixture_path}")

    artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "runtime_import_surface": compile_out_dir / "module.runtime-import-surface.json",
        "host_cache": compile_out_dir / "module.metaprogramming-macro-host-cache.json",
    }
    for artifact_path in artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")
    return artifacts


def compile_packaged_probe(
    *,
    clangxx: str,
    runtime_probe: Path,
    probe_exe: Path,
    cwd: Path,
    runtime_library: Path,
) -> object:
    return compile_probe(
        clangxx,
        runtime_probe,
        probe_exe,
        cwd=cwd,
        runtime_library=runtime_library,
    )


def run_packaged_probe(probe_exe: Path, *, cwd: Path) -> object:
    return run_capture([str(probe_exe)], cwd=cwd)


def run_packaged_execution_smoke(smoke_script: Path, *, cwd: Path) -> object:
    return run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(smoke_script)],
        cwd=cwd,
    )


def run_packaged_execution_replay(replay_script: Path, *, cwd: Path) -> object:
    return run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(replay_script)],
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
