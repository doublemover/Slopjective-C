"""Seed owner map loading and validation."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

from .config import (
    INVALID_OWNER_VALUES,
    OWNER_DATE_RE,
    OWNER_MAP_CONTRACT_ID,
    OWNER_MAP_SEED_ID,
    OWNER_TOKEN_RE,
    SEED_ID_RE,
)
from .models import OwnerMapContract, ParseError, SeedOwnerAssignment


def parse_nonempty_string(value: Any, *, context: str) -> str:
    if not isinstance(value, str):
        raise ParseError(f"{context} must be a string")
    cleaned = value.strip()
    if not cleaned:
        raise ParseError(f"{context} must be a non-empty string")
    return cleaned


def parse_owner_value(value: Any, *, context: str) -> str:
    owner = parse_nonempty_string(value, context=context)
    lowered = owner.lower()
    if lowered in INVALID_OWNER_VALUES:
        raise ParseError(f"{context} must not be placeholder value {owner!r}")
    if not OWNER_TOKEN_RE.match(owner):
        raise ParseError(f"{context} has invalid owner token: {owner!r}")
    return owner


def load_owner_map(owner_map_path: Path) -> OwnerMapContract:
    try:
        payload_text = owner_map_path.read_text(encoding="utf-8")
    except FileNotFoundError as exc:
        raise ParseError(f"owner map file not found: {owner_map_path}") from exc

    try:
        payload = json.loads(payload_text)
    except json.JSONDecodeError as exc:
        raise ParseError(f"owner map is not valid JSON: {owner_map_path}") from exc

    if not isinstance(payload, dict):
        raise ParseError("owner map root must be an object")

    contract_id = parse_nonempty_string(
        payload.get("contract_id"), context="owner map contract_id"
    )
    if contract_id != OWNER_MAP_CONTRACT_ID:
        raise ParseError(
            f"owner map contract_id must be {OWNER_MAP_CONTRACT_ID}; got {contract_id!r}"
        )

    snapshot_date = parse_nonempty_string(
        payload.get("snapshot_date"), context="owner map snapshot_date"
    )
    if not OWNER_DATE_RE.match(snapshot_date):
        raise ParseError(
            f"owner map snapshot_date must match YYYY-MM-DD; got {snapshot_date!r}"
        )

    source_matrix_path = parse_nonempty_string(
        payload.get("source_matrix_path"), context="owner map source_matrix_path"
    )
    seed_id = parse_nonempty_string(payload.get("seed_id"), context="owner map seed_id")
    if seed_id != OWNER_MAP_SEED_ID:
        raise ParseError(
            f"owner map seed_id must be {OWNER_MAP_SEED_ID}; got {seed_id!r}"
        )

    owner_registry_raw = payload.get("owner_registry")
    if not isinstance(owner_registry_raw, list) or not owner_registry_raw:
        raise ParseError("owner map owner_registry must be a non-empty array")

    owner_registry: dict[str, SeedOwnerAssignment] = {}
    seed_ids_in_order: list[str] = []
    for index, row in enumerate(owner_registry_raw):
        context = f"owner map owner_registry[{index}]"
        if not isinstance(row, dict):
            raise ParseError(f"{context} must be an object")

        seed_id = parse_nonempty_string(row.get("seed_id"), context=f"{context}.seed_id")
        if not SEED_ID_RE.match(seed_id):
            raise ParseError(f"{context}.seed_id has invalid format: {seed_id!r}")
        if seed_id in owner_registry:
            raise ParseError(f"duplicate seed id in owner map: {seed_id}")

        owner_primary = parse_owner_value(
            row.get("owner_primary"), context=f"{context}.owner_primary"
        )
        owner_backup = parse_owner_value(
            row.get("owner_backup"), context=f"{context}.owner_backup"
        )

        seed_ids_in_order.append(seed_id)
        owner_registry[seed_id] = SeedOwnerAssignment(
            seed_id=seed_id,
            owner_primary=owner_primary,
            owner_backup=owner_backup,
        )

    if seed_ids_in_order != sorted(seed_ids_in_order):
        raise ParseError("owner map owner_registry must be sorted by seed_id ascending")

    return OwnerMapContract(
        contract_id=contract_id,
        seed_id=seed_id,
        snapshot_date=snapshot_date,
        source_matrix_path=source_matrix_path,
        owner_registry=owner_registry,
    )


def validate_owner_map_against_seeds(
    *,
    owner_map: OwnerMapContract,
    matrix_path: Path,
    matrix_snapshot_date: str,
    seed_ids: set[str],
) -> dict[str, SeedOwnerAssignment]:
    expected_source_matrix_path = display_path(matrix_path.resolve())
    if owner_map.source_matrix_path != expected_source_matrix_path:
        raise ParseError(
            "owner map source_matrix_path mismatch; "
            f"expected {expected_source_matrix_path!r}, "
            f"got {owner_map.source_matrix_path!r}"
        )

    if owner_map.snapshot_date != matrix_snapshot_date:
        raise ParseError(
            "owner map snapshot_date mismatch; "
            f"expected {matrix_snapshot_date!r}, "
            f"got {owner_map.snapshot_date!r}"
        )

    owner_seed_ids = set(owner_map.owner_registry.keys())
    missing_seed_ids = sorted(seed_ids - owner_seed_ids)
    if missing_seed_ids:
        raise ParseError(
            "owner map missing seed id(s): " + ", ".join(missing_seed_ids)
        )

    unknown_seed_ids = sorted(owner_seed_ids - seed_ids)
    if unknown_seed_ids:
        raise ParseError(
            "owner map contains unknown seed id(s): " + ", ".join(unknown_seed_ids)
        )

    return owner_map.owner_registry
