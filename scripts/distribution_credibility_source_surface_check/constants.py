"""Static contracts for the distribution-credibility source surface."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "source-surface-summary.json"
SURFACE_CONTRACT_ID = "objc3c.distribution.credibility.source.surface.v1"
SUMMARY_CONTRACT_ID = "objc3c.distribution.credibility.source.surface.summary.v1"

EXPECTED_CONTRACT_IDS = {
    "trust_signal_architecture": "objc3c.distribution.credibility.trust.signal.architecture.v1",
    "package_install_distribution_credibility": (
        "objc3c.package_ecosystem.install_distribution_credibility.v1"
    ),
    "install_release_doc_surface": "objc3c.distribution.credibility.install.release.doc.surface.v1",
    "operator_release_policy": "objc3c.distribution.credibility.operator.release.policy.v1",
    "release_drill_policy": "objc3c.distribution.credibility.release.drill.policy.v1",
    "artifact_surface": "objc3c.distribution.credibility.artifact.surface.v1",
    "schema_surface": "objc3c.distribution.credibility.schema.surface.v1",
    "workflow_surface": "objc3c.distribution.credibility.workflow.surface.v1",
}

EXPECTED_TRUST_SIGNAL_IDS = [
    "release-foundation-lineage",
    "package-channel-install-smoke",
    "release-operations-metadata",
    "release-evidence-gate",
]

EXPECTED_WORKFLOW_ACTIONS = [
    "check-distribution-credibility-surface",
    "check-distribution-credibility-schema-surface",
    "build-distribution-credibility-dashboard",
    "publish-distribution-credibility",
    "validate-distribution-credibility",
    "validate-distribution-credibility-end-to-end",
]

EXPECTED_INTEGRATED_STEPS = [
    "validate-release-operations",
    "validate-package-install-distribution",
    "check-distribution-credibility-surface",
    "check-distribution-credibility-schema-surface",
    "build-distribution-credibility-dashboard",
    "publish-distribution-credibility",
]

EXPECTED_OUTPUTS = {
    "source_surface_summary": "tmp/reports/distribution-credibility/source-surface-summary.json",
    "schema_surface_summary": "tmp/reports/distribution-credibility/schema-surface-summary.json",
    "dashboard_summary": "tmp/reports/distribution-credibility/dashboard-summary.json",
    "publication_summary": "tmp/reports/distribution-credibility/publication-summary.json",
    "integration_summary": "tmp/reports/distribution-credibility/integration-summary.json",
    "end_to_end_summary": "tmp/reports/distribution-credibility/end-to-end-summary.json",
    "dashboard_artifact": "tmp/artifacts/distribution-credibility/dashboard/distribution-credibility-dashboard.json",
    "trust_report_json": "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.json",
    "trust_report_markdown": "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.md",
}
