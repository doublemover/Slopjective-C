"""Core native documentation action specs."""

from __future__ import annotations

from scripts.objc3c_workflow.actions.docs_documentation import (
    BUILD_NATIVE_DOCS_ACTION,
    BUILD_NATIVE_DOCS_BACKEND,
    BUILD_NATIVE_DOCS_SUMMARY,
    CHECK_NATIVE_DOCS_ACTION,
    CHECK_NATIVE_DOCS_BACKEND,
    CHECK_NATIVE_DOCS_SUMMARY,
    NATIVE_DOCS_GUARANTEE_OWNER,
    NATIVE_DOCS_VALIDATION_TIER,
)

from .action_spec import ActionSpec

CORE_NATIVE_DOCS_ACTION_SPECS: dict[str, ActionSpec] = {
    BUILD_NATIVE_DOCS_ACTION: ActionSpec(
        BUILD_NATIVE_DOCS_ACTION,
        BUILD_NATIVE_DOCS_SUMMARY,
        BUILD_NATIVE_DOCS_BACKEND,
    ),
    CHECK_NATIVE_DOCS_ACTION: ActionSpec(
        CHECK_NATIVE_DOCS_ACTION,
        CHECK_NATIVE_DOCS_SUMMARY,
        CHECK_NATIVE_DOCS_BACKEND,
        validation_tier=NATIVE_DOCS_VALIDATION_TIER,
        guarantee_owner=NATIVE_DOCS_GUARANTEE_OWNER,
    ),
}

__all__ = ["CORE_NATIVE_DOCS_ACTION_SPECS"]
