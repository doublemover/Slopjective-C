"""Validation rules for package channel payloads."""

from __future__ import annotations

import re
from typing import Any


SHA256_HEX = re.compile(r"^[0-9a-f]{64}$")
ARCHIVE_DIGEST_FIELDS = {
    "portable_archive": "portable-archive",
    "installer_archive": "local-installer",
    "offline_archive": "offline-bundle",
}
ARCHIVE_DIGEST_VERIFICATION_COMMAND = (
    "npm run objc3c -- validate-packaging-channels-end-to-end"
)


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
    if not valid_sha256(signature.get("sha256")):
        raise RuntimeError("package-channels installer_signature sha256 must be lowercase SHA-256")

    archive_digests = manifest_payload.get("archive_digests")
    if not isinstance(archive_digests, dict):
        raise RuntimeError("package-channels manifest missing archive_digests")

    required_digest_fields = metadata_surface["required_archive_digest_fields"]
    for archive_field, artifact_role in ARCHIVE_DIGEST_FIELDS.items():
        digest_record = archive_digests.get(archive_field)
        if not isinstance(digest_record, dict):
            raise RuntimeError(f"package-channels archive_digests missing {archive_field}")
        for field_name in required_digest_fields:
            if field_name not in digest_record:
                raise RuntimeError(
                    f"package-channels archive_digests.{archive_field} missing required field {field_name}"
                )
        if digest_record.get("artifact_role") != artifact_role:
            raise RuntimeError(f"package-channels archive_digests.{archive_field} role drifted")
        if digest_record.get("artifact") != manifest_payload.get(archive_field):
            raise RuntimeError(f"package-channels archive_digests.{archive_field} artifact drifted")
        if digest_record.get("digest_format") != "sha256":
            raise RuntimeError(f"package-channels archive_digests.{archive_field} format drifted")
        if not valid_sha256(digest_record.get("sha256")):
            raise RuntimeError(f"package-channels archive_digests.{archive_field} sha256 must be lowercase SHA-256")
        if digest_record.get("verification_command") != ARCHIVE_DIGEST_VERIFICATION_COMMAND:
            raise RuntimeError(f"package-channels archive_digests.{archive_field} verification command drifted")
        if digest_record.get("trust_scope") != "checked-in-artifact-digest":
            raise RuntimeError(f"package-channels archive_digests.{archive_field} trust scope drifted")

    installer_digest = archive_digests["installer_archive"]["sha256"]
    if signature.get("sha256") != installer_digest:
        raise RuntimeError("package-channels installer_signature digest drifted from installer archive digest")


def valid_sha256(value: object) -> bool:
    return isinstance(value, str) and SHA256_HEX.fullmatch(value) is not None
