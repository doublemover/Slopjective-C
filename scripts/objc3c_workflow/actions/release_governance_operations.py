"""Release operations workflow actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .schema_surfaces import RELEASE_OPERATIONS_SCHEMA_SURFACE_PY

RELEASE_OPERATIONS_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_release_operations_source_surface.py"
)
UPDATE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_update_manifest.py"
RELEASE_OPERATIONS_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_release_operations_metadata.py"
)
RELEASE_OPERATIONS_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_release_operations_end_to_end.py"
)


def action_check_release_operations_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_SOURCE_SURFACE_PY)])


def action_build_update_manifest(_: list[str]) -> int:
    return run([sys.executable, str(UPDATE_MANIFEST_PY)])


def action_publish_release_operations(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_OPERATIONS_PUBLICATION_PY)])


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
