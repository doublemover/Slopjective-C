"""Runtime probe compilation and link argument assembly."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.probe_compile import compile_probe as compile_runtime_probe

from objc3c_runtime_acceptance.compile_backends import (
    runtime_driver_linker_flags,
)
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.paths import RUNTIME_LIB
from objc3c_runtime_acceptance.process_execution import run


def compile_probe(
    clangxx: str,
    probe: Path,
    exe_path: Path,
    extra_objects: list[Path],
) -> None:
    compile_probe_with_args(clangxx, probe, exe_path, extra_objects, [])


def runtime_link_args_for_objects(extra_objects: list[Path]) -> list[str]:
    args: list[str] = []
    for obj_path in extra_objects:
        args.extend(runtime_driver_linker_flags(obj_path))
    return args


def compile_probe_with_args(
    clangxx: str,
    probe: Path,
    exe_path: Path,
    extra_objects: list[Path],
    extra_args: list[str],
) -> None:
    compile_runtime_probe(
        clangxx,
        probe,
        exe_path,
        cwd=ROOT,
        runtime_library=RUNTIME_LIB,
        object_inputs=extra_objects,
        extra_args=[*runtime_link_args_for_objects(extra_objects), *extra_args],
        failure_context=f"probe link failed for {probe}",
        runner=run,
    )


__all__ = [
    "compile_probe",
    "compile_probe_with_args",
    "runtime_link_args_for_objects",
]
