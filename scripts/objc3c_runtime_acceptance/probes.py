"""Probe compile, run, and output parsing helpers for runtime acceptance."""

from __future__ import annotations

import os
import subprocess
from pathlib import Path
from typing import Any

from .paths import ROOT
from .paths import RUNTIME_LIB
from .process_execution import run
from .progress_format import repo_display_path
from .progress_state import get_acceptance_progress
from objc3c_tooling.probe_compile import compile_probe as compile_runtime_probe
from objc3c_tooling.probe_output import parse_json_output
from objc3c_tooling.probe_output import parse_key_value_output

from .compile_backends import runtime_driver_linker_flags


RETRYABLE_PROBE_EXIT_CODES = {3221226356}
DEFAULT_PROBE_RETRIES = int(
    os.environ.get("OBJC3C_RUNTIME_ACCEPTANCE_PROBE_RETRIES", "1")
)
ACCEPTANCE_PROBE_RETRY_EVENTS: list[dict[str, Any]] = []


def compile_probe(clangxx: str, probe: Path, exe_path: Path, extra_objects: list[Path]) -> None:
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


def run_probe(
    exe_path: Path, *, env: dict[str, str] | None = None
) -> subprocess.CompletedProcess[str]:
    attempts: list[dict[str, Any]] = []
    for attempt in range(DEFAULT_PROBE_RETRIES + 1):
        result = run([str(exe_path)], env=env)
        attempts.append(
            {
                "attempt": attempt + 1,
                "returncode": result.returncode,
                "stdout_present": result.stdout != "",
                "stderr_present": result.stderr != "",
            }
        )
        if result.returncode == 0:
            if attempt > 0:
                ACCEPTANCE_PROBE_RETRY_EVENTS.append(
                    {
                        "probe": repo_display_path(exe_path),
                        "retry_count": attempt,
                        "attempts": attempts,
                        "model": (
                            "retry is fail-closed and only masks transient process exits "
                            "that are followed by a successful identical probe invocation"
                        ),
                    }
                )
            return result
        if (
            result.returncode not in RETRYABLE_PROBE_EXIT_CODES
            or attempt >= DEFAULT_PROBE_RETRIES
        ):
            raise RuntimeError(
                f"probe execution failed for {exe_path} (exit={result.returncode}):\n"
                f"STDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
            )
        progress = get_acceptance_progress()
        if progress:
            progress.emit(
                "PROBE retry "
                f"probe={repo_display_path(exe_path)} exit={result.returncode} "
                f"attempt={attempt + 1}/{DEFAULT_PROBE_RETRIES + 1}"
            )
    raise RuntimeError(f"probe execution failed for {exe_path}")

__all__ = [
    "ACCEPTANCE_PROBE_RETRY_EVENTS",
    "compile_probe",
    "compile_probe_with_args",
    "DEFAULT_PROBE_RETRIES",
    "parse_json_output",
    "parse_key_value_output",
    "RETRYABLE_PROBE_EXIT_CODES",
    "runtime_link_args_for_objects",
    "run_probe",
]
