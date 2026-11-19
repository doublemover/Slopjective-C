"""Compiler execution for individual objc3c fuzz-safety cases."""

from __future__ import annotations

import hashlib
import re
import subprocess
from pathlib import Path

from objc3c_tooling.paths import display_path
from objc3c_toolchain.diagnostic_termination import ABNORMAL_DIAGNOSTIC

from .corpus import CorpusCase, normalize_text

DIAGNOSTIC_SIGNAL_PATTERN = re.compile(
    r"(?:^|\n)(?:[^:\n]+:\d+(?::\d+)?:\s*)?(?:fatal error|error):"
    r"|\bO3[A-Z]\d{3}\b",
    re.IGNORECASE,
)
INFRASTRUCTURE_DIAGNOSTIC_PATTERN = re.compile(
    r"(?:^|\n)Traceback \(most recent call last\):"
    r"|\b(?:unknown|unrecognized|unsupported) (?:argument|option)\b"
    r"|\b(?:no input files|no such file or directory)\b"
    r"|\berror while loading shared libraries\b"
    r"|\bsymbol lookup error\b"
    r"|\b(?:cannot|unable to) (?:load|open) (?:shared |dynamic )?(?:library|object)\b"
    r"|\bterminate called after throwing\b",
    re.IGNORECASE,
)


def classify_termination(exit_code: object, timed_out: object) -> str:
    if timed_out is True:
        return "timeout"
    if type(exit_code) is not int:
        return "missing-status"
    if exit_code < 0:
        return "signal"
    if exit_code == 1:
        return "diagnostic-exit"
    return "unexpected-exit"


def classify_diagnostic(transcript: str) -> str:
    if ABNORMAL_DIAGNOSTIC.search(transcript):
        return "abnormal-runtime"
    if INFRASTRUCTURE_DIAGNOSTIC_PATTERN.search(transcript):
        return "infrastructure-error"
    if DIAGNOSTIC_SIGNAL_PATTERN.search(transcript):
        return "ordinary-source-diagnostic"
    return "missing-source-diagnostic"


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
    run1_termination = classify_termination(run1_exit, run1["timed_out"])
    run2_termination = classify_termination(run2_exit, run2["timed_out"])
    run1_diagnostic = classify_diagnostic(run1_output)
    run2_diagnostic = classify_diagnostic(run2_output)

    for iteration, exit_code, termination, diagnostic in (
        (1, run1_exit, run1_termination, run1_diagnostic),
        (2, run2_exit, run2_termination, run2_diagnostic),
    ):
        if termination != "diagnostic-exit":
            unsafe_errors.append(
                {
                    "check_id": "SAFE-001",
                    "detail": (
                        f"run{iteration} expected ordinary diagnostic exit 1, "
                        f"got {exit_code} ({termination})"
                    ),
                }
            )
        if termination == "signal":
            unsafe_errors.append(
                {
                    "check_id": "SAFE-004",
                    "detail": f"run{iteration} terminated by signal ({exit_code})",
                }
            )
        if diagnostic in ("abnormal-runtime", "infrastructure-error"):
            unsafe_errors.append(
                {
                    "check_id": "SAFE-005",
                    "detail": f"run{iteration} reported {diagnostic}",
                }
            )
        elif termination == "diagnostic-exit" and diagnostic != "ordinary-source-diagnostic":
            unsafe_errors.append(
                {
                    "check_id": "SAFE-002",
                    "detail": f"run{iteration} missing an ordinary source diagnostic",
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

    checks = {
        "diagnostic_exit": not any(item["check_id"] == "SAFE-001" for item in unsafe_errors),
        "ordinary_source_diagnostic": not any(
            item["check_id"] == "SAFE-002" for item in unsafe_errors
        ),
        "timeout_free": not any(item["check_id"] == "SAFE-003" for item in unsafe_errors),
        "no_crash_signal": not any(item["check_id"] == "SAFE-004" for item in unsafe_errors),
        "no_abnormal_diagnostic": not any(
            item["check_id"] == "SAFE-005" for item in unsafe_errors
        ),
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
        "run1_signal": -run1_exit if type(run1_exit) is int and run1_exit < 0 else None,
        "run2_signal": -run2_exit if type(run2_exit) is int and run2_exit < 0 else None,
        "run1_timed_out": run1["timed_out"],
        "run2_timed_out": run2["timed_out"],
        "run1_termination": run1_termination,
        "run2_termination": run2_termination,
        "run1_diagnostic_class": run1_diagnostic,
        "run2_diagnostic_class": run2_diagnostic,
        "run1_output_sha256": run1["output_sha256"],
        "run2_output_sha256": run2["output_sha256"],
        "checks": checks,
        "errors": sorted(
            unsafe_errors,
            key=lambda item: (item["check_id"], item["detail"]),
        ),
    }
