"""Packaging channels and platform hardening workflow actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .schema_surfaces import PACKAGING_CHANNELS_SCHEMA_SURFACE_PY

PACKAGING_CHANNELS_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_packaging_channels_source_surface.py"
)
PACKAGE_CHANNELS_BUILD_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
PACKAGING_CHANNELS_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"
)
PLATFORM_SUPPORT_MATRIX_PY = (
    ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
)
PLATFORM_HARDENING_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_platform_hardening_integration.py"
)
RUNNABLE_PLATFORM_HARDENING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_platform_hardening_end_to_end.py"
)


def action_check_packaging_channels_surface(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)])


def action_build_package_channels(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)])


def action_build_platform_support_matrix(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_SUPPORT_MATRIX_PY)])


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
