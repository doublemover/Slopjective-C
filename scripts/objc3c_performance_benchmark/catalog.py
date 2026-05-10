from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable


@dataclass(frozen=True)
class BenchmarkCatalog:
    workloads: list[dict[str, Any]]
    warmup_runs: int
    measured_runs: int
    normalization_mode: str


def load_benchmark_catalog(
    *,
    portfolio_path: Path,
    measurement_policy_path: Path,
    benchmark_parameters_path: Path,
    load_json_fn: Callable[[Path], dict[str, Any]],
    warmup_runs_override: int | None,
    measured_runs_override: int | None,
) -> BenchmarkCatalog:
    portfolio = load_json_fn(portfolio_path)
    policy = load_json_fn(measurement_policy_path)
    parameters = load_json_fn(benchmark_parameters_path)
    workloads = portfolio.get("objc3_workloads", [])
    if not isinstance(workloads, list) or not workloads:
        raise RuntimeError("benchmark portfolio did not publish objc3 workloads")

    warmup_runs = int(
        warmup_runs_override
        if warmup_runs_override is not None
        else policy["sample_policy"]["warmup_runs"]
    )
    measured_runs = int(
        measured_runs_override
        if measured_runs_override is not None
        else policy["sample_policy"]["measured_runs"]
    )
    normalization_mode = str(parameters["hardware_profile_capture"]["normalization_mode"])
    return BenchmarkCatalog(
        workloads=workloads,
        warmup_runs=warmup_runs,
        measured_runs=measured_runs,
        normalization_mode=normalization_mode,
    )
