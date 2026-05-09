from __future__ import annotations

import subprocess
from pathlib import Path


def tracked_generated_reports(root: Path) -> list[str]:
    try:
        result = subprocess.run(
            ["git", "ls-files", "reports"],
            cwd=root,
            check=False,
            text=True,
            capture_output=True,
        )
    except OSError:
        return []
    if result.returncode != 0:
        return []
    return sorted(
        line.strip().replace("\\", "/")
        for line in result.stdout.splitlines()
        if line.strip()
    )
