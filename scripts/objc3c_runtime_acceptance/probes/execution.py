"""Runtime probe process execution and retry semantics."""

from __future__ import annotations

import subprocess
from pathlib import Path

from objc3c_runtime_acceptance.process_execution import run
from objc3c_runtime_acceptance.progress_format import repo_display_path
from objc3c_runtime_acceptance.progress_state import get_acceptance_progress

from .retry import DEFAULT_PROBE_RETRIES
from .retry import RETRYABLE_PROBE_EXIT_CODES
from .retry import record_successful_probe_retry


def run_probe(
    exe_path: Path, *, env: dict[str, str] | None = None
) -> subprocess.CompletedProcess[str]:
    attempts: list[dict[str, object]] = []
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
                record_successful_probe_retry(
                    probe=exe_path,
                    retry_count=attempt,
                    attempts=attempts,
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


__all__ = ["run_probe"]
