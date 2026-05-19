from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions.validation_timing_changed_paths import (
    select_validation_profiles,
)
from scripts.objc3c_workflow.actions.validation_timing_profile_catalog_docs import (
    DOCS_PROFILE_RULES,
)
from scripts.objc3c_workflow.actions.validation_timing_profile_catalog_native import (
    NATIVE_PROFILE_RULES,
)
from scripts.objc3c_workflow.actions.validation_timing_profile_catalog_release import (
    RELEASE_PROFILE_RULES,
)
from scripts.objc3c_workflow.actions.validation_timing_profile_rules import (
    VALIDATION_PROFILE_RULES,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

OWNER_MODULES = (
    "validation_timing_profile_model",
    "validation_timing_profile_catalog_docs",
    "validation_timing_profile_catalog_native",
    "validation_timing_profile_catalog_release",
)


def test_validation_timing_profile_rules_is_catalog_facade() -> None:
    facade_text = (ACTION_ROOT / "validation_timing_profile_rules.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.actions.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "path_prefixes" not in facade_text
    assert "recommended_actions" not in facade_text
    assert "profile_owner" not in facade_text


def test_validation_timing_profile_catalog_preserves_owner_order() -> None:
    expected = {
        **DOCS_PROFILE_RULES,
        **NATIVE_PROFILE_RULES,
        **RELEASE_PROFILE_RULES,
    }

    assert VALIDATION_PROFILE_RULES == expected
    assert tuple(VALIDATION_PROFILE_RULES) == (
        "docs",
        "lowering",
        "runtime",
        "diagnostics",
        "conformance",
        "stress",
        "release-claim",
    )


def test_validation_timing_profile_selection_still_matches_native_paths() -> None:
    payload = select_validation_profiles(
        [
            "native/objc3c/src/runtime/dispatch.cpp",
            "tests/tooling/fixtures/native/negative/missing_method.objc3",
        ]
    )

    profiles = {profile["profile"]: profile for profile in payload["profiles"]}
    assert "runtime" in profiles
    assert "lowering" in profiles
    assert "diagnostics" in profiles
    assert "test-runtime-acceptance-fast" in profiles["runtime"][
        "recommended_actions"
    ]
    assert profiles["runtime"]["profile_owner"] == (
        "validation_timing_profile_catalog_native"
    )
    assert profiles["runtime"]["source_owner"] == "validation_timing_changed_paths"
    assert profiles["runtime"]["hard_blocking_decision_owner"] == (
        "validation_timing_budgets"
    )
    assert "deferred_actions" in profiles["runtime"]


def test_validation_timing_repo_profile_is_owner_explicit() -> None:
    payload = select_validation_profiles(["unknown/new_surface.objc3"])
    profile = payload["profiles"][0]

    assert profile["profile"] == "repo"
    assert profile["profile_owner"] == "validation_timing_changed_paths"
    assert profile["source_owner"] == "validation_timing_changed_paths"
    assert profile["hard_blocking_decision_owner"] == "validation_timing_budgets"
    assert payload["owners"] == {
        "source_owner": "validation_timing_changed_paths",
        "profile_owner": "validation_timing_profile_rules",
        "command_owner": "test_orchestration_commands",
        "hard_blocking_decision_owner": "validation_timing_budgets",
    }
