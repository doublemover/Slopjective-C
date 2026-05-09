"""Durable progress state for runtime acceptance runs."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from typing import Any

from .progress_format import command_display
from .progress_format import format_seconds
from .progress_format import repo_display_path
from .progress_format import round_seconds
from .progress_snapshots import build_final_progress_summary
from .progress_snapshots import build_progress_snapshot


class RuntimeAcceptanceProgress:
    def __init__(self, *, run_id: str, run_dir: Path, progress_path: Path, total_cases: int) -> None:
        self.run_id = run_id
        self.run_dir = run_dir
        self.progress_path = progress_path
        self.total_cases = total_cases
        self.started_at = perf_counter()
        self.current_case: dict[str, Any] | None = None
        self.current_command: dict[str, Any] | None = None
        self.last_completed_case: dict[str, Any] | None = None
        self.case_timings: list[dict[str, Any]] = []
        self.command_timings: list[dict[str, Any]] = []
        self._command_sequence = 0
        self.progress_write_count = 0
        self.progress_write_seconds = 0.0
        self.progress_write_max_seconds = 0.0

    def elapsed_seconds(self) -> float:
        return round_seconds(perf_counter() - self.started_at)

    def emit(self, message: str) -> None:
        print(f"runtime-acceptance-progress: {message}", flush=True)
        self.write_progress()

    def snapshot(self) -> dict[str, Any]:
        return build_progress_snapshot(
            run_id=self.run_id,
            run_dir=self.run_dir,
            progress_path=self.progress_path,
            elapsed_seconds=self.elapsed_seconds(),
            total_cases=self.total_cases,
            current_case=self.current_case,
            current_command=self.current_command,
            last_completed_case=self.last_completed_case,
            case_timings=self.case_timings,
            command_timings=self.command_timings,
            progress_write_count=self.progress_write_count,
            progress_write_seconds=self.progress_write_seconds,
            progress_write_max_seconds=self.progress_write_max_seconds,
        )

    def write_progress(self) -> None:
        self.progress_path.parent.mkdir(parents=True, exist_ok=True)
        started_at = perf_counter()
        self.progress_path.write_text(
            json.dumps(self.snapshot(), indent=2) + "\n",
            encoding="utf-8",
        )
        duration = perf_counter() - started_at
        self.progress_write_count += 1
        self.progress_write_seconds += duration
        self.progress_write_max_seconds = max(
            self.progress_write_max_seconds,
            duration,
        )

    def start_case(self, *, index: int, label: str) -> float:
        start = perf_counter()
        previous = self.last_completed_case["case_id"] if self.last_completed_case else "none"
        self.current_case = {
            "index": index,
            "total": self.total_cases,
            "label": label,
            "started_after_seconds": self.elapsed_seconds(),
            "last_completed_case_id": previous,
        }
        self.current_command = None
        self.emit(
            f"[{index}/{self.total_cases}] START case={label} elapsed={format_seconds(self.elapsed_seconds())} last={previous}"
        )
        return start

    def finish_case(self, *, index: int, label: str, result: Any, started_at: float) -> None:
        duration = round_seconds(perf_counter() - started_at)
        entry = {
            "index": index,
            "total": self.total_cases,
            "label": label,
            "case_id": result.case_id,
            "probe": result.probe,
            "fixture": result.fixture,
            "claim_class": result.claim_class,
            "passed": result.passed,
            "duration_seconds": duration,
            "elapsed_seconds": self.elapsed_seconds(),
        }
        self.case_timings.append(entry)
        self.last_completed_case = entry
        self.current_case = None
        self.current_command = None
        self.emit(
            f"[{index}/{self.total_cases}] DONE case={result.case_id} duration={format_seconds(duration)} elapsed={format_seconds(self.elapsed_seconds())}"
        )

    def fail_case(self, *, index: int, label: str, started_at: float, error: BaseException) -> None:
        duration = round_seconds(perf_counter() - started_at)
        self.current_case = {
            "index": index,
            "total": self.total_cases,
            "label": label,
            "failed_after_seconds": duration,
            "error": str(error),
        }
        self.emit(
            f"[{index}/{self.total_cases}] FAIL case={label} duration={format_seconds(duration)} error={error}"
        )

    def start_command(self, command: list[str], cwd: Path) -> float:
        self._command_sequence += 1
        start = perf_counter()
        self.current_command = {
            "sequence": self._command_sequence,
            "case_label": self.current_case.get("label") if self.current_case else None,
            "case_index": self.current_case.get("index") if self.current_case else None,
            "cwd": repo_display_path(cwd),
            "command": command_display(command),
            "started_after_seconds": self.elapsed_seconds(),
        }
        self.emit(
            f"COMMAND start seq={self._command_sequence} case={self.current_command.get('case_label')} command={self.current_command['command']}"
        )
        return start

    def finish_command(
        self,
        *,
        command: list[str],
        cwd: Path,
        started_at: float,
        returncode: int,
    ) -> None:
        duration = round_seconds(perf_counter() - started_at)
        entry = {
            "sequence": self._command_sequence,
            "case_label": self.current_case.get("label") if self.current_case else None,
            "case_index": self.current_case.get("index") if self.current_case else None,
            "cwd": repo_display_path(cwd),
            "command": command_display(command),
            "returncode": returncode,
            "duration_seconds": duration,
            "elapsed_seconds": self.elapsed_seconds(),
        }
        self.command_timings.append(entry)
        self.current_command = None
        self.emit(
            f"COMMAND done seq={entry['sequence']} rc={returncode} duration={format_seconds(duration)}"
        )

    def final_summary(self) -> dict[str, Any]:
        return build_final_progress_summary(self.snapshot())


ACCEPTANCE_PROGRESS: RuntimeAcceptanceProgress | None = None


def get_acceptance_progress() -> RuntimeAcceptanceProgress | None:
    return ACCEPTANCE_PROGRESS


def set_acceptance_progress(progress: RuntimeAcceptanceProgress | None) -> None:
    global ACCEPTANCE_PROGRESS
    ACCEPTANCE_PROGRESS = progress


__all__ = [
    "ACCEPTANCE_PROGRESS",
    "RuntimeAcceptanceProgress",
    "get_acceptance_progress",
    "set_acceptance_progress",
]
