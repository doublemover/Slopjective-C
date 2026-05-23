"""Subprocess execution boundary for runtime acceptance."""

from __future__ import annotations

import os
import subprocess
from pathlib import Path

from .paths import ROOT
from .progress_state import get_acceptance_progress

ELEVATION_REQUIRED_WINERROR = 740
ELEVATION_REQUIRED_EXIT_CODE = 740


def is_elevation_required_launch_error(error: OSError) -> bool:
    return getattr(error, "winerror", None) == ELEVATION_REQUIRED_WINERROR


def elevation_required_completed_process(
    command: list[str],
    error: OSError,
) -> subprocess.CompletedProcess[str]:
    executable = command[0] if command else "<empty-command>"
    stderr = (
        "runtime acceptance process launch failed: WinError 740 "
        "(ERROR_ELEVATION_REQUIRED) while launching "
        f"{executable}: {error}\n"
    )
    return subprocess.CompletedProcess(
        command,
        ELEVATION_REQUIRED_EXIT_CODE,
        stdout="",
        stderr=stderr,
    )


def run_command(
    command: list[str],
    *,
    cwd: Path,
    env: dict[str, str] | None = None,
) -> subprocess.CompletedProcess[str]:
    subprocess_env = None
    if env:
        subprocess_env = os.environ.copy()
        subprocess_env.update(env)
    try:
        return subprocess.run(
            command,
            cwd=str(cwd),
            env=subprocess_env,
            text=True,
            encoding="utf-8",
            errors="replace",
            capture_output=True,
            check=False,
        )
    except OSError as error:
        if is_elevation_required_launch_error(error):
            return elevation_required_completed_process(command, error)
        raise


def run(
    command: list[str],
    *,
    cwd: Path | None = None,
    env: dict[str, str] | None = None,
) -> subprocess.CompletedProcess[str]:
    resolved_cwd = cwd or ROOT
    progress = get_acceptance_progress()
    started_at = progress.start_command(command, resolved_cwd) if progress else None
    result = run_command(command, cwd=resolved_cwd, env=env)
    if progress and started_at is not None:
        progress.finish_command(
            command=command,
            cwd=resolved_cwd,
            started_at=started_at,
            returncode=result.returncode,
        )
    return result


__all__ = [
    "ELEVATION_REQUIRED_EXIT_CODE",
    "ELEVATION_REQUIRED_WINERROR",
    "elevation_required_completed_process",
    "is_elevation_required_launch_error",
    "run",
    "run_command",
]
