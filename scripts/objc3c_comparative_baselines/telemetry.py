"""Telemetry packet and sample construction for comparative baselines."""

from __future__ import annotations

import hashlib
import statistics
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file as write_json


def sha256_digest(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def summarize_durations(durations: list[float], normalization_mode: str, availability_status: str) -> dict[str, Any]:
    return {
        "sample_count": len(durations),
        "min_duration_ms": min(durations),
        "median_duration_ms": statistics.median(durations),
        "max_duration_ms": max(durations),
        "normalization_mode": normalization_mode,
        "availability_status": availability_status,
    }


def sample_from_step(
    *,
    baseline_id: str,
    language: str,
    sample_id: str,
    step_kind: str,
    step: dict[str, Any],
) -> dict[str, Any]:
    return {
        "sample_id": sample_id,
        "workload_id": baseline_id,
        "language": language,
        "step_kind": step_kind,
        "command": step["command"],
        "duration_ms": step["duration_ms"],
        "exit_code": step["exit_code"],
        "stdout_digest": sha256_digest(str(step["stdout"])),
        "stderr_digest": sha256_digest(str(step["stderr"])),
    }


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
            sample_from_step(
                baseline_id=baseline_id,
                language=language,
                sample_id=f"{baseline_id}-availability",
                step_kind="availability-probe",
                step=probe_step,
            )
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
