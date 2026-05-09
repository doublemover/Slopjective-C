from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_spec_lint import SPEC_LINT_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_tooling_developer import (
    TOOLING_DEVELOPER_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_tooling_inspection import (
    TOOLING_INSPECTION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_tooling_materialization import (
    TOOLING_MATERIALIZATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_tooling_parity import (
    TOOLING_PARITY_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_tooling_validation import (
    TOOLING_VALIDATION_ACTION_SPECS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "action_catalog_tooling_inspection",
    "action_catalog_tooling_materialization",
    "action_catalog_tooling_parity",
    "action_catalog_tooling_validation",
    "action_catalog_spec_lint",
)


def test_tooling_developer_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_tooling_developer.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text
    assert "python:scripts/" not in facade_text


def test_tooling_developer_catalog_preserves_owner_order() -> None:
    assert TOOLING_DEVELOPER_ACTION_SPECS == {
        **TOOLING_INSPECTION_ACTION_SPECS,
        **TOOLING_MATERIALIZATION_ACTION_SPECS,
        **TOOLING_PARITY_ACTION_SPECS,
        **TOOLING_VALIDATION_ACTION_SPECS,
        **SPEC_LINT_ACTION_SPECS,
    }
    assert tuple(TOOLING_DEVELOPER_ACTION_SPECS) == (
        "inspect-bonus-tool-integration",
        "inspect-validation-timing",
        "trace-compile-stages",
        "materialize-project-template",
        "test-capability-routed-source-parity",
        "validate-developer-tooling",
        "validate-bonus-experiences",
        "validate-runnable-bonus-experiences",
        "lint-spec",
    )
