"""Storage/reflection source-surface payload construction."""

from __future__ import annotations

from collections.abc import Iterable
from typing import Any

from ...case_result import CaseResult
from ..storage_reflection_owner_contracts import storage_reflection_surface_owner_payload
from .models import PayloadField, StorageReflectionSourceSurfaceDefinition
from .predicates import authoritative_case_ids


_RESERVED_PAYLOAD_KEYS = frozenset(
    {
        "contract_id",
        "owner_contract",
        "authoritative_case_ids",
        "authoritative_fixture_paths",
        "authoritative_probe_paths",
    }
)


def storage_reflection_source_surface_payload(
    results: list[CaseResult],
    surface: StorageReflectionSourceSurfaceDefinition,
) -> dict[str, Any]:
    expect_storage_reflection_source_surface_definition(surface)
    payload: dict[str, Any] = {
        "contract_id": surface.contract_id,
        "owner_contract": storage_reflection_surface_owner_payload(),
    }
    _add_payload_fields(payload, surface.pre_case_fields)
    payload["authoritative_case_ids"] = authoritative_case_ids(results, surface)
    payload["authoritative_fixture_paths"] = _shape_payload_value(
        surface.fixture_paths
    )
    payload["authoritative_probe_paths"] = _shape_payload_value(surface.probe_paths)
    _add_payload_fields(payload, surface.post_probe_fields)
    return payload


def expect_storage_reflection_source_surface_definition(
    surface: StorageReflectionSourceSurfaceDefinition,
) -> None:
    if not surface.contract_id:
        raise RuntimeError("storage-reflection source surface is missing a contract id")
    if not surface.case_ids:
        raise RuntimeError("storage-reflection source surface is missing case ids")
    _expect_payload_fields(
        (*surface.pre_case_fields, *surface.post_probe_fields),
        "source-surface",
    )


def _add_payload_fields(
    payload: dict[str, Any],
    fields: tuple[PayloadField, ...],
) -> None:
    for key, value in fields:
        payload[key] = _shape_payload_value(value)


def _expect_payload_fields(
    fields: Iterable[PayloadField],
    field_group: str,
) -> None:
    seen: set[str] = set()
    for key, _ in fields:
        if key in _RESERVED_PAYLOAD_KEYS:
            raise RuntimeError(
                f"storage-reflection source surface {field_group} field {key!r} is reserved"
            )
        if key in seen:
            raise RuntimeError(
                f"storage-reflection source surface {field_group} field {key!r} is duplicated"
            )
        seen.add(key)


def _shape_payload_value(value: Any) -> Any:
    if isinstance(value, tuple):
        return [_shape_payload_value(item) for item in value]
    if isinstance(value, dict):
        return {
            key: _shape_payload_value(nested_value)
            for key, nested_value in value.items()
        }
    return value


__all__ = [
    "expect_storage_reflection_source_surface_definition",
    "storage_reflection_source_surface_payload",
]
