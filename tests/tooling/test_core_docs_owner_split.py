from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_core_docs import CORE_DOCS_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_core_documentation_surface import (
    CORE_DOCUMENTATION_SURFACE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_documentation_validation import (
    CORE_DOCUMENTATION_VALIDATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_markdown import (
    CORE_MARKDOWN_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_native_docs import (
    CORE_NATIVE_DOCS_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_public_commands import (
    CORE_PUBLIC_COMMAND_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_site_docs import (
    CORE_SITE_DOCS_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_handlers_core_docs import (
    CORE_DOCS_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_documentation_surface import (
    CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_documentation_validation import (
    CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_markdown import (
    CORE_MARKDOWN_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_native_docs import (
    CORE_NATIVE_DOCS_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_public_commands import (
    CORE_PUBLIC_COMMAND_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_site_docs import (
    CORE_SITE_DOCS_ACTION_HANDLERS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

CATALOG_OWNER_MODULES = (
    "action_catalog_core_site_docs",
    "action_catalog_core_native_docs",
    "action_catalog_core_public_commands",
    "action_catalog_core_documentation_surface",
    "action_catalog_core_markdown",
    "action_catalog_core_documentation_validation",
)

HANDLER_OWNER_MODULES = (
    "action_handlers_core_site_docs",
    "action_handlers_core_native_docs",
    "action_handlers_core_public_commands",
    "action_handlers_core_documentation_surface",
    "action_handlers_core_markdown",
    "action_handlers_core_documentation_validation",
)


def test_core_docs_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_core_docs.py").read_text(
        encoding="utf-8"
    )

    for module_name in CATALOG_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text


def test_core_docs_catalog_preserves_owner_aggregation() -> None:
    assert CORE_DOCS_ACTION_SPECS == {
        **CORE_SITE_DOCS_ACTION_SPECS,
        **CORE_NATIVE_DOCS_ACTION_SPECS,
        **CORE_PUBLIC_COMMAND_ACTION_SPECS,
        **CORE_DOCUMENTATION_SURFACE_ACTION_SPECS,
        **CORE_MARKDOWN_ACTION_SPECS,
        **CORE_DOCUMENTATION_VALIDATION_ACTION_SPECS,
    }
    assert tuple(CORE_DOCS_ACTION_SPECS) == (
        "build-site",
        "check-site",
        "build-native-docs",
        "check-native-docs",
        "build-public-command-surface",
        "check-public-command-surface",
        "build-public-command-contract",
        "check-public-command-contract",
        "check-public-command-budget",
        "check-documentation-surface",
        "check-markdown",
        "format-markdown",
        "lint-markdown",
        "validate-documentation-surface",
    )
    assert "check-public-command-budget" in CORE_DOCS_ACTION_SPECS
    assert "validate-documentation-surface" in CORE_DOCS_ACTION_SPECS


def test_core_docs_handler_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_handlers_core_docs.py").read_text(
        encoding="utf-8"
    )

    for module_name in HANDLER_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"scripts.objc3c_workflow.{module_name}" in facade_text
    assert "docs.action_" not in facade_text


def test_core_docs_handler_preserves_owner_aggregation() -> None:
    assert CORE_DOCS_ACTION_HANDLERS == {
        **CORE_SITE_DOCS_ACTION_HANDLERS,
        **CORE_NATIVE_DOCS_ACTION_HANDLERS,
        **CORE_PUBLIC_COMMAND_ACTION_HANDLERS,
        **CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS,
        **CORE_MARKDOWN_ACTION_HANDLERS,
        **CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS,
    }
    assert "build-site" in CORE_DOCS_ACTION_HANDLERS
    assert "lint-markdown" in CORE_DOCS_ACTION_HANDLERS
