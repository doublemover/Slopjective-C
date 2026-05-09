"""Core native documentation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_NATIVE_DOCS_ACTION_SPECS: dict[str, ActionSpec] = {
    "build-native-docs": ActionSpec("build-native-docs", "build the generated native implementation docs", "python:scripts/build_objc3c_native_docs.py"),
    "check-native-docs": ActionSpec("check-native-docs", "check generated native implementation docs for drift", "python:scripts/build_objc3c_native_docs.py --check", validation_tier="docs", guarantee_owner="generated native implementation documentation stays in sync with docs/objc3c-native/src inputs"),
}

__all__ = ["CORE_NATIVE_DOCS_ACTION_SPECS"]
