"""Public-conformance evidence families and schema anchors."""

from __future__ import annotations

from .release_governance_public_conformance_models import (
    PublicConformanceEvidenceFamily,
    PublicConformanceSchemaAnchor,
)
from .release_governance_public_conformance_paths import (
    PUBLIC_CONFORMANCE_RUNBOOK,
    PUBLIC_CONFORMANCE_SCHEMA_SURFACE_PATH,
    PUBLIC_CONFORMANCE_STABILITY_POLICY_PATH,
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
            "tests/conformance/support_claim_runnable_evidence_catalog.json",
            "tests/conformance/public_suite_manifest.json",
            "scripts/check_conformance_corpus_surface.py",
            "scripts/generate_conformance_corpus_index.py",
            "scripts/check_objc3c_public_conformance_suite_manifest.py",
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
            "schemas/objc3c-public-conformance-suite-v1.schema.json",
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
        surface_key="public_suite_schema",
        registry_id="objc3c-public-conformance-suite-v1",
        schema_path="schemas/objc3c-public-conformance-suite-v1.schema.json",
        identity_property="contract_id",
        identity_value="objc3c.public_conformance_suite.manifest.v1",
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


__all__ = (
    "PUBLIC_CONFORMANCE_EVIDENCE_FAMILIES",
    "PUBLIC_CONFORMANCE_SCHEMA_ANCHORS",
)
