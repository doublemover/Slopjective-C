#!/usr/bin/env python3
"""Validate the integrated distribution-credibility workflow and publication outputs."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-distribution-credibility.json"
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "workflow_surface.json"
DASHBOARD_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "dashboard-summary.json"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "distribution-credibility" / "publication-summary.json"
PACKAGE_INSTALL_SUMMARY = ROOT / "tmp" / "reports" / "package-ecosystem" / "install-distribution-credibility-summary.json"
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "artifact_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "integration-summary.json"

REQUIRED_STEPS = [
    "validate-release-operations",
    "validate-package-install-distribution",
    "check-distribution-credibility-surface",
    "check-distribution-credibility-schema-surface",
    "build-distribution-credibility-dashboard",
    "publish-distribution-credibility",
]

REQUIRED_SIGNAL_IDS = [
    "release-foundation-lineage",
    "package-channel-install-smoke",
    "release-operations-metadata",
    "release-evidence-gate",
]


def fail(message: str) -> int:
    print(f"objc3c-distribution-credibility-integration: {message}", file=sys.stderr)
    return 1


def package_install_summary_is_from_nothing(payload: dict[str, Any]) -> bool:
    probe = payload.get("from_nothing_probe")
    if not isinstance(probe, dict):
        return False
    after_clean = probe.get("owned_outputs_exist_after_clean")
    return (
        probe.get("requested") is True
        and probe.get("generated_from_clean_owned_outputs") is True
        and isinstance(after_clean, dict)
        and after_clean.get("tmp/artifacts/package-ecosystem") is False
        and after_clean.get("tmp/reports/package-ecosystem") is False
    )




def main() -> int:
    for path in (WORKFLOW_REPORT, WORKFLOW_SURFACE, DASHBOARD_SUMMARY, PUBLICATION_SUMMARY, PACKAGE_INSTALL_SUMMARY, ARTIFACT_SURFACE):
        if not path.is_file():
            return fail(f"missing required artifact {repo_rel(path)}")

    workflow_report = load_json(WORKFLOW_REPORT)
    workflow_surface = load_json(WORKFLOW_SURFACE)
    dashboard_summary = load_json(DASHBOARD_SUMMARY)
    publication_summary = load_json(PUBLICATION_SUMMARY)
    package_install_summary_payload = load_json(PACKAGE_INSTALL_SUMMARY)
    artifact_surface = load_json(ARTIFACT_SURFACE)

    if workflow_report.get("status") != "PASS":
        return fail("workflow report did not pass")
    steps = workflow_report.get("steps")
    if not isinstance(steps, list):
        return fail("workflow report was missing steps")
    step_actions = [step.get("action") for step in steps if isinstance(step, dict)]
    if step_actions != REQUIRED_STEPS:
        return fail(f"workflow steps drifted: {step_actions}")
    if workflow_surface.get("validate_action") != "validate-distribution-credibility":
        return fail("workflow surface drifted from validate-distribution-credibility")
    if workflow_surface.get("integrated_required_steps") != REQUIRED_STEPS:
        return fail("workflow surface integrated steps drifted")
    if workflow_surface.get("from_nothing_evidence") != {
        "required_action": "validate-package-install-distribution",
        "required_flag": "--from-nothing",
        "required_summary": "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json",
        "stale_artifacts_allowed": False,
        "dashboard_blocks_if_missing": True,
    }:
        return fail("workflow surface from-nothing package install evidence drifted")
    if dashboard_summary.get("status") != "PASS":
        return fail("dashboard summary did not pass")
    if publication_summary.get("status") != "PASS":
        return fail("distribution-credibility publication summary did not pass")
    trust_signals = dashboard_summary.get("trust_signals")
    signal_ids = [signal.get("signal_id") for signal in trust_signals if isinstance(signal, dict)] if isinstance(trust_signals, list) else []
    if signal_ids != REQUIRED_SIGNAL_IDS:
        return fail(f"dashboard trust signal order drifted: {signal_ids}")
    if publication_summary.get("published_dashboard") != artifact_surface.get("dashboard_artifact"):
        return fail("publication summary dashboard artifact path drifted")
    if publication_summary.get("trust_report_json") != artifact_surface.get("trust_report_json"):
        return fail("publication summary trust report JSON path drifted")
    if publication_summary.get("trust_report_markdown") != artifact_surface.get("trust_report_markdown"):
        return fail("publication summary trust report markdown path drifted")
    upstream_reports = dashboard_summary.get("upstream_reports")
    if not isinstance(upstream_reports, dict):
        return fail("dashboard upstream reports drifted")
    package_install_summary_path = upstream_reports.get("package_install_distribution")
    if package_install_summary_path != "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json":
        return fail("dashboard package install distribution evidence path drifted")
    if not package_install_summary_is_from_nothing(package_install_summary_payload):
        return fail("package install distribution evidence was not regenerated from nothing")
    evidence_artifacts = publication_summary.get("evidence_artifacts")
    evidence_paths = [
        artifact.get("path")
        for artifact in evidence_artifacts
        if isinstance(artifact, dict)
    ] if isinstance(evidence_artifacts, list) else []
    if package_install_summary_path not in evidence_paths:
        return fail("publication summary omitted package install distribution evidence")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.distribution.credibility.integration.summary.v1",
        "status": "PASS",
        "workflow_report": repo_rel(WORKFLOW_REPORT),
        "validated_steps": REQUIRED_STEPS,
        "dashboard_summary": repo_rel(DASHBOARD_SUMMARY),
        "publication_summary": repo_rel(PUBLICATION_SUMMARY),
        "trust_state": dashboard_summary.get("trust_state"),
        "trust_report_json": publication_summary.get("trust_report_json"),
        "published_dashboard": publication_summary.get("published_dashboard"),
        "validated_signal_ids": signal_ids,
    }
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-distribution-credibility-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
