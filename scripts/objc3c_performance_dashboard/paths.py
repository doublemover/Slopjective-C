from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance"
SOURCE_SURFACE_PATH = FIXTURE_ROOT / "source_surface.json"
BUDGET_MODEL_PATH = FIXTURE_ROOT / "budget_model.json"
CLAIM_POLICY_PATH = FIXTURE_ROOT / "claim_policy.json"
TRIAGE_POLICY_PATH = FIXTURE_ROOT / "breach_triage_policy.json"
LAB_POLICY_PATH = FIXTURE_ROOT / "lab_policy.json"
WAIVERS_PATH = FIXTURE_ROOT / "waivers.json"
WORKFLOW_SURFACE_PATH = FIXTURE_ROOT / "workflow_surface.json"
PERFORMANCE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance" / "benchmark-summary.json"
PERFORMANCE_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "performance" / "integration-summary.json"
COMPARATIVE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance" / "comparative-baselines-summary.json"
COMPILER_SUMMARY_PATH = ROOT / "tmp" / "reports" / "compiler-throughput" / "benchmark-summary.json"
COMPILER_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "compiler-throughput" / "integration-summary.json"
RUNTIME_SUMMARY_PATH = ROOT / "tmp" / "reports" / "runtime-performance" / "benchmark-summary.json"
RUNTIME_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "runtime-performance" / "integration-summary.json"
OUTPUT_PATH = ROOT / "tmp" / "reports" / "performance-governance" / "dashboard-summary.json"
