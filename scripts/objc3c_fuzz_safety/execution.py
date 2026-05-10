"""Compiler execution for individual objc3c fuzz-safety cases."""

from __future__ import annotations

import hashlib
import re
import subprocess
from pathlib import Path

from objc3c_tooling.paths import display_path

from .corpus import CorpusCase, normalize_text

DIAGNOSTIC_SIGNAL_PATTERN = re.compile(r"(error|O3[A-Z]\d{3})", re.IGNORECASE)


def hash_output(stdout_text: str, stderr_text: str) -> str:
    payload = normalize_text(stdout_text) + "\n---stderr---\n" + normalize_text(stderr_text)
    return hashlib.sha256(payload.encode("utf-8")).hexdigest()


def run_case(
    *,
    case: CorpusCase,
    compiler_command: list[str],
    out_root: Path,
    timeout_sec: float,
) -> dict[str, object]:
    corpus_dir = out_root / "corpus"
    logs_dir = out_root / "logs"
    case_root = out_root / "cases" / case.case_id
    corpus_dir.mkdir(parents=True, exist_ok=True)
    logs_dir.mkdir(parents=True, exist_ok=True)
    case_root.mkdir(parents=True, exist_ok=True)

    source_path = corpus_dir / f"{case.case_id}.objc3"
    source_path.write_text(case.source, encoding="utf-8")

    outputs: list[dict[str, object]] = []
    unsafe_errors: list[dict[str, str]] = []

    for iteration in (1, 2):
        run_out_dir = case_root / f"run{iteration}"
        run_out_dir.mkdir(parents=True, exist_ok=True)
        command = [
            *compiler_command,
            str(source_path),
            "--out-dir",
            str(run_out_dir),
            "--emit-prefix",
            f"{case.case_id}_r{iteration}",
        ]

        try:
            completed = subprocess.run(
                command,
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                timeout=timeout_sec,
                check=False,
            )
            stdout_text = normalize_text(completed.stdout)
            stderr_text = normalize_text(completed.stderr)
            log_path = logs_dir / f"{case.case_id}.run{iteration}.log"
            log_path.write_text(
                f"command: {' '.join(command)}\n"
                f"exit_code: {completed.returncode}\n"
                "---stdout---\n"
                f"{stdout_text}\n"
                "---stderr---\n"
                f"{stderr_text}\n",
                encoding="utf-8",
            )
            outputs.append(
                {
                    "iteration": iteration,
                    "exit_code": completed.returncode,
                    "stdout": stdout_text,
                    "stderr": stderr_text,
                    "output_sha256": hash_output(stdout_text, stderr_text),
                    "timed_out": False,
                    "log_path": display_path(log_path),
                }
            )
        except subprocess.TimeoutExpired:
            timeout_log = logs_dir / f"{case.case_id}.run{iteration}.log"
            timeout_log.write_text(
                f"command: {' '.join(command)}\n"
                f"timeout_sec: {timeout_sec}\n"
                "status: TIMEOUT\n",
                encoding="utf-8",
            )
            outputs.append(
                {
                    "iteration": iteration,
                    "exit_code": None,
                    "stdout": "",
                    "stderr": "",
                    "output_sha256": "",
                    "timed_out": True,
                    "log_path": display_path(timeout_log),
                }
            )
            unsafe_errors.append(
                {
                    "check_id": "SAFE-003",
                    "detail": f"timeout at run{iteration} (> {timeout_sec:.3f}s)",
                }
            )

    run1 = outputs[0]
    run2 = outputs[1]

    run1_exit = run1["exit_code"]
    run2_exit = run2["exit_code"]
    run1_output = f"{run1['stdout']}\n{run1['stderr']}"
    run2_output = f"{run2['stdout']}\n{run2['stderr']}"

    if run1_exit is not None and run1_exit >= 0 and run1_exit == 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-001",
                "detail": "run1 returned exit code 0 for malformed corpus case",
            }
        )
    if run2_exit is not None and run2_exit >= 0 and run2_exit == 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-001",
                "detail": "run2 returned exit code 0 for malformed corpus case",
            }
        )
    if run1_exit is not None and run1_exit < 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-004",
                "detail": f"run1 terminated by signal ({run1_exit})",
            }
        )
    if run2_exit is not None and run2_exit < 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-004",
                "detail": f"run2 terminated by signal ({run2_exit})",
            }
        )

    if (
        run1_exit is not None
        and run2_exit is not None
        and run1_exit != run2_exit
    ):
        unsafe_errors.append(
            {
                "check_id": "DET-001",
                "detail": f"exit code drift run1={run1_exit} run2={run2_exit}",
            }
        )

    if (
        isinstance(run1["output_sha256"], str)
        and isinstance(run2["output_sha256"], str)
        and run1["output_sha256"]
        and run2["output_sha256"]
        and run1["output_sha256"] != run2["output_sha256"]
    ):
        unsafe_errors.append(
            {
                "check_id": "DET-002",
                "detail": "diagnostic output hash drift between run1 and run2",
            }
        )

    if (
        not run1["timed_out"]
        and run1_exit == 0
        and not DIAGNOSTIC_SIGNAL_PATTERN.search(run1_output)
    ):
        unsafe_errors.append(
            {
                "check_id": "SAFE-002",
                "detail": "run1 missing diagnostic signal token (error/O3*)",
            }
        )
    if (
        not run2["timed_out"]
        and run2_exit == 0
        and not DIAGNOSTIC_SIGNAL_PATTERN.search(run2_output)
    ):
        unsafe_errors.append(
            {
                "check_id": "SAFE-002",
                "detail": "run2 missing diagnostic signal token (error/O3*)",
            }
        )

    checks = {
        "nonzero_exit": not any(item["check_id"] == "SAFE-001" for item in unsafe_errors),
        "diagnostic_signal_present": not any(
            item["check_id"] == "SAFE-002" for item in unsafe_errors
        ),
        "timeout_free": not any(item["check_id"] == "SAFE-003" for item in unsafe_errors),
        "no_crash_signal": not any(item["check_id"] == "SAFE-004" for item in unsafe_errors),
        "deterministic_exit": not any(item["check_id"] == "DET-001" for item in unsafe_errors),
        "deterministic_output": not any(
            item["check_id"] == "DET-002" for item in unsafe_errors
        ),
    }

    return {
        "case_id": case.case_id,
        "subsystem": case.subsystem,
        "source_path": display_path(source_path),
        "run1_exit_code": run1_exit,
        "run2_exit_code": run2_exit,
        "run1_output_sha256": run1["output_sha256"],
        "run2_output_sha256": run2["output_sha256"],
        "checks": checks,
        "errors": sorted(
            unsafe_errors,
            key=lambda item: (item["check_id"], item["detail"]),
        ),
    }
