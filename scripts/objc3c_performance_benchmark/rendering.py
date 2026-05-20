from __future__ import annotations

import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Callable

from objc3c_tooling.paths import display_path, repo_rel


def render_summary_payload(
    *,
    failures: list[str],
    packet_paths: list[str],
    portfolio_path: Path,
    measurement_policy_path: Path,
    benchmark_parameters_path: Path,
    root: Path,
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.performance.benchmark.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/benchmark_objc3c_performance.py",
        "portfolio_path": repo_rel(portfolio_path, root=root),
        "measurement_policy_path": repo_rel(measurement_policy_path, root=root),
        "benchmark_parameters_path": repo_rel(benchmark_parameters_path, root=root),
        "telemetry_packets": packet_paths,
        "failures": failures,
    }


def publish_summary(
    summary_out: Path,
    payload: dict[str, Any],
    *,
    failures: list[str],
    write_json_fn: Callable[[Path, dict[str, Any]], None],
) -> int:
    write_json_fn(summary_out, payload)
    print(f"summary_path: {display_path(summary_out)}")
    if failures:
        print("objc3c-performance-benchmark: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-performance-benchmark: PASS")
    return 0
