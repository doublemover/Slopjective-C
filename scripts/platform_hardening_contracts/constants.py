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

PLATFORM_IDENTITY_CONTRACTS: dict[str, dict[str, object]] = {
    "windows-x64": {
        "host_os": "windows",
        "host_arch": "x64",
        "host_systems": ("windows",),
        "host_machines": ("amd64", "x86_64"),
        "host_triples": ("x86_64-pc-windows-msvc",),
        "promotion_state": "supported-boundary",
    },
    "linux-x64": {
        "host_os": "linux",
        "host_arch": "x64",
        "host_systems": ("linux",),
        "host_machines": ("amd64", "x86_64"),
        "host_triples": ("x86_64-unknown-linux-gnu",),
        "promotion_state": "fail-closed-until-native-host-evidence",
    },
    "darwin-arm64": {
        "host_os": "darwin",
        "host_arch": "arm64",
        "host_systems": ("darwin",),
        "host_machines": ("arm64", "aarch64"),
        "host_triples": ("aarch64-apple-darwin",),
        "promotion_state": "fail-closed-until-native-host-evidence",
    },
}

UNSUPPORTED_PROMOTION_PLATFORM_IDS: tuple[str, ...] = (
    "linux-x64",
    "darwin-arm64",
)

SUPPORTED_HOST_ARCH_ALIASES: dict[str, set[str]] = {
    "windows-x64": {"amd64", "x86_64"},
}

__all__ = [
    "PLATFORM_IDENTITY_CONTRACTS",
    "PLATFORM_HARDENING_OWNER_FIELDS",
    "PLATFORM_HARDENING_OWNER_POLICY",
    "PUBLICATION_SURFACE",
    "SUPPORTED_HOST_ARCH_ALIASES",
    "UNSUPPORTED_PROMOTION_PLATFORM_IDS",
]
