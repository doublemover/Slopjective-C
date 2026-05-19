"""Comparative baseline orchestration across catalog entries."""

from __future__ import annotations

import platform
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file as write_json
from objc3c_tooling.paths import repo_rel

from .catalog import BaselineRunConfig
from .execution import expand_command, run_timed_step
from .paths import ROOT
from .profiles import machine_profile
from .telemetry import record_unavailable_packet, sample_from_step, summarize_durations


@dataclass(frozen=True)
class BaselineRunResult:
    packet_paths: list[str]
    failures: list[str]


def benchmark_baseline(
    entry: dict[str, Any],
    *,
    warmup_runs: int,
    measured_runs: int,
    profile: dict[str, Any],
    versions: dict[str, str],
    normalization_mode: str,
) -> tuple[list[str], list[str]]:
    packet_paths: list[str] = []
    failures: list[str] = []
    baseline_id = str(entry["baseline_id"])
    language = str(entry["language"])
    source = str(entry["source"])
    output_exe = str((ROOT / "tmp" / "artifacts" / "performance" / "baselines" / f"{baseline_id}.exe").resolve())
    Path(output_exe).parent.mkdir(parents=True, exist_ok=True)

    probe = entry["availability_probe"]
    probe_command = expand_command(
        tool=str(probe["tool"]),
        args=list(probe.get("args", [])),
        source=source,
        output_exe=output_exe,
    )
    probe_step = run_timed_step(probe_command)
    versions[f"{baseline_id}_probe"] = str(probe_step["stdout"]).splitlines()[0] if probe_step["stdout"] else ""

    compile_packet_path = ROOT / "tmp" / "reports" / "performance" / "baselines" / "compile" / f"{baseline_id}.json"
    runtime_packet_path = ROOT / "tmp" / "reports" / "performance" / "baselines" / "runtime" / f"{baseline_id}.json"

    if probe_step["exit_code"] != 0:
        record_unavailable_packet(
            baseline_id=baseline_id,
            language=language,
            benchmark_kind="comparative-compile-baseline",
            probe_step=probe_step,
            profile=profile,
            versions=versions,
            out_path=compile_packet_path,
            normalization_mode=normalization_mode,
        )
        packet_paths.append(repo_rel(compile_packet_path))
        return packet_paths, failures

    compile_spec = entry["compile_invocation"]
    compile_command = expand_command(
        tool=str(compile_spec["tool"]),
        args=list(compile_spec["args"]),
        source=source,
        output_exe=output_exe,
    )

    for _ in range(warmup_runs):
        run_timed_step(compile_command)

    compile_raw_samples: list[dict[str, Any]] = []
    compile_durations: list[float] = []
    for sample_index in range(measured_runs):
        step = run_timed_step(compile_command)
        if step["exit_code"] != 0:
            failures.append(f"{baseline_id} compile sample failed")
        compile_raw_samples.append(
            sample_from_step(
                baseline_id=baseline_id,
                language=language,
                sample_id=f"{baseline_id}-compile-{sample_index + 1}",
                step_kind="compile",
                step=step,
            )
        )
        compile_durations.append(float(step["duration_ms"]))

    write_json(
        compile_packet_path,
        {
            "contract_id": "objc3c.performance.telemetry.v1",
            "schema_version": 1,
            "benchmark_kind": "comparative-compile-baseline",
            "generated_at_utc": datetime.now(timezone.utc).isoformat(),
            "machine_profile": profile,
            "tool_versions": versions,
            "raw_samples": compile_raw_samples,
            "normalized_summary": summarize_durations(
                compile_durations,
                normalization_mode,
                "local-measurement",
            ),
            "workload_id": baseline_id,
            "source_path": source,
            "ok": not failures,
            "failures": failures,
        },
    )
    packet_paths.append(repo_rel(compile_packet_path))

    if not bool(entry.get("supports_runtime_baseline")) or not entry.get("runtime_invocation"):
        return packet_paths, failures

    runtime_spec = entry["runtime_invocation"]
    runtime_command = expand_command(
        tool=str(runtime_spec["tool"]),
        args=list(runtime_spec.get("args", [])),
        source=source,
        output_exe=output_exe,
    )
    for _ in range(warmup_runs):
        run_timed_step(runtime_command)

    runtime_raw_samples: list[dict[str, Any]] = []
    runtime_durations: list[float] = []
    for sample_index in range(measured_runs):
        step = run_timed_step(runtime_command)
        runtime_raw_samples.append(
            sample_from_step(
                baseline_id=baseline_id,
                language=language,
                sample_id=f"{baseline_id}-runtime-{sample_index + 1}",
                step_kind="runtime",
                step=step,
            )
        )
        runtime_durations.append(float(step["duration_ms"]))

    write_json(
        runtime_packet_path,
        {
            "contract_id": "objc3c.performance.telemetry.v1",
            "schema_version": 1,
            "benchmark_kind": "comparative-runtime-baseline",
            "generated_at_utc": datetime.now(timezone.utc).isoformat(),
            "machine_profile": profile,
            "tool_versions": versions,
            "raw_samples": runtime_raw_samples,
            "normalized_summary": summarize_durations(
                runtime_durations,
                normalization_mode,
                "local-measurement",
            ),
            "workload_id": baseline_id,
            "source_path": source,
            "ok": not failures,
            "failures": failures,
        },
    )
    packet_paths.append(repo_rel(runtime_packet_path))
    return packet_paths, failures


def run_comparative_baselines(config: BaselineRunConfig) -> BaselineRunResult:
    profile = machine_profile()
    versions: dict[str, str] = {"python": platform.python_version()}
    packet_paths: list[str] = []
    failures: list[str] = []
    for entry in config.entries:
        entry_packet_paths, entry_failures = benchmark_baseline(
            entry,
            warmup_runs=config.warmup_runs,
            measured_runs=config.measured_runs,
            profile=profile,
            versions=dict(versions),
            normalization_mode=config.normalization_mode,
        )
        packet_paths.extend(entry_packet_paths)
        failures.extend(entry_failures)
    return BaselineRunResult(packet_paths=packet_paths, failures=failures)
