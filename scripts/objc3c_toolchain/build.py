"""Execute identified foundation plans and retain command timings and logs."""
from __future__ import annotations

from datetime import datetime, timezone
from contextlib import contextmanager
import json
import os
from pathlib import Path
import subprocess
import time

from .core import ToolchainError, build_plans, prepare, seal_development


@contextmanager
def foundation_lock(repo: Path):
    path = repo / "out/toolchains/objc3-development/.foundation-build.lock"
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a+b") as lock:
        lock.seek(0)
        if os.name == "nt":
            import msvcrt
            lock.write(b"0")
            lock.flush()
            lock.seek(0)
            acquire = lambda: msvcrt.locking(lock.fileno(), msvcrt.LK_NBLCK, 1)
            release = lambda: msvcrt.locking(lock.fileno(), msvcrt.LK_UNLCK, 1)
        else:
            import fcntl
            acquire = lambda: fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            release = lambda: fcntl.flock(lock, fcntl.LOCK_UN)
        try:
            acquire()
        except OSError as error:
            raise ToolchainError(f"a foundation build already holds {path}; preserving its outputs") from error
        try:
            yield
        finally:
            lock.seek(0)
            release()


def build_foundation(repo: Path, **options) -> dict:
    with foundation_lock(repo.resolve()):
        return _build_foundation(repo, **options)


def _build_foundation(repo: Path, *, host: str | None = None, fetch: bool = False,
                     stage: bool = False) -> dict:
    repo = repo.resolve()
    source = repo / "out/toolchains/objc3-development/src"
    if not source.exists():
        prepare(repo, source_dir=source, fetch=fetch)
        seal_development(repo, source_dir=source)
    plans = build_plans(repo, source_dir=source, host=host)
    report = {"status": "running", "source_identity": plans["configure"]["source_build_id"], "phases": [],
              "started_utc": datetime.now(timezone.utc).isoformat()}
    started = time.monotonic()
    report_path = None
    history_path = None
    run_name = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S.%fZ")
    try:
        for phase in ("configure", "build", "test", *(('stage',) if stage else ())):
            plan = plans[phase]
            directory = Path(plan["paths"]["build"])
            directory.mkdir(parents=True, exist_ok=True)
            report_path = directory / "foundation-build-report.json"
            run_directory = directory / "foundation-runs" / run_name
            run_directory.mkdir(parents=True, exist_ok=True)
            history_path = run_directory / "report.json"
            report["run_directory"] = str(run_directory)
            environment = os.environ.copy()
            environment.update(plan.get("command_environment", {}))
            environment["PYTHONDONTWRITEBYTECODE"] = "1"
            environment["CCACHE_COMPILERCHECK"] = "content"
            for index, command in enumerate([plan["command"], *plan.get("follow_up_commands", [])]):
                log_path = run_directory / f"{phase}-{index}.log"
                phase_started = time.monotonic()
                print(f"foundation {phase} [{index + 1}]: {log_path}", flush=True)
                with log_path.open("w") as log:
                    result = subprocess.run(command, cwd=repo, env=environment,
                                            stdout=log, stderr=subprocess.STDOUT)
                report["phases"].append({"phase": phase, "plan_identity": plan["identity"],
                    "command": command, "exit_code": result.returncode, "log": str(log_path),
                    "elapsed_seconds": round(time.monotonic() - phase_started, 6)})
                history_path.write_text(json.dumps(report, indent=2) + "\n")
                report_path.write_text(json.dumps(report, indent=2) + "\n")
                if result.returncode:
                    raise ToolchainError(f"foundation {phase} exited {result.returncode}; see {log_path}")
        refreshed = build_plans(repo, source_dir=source, host=host)["configure"]
        for key in ("source_build_id", "source_tree", "build_config_digest", "host_environment"):
            if refreshed[key] != plans["configure"][key]:
                raise ToolchainError(f"foundation {key} changed during the build")
        report["status"] = "passed"
    except (ToolchainError, OSError) as error:
        report.update(status="failed", error=str(error))
        raise
    finally:
        report.update(elapsed_seconds=round(time.monotonic() - started, 6),
                      finished_utc=datetime.now(timezone.utc).isoformat())
        if history_path is not None:
            history_path.write_text(json.dumps(report, indent=2) + "\n")
        if report_path is not None:
            report_path.write_text(json.dumps(report, indent=2) + "\n")
    return report
