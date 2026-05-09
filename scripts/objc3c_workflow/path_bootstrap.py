"""Import-root bootstrap for direct workflow entrypoints."""

from __future__ import annotations

import sys
from pathlib import Path


WORKFLOW_PACKAGE_ROOT = Path(__file__).resolve().parent
SCRIPT_ROOT = WORKFLOW_PACKAGE_ROOT.parent
REPOSITORY_ROOT = SCRIPT_ROOT.parent
WORKFLOW_IMPORT_ROOTS = (REPOSITORY_ROOT, SCRIPT_ROOT)


def install_workflow_import_roots() -> None:
    for import_root in WORKFLOW_IMPORT_ROOTS:
        import_root_text = str(import_root)
        if import_root_text not in sys.path:
            sys.path.insert(0, import_root_text)
