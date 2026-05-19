"""Fixture compile command execution for runtime acceptance."""

from __future__ import annotations

import subprocess
from pathlib import Path

from .compile_backends import DIRECT_COMPILE_BACKEND
from .compile_backends import compile_command
from .compile_truth import write_compile_output_provenance
from .process_execution import run
from .progress_state import get_acceptance_progress
from .registry import ACCEPTANCE_ARTIFACT_REGISTRY


def run_fixture_compile(
    fixture: Path,
    out_dir: Path,
    *,
    extra_args: list[str] | None = None,
    backend: str | None = None,
    write_provenance: bool = True,
    reuse_policy: str = "none",
) -> tuple[subprocess.CompletedProcess[str], str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    progress = get_acceptance_progress()
    command, selected_backend = compile_command(
        fixture,
        out_dir,
        extra_args=extra_args,
        backend=backend,
    )
    if ACCEPTANCE_ARTIFACT_REGISTRY.try_reuse(
        fixture=fixture,
        out_dir=out_dir,
        extra_args=extra_args,
        backend=selected_backend,
        emit_prefix="module",
        reuse_policy=reuse_policy,
        progress=progress,
    ):
        return (
            subprocess.CompletedProcess(
                args=command,
                returncode=0,
                stdout="runtime acceptance artifact registry reused compile outputs\n",
                stderr="",
            ),
            selected_backend,
        )
    result = run(command)
    if (
        write_provenance
        and result.returncode == 0
        and selected_backend == DIRECT_COMPILE_BACKEND
    ):
        write_compile_output_provenance(
            compile_dir=out_dir,
            input_path=fixture,
            compile_backend=selected_backend,
        )
    if write_provenance and result.returncode == 0:
        ACCEPTANCE_ARTIFACT_REGISTRY.register(
            fixture=fixture,
            out_dir=out_dir,
            extra_args=extra_args,
            backend=selected_backend,
            emit_prefix="module",
            reuse_policy=reuse_policy,
            progress=progress,
        )
    return result, selected_backend


__all__ = ["run_fixture_compile"]
