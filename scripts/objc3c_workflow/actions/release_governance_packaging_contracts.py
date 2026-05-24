"""Packaging-channel release governance action contracts."""

from __future__ import annotations

from .release_governance_owner_models import ReleaseGovernanceActionContract


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
        "build-package-channels-asan",
        "build reserved ASan portable archive, installer image, and offline bundle channels from the live runnable payload",
        "python:scripts/build_objc3c_package_channels.py --sanitizer-variant address",
        "packaging-channels",
        "repo",
        "ASan package channels stay explicit-opt-in, install-receipt backed, and reserved until native execution evidence exists",
    ),
    ReleaseGovernanceActionContract(
        "build-package-channels-ubsan",
        "build reserved UBSan portable archive, installer image, and offline bundle channels from the live runnable payload",
        "python:scripts/build_objc3c_package_channels.py --sanitizer-variant undefined",
        "packaging-channels",
        "repo",
        "UBSan package channels stay explicit-opt-in, install-receipt backed, and reserved until native execution evidence exists",
    ),
    ReleaseGovernanceActionContract(
        "validate-packaging-channels",
        "run the integrated packaging-channels workflow",
        "runner-internal packaging-channel child actions",
        "packaging-channels",
        "nightly",
        "portable archive installer image and offline bundle generation stay executable on the live release surface",
        pass_through_args=True,
    ),
    ReleaseGovernanceActionContract(
        "validate-packaging-channels-end-to-end",
        "validate install bootstrap revert and offline bundle behavior end to end",
        "python:scripts/check_objc3c_packaging_channels_end_to_end.py",
        "packaging-channels",
        "full",
        "packaging-channel artifacts stay installable revert-safe and offline-bootstrappable under temp-owned roots",
        pass_through_args=True,
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
        "ingest-platform-host-evidence",
        "ingest generated Linux/macOS hosted-runner evidence without source-truth promotion",
        "python:scripts/ingest_objc3c_platform_host_evidence.py",
        "packaging-channels",
        "ci",
        "generated hosted-runner build package install and execution reports stay review-only until checked source truth is promoted",
        pass_through_args=True,
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


__all__ = ["PACKAGING_CHANNEL_ACTION_CONTRACTS"]
