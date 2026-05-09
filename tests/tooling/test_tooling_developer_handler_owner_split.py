from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_handlers_ecosystem_publication import (
    ECOSYSTEM_PUBLICATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_tooling_developer import (
    DEVELOPER_ECOSYSTEM_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_tooling_hygiene import (
    TOOLING_HYGIENE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_tooling_inspection import (
    TOOLING_INSPECTION_ACTION_HANDLERS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

TOOLING_HANDLER_OWNER_MODULES = (
    "action_handlers_tooling_inspection",
    "action_handlers_ecosystem_publication",
    "action_handlers_tooling_hygiene",
)


def test_tooling_developer_handler_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_handlers_tooling_developer.py").read_text(
        encoding="utf-8"
    )

    for module_name in TOOLING_HANDLER_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"scripts.objc3c_workflow.{module_name}" in facade_text
    assert "developer_tooling.action_" not in facade_text
    assert "ecosystem_publication.action_" not in facade_text
    assert "hygiene.action_" not in facade_text


def test_tooling_developer_handler_preserves_owner_aggregation() -> None:
    assert DEVELOPER_ECOSYSTEM_ACTION_HANDLERS == {
        **TOOLING_INSPECTION_ACTION_HANDLERS,
        **ECOSYSTEM_PUBLICATION_ACTION_HANDLERS,
        **TOOLING_HYGIENE_ACTION_HANDLERS,
    }
    assert "inspect-bonus-tool-integration" in DEVELOPER_ECOSYSTEM_ACTION_HANDLERS
    assert "validate-package-ecosystem" in DEVELOPER_ECOSYSTEM_ACTION_HANDLERS
    assert "lint-spec" in DEVELOPER_ECOSYSTEM_ACTION_HANDLERS
