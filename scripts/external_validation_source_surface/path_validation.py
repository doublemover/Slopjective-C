"""Path and contract-id validation helpers."""

from __future__ import annotations

from pathlib import Path

from .diagnostics import ValidationFailure
from .paths import EXPECTED_REQUIRED_PATHS, ROOT
from .source_model import EXPECTED_CONTRACT_IDS, SCHEMA_VERSION


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def require_exact_path(source_surface: dict[str, object], field_name: str) -> str:
    expected_path = EXPECTED_REQUIRED_PATHS[field_name]
    if source_surface.get(field_name) != expected_path:
        raise ValidationFailure(f"{field_name} drifted")
    return expected_path


def require_exact_list(
    source_surface: dict[str, object],
    field_name: str,
    expected_items: tuple[str, ...],
) -> tuple[str, ...]:
    if source_surface.get(field_name) != list(expected_items):
        raise ValidationFailure(f"{field_name} drifted")
    return expected_items


def require_contract_id(payload: dict[str, object], field_name: str) -> bool:
    expected_contract_id = EXPECTED_CONTRACT_IDS[field_name]
    if payload.get("contract_id") != expected_contract_id:
        raise ValidationFailure(f"{field_name} contract_id drifted")
    if payload.get("schema_version") != SCHEMA_VERSION:
        raise ValidationFailure(f"{field_name} schema_version drifted")
    return True
