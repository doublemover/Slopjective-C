"""Public-conformance reporting source, schema, and artifact contracts."""

from __future__ import annotations

from .release_governance_public_conformance_models import (
    PublicConformanceEvidenceFamily,
    PublicConformanceSchemaAnchor,
    PublicConformanceScoreBand,
    PublicConformanceStabilityPolicy,
    PublicConformanceWorkflowSurface,
)

PUBLIC_CONFORMANCE_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.public_conformance_reporting.source.surface.v1"
)
PUBLIC_CONFORMANCE_SCHEMA_SURFACE_CONTRACT_ID = (
    "objc3c.public_conformance_reporting.schema.surface.v1"
)
PUBLIC_CONFORMANCE_STABILITY_POLICY_CONTRACT_ID = (
    "objc3c.public_conformance_reporting.stability.policy.v1"
)
PUBLIC_CONFORMANCE_WORKFLOW_SURFACE_CONTRACT_ID = (
    "objc3c.public_conformance_reporting.workflow.surface.v1"
)

PUBLIC_CONFORMANCE_FIXTURE_ROOT = (
    "tests/tooling/fixtures/public_conformance_reporting"
)
PUBLIC_CONFORMANCE_RUNBOOK = "docs/runbooks/objc3c_public_conformance_reporting.md"
PUBLIC_CONFORMANCE_SOURCE_SURFACE = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/source_surface.json"
)
PUBLIC_CONFORMANCE_SOURCE_README = f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/README.md"
PUBLIC_CONFORMANCE_STABILITY_POLICY_PATH = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/stability_policy.json"
)
PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PATH = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/schema_surface.json"
)
PUBLIC_CONFORMANCE_WORKFLOW_SURFACE_PATH = (
    f"{PUBLIC_CONFORMANCE_FIXTURE_ROOT}/workflow_surface.json"
)

PUBLIC_CONFORMANCE_REPORT_ROOT = "tmp/reports/public-conformance"
PUBLIC_CONFORMANCE_ARTIFACT_ROOT = "tmp/artifacts/public-conformance"
PUBLIC_CONFORMANCE_SOURCE_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/source-surface-summary.json"
)
PUBLIC_CONFORMANCE_SCHEMA_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/schema-surface-summary.json"
)
PUBLIC_CONFORMANCE_SCORECARD_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/scorecard-summary.json"
)
PUBLIC_CONFORMANCE_PUBLIC_SUMMARY = f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/public-summary.json"
PUBLIC_CONFORMANCE_INTEGRATION_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/integration-summary.json"
)
PUBLIC_CONFORMANCE_END_TO_END_SUMMARY = (
    f"{PUBLIC_CONFORMANCE_REPORT_ROOT}/end-to-end-summary.json"
)
PUBLIC_CONFORMANCE_PUBLISHED_SCORECARD = (
    f"{PUBLIC_CONFORMANCE_ARTIFACT_ROOT}/scorecard/public-conformance-scorecard.json"
)
PUBLIC_CONFORMANCE_PUBLISHED_BADGE = (
    f"{PUBLIC_CONFORMANCE_ARTIFACT_ROOT}/badge/public-conformance-badge.json"
)
PUBLIC_CONFORMANCE_PUBLISHED_REPORT = (
    f"{PUBLIC_CONFORMANCE_ARTIFACT_ROOT}/report/public-conformance-report.md"
)

PUBLIC_CONFORMANCE_SOURCE_CHECK_SCRIPT = (
    "scripts/check_public_conformance_reporting_source_surface.py"
)
PUBLIC_CONFORMANCE_SCHEMA_CHECK_SCRIPT = (
    "scripts/check_public_conformance_schema_surface.py"
)
PUBLIC_CONFORMANCE_SCORECARD_SCRIPT = (
    "scripts/build_objc3c_public_conformance_scorecard.py"
)
PUBLIC_CONFORMANCE_REPORT_SCRIPT = (
    "scripts/publish_objc3c_public_conformance_report.py"
)
PUBLIC_CONFORMANCE_INTEGRATION_SCRIPT = (
    "scripts/check_objc3c_public_conformance_reporting_integration.py"
)
PUBLIC_CONFORMANCE_END_TO_END_SCRIPT = (
    "scripts/check_objc3c_public_conformance_reporting_end_to_end.py"
)

PUBLIC_CONFORMANCE_REQUIRED_ACTIONS = (
    "check-public-conformance-reporting-surface",
    "check-public-conformance-schema-surface",
    "build-public-conformance-scorecard",
    "publish-public-conformance-report",
    "validate-public-conformance-reporting",
    "validate-public-conformance-reporting-integration",
    "validate-public-conformance-reporting-end-to-end",
)
PUBLIC_CONFORMANCE_COMPOSITE_CHILD_ACTIONS = PUBLIC_CONFORMANCE_REQUIRED_ACTIONS[:4]

PUBLIC_CONFORMANCE_REPORT_PATHS = (
    PUBLIC_CONFORMANCE_SOURCE_SUMMARY,
    PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,
    PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,
    PUBLIC_CONFORMANCE_PUBLIC_SUMMARY,
    PUBLIC_CONFORMANCE_INTEGRATION_SUMMARY,
    PUBLIC_CONFORMANCE_END_TO_END_SUMMARY,
)
PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS = (
    PUBLIC_CONFORMANCE_PUBLISHED_SCORECARD,
    PUBLIC_CONFORMANCE_PUBLISHED_BADGE,
    PUBLIC_CONFORMANCE_PUBLISHED_REPORT,
)
PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS = (
    (
        PUBLIC_CONFORMANCE_SOURCE_SUMMARY,
        "objc3c.public_conformance_reporting.source.surface.summary.v1",
    ),
    (
        PUBLIC_CONFORMANCE_SCHEMA_SUMMARY,
        "objc3c.public_conformance_reporting.schema.surface.summary.v1",
    ),
    (
        PUBLIC_CONFORMANCE_SCORECARD_SUMMARY,
        "objc3c.public_conformance_reporting.scorecard.summary.v1",
    ),
    (
        PUBLIC_CONFORMANCE_PUBLIC_SUMMARY,
        "objc3c.public_conformance_reporting.summary.v1",
    ),
)

PUBLIC_CONFORMANCE_EVIDENCE_FAMILIES = (
    PublicConformanceEvidenceFamily(
        family_id="upstream-conformance-evidence",
        claim_rule=(
            "public claims resolve to the checked-in corpus surface, retained "
            "suite policy, and deterministic corpus report builders"
        ),
        source_paths=(
            PUBLIC_CONFORMANCE_RUNBOOK,
            PUBLIC_CONFORMANCE_STABILITY_POLICY_PATH,
            PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PATH,
            "docs/runbooks/objc3c_conformance_corpus.md",
            "tests/conformance/corpus_surface.json",
            "tests/conformance/longitudinal_suites.json",
            "scripts/check_conformance_corpus_surface.py",
            "scripts/generate_conformance_corpus_index.py",
            "scripts/check_objc3c_conformance_corpus_integration.py",
        ),
    ),
    PublicConformanceEvidenceFamily(
        family_id="external-credibility-evidence",
        claim_rule=(
            "public credibility claims resolve to accepted external-validation "
            "fixture policy, replay, integration, and publication owners"
        ),
        source_paths=(
            "docs/runbooks/objc3c_external_validation.md",
            "tests/tooling/fixtures/external_validation/source_surface.json",
            "tests/tooling/fixtures/external_validation/trust_policy.json",
            "tests/tooling/fixtures/external_validation/intake_manifest.json",
            "tests/tooling/fixtures/external_validation/quarantine_manifest.json",
            "tests/tooling/fixtures/external_validation/artifact_surface.json",
            "scripts/check_external_validation_source_surface.py",
            "scripts/run_objc3c_external_validation_replay.py",
            "scripts/publish_objc3c_external_repro_corpus.py",
            "scripts/check_objc3c_external_validation_integration.py",
        ),
    ),
    PublicConformanceEvidenceFamily(
        family_id="public-reporting-schema-anchors",
        claim_rule=(
            "public scorecard and summary payloads resolve to checked-in schema "
            "anchors and release-evidence ownership"
        ),
        source_paths=(
            "schemas/objc3-conformance-dashboard-status-v1.schema.json",
            "schemas/objc3-conformance-evidence-bundle-v1.schema.json",
            "scripts/check_release_evidence.py",
            "scripts/check_conformance_corpus_surface.py",
            "schemas/objc3c-public-conformance-scorecard-v1.schema.json",
            "schemas/objc3c-public-conformance-summary-v1.schema.json",
        ),
    ),
)

PUBLIC_CONFORMANCE_SCHEMA_ANCHORS = (
    PublicConformanceSchemaAnchor(
        surface_key="dashboard_status_schema",
        registry_id="objc3-conformance-dashboard-status-v1",
        schema_path="schemas/objc3-conformance-dashboard-status-v1.schema.json",
        identity_property="schema_id",
        identity_value="objc3-conformance-dashboard-status/v1",
    ),
    PublicConformanceSchemaAnchor(
        surface_key="public_scorecard_schema",
        registry_id="objc3c-public-conformance-scorecard-v1",
        schema_path="schemas/objc3c-public-conformance-scorecard-v1.schema.json",
        identity_property="contract_id",
        identity_value="objc3c.public_conformance_reporting.scorecard.summary.v1",
    ),
    PublicConformanceSchemaAnchor(
        surface_key="public_summary_schema",
        registry_id="objc3c-public-conformance-summary-v1",
        schema_path="schemas/objc3c-public-conformance-summary-v1.schema.json",
        identity_property="contract_id",
        identity_value="objc3c.public_conformance_reporting.summary.v1",
    ),
)

PUBLIC_CONFORMANCE_STABILITY_POLICY = PublicConformanceStabilityPolicy(
    contract_id=PUBLIC_CONFORMANCE_STABILITY_POLICY_CONTRACT_ID,
    allowed_public_statuses=("pass", "caution", "blocked"),
    allowed_badges=("claim-ready", "provisional", "blocked"),
    score_bands=(
        PublicConformanceScoreBand("claim-ready", 95, 100, "claim-ready", "pass"),
        PublicConformanceScoreBand("provisional", 70, 94, "provisional", "caution"),
        PublicConformanceScoreBand("blocked", 0, 69, "blocked", "blocked"),
    ),
    fail_closed_conditions=(
        "corpus integration summary must report PASS",
        "external validation integration summary must report PASS",
        "checked-in dashboard and release-evidence schema anchors must exist",
        "public reporting cannot promote quarantined or blocked evidence into claim-ready status",
    ),
    capability_truth_rules=(
        "claim-ready means every upstream evidence owner reported PASS and no hard blocks remain",
        "provisional means publishable with explicit deductions and caution status",
        "blocked means the public artifact must report blocked and cannot imply conformance readiness",
    ),
)

PUBLIC_CONFORMANCE_WORKFLOW_SURFACE = PublicConformanceWorkflowSurface(
    contract_id=PUBLIC_CONFORMANCE_WORKFLOW_SURFACE_CONTRACT_ID,
    required_actions=PUBLIC_CONFORMANCE_REQUIRED_ACTIONS,
    report_paths=PUBLIC_CONFORMANCE_REPORT_PATHS,
    artifact_paths=PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS,
    child_report_contracts=PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS,
)
