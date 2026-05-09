"""Path and import-root ownership for the objc3c workflow package."""

from __future__ import annotations

import sys
from pathlib import Path


WORKFLOW_PACKAGE_ROOT = Path(__file__).resolve().parent
SCRIPT_ROOT = WORKFLOW_PACKAGE_ROOT.parents[0]
ROOT = WORKFLOW_PACKAGE_ROOT.parents[1]


def workflow_import_roots() -> tuple[Path, Path]:
    return (ROOT, SCRIPT_ROOT)


def ensure_workflow_import_paths() -> None:
    for import_root in workflow_import_roots():
        import_root_text = str(import_root)
        if import_root_text not in sys.path:
            sys.path.insert(0, import_root_text)
