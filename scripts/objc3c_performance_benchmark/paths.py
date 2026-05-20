from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PORTFOLIO_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_portfolio.json"
MEASUREMENT_POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "measurement_policy.json"
BENCHMARK_PARAMETERS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_parameters.json"
PERFORMANCE_BUDGET_MODEL_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "budget_model.json"
)
SUMMARY_OUT = ROOT / "tmp" / "reports" / "performance" / "benchmark-summary.json"
