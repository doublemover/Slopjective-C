from __future__ import annotations

import os
import platform
import socket
from typing import Callable, Sequence

from objc3c_tooling.subprocesses import run_capture


def machine_profile() -> dict[str, object]:
    cpu_model = (
        os.environ.get("PROCESSOR_IDENTIFIER")
        or platform.processor()
        or os.environ.get("PROCESSOR_ARCHITECTURE", "")
    )
    return {
        "hostname": socket.gethostname(),
        "os": platform.platform(),
        "arch": platform.machine() or os.environ.get("PROCESSOR_ARCHITECTURE", ""),
        "cpu_model": cpu_model,
        "cpu_count": os.cpu_count() or 1,
        "python_version": platform.python_version(),
    }


def first_line(text: str) -> str:
    return next((line.strip() for line in text.splitlines() if line.strip()), "")


def tool_versions(
    *,
    run_capture_fn: Callable[[Sequence[str]], object] = run_capture,
) -> dict[str, str]:
    versions = {
        "python": platform.python_version(),
    }
    clang = run_capture_fn(["clang", "--version"])
    if clang.returncode == 0:
        versions["clang"] = first_line(clang.stdout)
    return versions
