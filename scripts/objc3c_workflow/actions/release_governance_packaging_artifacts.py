"""Packaging-channel artifact workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_packaging_paths import (
    PACKAGE_CHANNELS_BUILD_PY,
    PACKAGING_CHANNELS_SOURCE_SURFACE_PY,
    PLATFORM_SUPPORT_MATRIX_PY,
)


def action_check_packaging_channels_surface(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)])


def action_build_package_channels(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_CHANNELS_BUILD_PY)])


def action_build_platform_support_matrix(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_SUPPORT_MATRIX_PY)])
