#!/usr/bin/env python3
"""Benchmark live objc3 workloads through the public compile and runtime paths."""

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
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
PORTFOLIO_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_portfolio.json"
MEASUREMENT_POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "measurement_policy.json"
BENCHMARK_PARAMETERS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_parameters.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "performance" / "benchmark-summary.json"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--warmup-runs", type=int, default=None)
    parser.add_argument("--measured-runs", type=int, default=None)
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {display_path(path)}")
    return payload


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


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


def first_line(text: str) -> str:
    return next((line.strip() for line in text.splitlines() if line.strip()), "")


def tool_versions() -> dict[str, str]:
    versions = {
        "python": platform.python_version(),
    }
    clang = run_capture(["clang", "--version"])
    if clang.returncode == 0:
        versions["clang"] = first_line(clang.stdout)
    return versions


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def summarize_durations(durations: list[float], normalization_mode: str) -> dict[str, Any]:
    return {
        "sample_count": len(durations),
        "min_duration_ms": min(durations),
        "median_duration_ms": statistics.median(durations),
        "max_duration_ms": max(durations),
        "normalization_mode": normalization_mode,
        "availability_status": "local-measurement",
    }


def benchmark_compile_workload(
    workload: dict[str, Any],
    *,
    warmup_runs: int,
    measured_runs: int,
    compile_root: Path,
    profile: dict[str, Any],
    versions: dict[str, str],
    normalization_mode: str,
) -> tuple[Path, list[str]]:
    failures: list[str] = []
    workload_id = str(workload["workload_id"])
    source = str(workload["source"])

    for warmup_index in range(warmup_runs):
        out_dir = compile_root / workload_id / f"warmup-{warmup_index + 1}"
        step = run_timed_step(
            [
                sys.executable,
                str(PUBLIC_RUNNER),
                "compile-objc3c",
                source,
                "--out-dir",
                out_dir.relative_to(ROOT).as_posix(),
                "--emit-prefix",
                "module",
            ]
        )
        expect(step["exit_code"] == 0, f"{workload_id} compile warmup failed", failures)

    raw_samples: list[dict[str, Any]] = []
    durations: list[float] = []
    for sample_index in range(measured_runs):
        out_dir = compile_root / workload_id / f"sample-{sample_index + 1}"
        step = run_timed_step(
            [
                sys.executable,
                str(PUBLIC_RUNNER),
                "compile-objc3c",
                source,
                "--out-dir",
                out_dir.relative_to(ROOT).as_posix(),
                "--emit-prefix",
                "module",
            ]
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
    packet_path = ROOT / "tmp" / "reports" / "performance" / "compile" / f"{workload_id}.json"
    write_json(packet_path, packet)
    return packet_path, failures


def benchmark_runtime_workload(
    workload: dict[str, Any],
    *,
    warmup_runs: int,
    measured_runs: int,
    profile: dict[str, Any],
    versions: dict[str, str],
    normalization_mode: str,
) -> tuple[Path, list[str]]:
    failures: list[str] = []
    workload_id = str(workload["workload_id"])

    for _ in range(warmup_runs):
        step = run_timed_step(
            [
                sys.executable,
                str(PUBLIC_RUNNER),
                "validate-showcase-runtime",
                "--example",
                workload_id,
            ]
        )
        expect(step["exit_code"] == 0, f"{workload_id} runtime warmup failed", failures)

    raw_samples: list[dict[str, Any]] = []
    durations: list[float] = []
    for sample_index in range(measured_runs):
        step = run_timed_step(
            [
                sys.executable,
                str(PUBLIC_RUNNER),
                "validate-showcase-runtime",
                "--example",
                workload_id,
            ]
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
    packet_path = ROOT / "tmp" / "reports" / "performance" / "runtime" / f"{workload_id}.json"
    write_json(packet_path, packet)
    return packet_path, failures


def main() -> int:
    args = parse_args(sys.argv[1:])
    portfolio = load_json(PORTFOLIO_PATH)
    policy = load_json(MEASUREMENT_POLICY_PATH)
    parameters = load_json(BENCHMARK_PARAMETERS_PATH)
    workloads = portfolio.get("objc3_workloads", [])
    if not isinstance(workloads, list) or not workloads:
        raise RuntimeError("benchmark portfolio did not publish objc3 workloads")

    warmup_runs = int(args.warmup_runs if args.warmup_runs is not None else policy["sample_policy"]["warmup_runs"])
    measured_runs = int(
        args.measured_runs if args.measured_runs is not None else policy["sample_policy"]["measured_runs"]
    )
    normalization_mode = str(parameters["hardware_profile_capture"]["normalization_mode"])
    compile_root = ROOT / "tmp" / "artifacts" / "performance" / "compile"
    compile_root.mkdir(parents=True, exist_ok=True)

    build_result = run_capture([sys.executable, str(PUBLIC_RUNNER), "build-native-binaries"])
    if build_result.returncode != 0:
        raise RuntimeError("build-native-binaries failed before benchmarking")

    profile = machine_profile()
    versions = tool_versions()
    packet_paths: list[str] = []
    failures: list[str] = []

    for workload in workloads:
        compile_packet_path, compile_failures = benchmark_compile_workload(
            workload,
            warmup_runs=warmup_runs,
            measured_runs=measured_runs,
            compile_root=compile_root,
            profile=profile,
            versions=versions,
            normalization_mode=normalization_mode,
        )
        packet_paths.append(repo_rel(compile_packet_path))
        failures.extend(compile_failures)

        runtime_packet_path, runtime_failures = benchmark_runtime_workload(
            workload,
            warmup_runs=warmup_runs,
            measured_runs=measured_runs,
            profile=profile,
            versions=versions,
            normalization_mode=normalization_mode,
        )
        packet_paths.append(repo_rel(runtime_packet_path))
        failures.extend(runtime_failures)

    payload = {
        "contract_id": "objc3c.performance.benchmark.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/benchmark_objc3c_performance.py",
        "portfolio_path": repo_rel(PORTFOLIO_PATH),
        "measurement_policy_path": repo_rel(MEASUREMENT_POLICY_PATH),
        "benchmark_parameters_path": repo_rel(BENCHMARK_PARAMETERS_PATH),
        "telemetry_packets": packet_paths,
        "failures": failures,
    }
    write_json(args.summary_out, payload)
    print(f"summary_path: {display_path(args.summary_out)}")
    if failures:
        print("objc3c-performance-benchmark: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-performance-benchmark: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
