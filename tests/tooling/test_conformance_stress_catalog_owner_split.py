from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_conformance import (
    CONFORMANCE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_conformance_stress import (
    CONFORMANCE_STRESS_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_external_validation import (
    EXTERNAL_VALIDATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_stress import STRESS_ACTION_SPECS

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "action_catalog_conformance",
    "action_catalog_stress",
    "action_catalog_external_validation",
)


def test_conformance_stress_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_conformance_stress.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text
    assert "python:scripts/" not in facade_text


def test_conformance_stress_catalog_preserves_public_order() -> None:
    expected = {
        **CONFORMANCE_ACTION_SPECS,
        **STRESS_ACTION_SPECS,
        **EXTERNAL_VALIDATION_ACTION_SPECS,
    }

    assert CONFORMANCE_STRESS_ACTION_SPECS == expected
    assert tuple(CONFORMANCE_STRESS_ACTION_SPECS)[:3] == tuple(
        CONFORMANCE_ACTION_SPECS
    )
    assert "validate-stress" in CONFORMANCE_STRESS_ACTION_SPECS
    assert "validate-external-validation" in CONFORMANCE_STRESS_ACTION_SPECS
