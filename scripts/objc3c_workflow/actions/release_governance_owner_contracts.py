"""Typed release-governance owner contracts."""

from __future__ import annotations

from dataclasses import dataclass

from ..action_spec import ActionSpec


@dataclass(frozen=True)
class ReleaseGateOwner:
    gate_id: str
    source_owner: str
    gate_owner: str
    blocker_owner: str
    source_surface: str
    workflow_surface: str
    owned_actions: tuple[str, ...]
    upstream_owner_actions: tuple[str, ...] = ()
    workflow_child_actions: tuple[str, ...] = ()
    hard_cutover_guardrails: tuple[tuple[str, object], ...] = ()

    def owner_policy(self) -> dict[str, object]:
        policy: dict[str, object] = {
            "source_owner": self.source_owner,
            "gate_owner": self.gate_owner,
            "blocker_owner": self.blocker_owner,
            "report_only_allowed": False,
            "owned_actions": list(self.owned_actions),
        }
        if self.hard_cutover_guardrails:
            policy["hard_cutover_guardrails"] = dict(self.hard_cutover_guardrails)
        return policy

    def workflow_owner_policy(self) -> dict[str, object]:
        policy = self.owner_policy()
        policy["upstream_owner_actions"] = list(self.upstream_owner_actions)
        policy["workflow_child_actions"] = list(self.workflow_child_actions)
        return policy

    def action_owner_map(self) -> dict[str, dict[str, str]]:
        return {
            action: {
                "source_owner": self.source_owner,
                "gate_owner": self.gate_owner,
                "blocker_owner": self.blocker_owner,
            }
            for action in self.owned_actions
        }


@dataclass(frozen=True)
class ReleaseGovernanceActionContract:
    action: str
    summary: str
    backend: str
    gate_id: str
    validation_tier: str
    guarantee_owner: str
    report_only_allowed: bool = False

    def to_action_spec(self) -> ActionSpec:
        if self.report_only_allowed:
            raise ValueError(f"{self.action} cannot be registered as report-only")
        owner = RELEASE_GATE_OWNERS[self.gate_id]
        return ActionSpec(
            self.action,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=(
                f"{owner.source_owner} -> {owner.gate_owner} -> "
                f"{owner.blocker_owner}: {self.guarantee_owner}"
            ),
        )


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
            ("report_only_release_claim_allowed", False),
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
            ("compatibility_update_fallback_allowed", False),
            ("update_fallback_support_allowed", False),
            ("publication_claim_owner", "release-operations-gate"),
            ("blocker_owner_required_before_publication", True),
            ("report_only_release_claim_allowed", False),
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

RELEASE_FOUNDATION_ACTION_CONTRACTS: tuple[ReleaseGovernanceActionContract, ...] = (
    ReleaseGovernanceActionContract(
        "check-release-foundation-surface",
        "validate the checked-in release-foundation source surface",
        "python:scripts/check_release_foundation_source_surface.py",
        "release-foundation",
        "repo",
        (
            "release foundation publishes only from checked-in taxonomy, trust, "
            "payload, and provenance contracts"
        ),
    ),
    ReleaseGovernanceActionContract(
        "check-release-foundation-schema-surface",
        "validate the checked-in release-foundation schema surface",
        "python:scripts/check_release_foundation_schema_surface.py",
        "release-foundation",
        "repo",
        "release manifest, sbom, and attestation artifacts stay on checked-in schemas",
    ),
    ReleaseGovernanceActionContract(
        "build-release-manifest",
        "derive the machine-owned release manifest from repeated runnable package assembly runs",
        "python:scripts/build_objc3c_release_manifest.py",
        "release-foundation",
        "repo",
        (
            "release payload selection and reproducibility proof stay tied to the "
            "live runnable package manifest"
        ),
    ),
    ReleaseGovernanceActionContract(
        "publish-release-provenance",
        "publish the machine-owned release sbom and attestation artifacts",
        "python:scripts/publish_objc3c_release_provenance.py",
        "release-foundation",
        "repo",
        (
            "release provenance publication stays traceable to the live manifest, "
            "package manifest, and release-evidence index"
        ),
    ),
    ReleaseGovernanceActionContract(
        "validate-release-foundation",
        "run the integrated release-foundation workflow",
        "runner-internal release-foundation child actions",
        "release-foundation",
        "nightly",
        (
            "release taxonomy, reproducible package assembly, and provenance "
            "publication stay executable on the live runnable package surface"
        ),
    ),
)

RELEASE_OPERATIONS_ACTION_CONTRACTS: tuple[ReleaseGovernanceActionContract, ...] = (
    ReleaseGovernanceActionContract(
        "check-release-operations-surface",
        "validate the checked-in release-operations source surface",
        "python:scripts/check_release_operations_source_surface.py",
        "release-operations",
        "repo",
        (
            "release operations publish only from checked-in versioning, upgrade, "
            "diagnostics, and channel policy contracts"
        ),
    ),
    ReleaseGovernanceActionContract(
        "check-release-operations-schema-surface",
        "validate the checked-in release-operations schema surface",
        "python:scripts/check_release_operations_schema_surface.py",
        "release-operations",
        "repo",
        "update manifest and upgrade-support artifacts stay on checked-in schemas",
    ),
    ReleaseGovernanceActionContract(
        "build-update-manifest",
        "derive the machine-owned update manifest from existing release, package-channel, and platform-support artifacts",
        "python:scripts/build_objc3c_update_manifest.py",
        "release-operations",
        "repo",
        "versioned channel metadata fails closed when required upstream artifacts are absent",
    ),
    ReleaseGovernanceActionContract(
        "publish-release-operations",
        "publish the machine-owned upgrade-support report and channel catalog",
        "python:scripts/publish_objc3c_release_operations_metadata.py",
        "release-operations",
        "repo",
        (
            "release-operations publication stays traceable to checked-in upgrade, "
            "revert, diagnostics, and channel policy contracts"
        ),
    ),
    ReleaseGovernanceActionContract(
        "validate-release-operations",
        "run the integrated release-operations workflow",
        "runner-internal release-operations child actions",
        "release-operations",
        "nightly",
        (
            "versioning, upgrade warnings, revert guidance, update metadata, and "
            "public action ownership stay executable on live release surfaces"
        ),
    ),
    ReleaseGovernanceActionContract(
        "validate-release-operations-end-to-end",
        "validate release-operations entrypoints, generated metadata, and packaged channel references end to end",
        "python:scripts/check_objc3c_release_operations_end_to_end.py",
        "release-operations",
        "full",
        (
            "release-operations metadata stays coherent with live package-channel "
            "artifacts and fail-closed revert paths"
        ),
    ),
)

PACKAGING_CHANNEL_ACTION_CONTRACTS: tuple[ReleaseGovernanceActionContract, ...] = (
    ReleaseGovernanceActionContract(
        "check-packaging-channels-surface",
        "validate the checked-in packaging-channels source surface",
        "python:scripts/check_packaging_channels_source_surface.py",
        "packaging-channels",
        "repo",
        "packaging-channel publication stays rooted in checked-in channel, platform, installer, and blocker-owner contracts",
    ),
    ReleaseGovernanceActionContract(
        "check-packaging-channels-schema-surface",
        "validate the checked-in packaging-channels schema surface",
        "python:scripts/check_packaging_channels_schema_surface.py",
        "packaging-channels",
        "repo",
        "packaging-channel and install-receipt artifacts stay on checked-in schema contracts",
    ),
    ReleaseGovernanceActionContract(
        "build-package-channels",
        "build the portable archive, installer image, and offline bundle channels from the live runnable payload",
        "python:scripts/build_objc3c_package_channels.py",
        "packaging-channels",
        "repo",
        "package channels stay derived from the live runnable package and release-foundation artifacts",
    ),
    ReleaseGovernanceActionContract(
        "validate-packaging-channels",
        "run the integrated packaging-channels workflow",
        "runner-internal packaging-channel child actions",
        "packaging-channels",
        "nightly",
        "portable archive installer image and offline bundle generation stay executable on the live release surface",
    ),
    ReleaseGovernanceActionContract(
        "validate-packaging-channels-end-to-end",
        "validate install bootstrap rollback and offline bundle behavior end to end",
        "python:scripts/check_objc3c_packaging_channels_end_to_end.py",
        "packaging-channels",
        "full",
        "packaging-channel artifacts stay installable rollback-safe and offline-bootstrappable under temp-owned roots",
    ),
    ReleaseGovernanceActionContract(
        "build-platform-support-matrix",
        "build the machine-owned platform support matrix artifact",
        "python:scripts/build_objc3c_platform_support_matrix.py",
        "packaging-channels",
        "repo",
        "published host and channel support matrix stays source-owned, narrow, and aligned with live packaging evidence",
    ),
    ReleaseGovernanceActionContract(
        "validate-platform-hardening",
        "run the integrated platform-hardening workflow",
        "python:scripts/check_objc3c_platform_hardening_integration.py",
        "packaging-channels",
        "nightly",
        "supported host package install and release claims stay tiered package-backed and fail-closed outside the checked-in matrix",
    ),
    ReleaseGovernanceActionContract(
        "validate-platform-hardening-end-to-end",
        "validate packaged platform-hardening publication and smoke behavior from the staged runnable bundle",
        "python:scripts/check_objc3c_runnable_platform_hardening_end_to_end.py",
        "packaging-channels",
        "full",
        "packaged platform support inspection and validation remain runnable from the staged toolchain bundle",
    ),
)


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


def release_action_specs(
    contracts: tuple[ReleaseGovernanceActionContract, ...],
) -> dict[str, ActionSpec]:
    return {contract.action: contract.to_action_spec() for contract in contracts}


__all__ = [
    "PACKAGING_CHANNEL_ACTION_CONTRACTS",
    "RELEASE_FOUNDATION_ACTION_CONTRACTS",
    "RELEASE_GATE_OWNERS",
    "RELEASE_OPERATIONS_ACTION_CONTRACTS",
    "ReleaseGateOwner",
    "ReleaseGovernanceActionContract",
    "release_action_owner_map",
    "release_action_specs",
    "release_gate_child_actions",
    "release_gate_hard_cutover_guardrails",
    "release_gate_owner",
    "release_gate_public_actions",
]
