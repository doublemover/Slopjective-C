from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_public_test_composites import (
    PUBLIC_TEST_COMPOSITE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_public_test_fixtures import (
    PUBLIC_TEST_FIXTURE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_public_test_native import (
    PUBLIC_TEST_NATIVE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_public_tests import (
    PUBLIC_TEST_ACTION_SPECS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "action_catalog_public_test_composites",
    "action_catalog_public_test_native",
    "action_catalog_public_test_fixtures",
)


def test_public_test_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_public_tests.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text
    assert "python:scripts/" not in facade_text
    assert "pwsh:scripts/" not in facade_text


def test_public_test_catalog_preserves_action_order_and_owner_membership() -> None:
    assert tuple(PUBLIC_TEST_ACTION_SPECS) == (
        "test-default",
        "test-behavior-matrix",
        "test-smoke",
        "test-ci",
        "test-recovery",
        "test-compile-wrapper-self-audit",
        "test-llvm-capability-routing",
        "test-execution-smoke",
        "test-hosted-execution-smoke",
        "test-execution-replay",
        "test-execution-replay-focused",
        "test-fixture-matrix",
        "test-negative-expectations",
        "test-full",
        "test-nightly",
    )
    assert PUBLIC_TEST_ACTION_SPECS["test-smoke"] is PUBLIC_TEST_COMPOSITE_ACTION_SPECS[
        "test-smoke"
    ]
    assert PUBLIC_TEST_ACTION_SPECS["test-execution-smoke"] is (
        PUBLIC_TEST_NATIVE_ACTION_SPECS["test-execution-smoke"]
    )
    assert PUBLIC_TEST_ACTION_SPECS["test-fixture-matrix"] is (
        PUBLIC_TEST_FIXTURE_ACTION_SPECS["test-fixture-matrix"]
    )
