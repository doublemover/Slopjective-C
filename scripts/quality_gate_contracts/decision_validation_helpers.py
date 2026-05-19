"""Low-level validation helpers for quality-gate contract payloads."""

from __future__ import annotations

from .decision_errors import ContractHardFailError
from .decision_validation_violations import SchemaViolation


def require_mapping(value: object, *, context: str) -> dict[str, object]:
    if not isinstance(value, dict):
        raise ContractHardFailError(f"{context}: expected object")
    return value


def require_exact_keys(
    payload: dict[str, object],
    *,
    expected_keys: tuple[str, ...],
    context: str,
) -> None:
    violation = SchemaViolation(
        missing=[key for key in expected_keys if key not in payload],
        unexpected=[key for key in payload if key not in expected_keys],
    )
    if violation.missing or violation.unexpected:
        raise ContractHardFailError(violation.message(context=context))


def require_non_empty_string(
    payload: dict[str, object],
    *,
    key: str,
    context: str,
) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value.strip():
        raise ContractHardFailError(f"{context}.{key}: expected non-empty string")
    return value.strip()


def require_string_list(
    payload: dict[str, object],
    *,
    key: str,
    context: str,
) -> list[str]:
    value = payload.get(key)
    if not isinstance(value, list):
        raise ContractHardFailError(f"{context}.{key}: expected array of strings")
    normalized: list[str] = []
    for index, item in enumerate(value):
        if not isinstance(item, str) or not item.strip():
            raise ContractHardFailError(
                f"{context}.{key}[{index}]: expected non-empty string"
            )
        normalized.append(item.strip())
    return normalized


__all__ = [
    "require_exact_keys",
    "require_mapping",
    "require_non_empty_string",
    "require_string_list",
]
