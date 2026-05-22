"""Packaging-channel and platform-hardening validation actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from .release_governance_packaging_paths import (
    PACKAGE_CHANNELS_BUILD_PY,
    PACKAGING_CHANNELS_END_TO_END_PY,
    PACKAGING_CHANNELS_SOURCE_SURFACE_PY,
    PLATFORM_HARDENING_INTEGRATION_PY,
    RELEASE_FOUNDATION_INTEGRATION_PY,
    RUNNABLE_PLATFORM_HARDENING_E2E_PY,
)
from .schema_surfaces import PACKAGING_CHANNELS_SCHEMA_SURFACE_PY


def action_validate_packaging_channels(rest: list[str]) -> int:
    reuse_upstream_report = False
    if rest == ["--reuse-upstream-report"]:
        reuse_upstream_report = True
    elif rest:
        raise RuntimeError(f"unexpected validate-packaging-channels arguments: {rest}")

    release_foundation_command = (
        [sys.executable, str(RELEASE_FOUNDATION_INTEGRATION_PY)]
        if reuse_upstream_report
        else workflow_command("validate-release-foundation")
    )
    package_channels_build_command = [sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)]
    if reuse_upstream_report:
        package_channels_build_command.append("--reuse-release-foundation-artifacts")
    return run_composite_validation(
        "validate-packaging-channels",
        [
            ("validate-release-foundation", release_foundation_command),
            (
                "check-packaging-channels-surface",
                [sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)],
            ),
            (
                "check-packaging-channels-schema-surface",
                [sys.executable, str(PACKAGING_CHANNELS_SCHEMA_SURFACE_PY)],
            ),
            ("build-package-channels", package_channels_build_command),
        ],
    )


def action_validate_packaging_channels_end_to_end(rest: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_END_TO_END_PY), *rest])


def action_validate_platform_hardening(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_HARDENING_INTEGRATION_PY)])


def action_validate_platform_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PLATFORM_HARDENING_E2E_PY)])
