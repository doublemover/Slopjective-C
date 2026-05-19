"""Shared paths for the comparative baseline runner."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "comparative_baseline_manifest.json"
MEASUREMENT_POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "measurement_policy.json"
BENCHMARK_PARAMETERS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_parameters.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "performance" / "comparative-baselines-summary.json"
