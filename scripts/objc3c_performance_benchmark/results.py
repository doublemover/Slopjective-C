from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Callable, Sequence

from scripts.objc3c_workflow.public_command_api import public_workflow_command

from objc3c_performance_benchmark.comparison import summarize_durations
from objc3c_performance_benchmark.execution import run_timed_step, sha256_digest
from objc3c_performance_benchmark.paths import ROOT


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def benchmark_compile_workload(
    workload: dict[str, Any],
    *,
    warmup_runs: int,
    measured_runs: int,
    compile_root: Path,
    profile: dict[str, Any],
    versions: dict[str, str],
    normalization_mode: str,
    write_json_fn: Callable[[Path, dict[str, Any]], None],
    root: Path = ROOT,
    run_timed_step_fn: Callable[[Sequence[str]], dict[str, Any]] = run_timed_step,
) -> tuple[Path, list[str]]:
    failures: list[str] = []
    workload_id = str(workload["workload_id"])
    source = str(workload["source"])

    for warmup_index in range(warmup_runs):
        out_dir = compile_root / workload_id / f"warmup-{warmup_index + 1}"
        step = run_timed_step_fn(
            public_workflow_command(
                "compile-objc3c",
                source,
                "--out-dir",
                out_dir.relative_to(root).as_posix(),
                "--emit-prefix",
                "module",
            )
        )
        expect(step["exit_code"] == 0, f"{workload_id} compile warmup failed", failures)

    raw_samples: list[dict[str, Any]] = []
    durations: list[float] = []
    for sample_index in range(measured_runs):
        out_dir = compile_root / workload_id / f"sample-{sample_index + 1}"
        step = run_timed_step_fn(
            public_workflow_command(
                "compile-objc3c",
                source,
                "--out-dir",
                out_dir.relative_to(root).as_posix(),
                "--emit-prefix",
                "module",
            )
        )
        expect(step["exit_code"] == 0, f"{workload_id} compile sample failed", failures)
        raw_samples.append(
            {
                "sample_id": f"{workload_id}-compile-{sample_index + 1}",
                "workload_id": workload_id,
                "language": "objective-c-3",
                "step_kind": "compile",
                "command": step["command"],
                "duration_ms": step["duration_ms"],
                "exit_code": step["exit_code"],
                "stdout_digest": sha256_digest(str(step["stdout"])),
                "stderr_digest": sha256_digest(str(step["stderr"])),
            }
        )
        durations.append(float(step["duration_ms"]))

    packet = {
        "contract_id": "objc3c.performance.telemetry.v1",
        "schema_version": 1,
        "benchmark_kind": "compile-latency",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "machine_profile": profile,
        "tool_versions": versions,
        "raw_samples": raw_samples,
        "normalized_summary": summarize_durations(durations, normalization_mode),
        "workload_id": workload_id,
        "source_path": source,
        "failures": failures,
        "ok": not failures,
    }
    packet_path = root / "tmp" / "reports" / "performance" / "compile" / f"{workload_id}.json"
    write_json_fn(packet_path, packet)
    return packet_path, failures


def benchmark_runtime_workload(
    workload: dict[str, Any],
    *,
    warmup_runs: int,
    measured_runs: int,
    profile: dict[str, Any],
    versions: dict[str, str],
    normalization_mode: str,
    write_json_fn: Callable[[Path, dict[str, Any]], None],
    root: Path = ROOT,
    run_timed_step_fn: Callable[[Sequence[str]], dict[str, Any]] = run_timed_step,
) -> tuple[Path, list[str]]:
    failures: list[str] = []
    workload_id = str(workload["workload_id"])

    for _ in range(warmup_runs):
        step = run_timed_step_fn(
            public_workflow_command(
                "validate-showcase-runtime",
                "--example",
                workload_id,
            )
        )
        expect(step["exit_code"] == 0, f"{workload_id} runtime warmup failed", failures)

    raw_samples: list[dict[str, Any]] = []
    durations: list[float] = []
    for sample_index in range(measured_runs):
        step = run_timed_step_fn(
            public_workflow_command(
                "validate-showcase-runtime",
                "--example",
                workload_id,
            )
        )
        expect(step["exit_code"] == 0, f"{workload_id} runtime sample failed", failures)
        raw_samples.append(
            {
                "sample_id": f"{workload_id}-runtime-{sample_index + 1}",
                "workload_id": workload_id,
                "language": "objective-c-3",
                "step_kind": "runtime",
                "command": step["command"],
                "duration_ms": step["duration_ms"],
                "exit_code": step["exit_code"],
                "stdout_digest": sha256_digest(str(step["stdout"])),
                "stderr_digest": sha256_digest(str(step["stderr"])),
            }
        )
        durations.append(float(step["duration_ms"]))

    packet = {
        "contract_id": "objc3c.performance.telemetry.v1",
        "schema_version": 1,
        "benchmark_kind": "runtime-wall-clock",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "machine_profile": profile,
        "tool_versions": versions,
        "raw_samples": raw_samples,
        "normalized_summary": summarize_durations(durations, normalization_mode),
        "workload_id": workload_id,
        "source_path": str(workload["source"]),
        "failures": failures,
        "ok": not failures,
    }
    packet_path = root / "tmp" / "reports" / "performance" / "runtime" / f"{workload_id}.json"
    write_json_fn(packet_path, packet)
    return packet_path, failures
