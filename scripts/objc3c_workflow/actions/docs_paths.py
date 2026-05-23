"""Documentation and public-command workflow paths."""

from __future__ import annotations

from ..environment import ROOT

DOC_ACTION_MARKERS = (
    "docs",
    "documentation",
    "markdown",
    "site",
    "native-docs",
    "public-command",
    "command-surface",
    "command-contract",
)

SITE_SCRIPT = "scripts/build_site_index.py"
NATIVE_DOCS_SCRIPT = "scripts/build_objc3c_native_docs.py"
PUBLIC_COMMAND_SURFACE_SCRIPT = "scripts/render_objc3c_public_command_surface.py"
PUBLIC_COMMAND_CONTRACT_SCRIPT = "scripts/build_objc3c_public_command_contract.py"
PUBLIC_COMMAND_BUDGET_SCRIPT = "scripts/check_objc3c_public_command_budget.py"

SITE_PY = ROOT / SITE_SCRIPT
NATIVE_DOCS_PY = ROOT / NATIVE_DOCS_SCRIPT
PUBLIC_COMMAND_SURFACE_PY = ROOT / PUBLIC_COMMAND_SURFACE_SCRIPT
PUBLIC_COMMAND_CONTRACT_PY = ROOT / PUBLIC_COMMAND_CONTRACT_SCRIPT
PUBLIC_COMMAND_BUDGET_PY = ROOT / PUBLIC_COMMAND_BUDGET_SCRIPT

SITE_SOURCE_DIR = ROOT / "site" / "src"
SITE_OUTPUT_MD = ROOT / "site" / "index.md"
NATIVE_DOCS_SOURCE_DIR = ROOT / "docs" / "objc3c-native" / "src"
NATIVE_DOCS_OUTPUT_MD = ROOT / "docs" / "objc3c-native.md"
PUBLIC_COMMAND_SURFACE_OUTPUT_MD = (
    ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
)
PUBLIC_COMMAND_CONTRACT_JSON = (
    ROOT
    / "tmp"
    / "artifacts"
    / "public-command-surface"
    / "objc3c-public-command-contract.json"
)
PUBLIC_WORKFLOW_REPORT_DIR = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
DOCUMENTATION_BUILD_OUTPUTS = (
    SITE_OUTPUT_MD,
    NATIVE_DOCS_OUTPUT_MD,
    PUBLIC_COMMAND_SURFACE_OUTPUT_MD,
)
DOCUMENTATION_REPORT_PATHS = (
    PUBLIC_COMMAND_CONTRACT_JSON,
    PUBLIC_WORKFLOW_REPORT_DIR,
)
