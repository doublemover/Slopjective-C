"""Summary payload construction and console rendering."""

from __future__ import annotations

import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path, repo_rel

from .catalog import BaselineRunConfig
from .comparison import status_from_failures


def build_summary_payload(
    *,
    config: BaselineRunConfig,
    packet_paths: list[str],
    failures: list[str],
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.performance.comparative.baselines.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": status_from_failures(failures),
        "runner_path": "scripts/run_objc3c_comparative_baselines.py",
        "manifest_path": repo_rel(config.manifest_path),
        "measurement_policy_path": repo_rel(config.measurement_policy_path),
        "benchmark_parameters_path": repo_rel(config.benchmark_parameters_path),
        "telemetry_packets": packet_paths,
        "failures": failures,
    }


def render_console_result(*, summary_out: Path, failures: list[str]) -> int:
    print(f"summary_path: {display_path(summary_out)}")
    if failures:
        print("objc3c-comparative-baselines: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-comparative-baselines: PASS")
    return 0
