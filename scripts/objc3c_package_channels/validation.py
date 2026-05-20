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

    signature = manifest_payload.get("installer_signature")
    if not isinstance(signature, dict):
        raise RuntimeError("package-channels manifest missing installer_signature")
    for field_name in metadata_surface["required_installer_signature_fields"]:
        if field_name not in signature:
            raise RuntimeError(f"package-channels installer_signature missing required field {field_name}")
    if signature.get("artifact") != manifest_payload.get("installer_archive"):
        raise RuntimeError("package-channels installer_signature artifact drifted from installer_archive")
    if signature.get("signature_format") != "objc3c-local-sha256-v1":
        raise RuntimeError("package-channels installer_signature format drifted")
