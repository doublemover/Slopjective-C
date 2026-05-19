from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_handlers_core_capabilities import (
    CORE_CAPABILITY_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_hygiene import (
    CORE_HYGIENE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_linting import (
    CORE_LINTING_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_source_hygiene import (
    CORE_SOURCE_HYGIENE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_superclean import (
    CORE_SUPERCLEAN_ACTION_HANDLERS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "action_handlers_core_linting",
    "action_handlers_core_capabilities",
    "action_handlers_core_source_hygiene",
    "action_handlers_core_superclean",
)


def test_core_hygiene_handlers_are_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_handlers_core_hygiene.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from scripts.objc3c_workflow.{module_name} import" in facade_text
    assert "developer_tooling.action_" not in facade_text
    assert "hygiene.action_" not in facade_text


def test_core_hygiene_handlers_preserve_public_order_and_owner_membership() -> None:
    assert tuple(CORE_HYGIENE_ACTION_HANDLERS) == (
        "lint",
        "check-dependency-boundaries",
        "check-llvm-capabilities",
        "check-hosted-llvm-capabilities",
        "check-release-evidence",
        "check-source-hygiene-authenticity",
        "check-source-hygiene-hard-cutover",
        "check-task-hygiene",
        "check-repo-superclean-surface",
        "validate-repo-superclean",
    )
    assert CORE_HYGIENE_ACTION_HANDLERS["lint"] is CORE_LINTING_ACTION_HANDLERS["lint"]
    assert CORE_HYGIENE_ACTION_HANDLERS["check-llvm-capabilities"] is (
        CORE_CAPABILITY_ACTION_HANDLERS["check-llvm-capabilities"]
    )
    assert CORE_HYGIENE_ACTION_HANDLERS["check-source-hygiene-hard-cutover"] is (
        CORE_SOURCE_HYGIENE_ACTION_HANDLERS["check-source-hygiene-hard-cutover"]
    )
    assert CORE_HYGIENE_ACTION_HANDLERS["validate-repo-superclean"] is (
        CORE_SUPERCLEAN_ACTION_HANDLERS["validate-repo-superclean"]
    )
