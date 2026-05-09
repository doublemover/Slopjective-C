"""Release-operations artifact actions."""

from __future__ import annotations

import sys

from ..commands import run
from .release_governance_operations_paths import (
    RELEASE_OPERATIONS_SCHEMA_SURFACE_PY,
    RELEASE_OPERATIONS_PUBLICATION_PY,
    RELEASE_OPERATIONS_SOURCE_SURFACE_PY,
    UPDATE_MANIFEST_PY,
)


def action_check_release_operations_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_SOURCE_SURFACE_PY)])


def action_check_release_operations_schema_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_SCHEMA_SURFACE_PY)])


def action_build_update_manifest(_: list[str]) -> int:
    return run([sys.executable, str(UPDATE_MANIFEST_PY)])


def action_publish_release_operations(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_PUBLICATION_PY)])
