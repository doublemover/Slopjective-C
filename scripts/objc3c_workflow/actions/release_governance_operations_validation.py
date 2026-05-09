"""Release-operations validation actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from .release_governance_operations_paths import (
    RELEASE_OPERATIONS_END_TO_END_PY,
    RELEASE_OPERATIONS_PUBLICATION_PY,
    RELEASE_OPERATIONS_SCHEMA_SURFACE_PY,
    RELEASE_OPERATIONS_SOURCE_SURFACE_PY,
    UPDATE_MANIFEST_PY,
)


def action_validate_release_operations(_: list[str]) -> int:
    rc = run_composite_validation(
        "validate-release-operations",
        [
            ("validate-packaging-channels", workflow_command("validate-packaging-channels")),
            (
                "check-release-operations-surface",
                [sys.executable, str(RELEASE_OPERATIONS_SOURCE_SURFACE_PY)],
            ),
            (
                "check-release-operations-schema-surface",
                [sys.executable, str(RELEASE_OPERATIONS_SCHEMA_SURFACE_PY)],
            ),
            ("build-update-manifest", [sys.executable, str(UPDATE_MANIFEST_PY)]),
            (
                "publish-release-operations",
                [sys.executable, str(RELEASE_OPERATIONS_PUBLICATION_PY)],
            ),
        ],
    )
    if rc != 0:
        return rc
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY), "--skip-upstream"])


def action_validate_release_operations_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY)])
