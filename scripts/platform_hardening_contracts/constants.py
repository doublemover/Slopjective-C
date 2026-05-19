"""Stable literal constants for platform-hardening contracts."""

from __future__ import annotations

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

SUPPORTED_HOST_ARCH_ALIASES: dict[str, set[str]] = {
    "windows-x64": {"amd64", "x86_64"},
}

__all__ = [
    "PLATFORM_HARDENING_OWNER_FIELDS",
    "PLATFORM_HARDENING_OWNER_POLICY",
    "PUBLICATION_SURFACE",
    "SUPPORTED_HOST_ARCH_ALIASES",
]
