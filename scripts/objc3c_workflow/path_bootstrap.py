"""Import-root bootstrap for direct workflow entrypoints."""

from __future__ import annotations

import sys

if __package__:
    from .paths import ROOT, SCRIPT_ROOT, WORKFLOW_PACKAGE_ROOT, workflow_import_roots
else:
    from paths import ROOT, SCRIPT_ROOT, WORKFLOW_PACKAGE_ROOT, workflow_import_roots


REPOSITORY_ROOT = ROOT
WORKFLOW_IMPORT_ROOTS = workflow_import_roots()


def install_workflow_import_roots() -> None:
    for import_root in WORKFLOW_IMPORT_ROOTS:
        import_root_text = str(import_root)
        if import_root_text not in sys.path:
            sys.path.insert(0, import_root_text)


__all__ = [
    "REPOSITORY_ROOT",
    "SCRIPT_ROOT",
    "WORKFLOW_IMPORT_ROOTS",
    "WORKFLOW_PACKAGE_ROOT",
    "install_workflow_import_roots",
]
