"""Progress reporting for runtime acceptance runs."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from typing import Any


ROOT = Path(__file__).resolve().parents[2]


def round_seconds(seconds: float) -> float:
    return round(seconds, 6)


def format_seconds(seconds: float) -> str:
    return f"{seconds:.3f}s"


def repo_display_path(path: Path) -> str:
    if str(path) == "":
        return ""
    try:
        return str(path.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(path).replace("\\", "/")


def command_display(command: list[str]) -> str:
    display_parts: list[str] = []
    for token in command:
        token_path = Path(token)
        if token_path.is_absolute():
            display_parts.append(repo_display_path(token_path))
        else:
            display_parts.append(token)
    rendered = " ".join(display_parts)
    if len(rendered) > 240:
        return rendered[:237] + "..."
    return rendered


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
        return {
            "status": "RUNNING",
            "run_id": self.run_id,
            "run_dir": repo_display_path(self.run_dir),
            "progress_path": repo_display_path(self.progress_path),
            "elapsed_seconds": self.elapsed_seconds(),
            "total_case_count": self.total_cases,
            "completed_case_count": len(self.case_timings),
            "current_case": self.current_case,
            "current_command": self.current_command,
            "last_completed_case": self.last_completed_case,
            "case_timings": self.case_timings,
            "command_timings": self.command_timings,
            "progress_report_write_overhead": {
                "contract_id": "objc3c.runtime.acceptance.progress.write.overhead.v1",
                "write_count": self.progress_write_count,
                "total_seconds": round_seconds(self.progress_write_seconds),
                "max_seconds": round_seconds(self.progress_write_max_seconds),
                "overhead_percent_of_elapsed": round_seconds(
                    (
                        self.progress_write_seconds
                        / max(perf_counter() - self.started_at, 0.000001)
                    )
                    * 100.0
                ),
                "model": (
                    "console progress stays immediate; progress.json remains the "
                    "durable current-state snapshot while report-write overhead is measured"
                ),
            },
            "slowest_cases": sorted(
                self.case_timings,
                key=lambda entry: float(entry.get("duration_seconds", 0.0)),
                reverse=True,
            )[:10],
            "slowest_commands": sorted(
                self.command_timings,
                key=lambda entry: float(entry.get("duration_seconds", 0.0)),
                reverse=True,
            )[:10],
        }

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
        summary = self.snapshot()
        summary["status"] = "PASS"
        summary["current_case"] = None
        summary["current_command"] = None
        return summary


ACCEPTANCE_PROGRESS: RuntimeAcceptanceProgress | None = None


def get_acceptance_progress() -> RuntimeAcceptanceProgress | None:
    return ACCEPTANCE_PROGRESS


def set_acceptance_progress(progress: RuntimeAcceptanceProgress | None) -> None:
    global ACCEPTANCE_PROGRESS
    ACCEPTANCE_PROGRESS = progress
