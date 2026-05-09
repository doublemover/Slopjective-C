"""Release-foundation artifact actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_foundation_paths import (
    RELEASE_FOUNDATION_SOURCE_SURFACE_PY,
    RELEASE_MANIFEST_PY,
    RELEASE_PROVENANCE_PY,
)


def action_check_release_foundation_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)])


def action_build_release_manifest(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_MANIFEST_PY)])


def action_publish_release_provenance(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_PROVENANCE_PY)])
