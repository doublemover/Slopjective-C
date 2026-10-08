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
        pass_through_args=True,
    ),
    "package-runnable-toolchain-asan": ActionSpec(
        "package-runnable-toolchain-asan",
        "package the runnable native toolchain with the reserved ASan runtime selector",
        "pwsh:scripts/package_objc3c_runnable_toolchain.ps1 -SanitizerVariant address",
        validation_tier="repo",
        guarantee_owner=(
            "packaging-channels-source -> packaging-channels-gate -> "
            "packaging-channels-blockers: ASan package output uses an explicit "
            "sanitizer selector, emits install-receipt metadata, and remains "
            "reserved until package install and native execution evidence exists"
        ),
    ),
    "package-runnable-toolchain-ubsan": ActionSpec(
        "package-runnable-toolchain-ubsan",
        "package the runnable native toolchain with the reserved UBSan runtime selector",
        "pwsh:scripts/package_objc3c_runnable_toolchain.ps1 -SanitizerVariant undefined",
        validation_tier="repo",
        guarantee_owner=(
            "packaging-channels-source -> packaging-channels-gate -> "
            "packaging-channels-blockers: UBSan package output uses an explicit "
            "sanitizer selector, emits install-receipt metadata, and remains "
            "reserved until package install and native execution evidence exists"
        ),
    ),
}

__all__ = [
    "NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS",
    "NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY",
]
