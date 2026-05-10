"""Release-governance gate owner registry."""

from __future__ import annotations

from .release_governance_owner_models import ReleaseGateOwner


RELEASE_GATE_OWNERS: dict[str, ReleaseGateOwner] = {
    "release-foundation": ReleaseGateOwner(
        gate_id="release-foundation",
        source_owner="release-foundation-source",
        gate_owner="release-foundation-gate",
        blocker_owner="release-foundation-blockers",
        source_surface="tests/tooling/fixtures/release_foundation/source_surface.json",
        workflow_surface="tests/tooling/fixtures/release_foundation/workflow_surface.json",
        owned_actions=(
            "check-release-foundation-surface",
            "check-release-foundation-schema-surface",
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
            "build-release-manifest",
            "publish-release-provenance",
        ),
    ),
    "packaging-channels": ReleaseGateOwner(
        gate_id="packaging-channels",
        source_owner="packaging-channels-source",
        gate_owner="packaging-channels-gate",
        blocker_owner="packaging-channels-blockers",
        source_surface="tests/tooling/fixtures/packaging_channels/source_surface.json",
        workflow_surface="tests/tooling/fixtures/packaging_channels/workflow_surface.json",
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
    "release-operations": ReleaseGateOwner(
        gate_id="release-operations",
        source_owner="release-operations-source",
        gate_owner="release-operations-gate",
        blocker_owner="release-operations-blockers",
        source_surface="tests/tooling/fixtures/release_operations/source_surface.json",
        workflow_surface="tests/tooling/fixtures/release_operations/workflow_surface.json",
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
    "public-conformance-reporting": ReleaseGateOwner(
        gate_id="public-conformance-reporting",
        source_owner="public-conformance-source",
        gate_owner="public-conformance-gate",
        blocker_owner="public-conformance-blockers",
        source_surface=(
            "tests/tooling/fixtures/public_conformance_reporting/source_surface.json"
        ),
        workflow_surface=(
            "tests/tooling/fixtures/public_conformance_reporting/workflow_surface.json"
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
    "distribution-credibility": ReleaseGateOwner(
        gate_id="distribution-credibility",
        source_owner="distribution-credibility-source",
        gate_owner="distribution-credibility-gate",
        blocker_owner="distribution-credibility-blockers",
        source_surface=(
            "tests/tooling/fixtures/distribution_credibility/source_surface.json"
        ),
        workflow_surface=(
            "tests/tooling/fixtures/distribution_credibility/workflow_surface.json"
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
            "validate-release-operations",
        ),
        workflow_child_actions=(
            "validate-release-operations",
            "check-distribution-credibility-surface",
            "check-distribution-credibility-schema-surface",
            "build-distribution-credibility-dashboard",
            "publish-distribution-credibility",
        ),
    ),
    "security-hardening": ReleaseGateOwner(
        gate_id="security-hardening",
        source_owner="security-hardening-source",
        gate_owner="security-hardening-gate",
        blocker_owner="security-hardening-blockers",
        source_surface="tests/tooling/fixtures/security_hardening/source_surface.json",
        workflow_surface="tests/tooling/fixtures/security_hardening/workflow_surface.json",
        owned_actions=(
            "check-security-hardening-surface",
            "check-security-hardening-schema-surface",
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
            "check-security-response-drill",
            "check-security-runtime-hardening",
            "check-security-hardening-surface",
            "check-security-hardening-schema-surface",
            "build-security-posture",
            "publish-security-advisories",
        ),
    ),
}


def release_gate_owner(gate_id: str) -> ReleaseGateOwner:
    return RELEASE_GATE_OWNERS[gate_id]


def release_gate_public_actions(gate_id: str) -> tuple[str, ...]:
    return release_gate_owner(gate_id).owned_actions


def release_gate_child_actions(gate_id: str) -> tuple[str, ...]:
    return release_gate_owner(gate_id).workflow_child_actions


def release_gate_hard_cutover_guardrails(gate_id: str) -> dict[str, object]:
    return dict(release_gate_owner(gate_id).hard_cutover_guardrails)


def release_action_owner_map() -> dict[str, dict[str, str]]:
    owner_map: dict[str, dict[str, str]] = {}
    for owner in RELEASE_GATE_OWNERS.values():
        owner_map.update(owner.action_owner_map())
    return owner_map


__all__ = [
    "RELEASE_GATE_OWNERS",
    "release_action_owner_map",
    "release_gate_child_actions",
    "release_gate_hard_cutover_guardrails",
    "release_gate_owner",
    "release_gate_public_actions",
]
