from __future__ import annotations

SUMMARY_CONTRACT_ID = "objc3c.performance.governance.dashboard.summary.v1"

EXPECTED_POLICY_CONTRACTS = {
    "source_surface": "objc3c.performance.governance.source.surface.v1",
    "budget_model": "objc3c.performance.governance.budget.model.v1",
    "claim_policy": "objc3c.performance.governance.claim.policy.v1",
    "breach_triage_policy": "objc3c.performance.governance.breach.triage.policy.v1",
    "lab_policy": "objc3c.performance.governance.lab.policy.v1",
    "waiver_registry": "objc3c.performance.governance.waiver.registry.v1",
    "workflow_surface": "objc3c.performance.governance.workflow.surface.v1",
}

EXPECTED_UPSTREAM_REPORT_CONTRACTS = {
    "performance_summary": "objc3c.performance.benchmark.summary.v1",
    "performance_integration": "objc3c.performance.integration.summary.v1",
    "comparative_summary": "objc3c.performance.comparative.baselines.summary.v1",
    "compiler_summary": "objc3c.compiler.throughput.summary.v1",
    "compiler_integration": "objc3c.compiler.throughput.integration.summary.v1",
    "runtime_summary": "objc3c.runtime.performance.summary.v1",
    "runtime_integration": "objc3c.runtime.performance.integration.summary.v1",
}

EXPECTED_CLAIM_STATUS_REQUIREMENTS = {
    "release-ready": [
        "all_required_budget_families_pass",
        "no_blocking_breach",
        "no_expired_waiver",
    ],
    "caution": [
        "all_required_budget_families_present",
        "no_blocking_environment_drift",
    ],
    "blocked": [
        "blocked-when-any-release-blocking-condition-is-met",
    ],
}
