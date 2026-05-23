from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_core_docs import CORE_DOCS_ACTION_SPECS
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
from scripts.objc3c_workflow.actions import docs_documentation
from scripts.objc3c_workflow.actions import docs_paths
from scripts.objc3c_workflow.actions import docs_public_commands

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

CATALOG_OWNER_MODULES = (
    "action_catalog_core_site_docs",
    "action_catalog_core_native_docs",
    "action_catalog_core_public_commands",
    "action_catalog_core_markdown",
    "action_catalog_core_documentation_validation",
)

HANDLER_OWNER_MODULES = (
    "action_handlers_core_site_docs",
    "action_handlers_core_native_docs",
    "action_handlers_core_public_commands",
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
        "check-markdown",
        "format-markdown",
        "lint-markdown",
        "validate-umbrella-readiness",
    )
    assert "check-public-command-budget" in CORE_DOCS_ACTION_SPECS
    assert "validate-umbrella-readiness" in CORE_DOCS_ACTION_SPECS


def test_core_docs_child_catalogs_use_owner_constants() -> None:
    native_text = (WORKFLOW_ROOT / "action_catalog_core_native_docs.py").read_text(
        encoding="utf-8"
    )
    public_text = (WORKFLOW_ROOT / "action_catalog_core_public_commands.py").read_text(
        encoding="utf-8"
    )
    validation_text = (
        WORKFLOW_ROOT / "action_catalog_core_documentation_validation.py"
    ).read_text(encoding="utf-8")

    for catalog_text in (native_text, public_text, validation_text):
        assert "python:scripts/" not in catalog_text
        assert "validation_tier=\"docs\"" not in catalog_text
        assert "guarantee_owner=\"" not in catalog_text

    assert (
        CORE_NATIVE_DOCS_ACTION_SPECS[docs_documentation.CHECK_NATIVE_DOCS_ACTION].backend
        == docs_documentation.CHECK_NATIVE_DOCS_BACKEND
    )
    assert (
        CORE_PUBLIC_COMMAND_ACTION_SPECS[
            docs_public_commands.CHECK_PUBLIC_COMMAND_BUDGET_ACTION
        ].guarantee_owner
        == docs_public_commands.PUBLIC_COMMAND_BUDGET_GUARANTEE_OWNER
    )
    assert (
        CORE_DOCUMENTATION_VALIDATION_ACTION_SPECS[
            docs_documentation.VALIDATE_UMBRELLA_READINESS_ACTION
        ].guarantee_owner
        == docs_documentation.UMBRELLA_READINESS_GUARANTEE_OWNER
    )


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
        **CORE_MARKDOWN_ACTION_HANDLERS,
        **CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS,
    }
    assert "build-site" in CORE_DOCS_ACTION_HANDLERS
    assert "lint-markdown" in CORE_DOCS_ACTION_HANDLERS


def test_core_docs_child_handlers_use_direct_owner_modules() -> None:
    handler_expectations = {
        "action_handlers_core_native_docs.py": "actions.docs_documentation",
        "action_handlers_core_documentation_validation.py": "actions.docs_documentation",
        "action_handlers_core_public_commands.py": "actions.docs_public_commands",
    }

    for handler_file, owner_module in handler_expectations.items():
        handler_text = (WORKFLOW_ROOT / handler_file).read_text(encoding="utf-8")
        assert owner_module in handler_text
        assert "from scripts.objc3c_workflow.actions import docs" not in handler_text
        assert "docs.action_" not in handler_text


def test_core_docs_public_commands_and_paths_are_explicit_owner_surface() -> None:
    assert docs_documentation.BUILD_NATIVE_DOCS_ACTION == "build-native-docs"
    assert docs_documentation.CHECK_NATIVE_DOCS_ACTION == "check-native-docs"
    assert docs_documentation.VALIDATE_UMBRELLA_READINESS_ACTION == "validate-umbrella-readiness"
    assert (
        docs_public_commands.CHECK_PUBLIC_COMMAND_BUDGET_ACTION
        == "check-public-command-budget"
    )

    assert docs_paths.NATIVE_DOCS_SCRIPT == "scripts/build_objc3c_native_docs.py"
    assert (
        docs_paths.PUBLIC_COMMAND_SURFACE_SCRIPT
        == "scripts/render_objc3c_public_command_surface.py"
    )
    assert docs_paths.NATIVE_DOCS_OUTPUT_MD.relative_to(ROOT).as_posix() == (
        "docs/objc3c-native.md"
    )
    assert docs_paths.PUBLIC_COMMAND_SURFACE_OUTPUT_MD.relative_to(ROOT).as_posix() == (
        "docs/runbooks/objc3c_public_command_surface.md"
    )
    assert docs_paths.PUBLIC_WORKFLOW_REPORT_DIR.relative_to(ROOT).as_posix() == (
        "tmp/reports/objc3c-public-workflow"
    )

    assert docs_documentation.VALIDATE_UMBRELLA_READINESS_COMMAND[1] == (
        "scripts/check_objc3c_umbrella_readiness.py"
    )
