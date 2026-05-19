"""Owned path and import-root policy for workflow entrypoints."""

from __future__ import annotations

from collections.abc import Iterable
from pathlib import Path

WORKFLOW_PATH_POLICY_OWNER = "objc3c-workflow-path-policy"
WORKFLOW_IMPORT_ROOT_POLICY_OWNER = "objc3c-workflow-import-root-policy"


def ordered_import_roots(root: Path, script_root: Path) -> tuple[Path, Path]:
    return (root, script_root)


def import_root_text(import_root: Path) -> str:
    return str(import_root)


def missing_import_roots(path_entries: Iterable[str], import_roots: Iterable[Path]) -> list[str]:
    existing = set(path_entries)
    return [import_root_text(import_root) for import_root in import_roots if import_root_text(import_root) not in existing]


__all__ = [
    "WORKFLOW_IMPORT_ROOT_POLICY_OWNER",
    "WORKFLOW_PATH_POLICY_OWNER",
    "import_root_text",
    "missing_import_roots",
    "ordered_import_roots",
]
