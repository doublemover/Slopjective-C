"""Validation rules for package channel payloads."""

from __future__ import annotations

import re
from typing import Any

from .sanitizer_contracts import (
    PACKAGE_CHANNEL_IDS,
    PACKAGE_IDS,
    SANITIZER_RUNTIME_LIBRARY_ENTRIES,
    SANITIZER_RUNTIME_LIBRARY_MANIFEST_PATHS,
    SANITIZER_VARIANTS,
    payload_entries_for_variant,
)
from .model import (
    DEFAULT_TARGET_PLATFORM_ID,
    RELEASE_PACKAGE_LAYOUT_BY_PLATFORM,
    RELEASE_PACKAGE_TARGET_PLATFORM_IDS,
    REUSABLE_RUNNABLE_PACKAGE_IDENTITY_FIELDS,
    MANIFEST_RELATIVE_PATH,
    release_package_artifact_identity_for_platform,
    release_package_channel_id_for_platform,
    release_package_id_for_platform,
)


SHA256_HEX = re.compile(r"^[0-9a-f]{64}$")
ARCHIVE_DIGEST_FIELDS = {
    "portable_archive": "portable-archive",
    "installer_archive": "local-installer",
    "offline_archive": "offline-bundle",
}
ARCHIVE_DIGEST_VERIFICATION_COMMAND = (
    "npm run objc3c -- validate-packaging-channels-end-to-end"
)
INSTALL_RECEIPT_CONTRACT_ID = "objc3c.packaging.channels.install-receipt.v1"
INSTALL_RECEIPT_SCHEMA = "schemas/objc3c-package-install-receipt-v1.schema.json"
INSTALL_RECEIPT_PATH = "objc3c-install-receipt.json"
INSTALL_COMMAND = "npm run objc3c -- build-package-channels"
INSTALL_BOOTSTRAP_ENTRYPOINT = "Bootstrap-objc3cEnvironment.ps1"
RECEIPT_PLATFORM_FIELDS = [
    "target_platform_id",
    "package_id",
    "package_channel_id",
    "sanitizer_variant",
    "package_runtime_model",
    "support_truth",
    "native_execution_claimed",
]
REQUIRED_PAYLOAD_ENTRIES = RELEASE_PACKAGE_LAYOUT_BY_PLATFORM[DEFAULT_TARGET_PLATFORM_ID]
REQUIRED_RECEIPT_FIELDS = [
    "contract_id",
    "install_root",
    "install_home",
    "channel_id",
    "bootstrap_entrypoint",
    "package_bridge",
    "install_command",
    "payload_manifest",
    "payload_manifest_sha256",
    "payload_required_entries",
    *RECEIPT_PLATFORM_FIELDS,
    "installed_at_utc",
]
SANITIZER_REQUIRED_RECEIPT_FIELDS = [*REQUIRED_RECEIPT_FIELDS, "sanitizer_package_variant"]


def required_payload_entries(
    sanitizer_variant: str = "release",
    *,
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> list[str]:
    if target_platform_id not in RELEASE_PACKAGE_TARGET_PLATFORM_IDS:
        raise RuntimeError(
            f"package-channels unsupported target platform: {target_platform_id}"
        )
    if sanitizer_variant != "release" and target_platform_id != DEFAULT_TARGET_PLATFORM_ID:
        raise RuntimeError("package-channels sanitizer variants are windows-x64 only")
    return payload_entries_for_variant(
        RELEASE_PACKAGE_LAYOUT_BY_PLATFORM[target_platform_id],
        sanitizer_variant,
    )


def target_platform_id_from_manifest(manifest_payload: dict[str, Any]) -> str:
    target_platform_id = str(manifest_payload.get("platform_id", ""))
    if target_platform_id not in RELEASE_PACKAGE_TARGET_PLATFORM_IDS:
        raise RuntimeError(
            f"package-channels manifest platform_id must be one of {', '.join(sorted(RELEASE_PACKAGE_TARGET_PLATFORM_IDS))}"
        )
    return target_platform_id


def expected_package_identity(
    sanitizer_variant: str,
    target_platform_id: str,
) -> tuple[str, str]:
    if sanitizer_variant == "release":
        return (
            release_package_id_for_platform(target_platform_id),
            release_package_channel_id_for_platform(target_platform_id),
        )
    if target_platform_id != DEFAULT_TARGET_PLATFORM_ID:
        raise RuntimeError("package-channels sanitizer variants are windows-x64 only")
    return PACKAGE_IDS[sanitizer_variant], PACKAGE_CHANNEL_IDS[sanitizer_variant]


def validate_manifest_required_fields(
    *,
    manifest_payload: dict[str, Any],
    metadata_surface: dict[str, Any],
) -> None:
    for field_name in metadata_surface["required_manifest_fields"]:
        if field_name not in manifest_payload:
            raise RuntimeError(f"package-channels manifest missing required field {field_name}")
    sanitizer_variant = str(manifest_payload.get("sanitizer_variant", "release"))
    if sanitizer_variant not in PACKAGE_IDS:
        allowed_variants = ", ".join(SANITIZER_VARIANTS)
        raise RuntimeError(f"package-channels sanitizer_variant must be {allowed_variants}")
    target_platform_id = target_platform_id_from_manifest(manifest_payload)
    expected_package_id, expected_package_channel_id = expected_package_identity(
        sanitizer_variant,
        target_platform_id,
    )
    if manifest_payload.get("package_id") != expected_package_id:
        raise RuntimeError("package-channels package_id drifted from sanitizer variant")
    if manifest_payload.get("package_channel_id") != expected_package_channel_id:
        raise RuntimeError("package-channels package_channel_id drifted from sanitizer variant")
    if manifest_payload.get("support_truth") is not False:
        raise RuntimeError("package-channels manifest must not promote sanitizer support truth")
    if manifest_payload.get("native_execution_claimed") is not False:
        raise RuntimeError("package-channels manifest must not claim sanitizer native execution")

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

    validate_payload_contract(
        manifest_payload=manifest_payload,
        metadata_surface=metadata_surface,
        sanitizer_variant=sanitizer_variant,
        target_platform_id=target_platform_id,
    )
    validate_receipt_contracts(
        manifest_payload=manifest_payload,
        metadata_surface=metadata_surface,
        target_platform_id=target_platform_id,
    )
    validate_reusable_runnable_package_identity_contract(
        manifest_payload=manifest_payload,
        metadata_surface=metadata_surface,
    )


def valid_sha256(value: object) -> bool:
    return isinstance(value, str) and SHA256_HEX.fullmatch(value) is not None


def validate_payload_contract(
    *,
    manifest_payload: dict[str, Any],
    metadata_surface: dict[str, Any],
    sanitizer_variant: str = "release",
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> None:
    payload_contract = manifest_payload.get("payload_contract")
    if not isinstance(payload_contract, dict):
        raise RuntimeError("package-channels manifest missing payload_contract")
    for field_name in metadata_surface["required_payload_contract_fields"]:
        if field_name not in payload_contract:
            raise RuntimeError(f"package-channels payload_contract missing required field {field_name}")
    if payload_contract.get("contract_id") != "objc3c.packaging.channels.payload-contract.v1":
        raise RuntimeError("package-channels payload_contract identity drifted")
    if payload_contract.get("source") != "canonical-runnable-toolchain-package":
        raise RuntimeError("package-channels payload_contract source drifted")
    if payload_contract.get("manifest_relative_path") != MANIFEST_RELATIVE_PATH:
        raise RuntimeError("package-channels payload_contract manifest path drifted")
    package_root = str(manifest_payload.get("package_root", "")).rstrip("/")
    if payload_contract.get("manifest_artifact") != f"{package_root}/{MANIFEST_RELATIVE_PATH}":
        raise RuntimeError("package-channels payload_contract manifest artifact drifted")
    if not valid_sha256(payload_contract.get("manifest_sha256")):
        raise RuntimeError("package-channels payload_contract manifest_sha256 must be lowercase SHA-256")
    if payload_contract.get("target_platform_id") != target_platform_id:
        raise RuntimeError("package-channels payload_contract target platform drifted")
    expected_payload_entries = required_payload_entries(
        sanitizer_variant,
        target_platform_id=target_platform_id,
    )
    if payload_contract.get("required_entries") != expected_payload_entries:
        raise RuntimeError("package-channels payload_contract required entries drifted")
    if payload_contract.get("clean_room_source_policy") != "fresh-owned-tmp-root-only":
        raise RuntimeError("package-channels payload_contract clean-room source policy drifted")

    entry_digests = payload_contract.get("entry_digests")
    if not isinstance(entry_digests, dict):
        raise RuntimeError("package-channels payload_contract missing entry_digests")
    for relative_path in expected_payload_entries:
        entry_digest = entry_digests.get(relative_path)
        if not isinstance(entry_digest, dict):
            raise RuntimeError(f"package-channels payload_contract entry_digests missing {relative_path}")
        if entry_digest.get("digest_format") != "sha256":
            raise RuntimeError(f"package-channels payload_contract {relative_path} digest format drifted")
        if entry_digest.get("artifact") != relative_path:
            raise RuntimeError(f"package-channels payload_contract {relative_path} artifact drifted")
        if not valid_sha256(entry_digest.get("sha256")):
            raise RuntimeError(f"package-channels payload_contract {relative_path} sha256 must be lowercase SHA-256")
    if entry_digests[MANIFEST_RELATIVE_PATH]["sha256"] != payload_contract.get("manifest_sha256"):
        raise RuntimeError("package-channels payload_contract manifest digest drifted from entry digest")


def validate_reusable_runnable_package_identity_contract(
    *,
    manifest_payload: dict[str, Any],
    metadata_surface: dict[str, Any],
) -> None:
    contract = manifest_payload.get("reusable_runnable_package_identity_contract")
    if not isinstance(contract, dict):
        raise RuntimeError(
            "package-channels manifest missing reusable_runnable_package_identity_contract"
        )
    if (
        contract.get("contract_id")
        != "objc3c.packaging.channels.reusable-runnable-package-identity.v1"
    ):
        raise RuntimeError(
            "package-channels reusable runnable package identity contract drifted"
        )
    if contract.get("manifest_relative_path") != MANIFEST_RELATIVE_PATH:
        raise RuntimeError(
            "package-channels reusable runnable package manifest path drifted"
        )
    expected_required_fields = metadata_surface.get(
        "required_reusable_runnable_package_identity_fields",
        REUSABLE_RUNNABLE_PACKAGE_IDENTITY_FIELDS,
    )
    if contract.get("required_fields") != expected_required_fields:
        raise RuntimeError(
            "package-channels reusable runnable package required identity fields drifted"
        )
    if (
        contract.get("reuse_policy")
        != "manifest-target-and-artifact-identity-must-match-requested-platform"
    ):
        raise RuntimeError(
            "package-channels reusable runnable package reuse policy drifted"
        )
    if (
        contract.get("generated_evidence_policy")
        != "generated-host-output-is-review-input-not-source-truth"
    ):
        raise RuntimeError(
            "package-channels reusable runnable package evidence policy drifted"
        )
    target_platforms = contract.get("target_platforms")
    if not isinstance(target_platforms, list):
        raise RuntimeError(
            "package-channels reusable runnable package target platform identities missing"
        )
    identities_by_platform = {
        str(identity.get("target_platform_id")): identity
        for identity in target_platforms
        if isinstance(identity, dict)
    }
    if set(identities_by_platform) != set(RELEASE_PACKAGE_TARGET_PLATFORM_IDS):
        raise RuntimeError(
            "package-channels reusable runnable package target platform set drifted"
        )
    for platform_id in RELEASE_PACKAGE_TARGET_PLATFORM_IDS:
        identity = identities_by_platform[platform_id]
        expected_identity = release_package_artifact_identity_for_platform(platform_id)
        for field_name, expected_value in expected_identity.items():
            if identity.get(field_name) != expected_value:
                raise RuntimeError(
                    "package-channels reusable runnable package artifact identity "
                    f"drifted for {platform_id} {field_name}"
                )
        if identity.get("package_id") != release_package_id_for_platform(platform_id):
            raise RuntimeError(
                f"package-channels reusable runnable package id drifted for {platform_id}"
            )
        expected_package_channel_id = release_package_channel_id_for_platform(platform_id)
        if identity.get("package_channel_id") != expected_package_channel_id:
            raise RuntimeError(
                f"package-channels reusable runnable package channel drifted for {platform_id}"
            )
        if (
            identity.get("package_root_layout")
            != RELEASE_PACKAGE_LAYOUT_BY_PLATFORM[platform_id]
        ):
            raise RuntimeError(
                f"package-channels reusable runnable package layout drifted for {platform_id}"
            )
        if identity.get("support_truth") is not False:
            raise RuntimeError(
                f"package-channels reusable runnable package promoted support truth for {platform_id}"
            )
        if identity.get("native_execution_claimed") is not False:
            raise RuntimeError(
                f"package-channels reusable runnable package claimed native execution for {platform_id}"
            )


def validate_receipt_contracts(
    *,
    manifest_payload: dict[str, Any],
    metadata_surface: dict[str, Any],
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> None:
    receipt_contracts = manifest_payload.get("receipt_contracts")
    if not isinstance(receipt_contracts, dict):
        raise RuntimeError("package-channels manifest missing receipt_contracts")
    expected_contracts = {
        "install_receipt": {
            "channel_id": "local-installer",
            "emitted_by": "Install-objc3c.ps1",
            "network_policy": "local-filesystem-only",
        },
        "offline_install_receipt": {
            "channel_id": "offline-bundle",
            "emitted_by": "OfflineBootstrap-objc3c.ps1",
            "network_policy": "no-network",
            "delegates_to": "local-installer",
        },
    }
    for contract_name, expected in expected_contracts.items():
        receipt_contract = receipt_contracts.get(contract_name)
        if not isinstance(receipt_contract, dict):
            raise RuntimeError(f"package-channels receipt_contracts missing {contract_name}")
        required_receipt_contract_fields = (
            metadata_surface.get(
                "sanitizer_required_receipt_contract_fields",
                [
                    *metadata_surface["required_receipt_contract_fields"],
                    "sanitizer_install_selector",
                    "sanitizer_runtime_library_manifest_path",
                    "sanitizer_runtime_library_required_entries",
                    "missing_runtime_behavior",
                ],
            )
            if str(manifest_payload.get("sanitizer_variant", "release")) != "release"
            else metadata_surface["required_receipt_contract_fields"]
        )
        for field_name in required_receipt_contract_fields:
            if field_name not in receipt_contract:
                raise RuntimeError(
                    f"package-channels receipt_contracts.{contract_name} missing required field {field_name}"
                )
        if receipt_contract.get("contract_id") != INSTALL_RECEIPT_CONTRACT_ID:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} identity drifted")
        if receipt_contract.get("schema") != INSTALL_RECEIPT_SCHEMA:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} schema drifted")
        if receipt_contract.get("receipt_path") != INSTALL_RECEIPT_PATH:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} receipt path drifted")
        if receipt_contract.get("bootstrap_entrypoint") != INSTALL_BOOTSTRAP_ENTRYPOINT:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} bootstrap drifted")
        if receipt_contract.get("package_bridge") != "objc3c":
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} package bridge drifted")
        if receipt_contract.get("install_command") != INSTALL_COMMAND:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} install command drifted")
        if receipt_contract.get("payload_manifest") != MANIFEST_RELATIVE_PATH:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} payload manifest drifted")
        expected_payload_entries = required_payload_entries(
            str(manifest_payload.get("sanitizer_variant", "release")),
            target_platform_id=target_platform_id,
        )
        if receipt_contract.get("payload_required_entries") != expected_payload_entries:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} payload entries drifted")
        expected_required_fields = (
            metadata_surface.get(
                "sanitizer_required_receipt_fields",
                SANITIZER_REQUIRED_RECEIPT_FIELDS,
            )
            if str(receipt_contract.get("sanitizer_variant", "release")) != "release"
            else metadata_surface["required_receipt_fields"]
        )
        if receipt_contract.get("required_fields") != expected_required_fields:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} required fields drifted")
        if receipt_contract.get("rollback_required") is not True:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} rollback requirement drifted")
        receipt_sanitizer_variant = str(receipt_contract.get("sanitizer_variant", "release"))
        if receipt_sanitizer_variant != str(manifest_payload.get("sanitizer_variant", "release")):
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} sanitizer variant drifted from manifest")
        if receipt_sanitizer_variant not in PACKAGE_IDS:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} sanitizer variant drifted")
        expected_package_id, expected_package_channel_id = expected_package_identity(
            receipt_sanitizer_variant,
            target_platform_id,
        )
        if receipt_contract.get("target_platform_id") != target_platform_id:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} target platform drifted")
        if receipt_contract.get("package_id") != expected_package_id:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} package id drifted")
        if receipt_contract.get("package_channel_id") != expected_package_channel_id:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} package channel id drifted")
        runtime_model = receipt_contract.get("package_runtime_model")
        if not isinstance(runtime_model, dict):
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} missing runtime model")
        if runtime_model.get("target_platform_id") != target_platform_id:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} runtime target platform drifted")
        if runtime_model.get("package_id") != expected_package_id:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} runtime package id drifted")
        if runtime_model.get("package_channel_id") != expected_package_channel_id:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} runtime channel drifted")
        if runtime_model.get("package_root_layout") != expected_payload_entries:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} runtime layout drifted")
        if receipt_contract.get("support_truth") is not False:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} must not promote support truth")
        if receipt_contract.get("native_execution_claimed") is not False:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} must not claim native execution")
        if receipt_sanitizer_variant != "release":
            expected_selector = f"sanitizer={receipt_sanitizer_variant}"
            if receipt_contract.get("sanitizer_install_selector") != expected_selector:
                raise RuntimeError(f"package-channels receipt_contracts.{contract_name} sanitizer selector drifted")
            expected_runtime_manifest = SANITIZER_RUNTIME_LIBRARY_MANIFEST_PATHS[
                receipt_sanitizer_variant
            ]
            if (
                receipt_contract.get("sanitizer_runtime_library_manifest_path")
                != expected_runtime_manifest
            ):
                raise RuntimeError(
                    f"package-channels receipt_contracts.{contract_name} sanitizer runtime manifest path drifted"
                )
            if (
                receipt_contract.get("sanitizer_runtime_library_required_entries")
                != SANITIZER_RUNTIME_LIBRARY_ENTRIES[receipt_sanitizer_variant]
            ):
                raise RuntimeError(
                    f"package-channels receipt_contracts.{contract_name} sanitizer runtime entries drifted"
                )
            if (
                receipt_contract.get("missing_runtime_behavior")
                != "fail-closed-before-package-install"
            ):
                raise RuntimeError(
                    f"package-channels receipt_contracts.{contract_name} missing runtime behavior drifted"
                )
        elif "sanitizer_install_selector" in receipt_contract:
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} release receipt exposed sanitizer selector")
        elif (
            "sanitizer_runtime_library_manifest_path" in receipt_contract
            or "sanitizer_runtime_library_required_entries" in receipt_contract
            or "missing_runtime_behavior" in receipt_contract
        ):
            raise RuntimeError(f"package-channels receipt_contracts.{contract_name} release receipt exposed sanitizer runtime contract")
        for field_name, expected_value in expected.items():
            if receipt_contract.get(field_name) != expected_value:
                raise RuntimeError(f"package-channels receipt_contracts.{contract_name} {field_name} drifted")
