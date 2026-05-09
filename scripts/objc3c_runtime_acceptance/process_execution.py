"""Subprocess execution boundary for runtime acceptance."""

from __future__ import annotations

import os
import subprocess
from pathlib import Path

from .paths import ROOT
from .progress_state import get_acceptance_progress


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
    return subprocess.run(
        command,
        cwd=str(cwd),
        env=subprocess_env,
        text=True,
        capture_output=True,
        check=False,
    )


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


__all__ = ["run", "run_command"]
