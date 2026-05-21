"""Line-deletion minimization loop for stable stress failures."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from .execution import compile_source, run_runtime_or_execution_source
from .models import MinCase


RUNTIME_EXECUTION_SUBSYSTEMS = {"runtime", "execution"}


def _evaluate_source(
    compiler: Path,
    case: MinCase,
    source_text: str,
    work_dir: Path,
    timeout_sec: float,
) -> dict[str, Any]:
    if case.subsystem in RUNTIME_EXECUTION_SUBSYSTEMS:
        return run_runtime_or_execution_source(
            compiler,
            case.source_path,
            source_text,
            work_dir,
            timeout_sec,
        )
    return compile_source(compiler, source_text, work_dir, timeout_sec)


def reduce_source(
    compiler: Path,
    case: MinCase,
    original_source: str,
    baseline_signature: str,
    reduced_dir: Path,
    timeout_sec: float,
) -> tuple[str, list[dict[str, Any]]]:
    candidate = original_source
    attempts: list[dict[str, Any]] = []
    changed = True
    while changed:
        changed = False
        lines = candidate.splitlines(keepends=True)
        if len(lines) <= 1:
            break
        for index in range(len(lines)):
            trial_lines = lines[:index] + lines[index + 1 :]
            trial_source = "".join(trial_lines)
            if not trial_source.strip():
                continue
            attempt_dir = reduced_dir / "attempts" / f"{len(attempts):03d}"
            result = _evaluate_source(compiler, case, trial_source, attempt_dir, timeout_sec)
            accepted = result["signature_sha256"] == baseline_signature and len(trial_source) < len(candidate)
            attempts.append(
                {
                    "attempt_index": len(attempts),
                    "removed_line_index": index,
                    "accepted": accepted,
                    "signature_sha256": result["signature_sha256"],
                    "returncode": result["returncode"],
                    "failure_stage": result.get("failure_stage", "compile"),
                    "diagnostic_line_count": len(result["diagnostic_lines"]),
                }
            )
            if accepted:
                candidate = trial_source
                changed = True
                break
    return candidate, attempts


__all__ = ["reduce_source"]
