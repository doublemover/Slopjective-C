from __future__ import annotations

from typing import Any

from .errors import PublicationError


def require_dict(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise PublicationError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise PublicationError(f"{name} must be an array")
    return value


def require_str(record: dict[str, Any], key: str, context: str) -> str:
    value = record.get(key)
    if not isinstance(value, str) or not value.strip():
        raise PublicationError(f"{context}.{key} must be a non-empty string")
    return value
