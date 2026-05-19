from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_core_capabilities import (
    CORE_CAPABILITY_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_hygiene import (
    CORE_HYGIENE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_linting import (
    CORE_LINTING_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_source_hygiene import (
    CORE_SOURCE_HYGIENE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_superclean import (
    CORE_SUPERCLEAN_ACTION_SPECS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "action_catalog_core_linting",
    "action_catalog_core_capabilities",
    "action_catalog_core_source_hygiene",
    "action_catalog_core_superclean",
)


def test_core_hygiene_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_core_hygiene.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text
    assert "python:scripts/" not in facade_text


def test_core_hygiene_catalog_preserves_public_order() -> None:
    expected = {
        **CORE_LINTING_ACTION_SPECS,
        **CORE_CAPABILITY_ACTION_SPECS,
        "check-release-evidence": CORE_SUPERCLEAN_ACTION_SPECS[
            "check-release-evidence"
        ],
        **CORE_SOURCE_HYGIENE_ACTION_SPECS,
        "check-repo-superclean-surface": CORE_SUPERCLEAN_ACTION_SPECS[
            "check-repo-superclean-surface"
        ],
        "validate-repo-superclean": CORE_SUPERCLEAN_ACTION_SPECS[
            "validate-repo-superclean"
        ],
    }

    assert CORE_HYGIENE_ACTION_SPECS == expected
    assert tuple(CORE_HYGIENE_ACTION_SPECS) == (
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
