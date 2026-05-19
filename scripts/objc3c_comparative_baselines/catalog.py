"""Baseline catalog and run-configuration loading."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json

from .paths import BENCHMARK_PARAMETERS_PATH, MANIFEST_PATH, MEASUREMENT_POLICY_PATH


@dataclass(frozen=True)
class BaselineRunConfig:
    entries: list[dict[str, Any]]
    warmup_runs: int
    measured_runs: int
    normalization_mode: str
    manifest_path: Path = MANIFEST_PATH
    measurement_policy_path: Path = MEASUREMENT_POLICY_PATH
    benchmark_parameters_path: Path = BENCHMARK_PARAMETERS_PATH


def load_run_config(
    *,
    warmup_runs_override: int | None = None,
    measured_runs_override: int | None = None,
    manifest_path: Path = MANIFEST_PATH,
    measurement_policy_path: Path = MEASUREMENT_POLICY_PATH,
    benchmark_parameters_path: Path = BENCHMARK_PARAMETERS_PATH,
) -> BaselineRunConfig:
    manifest = load_json(manifest_path)
    policy = load_json(measurement_policy_path)
    parameters = load_json(benchmark_parameters_path)

    entries = manifest.get("baseline_entries", [])
    if not isinstance(entries, list) or not entries:
        raise RuntimeError("comparative baseline manifest did not publish baseline_entries")

    sample_policy = policy["sample_policy"]
    warmup_runs = int(warmup_runs_override if warmup_runs_override is not None else sample_policy["warmup_runs"])
    measured_runs = int(
        measured_runs_override if measured_runs_override is not None else sample_policy["measured_runs"]
    )
    normalization_mode = str(parameters["hardware_profile_capture"]["normalization_mode"])

    return BaselineRunConfig(
        entries=entries,
        warmup_runs=warmup_runs,
        measured_runs=measured_runs,
        normalization_mode=normalization_mode,
        manifest_path=manifest_path,
        measurement_policy_path=measurement_policy_path,
        benchmark_parameters_path=benchmark_parameters_path,
    )
