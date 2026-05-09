"""Packaging channel action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PACKAGING_CHANNEL_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-packaging-channels-surface": ActionSpec("check-packaging-channels-surface", "validate the checked-in packaging-channels source surface", "python:scripts/check_packaging_channels_source_surface.py", validation_tier="repo", guarantee_owner="packaging-channel publication stays rooted in checked-in channel, platform, and installer policy contracts"),
    "check-packaging-channels-schema-surface": ActionSpec("check-packaging-channels-schema-surface", "validate the checked-in packaging-channels schema surface", "python:scripts/check_packaging_channels_schema_surface.py", validation_tier="repo", guarantee_owner="packaging-channel and install-receipt artifacts stay on checked-in schema contracts"),
    "build-package-channels": ActionSpec("build-package-channels", "build the portable archive, installer image, and offline bundle channels from the live runnable payload", "python:scripts/build_objc3c_package_channels.py", validation_tier="repo", guarantee_owner="package channels stay derived from the live runnable package and release-foundation artifacts"),
    "validate-packaging-channels": ActionSpec("validate-packaging-channels", "run the integrated packaging-channels workflow", "runner-internal packaging-channel child actions", validation_tier="nightly", guarantee_owner="portable archive installer image and offline bundle generation stay executable on the live release surface"),
    "validate-packaging-channels-end-to-end": ActionSpec("validate-packaging-channels-end-to-end", "validate install bootstrap rollback and offline bundle behavior end to end", "python:scripts/check_objc3c_packaging_channels_end_to_end.py", validation_tier="full", guarantee_owner="packaging-channel artifacts stay installable rollback-safe and offline-bootstrappable under temp-owned roots"),
    "build-platform-support-matrix": ActionSpec("build-platform-support-matrix", "build the machine-owned platform support matrix artifact", "python:scripts/build_objc3c_platform_support_matrix.py", validation_tier="repo", guarantee_owner="the published host and channel support matrix stays machine-owned narrow and aligned with live packaging and release evidence"),
    "validate-platform-hardening": ActionSpec("validate-platform-hardening", "run the integrated platform-hardening workflow", "python:scripts/check_objc3c_platform_hardening_integration.py", validation_tier="nightly", guarantee_owner="supported host package install and release claims stay tiered package-backed and fail-closed outside the checked-in matrix"),
    "validate-platform-hardening-end-to-end": ActionSpec("validate-platform-hardening-end-to-end", "validate packaged platform-hardening publication and smoke behavior from the staged runnable bundle", "python:scripts/check_objc3c_runnable_platform_hardening_end_to_end.py", validation_tier="full", guarantee_owner="packaged platform support inspection and validation remain runnable from the staged toolchain bundle"),
}


__all__ = ["PACKAGING_CHANNEL_ACTION_SPECS"]
