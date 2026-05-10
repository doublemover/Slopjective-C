from __future__ import annotations

import hashlib
import time
from typing import Any, Callable, Sequence

from objc3c_tooling.subprocesses import run_capture


def run_timed_step(
    command: Sequence[str],
    *,
    run_capture_fn: Callable[[Sequence[str]], Any] = run_capture,
) -> dict[str, Any]:
    started = time.perf_counter()
    completed = run_capture_fn(command)
    duration_ms = round((time.perf_counter() - started) * 1000.0, 3)
    return {
        "command": [str(token) for token in command],
        "exit_code": completed.returncode,
        "duration_ms": duration_ms,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def sha256_digest(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()
