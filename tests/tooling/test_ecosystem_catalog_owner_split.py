from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_package_ecosystem import (
    PACKAGE_ECOSYSTEM_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_integration import (
    PACKAGE_INTEGRATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_lock import (
    PACKAGE_LOCK_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_registry import (
    PACKAGE_REGISTRY_ACTION_SPECS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"


PACKAGE_OWNER_MODULES = (
    "action_catalog_package_lock",
    "action_catalog_package_registry",
    "action_catalog_package_integration",
)


def test_package_ecosystem_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_package_ecosystem.py").read_text(
        encoding="utf-8"
    )

    for module_name in PACKAGE_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text


def test_package_ecosystem_catalog_preserves_owner_aggregation() -> None:
    assert PACKAGE_ECOSYSTEM_ACTION_SPECS == {
        **PACKAGE_LOCK_ACTION_SPECS,
        **PACKAGE_REGISTRY_ACTION_SPECS,
        **PACKAGE_INTEGRATION_ACTION_SPECS,
    }
    assert "build-package-lock" in PACKAGE_ECOSYSTEM_ACTION_SPECS
    assert "validate-runnable-package-ecosystem" in PACKAGE_ECOSYSTEM_ACTION_SPECS
