"""Payload predicate helpers for Object Model metaclass sample probes."""

from __future__ import annotations

from collections.abc import Mapping
from typing import Any


def mapping_has_expected_fields(
    payload: Mapping[str, Any],
    expected_fields: Mapping[str, Any],
) -> bool:
    return all(payload.get(key) == value for key, value in expected_fields.items())


__all__ = ["mapping_has_expected_fields"]
