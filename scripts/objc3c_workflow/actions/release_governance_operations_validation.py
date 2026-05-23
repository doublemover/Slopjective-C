"""Release-operations validation actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from .release_governance_operations_paths import (
    PACKAGING_CHANNELS_INTEGRATION_PY,
    RELEASE_OPERATIONS_END_TO_END_PY,
    RELEASE_OPERATIONS_PUBLICATION_PY,
    RELEASE_OPERATIONS_SCHEMA_SURFACE_PY,
    RELEASE_OPERATIONS_SOURCE_SURFACE_PY,
    UPDATE_MANIFEST_PY,
)


def action_validate_release_operations(rest: list[str]) -> int:
    skip_upstream = False
    reuse_upstream_report = False
    if rest == ["--skip-upstream"]:
        skip_upstream = True
    elif rest == ["--reuse-upstream-report"]:
        reuse_upstream_report = True
    elif rest:
        raise RuntimeError(f"unexpected validate-release-operations arguments: {rest}")

    steps = []
    if not skip_upstream:
        packaging_channels_command = (
            [sys.executable, str(PACKAGING_CHANNELS_INTEGRATION_PY)]
            if reuse_upstream_report
            else workflow_command("validate-packaging-channels")
        )
        steps.append(("validate-packaging-channels", packaging_channels_command))
    steps.extend(
        [
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
        ]
    )
    rc = run_composite_validation(
        "validate-release-operations",
        steps,
    )
    if rc != 0:
        return rc
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY), "--skip-upstream"])


def action_validate_release_operations_end_to_end(rest: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_END_TO_END_PY), *rest])
