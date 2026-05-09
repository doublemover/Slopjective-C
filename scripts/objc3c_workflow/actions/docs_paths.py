"""Documentation and public-command workflow paths."""

from __future__ import annotations

from ..environment import ROOT

DOC_ACTION_MARKERS = (
    "docs",
    "documentation",
    "markdown",
    "site",
    "command-surface",
    "command-contract",
)

SITE_PY = ROOT / "scripts" / "build_site_index.py"
NATIVE_DOCS_PY = ROOT / "scripts" / "build_objc3c_native_docs.py"
PUBLIC_COMMAND_SURFACE_PY = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
PUBLIC_COMMAND_CONTRACT_PY = ROOT / "scripts" / "build_objc3c_public_command_contract.py"
PUBLIC_COMMAND_BUDGET_PY = ROOT / "scripts" / "check_objc3c_public_command_budget.py"
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
