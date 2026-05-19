"""Compile artifact predicates for Object Model metaclass samples."""

from __future__ import annotations

from collections.abc import Mapping
from typing import Any


def mapping_has_expected_fields(
    payload: Mapping[str, Any],
    expected_fields: Mapping[str, Any],
) -> bool:
    return all(payload.get(key) == value for key, value in expected_fields.items())


def text_contains_all(text: str, markers: tuple[str, ...]) -> bool:
    return all(marker in text for marker in markers)


__all__ = ["mapping_has_expected_fields", "text_contains_all"]
