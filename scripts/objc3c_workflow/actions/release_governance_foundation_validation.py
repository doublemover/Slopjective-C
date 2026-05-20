"""Release-foundation validation actions."""

from __future__ import annotations

import sys

from ..commands import workflow_command
from ..composite_validation import run_composite_validation
from .release_governance_foundation_paths import (
    RELEASE_ABI_API_DRIFT_PY,
    RELEASE_EVIDENCE_PY,
    RELEASE_MANIFEST_PY,
    RELEASE_PROVENANCE_PY,
)
from .schema_surfaces import RELEASE_FOUNDATION_SCHEMA_SURFACE_PY


def action_validate_release_foundation(_: list[str]) -> int:
    return run_composite_validation(
        "validate-release-foundation",
        [
            ("validate-performance-governance", workflow_command("validate-performance-governance")),
            ("validate-runnable-release-candidate", workflow_command("validate-runnable-release-candidate")),
            (
                "check-release-evidence",
                [sys.executable, str(RELEASE_EVIDENCE_PY)],
            ),
            (
                "check-release-foundation-surface",
                workflow_command("check-release-foundation-surface"),
            ),
            (
                "check-release-foundation-schema-surface",
                [sys.executable, str(RELEASE_FOUNDATION_SCHEMA_SURFACE_PY)],
            ),
            (
                "check-release-abi-api-drift",
                [sys.executable, str(RELEASE_ABI_API_DRIFT_PY)],
            ),
            ("build-release-manifest", [sys.executable, str(RELEASE_MANIFEST_PY)]),
            (
                "publish-release-provenance",
                [sys.executable, str(RELEASE_PROVENANCE_PY)],
            ),
        ],
    )
