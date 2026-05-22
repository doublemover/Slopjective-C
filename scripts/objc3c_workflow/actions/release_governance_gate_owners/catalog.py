"""Release-governance gate owner catalog."""

from __future__ import annotations

from .constants import (
    DISTRIBUTION_CREDIBILITY_GATE_ID,
    PACKAGING_CHANNELS_GATE_ID,
    PUBLIC_CONFORMANCE_REPORTING_GATE_ID,
    RELEASE_FOUNDATION_GATE_ID,
    RELEASE_OPERATIONS_GATE_ID,
    SECURITY_HARDENING_GATE_ID,
)
from .models import ReleaseGateOwner, ReleaseGateOwnerCatalog
from .validation_rules import validate_release_gate_catalog


RELEASE_GATE_OWNERS: ReleaseGateOwnerCatalog = validate_release_gate_catalog(
    {
        RELEASE_FOUNDATION_GATE_ID: ReleaseGateOwner(
            gate_id=RELEASE_FOUNDATION_GATE_ID,
            source_owner="release-foundation-source",
            gate_owner="release-foundation-gate",
            blocker_owner="release-foundation-blockers",
            source_surface=(
                "tests/tooling/fixtures/release_foundation/source_surface.json"
            ),
            workflow_surface=(
                "tests/tooling/fixtures/release_foundation/workflow_surface.json"
            ),
            owned_actions=(
                "check-release-foundation-surface",
                "check-release-foundation-schema-surface",
                "check-release-abi-api-drift",
                "build-release-manifest",
                "publish-release-provenance",
                "validate-release-foundation",
            ),
            upstream_owner_actions=(
                "validate-performance-governance",
                "validate-runnable-release-candidate",
                "check-release-evidence",
            ),
            workflow_child_actions=(
                "validate-performance-governance",
                "validate-runnable-release-candidate",
                "check-release-evidence",
                "check-release-foundation-surface",
                "check-release-foundation-schema-surface",
                "check-release-abi-api-drift",
                "build-release-manifest",
                "publish-release-provenance",
            ),
        ),
        PACKAGING_CHANNELS_GATE_ID: ReleaseGateOwner(
            gate_id=PACKAGING_CHANNELS_GATE_ID,
            source_owner="packaging-channels-source",
            gate_owner="packaging-channels-gate",
            blocker_owner="packaging-channels-blockers",
            source_surface=(
                "tests/tooling/fixtures/packaging_channels/source_surface.json"
            ),
            workflow_surface=(
                "tests/tooling/fixtures/packaging_channels/workflow_surface.json"
            ),
            owned_actions=(
                "check-packaging-channels-surface",
                "check-packaging-channels-schema-surface",
                "build-package-channels",
                "build-platform-support-matrix",
                "validate-packaging-channels",
                "validate-packaging-channels-end-to-end",
                "validate-platform-hardening",
                "validate-platform-hardening-end-to-end",
            ),
            upstream_owner_actions=("validate-release-foundation",),
            workflow_child_actions=(
                "validate-release-foundation",
                "check-packaging-channels-surface",
                "check-packaging-channels-schema-surface",
                "build-package-channels",
                "build-platform-support-matrix",
            ),
            hard_cutover_guardrails=(
                ("unsupported_host_success_allowed", False),
                ("supported_host_claim_owner", "platform-hardening-support-source"),
                (
                    "toolchain_archive_claim_owner",
                    "platform-hardening-build-package-validation",
                ),
                ("toolchain_archive_claim_requires_owner", True),
                ("package_payload_owner_action", "package-runnable-toolchain"),
                ("evidence_log_release_claim_allowed", False),
                ("wrapper_only_action_surface_allowed", False),
            ),
        ),
        RELEASE_OPERATIONS_GATE_ID: ReleaseGateOwner(
            gate_id=RELEASE_OPERATIONS_GATE_ID,
            source_owner="release-operations-source",
            gate_owner="release-operations-gate",
            blocker_owner="release-operations-blockers",
            source_surface=(
                "tests/tooling/fixtures/release_operations/source_surface.json"
            ),
            workflow_surface=(
                "tests/tooling/fixtures/release_operations/workflow_surface.json"
            ),
            owned_actions=(
                "check-release-operations-surface",
                "check-release-operations-schema-surface",
                "build-update-manifest",
                "publish-release-operations",
                "validate-release-operations",
                "validate-release-operations-end-to-end",
            ),
            upstream_owner_actions=(
                "validate-release-foundation",
                "validate-packaging-channels",
            ),
            workflow_child_actions=(
                "validate-packaging-channels",
                "check-release-operations-surface",
                "check-release-operations-schema-surface",
                "build-update-manifest",
                "publish-release-operations",
            ),
            hard_cutover_guardrails=(
                ("missing_upstream_artifact_behavior", "fail-closed"),
                ("retired_update_path_allowed", False),
                ("alternate_update_support_path_allowed", False),
                ("publication_claim_owner", "release-operations-gate"),
                ("blocker_owner_required_before_publication", True),
                ("evidence_log_release_claim_allowed", False),
                ("wrapper_only_action_surface_allowed", False),
            ),
        ),
        PUBLIC_CONFORMANCE_REPORTING_GATE_ID: ReleaseGateOwner(
            gate_id=PUBLIC_CONFORMANCE_REPORTING_GATE_ID,
            source_owner="public-conformance-source",
            gate_owner="public-conformance-gate",
            blocker_owner="public-conformance-blockers",
            source_surface=(
                "tests/tooling/fixtures/public_conformance_reporting/"
                "source_surface.json"
            ),
            workflow_surface=(
                "tests/tooling/fixtures/public_conformance_reporting/"
                "workflow_surface.json"
            ),
            owned_actions=(
                "check-public-conformance-reporting-surface",
                "check-public-conformance-schema-surface",
                "build-public-conformance-scorecard",
                "publish-public-conformance-report",
                "validate-public-conformance-reporting",
                "validate-public-conformance-reporting-integration",
                "validate-public-conformance-reporting-end-to-end",
            ),
            upstream_owner_actions=(
                "validate-conformance-corpus",
                "validate-external-validation",
            ),
            workflow_child_actions=(
                "check-public-conformance-reporting-surface",
                "check-public-conformance-schema-surface",
                "build-public-conformance-scorecard",
                "publish-public-conformance-report",
            ),
        ),
        DISTRIBUTION_CREDIBILITY_GATE_ID: ReleaseGateOwner(
            gate_id=DISTRIBUTION_CREDIBILITY_GATE_ID,
            source_owner="distribution-credibility-source",
            gate_owner="distribution-credibility-gate",
            blocker_owner="distribution-credibility-blockers",
            source_surface=(
                "tests/tooling/fixtures/distribution_credibility/"
                "source_surface.json"
            ),
            workflow_surface=(
                "tests/tooling/fixtures/distribution_credibility/"
                "workflow_surface.json"
            ),
            owned_actions=(
                "check-distribution-credibility-surface",
                "check-distribution-credibility-schema-surface",
                "build-distribution-credibility-dashboard",
                "publish-distribution-credibility",
                "validate-distribution-credibility",
                "validate-distribution-credibility-end-to-end",
            ),
            upstream_owner_actions=(
                "validate-release-foundation",
                "validate-packaging-channels",
                "validate-packaging-channels-end-to-end",
                "validate-package-install-distribution",
                "validate-release-operations",
            ),
            workflow_child_actions=(
                "validate-release-operations",
                "validate-packaging-channels-end-to-end",
                "validate-package-install-distribution",
                "check-distribution-credibility-surface",
                "check-distribution-credibility-schema-surface",
                "build-distribution-credibility-dashboard",
                "publish-distribution-credibility",
            ),
        ),
        SECURITY_HARDENING_GATE_ID: ReleaseGateOwner(
            gate_id=SECURITY_HARDENING_GATE_ID,
            source_owner="security-hardening-source",
            gate_owner="security-hardening-gate",
            blocker_owner="security-hardening-blockers",
            source_surface=(
                "tests/tooling/fixtures/security_hardening/source_surface.json"
            ),
            workflow_surface=(
                "tests/tooling/fixtures/security_hardening/workflow_surface.json"
            ),
            owned_actions=(
                "check-security-hardening-surface",
                "check-security-hardening-schema-surface",
                "check-security-sanitizer-validation",
                "check-security-language-runtime-threat-model",
                "build-security-posture",
                "publish-security-advisories",
                "validate-security-hardening",
                "validate-security-hardening-end-to-end",
            ),
            upstream_owner_actions=(
                "validate-release-operations",
                "validate-distribution-credibility",
                "validate-platform-hardening",
            ),
            workflow_child_actions=(
                "check-security-hardening-surface",
                "check-security-hardening-schema-surface",
                "check-security-runtime-hardening",
                "check-security-sanitizer-validation",
                "check-security-language-runtime-threat-model",
                "build-security-posture",
                "check-security-response-drill",
                "publish-security-advisories",
            ),
        ),
    }
)

__all__ = ["RELEASE_GATE_OWNERS"]
