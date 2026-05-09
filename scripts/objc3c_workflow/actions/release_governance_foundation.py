"""Release foundation workflow actions."""

from __future__ import annotations

import sys

from ..commands import run, workflow_command
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .schema_surfaces import RELEASE_FOUNDATION_SCHEMA_SURFACE_PY

RELEASE_FOUNDATION_SOURCE_SURFACE_PY = (
    ROOT / "scripts" / "check_release_foundation_source_surface.py"
)
RELEASE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_release_manifest.py"
RELEASE_PROVENANCE_PY = ROOT / "scripts" / "publish_objc3c_release_provenance.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"


def action_check_release_foundation_surface(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)])


def action_build_release_manifest(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_MANIFEST_PY)])


def action_publish_release_provenance(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_PROVENANCE_PY)])


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
                [sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)],
            ),
            (
                "check-release-foundation-schema-surface",
                [sys.executable, str(RELEASE_FOUNDATION_SCHEMA_SURFACE_PY)],
            ),
            ("build-release-manifest", [sys.executable, str(RELEASE_MANIFEST_PY)]),
            (
                "publish-release-provenance",
                [sys.executable, str(RELEASE_PROVENANCE_PY)],
            ),
        ],
    )
