"""Validation rules for package channel payloads."""

from __future__ import annotations

from typing import Any


def validate_manifest_required_fields(
    *,
    manifest_payload: dict[str, Any],
    metadata_surface: dict[str, Any],
) -> None:
    for field_name in metadata_surface["required_manifest_fields"]:
        if field_name not in manifest_payload:
            raise RuntimeError(f"package-channels manifest missing required field {field_name}")
