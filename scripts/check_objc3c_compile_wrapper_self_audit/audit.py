"""Execution pipeline for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from time import perf_counter
from typing import Any

from .config import FIXTURE, REPORT_ROOT, ROOT, RUN_ROOT, SELF_AUDIT_CONTRACT_ID, WRAPPER
from .contracts import wrapper_truth_owner_contract
from .environment import find_pwsh
from .expectations import expect, validate_compile_output
from .paths import repo_display_path


def build_command(shell: str, out_dir: Path) -> list[str]:
    return [
        shell,
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(WRAPPER),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
    ]


def run_compile_wrapper(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=str(ROOT),
        capture_output=True,
        text=True,
        check=False,
    )


def build_payload(
    status: str,
    run_id: str,
    elapsed: float,
    out_dir: Path,
    command: list[str],
    result: subprocess.CompletedProcess[str] | None,
    validation: dict[str, Any],
    error: str | None,
) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "status": status,
        "contract_id": SELF_AUDIT_CONTRACT_ID,
        "owner_contract": wrapper_truth_owner_contract(),
        "run_id": run_id,
        "elapsed_seconds": elapsed,
        "wrapper": repo_display_path(WRAPPER),
        "fixture": repo_display_path(FIXTURE),
        "out_dir": repo_display_path(out_dir),
        "command": command,
        "exit_code": result.returncode if result is not None else None,
        "validation": validation,
        "audit_model": (
            "one wrapper compile validates invariant compile-output provenance, "
            "truthfulness, registration-manifest digest binding, and required "
            "artifact publication; downstream direct-native compiles can "
            "then avoid relaunching the PowerShell wrapper per fixture"
        ),
    }
    if error is not None:
        payload["error"] = error
    return payload


def write_report(payload: dict[str, Any], report_path: Path) -> None:
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def run_self_audit() -> tuple[dict[str, Any], Path]:
    started_at = perf_counter()
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    out_dir = RUN_ROOT / run_id / "compile"
    report_path = REPORT_ROOT / "summary.json"
    out_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    status = "FAIL"
    error: str | None = None
    command: list[str] = []
    validation: dict[str, Any] = {}
    result: subprocess.CompletedProcess[str] | None = None
    try:
        shell = find_pwsh()
        command = build_command(shell, out_dir)
        print(
            "compile-wrapper-self-audit: START "
            f"fixture={repo_display_path(FIXTURE)} out_dir={repo_display_path(out_dir)}",
            flush=True,
        )
        result = run_compile_wrapper(command)
        expect(
            result.returncode == 0,
            "compile wrapper exited non-zero:\nSTDOUT:\n"
            + result.stdout
            + "\nSTDERR:\n"
            + result.stderr,
        )
        validation = validate_compile_output(out_dir)
        status = "PASS"
    except Exception as exc:
        error = str(exc)

    elapsed = round(perf_counter() - started_at, 6)
    payload = build_payload(
        status=status,
        run_id=run_id,
        elapsed=elapsed,
        out_dir=out_dir,
        command=command,
        result=result,
        validation=validation,
        error=error,
    )
    write_report(payload, report_path)
    return payload, report_path


def main() -> int:
    payload, report_path = run_self_audit()
    elapsed = payload["elapsed_seconds"]
    status = payload["status"]
    error = payload.get("error")
    print(f"summary_path: {repo_display_path(report_path)}")
    print(f"compile-wrapper-self-audit-report: {repo_display_path(report_path)}")
    if status == "PASS":
        print(f"compile-wrapper-self-audit: PASS elapsed={elapsed:.3f}s")
        return 0
    print(f"compile-wrapper-self-audit: FAIL elapsed={elapsed:.3f}s", file=sys.stderr)
    if error:
        print(error, file=sys.stderr)
    return 1


__all__ = [
    "build_command",
    "build_payload",
    "main",
    "run_compile_wrapper",
    "run_self_audit",
    "write_report",
]
