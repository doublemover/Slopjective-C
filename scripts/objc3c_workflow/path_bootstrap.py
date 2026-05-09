"""Import-root bootstrap for direct workflow entrypoints."""

from __future__ import annotations

if __package__:
    from .path_imports import install_import_roots
    from .paths import ROOT, SCRIPT_ROOT, WORKFLOW_PACKAGE_ROOT, workflow_import_roots
else:
    from path_imports import install_import_roots
    from paths import ROOT, SCRIPT_ROOT, WORKFLOW_PACKAGE_ROOT, workflow_import_roots


REPOSITORY_ROOT = ROOT
WORKFLOW_IMPORT_ROOTS = workflow_import_roots()


def install_workflow_import_roots() -> None:
    install_import_roots(WORKFLOW_IMPORT_ROOTS)


__all__ = [
    "REPOSITORY_ROOT",
    "SCRIPT_ROOT",
    "WORKFLOW_IMPORT_ROOTS",
    "WORKFLOW_PACKAGE_ROOT",
    "install_workflow_import_roots",
]
