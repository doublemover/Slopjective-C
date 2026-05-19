"""Canonical object dispatch runtime payload assertions."""

from __future__ import annotations

from typing import Any

from .object_model_dispatch_expected_results import (
    assert_canonical_dispatch_expected_results,
)


def assert_canonical_dispatch_payload(payload: dict[str, Any]) -> None:
    assert_canonical_dispatch_expected_results(payload)


def build_canonical_dispatch_summary(payload: dict[str, Any]) -> dict[str, Any]:
    method_state = payload.get("method_state", {})
    return {
        "traced_value": payload["traced_value"],
        "inherited_value": payload["inherited_value"],
        "class_value": payload["class_value"],
        "live_dispatch_count": method_state["live_dispatch_count"],
        "attached_category_count": payload.get("graph_state", {}).get("attached_category_count"),
        "ignored_strict_error": payload["ignored_expected"],
    }


__all__ = [
    "assert_canonical_dispatch_payload",
    "build_canonical_dispatch_summary",
]
