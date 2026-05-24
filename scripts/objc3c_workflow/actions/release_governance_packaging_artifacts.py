"""Packaging-channel artifact workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_packaging_paths import (
    PACKAGE_CHANNELS_BUILD_PY,
    PACKAGING_CHANNELS_SOURCE_SURFACE_PY,
    PLATFORM_HOST_EVIDENCE_INGESTION_PY,
    PLATFORM_HOST_EVIDENCE_REVIEW_PY,
    PLATFORM_HOST_PROMOTION_EVIDENCE_PY,
    PLATFORM_SUPPORT_MATRIX_PY,
)


def action_check_packaging_channels_surface(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGING_CHANNELS_SOURCE_SURFACE_PY)])


def action_build_package_channels(rest: list[str]) -> int:
    if rest[:1] == ["--"]:
        rest = rest[1:]
    return run([sys.executable, str(PACKAGE_CHANNELS_BUILD_PY), *rest])


def action_build_package_channels_asan(_: list[str]) -> int:
    return run(
        [
            sys.executable,
            str(PACKAGE_CHANNELS_BUILD_PY),
            "--sanitizer-variant",
            "address",
        ]
    )


def action_build_package_channels_ubsan(_: list[str]) -> int:
    return run(
        [
            sys.executable,
            str(PACKAGE_CHANNELS_BUILD_PY),
            "--sanitizer-variant",
            "undefined",
        ]
    )


def action_build_platform_support_matrix(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_SUPPORT_MATRIX_PY)])


def action_ingest_platform_host_evidence(rest: list[str]) -> int:
    if rest[:1] == ["--"]:
        rest = rest[1:]
    return run([sys.executable, str(PLATFORM_HOST_EVIDENCE_INGESTION_PY), *rest])


def action_review_platform_host_evidence(rest: list[str]) -> int:
    if rest[:1] == ["--"]:
        rest = rest[1:]
    return run([sys.executable, str(PLATFORM_HOST_EVIDENCE_REVIEW_PY), *rest])


def action_check_platform_host_promotion_evidence(_: list[str]) -> int:
    return run([sys.executable, str(PLATFORM_HOST_PROMOTION_EVIDENCE_PY)])
