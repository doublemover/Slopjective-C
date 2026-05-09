"""Native runnable toolchain package action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY: dict[str, object] = {
    "source_owner": "packaging-channels-source",
    "gate_owner": "packaging-channels-gate",
    "blocker_owner": "packaging-channels-blockers",
    "payload_owner_action": "package-runnable-toolchain",
    "unsupported_host_success_allowed": False,
    "toolchain_archive_claim_owner": "platform-hardening-build-package-validation",
    "toolchain_archive_claim_requires_owner": True,
    "wrapper_only_action_surface_allowed": False,
}

NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS: dict[str, ActionSpec] = {
    "package-runnable-toolchain": ActionSpec(
        "package-runnable-toolchain",
        "package the runnable native toolchain",
        "pwsh:scripts/package_objc3c_runnable_toolchain.ps1",
        validation_tier="repo",
        guarantee_owner=(
            "packaging-channels-source -> packaging-channels-gate -> "
            "packaging-channels-blockers: runnable native toolchain package output "
            "stays rooted in the checked-in package script, unsupported hosts cannot "
            "succeed, and toolchain archive claims require "
            "platform-hardening-build-package-validation ownership"
        ),
    ),
}

__all__ = [
    "NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS",
    "NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY",
]
