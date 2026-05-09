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
    RUNNABLE_PLATFORM_HARDENING_E2E_PY,
)
from .schema_surfaces import PACKAGING_CHANNELS_SCHEMA_SURFACE_PY


def action_validate_packaging_channels(_: list[str]) -> int:
    return run_composite_validation(
        "validate-packaging-channels",
        [
            ("validate-release-foundation", workflow_command("validate-release-foundation")),
            (
                "check-packaging-channels-surface",
                [sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)],
            ),
            (
                "check-packaging-channels-schema-surface",
                [sys.executable, str(PACKAGING_CHANNELS_SCHEMA_SURFACE_PY)],
            ),
            ("build-package-channels", [sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)]),
        ],
    )


def action_validate_packaging_channels_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_END_TO_END_PY)])


def action_validate_platform_hardening(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_HARDENING_INTEGRATION_PY)])


def action_validate_platform_hardening_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PLATFORM_HARDENING_E2E_PY)])
