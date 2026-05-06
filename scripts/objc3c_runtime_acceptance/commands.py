"""Subprocess execution for runtime acceptance."""

from __future__ import annotations

import os
import subprocess
from pathlib import Path


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
