"""Release-foundation artifact actions."""

from __future__ import annotations

import sys

from scripts.objc3c_workflow.action_execution_dispatch import execute_registered_action

from ..commands import run
from .release_governance_foundation_paths import (
    RELEASE_ABI_API_DRIFT_PY,
    RELEASE_FOUNDATION_SOURCE_SURFACE_PY,
    RELEASE_MANIFEST_PY,
    RELEASE_PROVENANCE_PY,
)


REPO_SUPERCLEAN_REFRESH_ACTION = "check-repo-superclean-surface"


def refresh_release_foundation_generated_upstreams() -> int:
    return execute_registered_action(REPO_SUPERCLEAN_REFRESH_ACTION, [])


def action_check_release_foundation_surface(_: list[str]) -> int:
    rc = refresh_release_foundation_generated_upstreams()
    if rc != 0:
        return rc
    return run([sys.executable, str(RELEASE_FOUNDATION_SOURCE_SURFACE_PY)])


def action_check_release_abi_api_drift(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_ABI_API_DRIFT_PY)])


def action_build_release_manifest(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_MANIFEST_PY)])


def action_publish_release_provenance(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_PROVENANCE_PY)])
