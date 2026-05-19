"""Shared constants for platform-hardening contracts."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PLATFORM_FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "platform_hardening"
PACKAGING_FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels"
RELEASE_OPERATIONS_ROOT = ROOT / "tmp" / "artifacts" / "release-operations"
PLATFORM_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "platform-hardening"
PLATFORM_REPORT_ROOT = ROOT / "tmp" / "reports" / "platform-hardening"

BOUNDARY_INVENTORY_PATH = PLATFORM_FIXTURE_ROOT / "boundary_inventory.json"
SUPPORT_TIER_POLICY_PATH = PLATFORM_FIXTURE_ROOT / "platform_support_tier_policy.json"
UNSUPPORTED_HOST_POLICY_PATH = PLATFORM_FIXTURE_ROOT / "unsupported_host_fail_closed_policy.json"
TOOLCHAIN_ARCHIVE_POLICY_PATH = PLATFORM_FIXTURE_ROOT / "toolchain_archive_claim_policy.json"
PLATFORM_MATRIX_ARTIFACT_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "platform_matrix_artifact_contract.json"
BUILD_PACKAGE_VALIDATION_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "build_package_validation_contract.json"
TOOLCHAIN_RANGE_REPLAY_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "toolchain_range_replay_contract.json"
INSTALL_MATRIX_INTEGRATION_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "install_matrix_integration_contract.json"
PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH = PLATFORM_FIXTURE_ROOT / "packaged_smoke_integration_contract.json"
SUPPORTED_PLATFORMS_PATH = PACKAGING_FIXTURE_ROOT / "supported_platforms.json"

PLATFORM_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_platform_hardening.md"
PACKAGING_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_packaging_channels.md"
RELEASE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_release_operations.md"
PLATFORM_SUPPORT_MATRIX_SCHEMA_PATH = ROOT / "schemas" / "objc3c-platform-support-matrix-v1.schema.json"

SUPPORT_MATRIX_ARTIFACT_PATH = PLATFORM_ARTIFACT_ROOT / "objc3c-platform-support-matrix.json"
SUPPORT_MATRIX_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "platform-support-matrix-summary.json"
BOUNDARY_INVENTORY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "boundary-inventory" / "boundary_inventory_summary.json"
SUPPORT_TIER_POLICY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "support-tier-policy" / "support_tier_policy_summary.json"
UNSUPPORTED_HOST_POLICY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "unsupported-host-policy" / "unsupported_host_policy_summary.json"
TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "toolchain-archive-policy" / "toolchain_archive_policy_summary.json"
ARTIFACT_CONTRACT_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "artifact-contract" / "artifact_contract_summary.json"
BUILD_PACKAGE_VALIDATION_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "build-package-validation-summary.json"
TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "toolchain-range-replay-summary.json"
INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "install-matrix-integration-summary.json"
PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "integration-summary.json"
RUNNABLE_END_TO_END_SUMMARY_PATH = PLATFORM_REPORT_ROOT / "runnable-end-to-end-summary.json"

PACKAGE_CHANNELS_SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "end-to-end-summary.json"
RELEASE_PUBLICATION_SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "publication-summary.json"
UPDATE_MANIFEST_PATH = RELEASE_OPERATIONS_ROOT / "update-manifest" / "objc3c-update-manifest.json"
UPGRADE_SUPPORT_REPORT_PATH = RELEASE_OPERATIONS_ROOT / "publication" / "objc3c-upgrade-support-report.json"
CHANNEL_CATALOG_PATH = RELEASE_OPERATIONS_ROOT / "publication" / "objc3c-release-channel-catalog.json"
PACKAGE_MANIFEST_PATH = ROOT / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"

BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT = ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
BUILD_PACKAGE_VALIDATION_SCRIPT = ROOT / "scripts" / "check_platform_hardening_build_package_validation.py"
TOOLCHAIN_RANGE_REPLAY_SCRIPT = ROOT / "scripts" / "check_platform_hardening_toolchain_range_replay.py"
INSTALL_MATRIX_INTEGRATION_SCRIPT = ROOT / "scripts" / "check_platform_hardening_install_matrix_integration.py"

PUBLICATION_SURFACE: dict[str, str] = {
    "package_bridge": "objc3c",
    "inspect_support_matrix_command": "build-platform-support-matrix",
    "package_command": "package-runnable-toolchain",
    "package_channels_command": "build-package-channels",
    "packaging_validation_command": "validate-packaging-channels",
    "packaging_end_to_end_command": "validate-packaging-channels-end-to-end",
    "platform_hardening_validation_command": "validate-platform-hardening",
    "platform_hardening_end_to_end_command": "validate-platform-hardening-end-to-end",
    "release_operations_command": "validate-release-operations",
    "release_operations_end_to_end_command": "validate-release-operations-end-to-end",
}

PLATFORM_HARDENING_OWNER_POLICY: dict[str, object] = {
    "channel_owner": "packaging-channels-source",
    "platform_support_owner": "platform-hardening-support-source",
    "installer_validation_owner": "platform-hardening-install-validation",
    "build_package_validation_owner": "platform-hardening-build-package-validation",
    "toolchain_archive_claim_owner": "platform-hardening-build-package-validation",
    "unsupported_host_failure_owner": "platform-hardening-unsupported-host-fail-closed",
    "blocker_owner": "platform-hardening-blockers",
    "source_authority": "checked-in-platform-hardening-contracts",
    "evidence_log_allowed": False,
}

PLATFORM_HARDENING_OWNER_FIELDS: tuple[str, ...] = tuple(PLATFORM_HARDENING_OWNER_POLICY)

PLATFORM_HARDENING_SUMMARY_BUILDERS: tuple[Path, ...] = (
    ROOT / "scripts" / "build_platform_hardening_boundary_inventory_summary.py",
    ROOT / "scripts" / "build_platform_hardening_support_tier_policy_summary.py",
    ROOT / "scripts" / "build_platform_hardening_unsupported_host_policy_summary.py",
    ROOT / "scripts" / "build_platform_hardening_toolchain_archive_policy_summary.py",
    ROOT / "scripts" / "build_platform_hardening_artifact_contract_summary.py",
)

PLATFORM_HARDENING_SUMMARY_PATHS: tuple[Path, ...] = (
    BOUNDARY_INVENTORY_SUMMARY_PATH,
    SUPPORT_TIER_POLICY_SUMMARY_PATH,
    UNSUPPORTED_HOST_POLICY_SUMMARY_PATH,
    TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH,
    ARTIFACT_CONTRACT_SUMMARY_PATH,
)

SUPPORTED_HOST_ARCH_ALIASES: dict[str, set[str]] = {
    "windows-x64": {"amd64", "x86_64"},
}


__all__ = [
    "ARTIFACT_CONTRACT_SUMMARY_PATH",
    "BOUNDARY_INVENTORY_PATH",
    "BOUNDARY_INVENTORY_SUMMARY_PATH",
    "BUILD_PACKAGE_VALIDATION_CONTRACT_PATH",
    "BUILD_PACKAGE_VALIDATION_SCRIPT",
    "BUILD_PACKAGE_VALIDATION_SUMMARY_PATH",
    "BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT",
    "CHANNEL_CATALOG_PATH",
    "INSTALL_MATRIX_INTEGRATION_CONTRACT_PATH",
    "INSTALL_MATRIX_INTEGRATION_SCRIPT",
    "INSTALL_MATRIX_INTEGRATION_SUMMARY_PATH",
    "PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH",
    "PACKAGE_CHANNELS_SUMMARY_PATH",
    "PACKAGE_MANIFEST_PATH",
    "PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH",
    "PACKAGING_FIXTURE_ROOT",
    "PACKAGING_RUNBOOK_PATH",
    "PLATFORM_ARTIFACT_ROOT",
    "PLATFORM_FIXTURE_ROOT",
    "PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH",
    "PLATFORM_HARDENING_OWNER_FIELDS",
    "PLATFORM_HARDENING_OWNER_POLICY",
    "PLATFORM_HARDENING_SUMMARY_BUILDERS",
    "PLATFORM_HARDENING_SUMMARY_PATHS",
    "PLATFORM_MATRIX_ARTIFACT_CONTRACT_PATH",
    "PLATFORM_REPORT_ROOT",
    "PLATFORM_RUNBOOK_PATH",
    "PLATFORM_SUPPORT_MATRIX_SCHEMA_PATH",
    "PUBLICATION_SURFACE",
    "RELEASE_OPERATIONS_ROOT",
    "RELEASE_PUBLICATION_SUMMARY_PATH",
    "RELEASE_RUNBOOK_PATH",
    "ROOT",
    "RUNNABLE_END_TO_END_SUMMARY_PATH",
    "SUPPORT_MATRIX_ARTIFACT_PATH",
    "SUPPORT_MATRIX_SUMMARY_PATH",
    "SUPPORT_TIER_POLICY_PATH",
    "SUPPORT_TIER_POLICY_SUMMARY_PATH",
    "SUPPORTED_HOST_ARCH_ALIASES",
    "SUPPORTED_PLATFORMS_PATH",
    "TOOLCHAIN_ARCHIVE_POLICY_PATH",
    "TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH",
    "TOOLCHAIN_RANGE_REPLAY_CONTRACT_PATH",
    "TOOLCHAIN_RANGE_REPLAY_SCRIPT",
    "TOOLCHAIN_RANGE_REPLAY_SUMMARY_PATH",
    "UNSUPPORTED_HOST_POLICY_PATH",
    "UNSUPPORTED_HOST_POLICY_SUMMARY_PATH",
    "UPDATE_MANIFEST_PATH",
    "UPGRADE_SUPPORT_REPORT_PATH",
]
