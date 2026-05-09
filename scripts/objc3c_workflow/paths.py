"""Path and import-root ownership for the objc3c workflow package."""

from __future__ import annotations

from pathlib import Path

from .path_imports import install_import_roots


WORKFLOW_PACKAGE_ROOT = Path(__file__).resolve().parent
SCRIPT_ROOT = WORKFLOW_PACKAGE_ROOT.parents[0]
ROOT = WORKFLOW_PACKAGE_ROOT.parents[1]


def workflow_import_roots() -> tuple[Path, Path]:
    return (ROOT, SCRIPT_ROOT)


def ensure_workflow_import_paths() -> None:
    install_import_roots(workflow_import_roots())


__all__ = [
    "ROOT",
    "SCRIPT_ROOT",
    "WORKFLOW_PACKAGE_ROOT",
    "ensure_workflow_import_paths",
    "workflow_import_roots",
]
