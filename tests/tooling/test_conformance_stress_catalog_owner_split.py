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
from scripts.objc3c_workflow.action_catalog_external_validation_publication import (
    EXTERNAL_VALIDATION_PUBLICATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_external_validation_replay import (
    EXTERNAL_VALIDATION_REPLAY_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_external_validation_source import (
    EXTERNAL_VALIDATION_SOURCE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_external_validation_workflow import (
    EXTERNAL_VALIDATION_WORKFLOW_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_stress import STRESS_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_stress_runtime import (
    STRESS_RUNTIME_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_stress_source import (
    STRESS_SOURCE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_stress_validation import (
    STRESS_VALIDATION_ACTION_SPECS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "action_catalog_conformance",
    "action_catalog_stress",
    "action_catalog_external_validation",
)

STRESS_OWNER_MODULES = (
    "action_catalog_stress_source",
    "action_catalog_stress_runtime",
    "action_catalog_stress_validation",
)

EXTERNAL_VALIDATION_OWNER_MODULES = (
    "action_catalog_external_validation_source",
    "action_catalog_external_validation_replay",
    "action_catalog_external_validation_publication",
    "action_catalog_external_validation_workflow",
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


def test_stress_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_stress.py").read_text(
        encoding="utf-8"
    )

    for module_name in STRESS_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text
    assert "python:scripts/" not in facade_text


def test_external_validation_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_external_validation.py").read_text(
        encoding="utf-8"
    )

    for module_name in EXTERNAL_VALIDATION_OWNER_MODULES:
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


def test_stress_catalog_preserves_public_order_and_owner_membership() -> None:
    assert tuple(STRESS_ACTION_SPECS) == (
        "check-stress-surface",
        "test-fuzz-safety",
        "test-lowering-runtime-stress",
        "test-mixed-module-differential",
        "test-stress-minimization",
        "test-stress-crash-triage",
        "validate-stress",
        "validate-stress-integration",
        "validate-stress-end-to-end",
    )
    assert STRESS_ACTION_SPECS["check-stress-surface"] is STRESS_SOURCE_ACTION_SPECS[
        "check-stress-surface"
    ]
    assert STRESS_ACTION_SPECS["test-lowering-runtime-stress"] is (
        STRESS_RUNTIME_ACTION_SPECS["test-lowering-runtime-stress"]
    )
    assert STRESS_ACTION_SPECS["validate-stress"] is (
        STRESS_VALIDATION_ACTION_SPECS["validate-stress"]
    )


def test_external_validation_catalog_preserves_public_order_and_owner_membership() -> None:
    assert tuple(EXTERNAL_VALIDATION_ACTION_SPECS) == (
        "check-external-validation-surface",
        "test-external-validation-replay",
        "publish-external-repro-corpus",
        "validate-external-validation",
        "validate-external-validation-integration",
    )
    assert EXTERNAL_VALIDATION_ACTION_SPECS["check-external-validation-surface"] is (
        EXTERNAL_VALIDATION_SOURCE_ACTION_SPECS["check-external-validation-surface"]
    )
    assert EXTERNAL_VALIDATION_ACTION_SPECS["test-external-validation-replay"] is (
        EXTERNAL_VALIDATION_REPLAY_ACTION_SPECS["test-external-validation-replay"]
    )
    assert EXTERNAL_VALIDATION_ACTION_SPECS["publish-external-repro-corpus"] is (
        EXTERNAL_VALIDATION_PUBLICATION_ACTION_SPECS["publish-external-repro-corpus"]
    )
    assert EXTERNAL_VALIDATION_ACTION_SPECS["validate-external-validation"] is (
        EXTERNAL_VALIDATION_WORKFLOW_ACTION_SPECS["validate-external-validation"]
    )
