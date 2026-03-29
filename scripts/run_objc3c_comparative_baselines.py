#!/usr/bin/env python3
"""Run the checked-in ObjC2, Swift, and C++ comparative baseline workloads."""

from __future__ import annotations

import argparse
import json
import os
import platform
import socket
import statistics
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "comparative_baseline_manifest.json"
MEASUREMENT_POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "measurement_policy.json"
BENCHMARK_PARAMETERS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_parameters.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "performance" / "comparative-baselines-summary.json"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--warmup-runs", type=int, default=None)
    parser.add_argument("--measured-runs", type=int, default=None)
    return parser.parse_args(argv)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {path}")
    return payload


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def display_path(path: Path) -> str:
    try:
        return repo_rel(path.resolve())
    except ValueError:
        return path.resolve().as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=env,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def run_timed_step(command: Sequence[str]) -> dict[str, Any]:
    started = time.perf_counter()
    completed = run_capture(command)
    duration_ms = round((time.perf_counter() - started) * 1000.0, 3)
    return {
        "command": [str(token) for token in command],
        "exit_code": completed.returncode,
        "duration_ms": duration_ms,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def sha256_digest(text: str) -> str:
    import hashlib

    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def machine_profile() -> dict[str, Any]:
    cpu_model = (
        os.environ.get("PROCESSOR_IDENTIFIER")
        or platform.processor()
        or os.environ.get("PROCESSOR_ARCHITECTURE", "")
    )
    return {
        "hostname": socket.gethostname(),
        "os": platform.platform(),
        "arch": platform.machine() or os.environ.get("PROCESSOR_ARCHITECTURE", ""),
        "cpu_model": cpu_model,
        "cpu_count": os.cpu_count() or 1,
        "python_version": platform.python_version(),
    }


def summarize_durations(durations: list[float], normalization_mode: str, availability_status: str) -> dict[str, Any]:
    return {
        "sample_count": len(durations),
        "min_duration_ms": min(durations),
        "median_duration_ms": statistics.median(durations),
        "max_duration_ms": max(durations),
        "normalization_mode": normalization_mode,
        "availability_status": availability_status,
    }


def expand_command(
    *,
    tool: str,
    args: Sequence[str],
    source: str,
    output_exe: str,
) -> list[str]:
    expanded = [tool.format(source=source, output_exe=output_exe)]
    expanded.extend(argument.format(source=source, output_exe=output_exe) for argument in args)
    return expanded


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def record_unavailable_packet(
    *,
    baseline_id: str,
    language: str,
    benchmark_kind: str,
    probe_step: dict[str, Any],
    profile: dict[str, Any],
    versions: dict[str, str],
    out_path: Path,
    normalization_mode: str,
) -> None:
    packet = {
        "contract_id": "objc3c.performance.telemetry.v1",
        "schema_version": 1,
        "benchmark_kind": benchmark_kind,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "machine_profile": profile,
        "tool_versions": versions,
        "raw_samples": [
            {
                "sample_id": f"{baseline_id}-availability",
                "workload_id": baseline_id,
                "language": language,
                "step_kind": "availability-probe",
                "command": probe_step["command"],
                "duration_ms": probe_step["duration_ms"],
                "exit_code": probe_step["exit_code"],
                "stdout_digest": sha256_digest(str(probe_step["stdout"])),
                "stderr_digest": sha256_digest(str(probe_step["stderr"])),
            }
        ],
        "normalized_summary": {
            "sample_count": 1,
            "min_duration_ms": float(probe_step["duration_ms"]),
            "median_duration_ms": float(probe_step["duration_ms"]),
            "max_duration_ms": float(probe_step["duration_ms"]),
            "normalization_mode": normalization_mode,
            "availability_status": "availability-limited",
        },
        "workload_id": baseline_id,
        "source_path": "",
        "ok": True,
        "failures": [],
    }
    write_json(out_path, packet)


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
            {
                "sample_id": f"{baseline_id}-compile-{sample_index + 1}",
                "workload_id": baseline_id,
                "language": language,
                "step_kind": "compile",
                "command": step["command"],
                "duration_ms": step["duration_ms"],
                "exit_code": step["exit_code"],
                "stdout_digest": sha256_digest(str(step["stdout"])),
                "stderr_digest": sha256_digest(str(step["stderr"])),
            }
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
            {
                "sample_id": f"{baseline_id}-runtime-{sample_index + 1}",
                "workload_id": baseline_id,
                "language": language,
                "step_kind": "runtime",
                "command": step["command"],
                "duration_ms": step["duration_ms"],
                "exit_code": step["exit_code"],
                "stdout_digest": sha256_digest(str(step["stdout"])),
                "stderr_digest": sha256_digest(str(step["stderr"])),
            }
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


def main() -> int:
    args = parse_args(sys.argv[1:])
    manifest = load_json(MANIFEST_PATH)
    policy = load_json(MEASUREMENT_POLICY_PATH)
    parameters = load_json(BENCHMARK_PARAMETERS_PATH)
    entries = manifest.get("baseline_entries", [])
    if not isinstance(entries, list) or not entries:
        raise RuntimeError("comparative baseline manifest did not publish baseline_entries")

    warmup_runs = int(args.warmup_runs if args.warmup_runs is not None else policy["sample_policy"]["warmup_runs"])
    measured_runs = int(
        args.measured_runs if args.measured_runs is not None else policy["sample_policy"]["measured_runs"]
    )
    normalization_mode = str(parameters["hardware_profile_capture"]["normalization_mode"])
    profile = machine_profile()
    versions: dict[str, str] = {"python": platform.python_version()}
    packet_paths: list[str] = []
    failures: list[str] = []
    for entry in entries:
        entry_packet_paths, entry_failures = benchmark_baseline(
            entry,
            warmup_runs=warmup_runs,
            measured_runs=measured_runs,
            profile=profile,
            versions=dict(versions),
            normalization_mode=normalization_mode,
        )
        packet_paths.extend(entry_packet_paths)
        failures.extend(entry_failures)

    payload = {
        "contract_id": "objc3c.performance.comparative.baselines.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/run_objc3c_comparative_baselines.py",
        "manifest_path": repo_rel(MANIFEST_PATH),
        "measurement_policy_path": repo_rel(MEASUREMENT_POLICY_PATH),
        "benchmark_parameters_path": repo_rel(BENCHMARK_PARAMETERS_PATH),
        "telemetry_packets": packet_paths,
        "failures": failures,
    }
    write_json(args.summary_out, payload)
    print(f"summary_path: {display_path(args.summary_out)}")
    if failures:
        print("objc3c-comparative-baselines: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-comparative-baselines: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
