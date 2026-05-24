"""Package channel data model and payload builders."""

from __future__ import annotations

from copy import deepcopy
from dataclasses import dataclass
from datetime import datetime, timezone
import hashlib
import json
import os
import platform
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .sanitizer_contracts import (
    payload_entries_for_variant,
    sanitizer_variant_metadata,
)
from .paths import (
    ARTIFACT_ROOT,
    PLATFORM_SUPPORT_MATRIX_ARTIFACT,
    PLATFORM_SUPPORT_MATRIX_SUMMARY,
    RELEASE_FOUNDATION_ATTESTATION,
    RELEASE_FOUNDATION_MANIFEST,
    RELEASE_FOUNDATION_SBOM,
    ROOT,
)


IMPLEMENTED_CHANNELS = ["portable-archive", "local-installer", "offline-bundle"]
MANIFEST_RELATIVE_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"
INSTALL_RECEIPT_CONTRACT_ID = "objc3c.packaging.channels.install-receipt.v1"
INSTALL_RECEIPT_SCHEMA = "schemas/objc3c-package-install-receipt-v1.schema.json"
INSTALL_RECEIPT_PATH = "objc3c-install-receipt.json"
INSTALL_COMMAND = "npm run objc3c -- build-package-channels"
INSTALL_BOOTSTRAP_ENTRYPOINT = "Bootstrap-objc3cEnvironment.ps1"
DEFAULT_TARGET_PLATFORM_ID = "windows-x64"
REQUIRED_PAYLOAD_ENTRIES = [
    MANIFEST_RELATIVE_PATH,
    "artifacts/bin/objc3c-native.exe",
    "artifacts/lib/objc3_runtime.lib",
    "stdlib/workspace.json",
    "stdlib/modules/objc3.core/module.json",
    "docs/runbooks/objc3c_packaging_channels.md",
]
ARCHIVE_DIGEST_FIELDS = {
    "portable_archive": "portable-archive",
    "installer_archive": "local-installer",
    "offline_archive": "offline-bundle",
}
RECEIPT_PLATFORM_FIELDS = [
    "target_platform_id",
    "package_id",
    "package_channel_id",
    "sanitizer_variant",
    "package_runtime_model",
    "support_truth",
    "native_execution_claimed",
]
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
RELEASE_RUNTIME_LIBRARY_NAMES_BY_PLATFORM = {
    "windows-x64": ["objc3_runtime.lib"],
    "linux-x64": ["libobjc3-runtime.so"],
    "darwin-arm64": ["libobjc3-runtime.dylib"],
}
RELEASE_PACKAGE_LAYOUT_BY_PLATFORM = {
    "windows-x64": [
        MANIFEST_RELATIVE_PATH,
        "artifacts/bin/objc3c-native.exe",
        "artifacts/lib/objc3_runtime.lib",
        "stdlib/workspace.json",
        "stdlib/modules/objc3.core/module.json",
        "docs/runbooks/objc3c_packaging_channels.md",
    ],
    "linux-x64": [
        MANIFEST_RELATIVE_PATH,
        "artifacts/bin/objc3c-native",
        "artifacts/lib/libobjc3-runtime.so",
        "stdlib/workspace.json",
        "stdlib/modules/objc3.core/module.json",
        "docs/runbooks/objc3c_packaging_channels.md",
    ],
    "darwin-arm64": [
        MANIFEST_RELATIVE_PATH,
        "artifacts/bin/objc3c-native",
        "artifacts/lib/libobjc3-runtime.dylib",
        "stdlib/workspace.json",
        "stdlib/modules/objc3.core/module.json",
        "docs/runbooks/objc3c_packaging_channels.md",
    ],
}
RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES = tuple(RELEASE_PACKAGE_LAYOUT_BY_PLATFORM)
RELEASE_PACKAGE_TARGET_PLATFORM_IDS = frozenset(RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES)


@dataclass(frozen=True)
class PackageChannelSurfaceInputs:
    supported_platforms: dict[str, Any]
    metadata_surface: dict[str, Any]


@dataclass(frozen=True)
class PackageChannelInputs:
    supported_platforms: dict[str, Any]
    metadata_surface: dict[str, Any]
    platform_support_matrix: dict[str, Any]
    interop_loader_metadata: dict[str, Any]


@dataclass(frozen=True)
class PackageChannelPaths:
    run_id: str
    sanitizer_variant: str
    package_id: str
    package_channel_id: str
    package_root: Path
    build_root: Path
    portable_archive: Path
    installer_image_root: Path
    installer_archive: Path
    offline_bundle_root: Path
    offline_archive: Path
    manifest_path: Path
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID


def package_channel_paths(
    run_id: str,
    sanitizer_variant: str = "release",
    *,
    target_platform_id: str | None = None,
) -> PackageChannelPaths:
    metadata = sanitizer_variant_metadata(sanitizer_variant)
    resolved_target_platform_id = package_channel_target_platform_id(
        sanitizer_variant,
        target_platform_id=target_platform_id,
    )
    package_id = str(metadata["package_id"])
    package_channel_id = str(metadata["package_channel_id"])
    archive_suffix = str(metadata["archive_suffix"])
    if sanitizer_variant == "release":
        package_id = release_package_id_for_platform(resolved_target_platform_id)
        package_channel_id = release_package_channel_id_for_platform(
            resolved_target_platform_id
        )
        archive_suffix = resolved_target_platform_id
    package_root_name = "runnable" if sanitizer_variant == "release" else f"runnable-{sanitizer_variant}"
    package_root = ROOT / "tmp" / "pkg" / "objc3c-package-channels" / run_id / package_root_name
    build_root = ARTIFACT_ROOT / run_id / archive_suffix
    portable_archive = build_root / "portable" / f"objc3c-{archive_suffix}-portable.zip"
    installer_image_root = build_root / "installer" / "image"
    installer_archive = build_root / "installer" / f"objc3c-{archive_suffix}-installer.zip"
    offline_bundle_root = build_root / "offline" / "bundle"
    offline_archive = build_root / "offline" / f"objc3c-{archive_suffix}-offline-bundle.zip"
    manifest_path = build_root / "objc3c-package-channels-manifest.json"
    return PackageChannelPaths(
        run_id=run_id,
        sanitizer_variant=sanitizer_variant,
        package_id=package_id,
        package_channel_id=package_channel_id,
        target_platform_id=resolved_target_platform_id,
        package_root=package_root,
        build_root=build_root,
        portable_archive=portable_archive,
        installer_image_root=installer_image_root,
        installer_archive=installer_archive,
        offline_bundle_root=offline_bundle_root,
        offline_archive=offline_archive,
        manifest_path=manifest_path,
    )


def detected_host_target_platform_id() -> str:
    configured = os.environ.get("OBJC3C_TARGET_PLATFORM_ID", "") or os.environ.get(
        "OBJC3C_PLATFORM_ID", ""
    )
    if configured in RELEASE_PACKAGE_TARGET_PLATFORM_IDS:
        return configured
    system = platform.system().lower()
    machine = platform.machine().lower()
    if system == "windows" and machine in {"amd64", "x86_64"}:
        return "windows-x64"
    if system == "linux" and machine in {"amd64", "x86_64"}:
        return "linux-x64"
    if system == "darwin" and machine in {"arm64", "aarch64"}:
        return "darwin-arm64"
    return DEFAULT_TARGET_PLATFORM_ID


def package_channel_target_platform_id(
    sanitizer_variant: str = "release",
    *,
    target_platform_id: str | None = None,
) -> str:
    if sanitizer_variant != "release":
        if target_platform_id not in (None, ""):
            raise RuntimeError(
                "target-platform override is only supported for release package channels; "
                "sanitizer package channels are currently windows-x64 only"
            )
        return DEFAULT_TARGET_PLATFORM_ID
    if target_platform_id:
        if target_platform_id not in RELEASE_PACKAGE_TARGET_PLATFORM_IDS:
            raise RuntimeError(
                f"unsupported package-channel target platform: {target_platform_id}"
            )
        return target_platform_id
    return detected_host_target_platform_id()


def load_runnable_package_manifest(paths: PackageChannelPaths) -> dict[str, Any]:
    manifest_path = paths.package_root / MANIFEST_RELATIVE_PATH
    try:
        payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise RuntimeError(
            f"package-channels runnable package manifest is missing: {repo_rel(manifest_path)}"
        ) from exc
    if not isinstance(payload, dict):
        raise RuntimeError("package-channels runnable package manifest must be a JSON object")
    return payload


def target_platform_id_from_manifest(manifest: dict[str, Any]) -> str:
    target_platform_id = str(manifest.get("target_platform_id", ""))
    if target_platform_id not in RELEASE_PACKAGE_TARGET_PLATFORM_IDS:
        raise RuntimeError(
            f"package-channels unsupported runnable package target platform: {target_platform_id}"
        )
    return target_platform_id


def target_platform_id_for_paths(paths: PackageChannelPaths) -> str:
    manifest_path = paths.package_root / MANIFEST_RELATIVE_PATH
    if not manifest_path.is_file():
        return paths.target_platform_id
    target_platform_id = target_platform_id_from_manifest(
        load_runnable_package_manifest(paths)
    )
    if target_platform_id != paths.target_platform_id:
        raise RuntimeError(
            "package-channels target platform drifted between path model and runnable "
            f"manifest: {paths.target_platform_id} != {target_platform_id}"
        )
    return target_platform_id


def package_channels_manifest_payload(
    *,
    inputs: PackageChannelInputs,
    paths: PackageChannelPaths,
    installer_signature: dict[str, Any],
    archive_digests: dict[str, Any] | None = None,
    payload_contract: dict[str, Any] | None = None,
    receipt_contracts: dict[str, Any] | None = None,
) -> dict[str, Any]:
    manifest_path = paths.package_root / MANIFEST_RELATIVE_PATH
    runnable_manifest = (
        load_runnable_package_manifest(paths) if manifest_path.is_file() else None
    )
    target_platform_id = (
        target_platform_id_from_manifest(runnable_manifest)
        if runnable_manifest is not None
        else paths.target_platform_id
    )
    if target_platform_id != paths.target_platform_id:
        raise RuntimeError(
            "package-channels target platform drifted between path model and runnable "
            f"manifest: {paths.target_platform_id} != {target_platform_id}"
        )
    package_id = paths.package_id
    package_channel_id = paths.package_channel_id
    if paths.sanitizer_variant == "release":
        package_id = release_package_id_for_platform(target_platform_id)
        package_channel_id = release_package_channel_id_for_platform(target_platform_id)
    elif target_platform_id != DEFAULT_TARGET_PLATFORM_ID:
        raise RuntimeError(
            "package-channels sanitizer variants are currently windows-x64 only"
        )
    resolved_archive_digests = (
        archive_digest_payloads(paths) if archive_digests is None else archive_digests
    )
    resolved_payload_contract = (
        package_payload_contract(paths, runnable_manifest=runnable_manifest)
        if payload_contract is None
        else payload_contract
    )
    resolved_receipt_contracts = (
        receipt_contract_payloads(
            paths.sanitizer_variant,
            target_platform_id=target_platform_id,
        )
        if receipt_contracts is None
        else receipt_contracts
    )
    return {
        "contract_id": "objc3c.packaging.channels.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": target_platform_id,
        "package_id": package_id,
        "package_channel_id": package_channel_id,
        "sanitizer_variant": paths.sanitizer_variant,
        "package_root": repo_rel(paths.package_root),
        "portable_archive": repo_rel(paths.portable_archive),
        "installer_archive": repo_rel(paths.installer_archive),
        "offline_archive": repo_rel(paths.offline_archive),
        "installer_image_root": repo_rel(paths.installer_image_root),
        "offline_bundle_root": repo_rel(paths.offline_bundle_root),
        "platform_support_matrix": repo_rel(PLATFORM_SUPPORT_MATRIX_ARTIFACT),
        "platform_support_summary": repo_rel(PLATFORM_SUPPORT_MATRIX_SUMMARY),
        "supported_platform_ids": inputs.platform_support_matrix["claim_boundary"]["supported_platform_ids"],
        "support_tiers": inputs.platform_support_matrix["tiers"],
        "package_runtime_models": package_runtime_models(inputs.supported_platforms),
        "implemented_channels": IMPLEMENTED_CHANNELS,
        "interop_loader_metadata": inputs.interop_loader_metadata,
        "installer_signature": installer_signature,
        "archive_digests": resolved_archive_digests,
        "payload_contract": resolved_payload_contract,
        "receipt_contracts": resolved_receipt_contracts,
        "support_truth": False,
        "native_execution_claimed": False,
        "release_foundation_artifacts": {
            "manifest": repo_rel(RELEASE_FOUNDATION_MANIFEST),
            "sbom": repo_rel(RELEASE_FOUNDATION_SBOM),
            "attestation": repo_rel(RELEASE_FOUNDATION_ATTESTATION),
        },
    }


def package_channels_report_payload(
    *,
    inputs: PackageChannelInputs,
    paths: PackageChannelPaths,
    manifest_payload: dict[str, Any],
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.packaging.channels.summary.report.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": manifest_payload["platform_id"],
        "package_id": manifest_payload["package_id"],
        "package_channel_id": manifest_payload["package_channel_id"],
        "sanitizer_variant": paths.sanitizer_variant,
        "package_root": repo_rel(paths.package_root),
        "manifest_path": repo_rel(paths.manifest_path),
        "portable_archive": repo_rel(paths.portable_archive),
        "installer_image_root": repo_rel(paths.installer_image_root),
        "installer_archive": repo_rel(paths.installer_archive),
        "offline_bundle_root": repo_rel(paths.offline_bundle_root),
        "offline_archive": repo_rel(paths.offline_archive),
        "platform_support_matrix": repo_rel(PLATFORM_SUPPORT_MATRIX_ARTIFACT),
        "supported_platform_ids": inputs.platform_support_matrix["claim_boundary"]["supported_platform_ids"],
        "support_tiers": inputs.platform_support_matrix["tiers"],
        "package_runtime_models": manifest_payload["package_runtime_models"],
        "implemented_channels": manifest_payload["implemented_channels"],
        "interop_loader_metadata": manifest_payload["interop_loader_metadata"],
        "installer_signature": manifest_payload["installer_signature"],
        "archive_digests": manifest_payload["archive_digests"],
        "payload_contract": manifest_payload["payload_contract"],
        "receipt_contracts": manifest_payload["receipt_contracts"],
        "support_truth": manifest_payload["support_truth"],
        "native_execution_claimed": manifest_payload["native_execution_claimed"],
    }


def required_payload_entries(sanitizer_variant: str = "release") -> list[str]:
    return required_payload_entries_for_platform(
        sanitizer_variant=sanitizer_variant,
        target_platform_id=package_channel_target_platform_id(sanitizer_variant),
    )


def required_payload_entries_for_platform(
    *,
    sanitizer_variant: str = "release",
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> list[str]:
    base_entries = RELEASE_PACKAGE_LAYOUT_BY_PLATFORM.get(
        target_platform_id,
        RELEASE_PACKAGE_LAYOUT_BY_PLATFORM[DEFAULT_TARGET_PLATFORM_ID],
    )
    if sanitizer_variant != "release" and target_platform_id != DEFAULT_TARGET_PLATFORM_ID:
        raise RuntimeError("sanitizer package channels are currently windows-x64 only")
    return payload_entries_for_variant(base_entries, sanitizer_variant)


def release_package_id_for_platform(platform_id: str) -> str:
    if platform_id == DEFAULT_TARGET_PLATFORM_ID:
        return "org.objc3c.runtime:objc3c-runtime-release"
    return f"org.objc3c.runtime:objc3c-runtime-{platform_id}-release"


def release_package_channel_id_for_platform(platform_id: str) -> str:
    return f"{platform_id}-release"


def fallback_release_package_runtime_model(platform_id: str) -> dict[str, Any]:
    host_os, _, host_arch = platform_id.partition("-")
    return {
        "platform_id": platform_id,
        "target_platform_id": platform_id,
        "host_os": host_os,
        "host_arch": host_arch or "unknown",
        "support_state": "supported"
        if platform_id == DEFAULT_TARGET_PLATFORM_ID
        else "unsupported",
        "claim_state": "evidence-bound"
        if platform_id == DEFAULT_TARGET_PLATFORM_ID
        else "fail-closed",
        "package_id": release_package_id_for_platform(platform_id),
        "package_channel_id": release_package_channel_id_for_platform(platform_id),
        "package_variant_row_id": f"objc3c.package.runtime.{platform_id}.release",
        "runtime_variant": "release",
        "runtime_library_ids": ["objc3-runtime"],
        "runtime_library_names": RELEASE_RUNTIME_LIBRARY_NAMES_BY_PLATFORM.get(
            platform_id,
            ["objc3-runtime"],
        ),
        "package_root_layout": RELEASE_PACKAGE_LAYOUT_BY_PLATFORM.get(
            platform_id,
            RELEASE_PACKAGE_LAYOUT_BY_PLATFORM[DEFAULT_TARGET_PLATFORM_ID],
        ),
        "missing_runtime_behavior": "fail-closed-before-native-execution-claim",
        "unsupported_behavior": "fail-closed",
        "required_evidence_classes": ["package", "install", "execution"],
        "required_missing_evidence_classes": []
        if platform_id == DEFAULT_TARGET_PLATFORM_ID
        else ["build", "package", "install", "execution"],
        "support_truth": False,
        "native_execution_required": True,
        "native_execution_claimed": False,
        "promotion_allowed": False,
    }


def package_runtime_models(supported_platforms: dict[str, Any]) -> list[dict[str, Any]]:
    models = supported_platforms.get("package_runtime_models")
    if isinstance(models, list) and models:
        return deepcopy(models)
    platform_id = str(
        supported_platforms.get("default_platform_id", DEFAULT_TARGET_PLATFORM_ID)
    )
    return [fallback_release_package_runtime_model(platform_id)]


def receipt_package_runtime_model(
    sanitizer_variant: str = "release",
    *,
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> dict[str, Any]:
    metadata = sanitizer_variant_metadata(sanitizer_variant)
    if sanitizer_variant == "release":
        model = fallback_release_package_runtime_model(target_platform_id)
        model["package_root_layout"] = required_payload_entries_for_platform(
            sanitizer_variant=sanitizer_variant,
            target_platform_id=target_platform_id,
        )
        return model
    if target_platform_id != DEFAULT_TARGET_PLATFORM_ID:
        raise RuntimeError("sanitizer package channels are currently windows-x64 only")
    runtime_library_names = [
        entry.rsplit("/", 1)[-1]
        for entry in metadata.get("runtime_library_payload_entries", [])
        if not str(entry).endswith(".json")
    ]
    if runtime_library_names:
        runtime_library_names = [
            *RELEASE_RUNTIME_LIBRARY_NAMES_BY_PLATFORM[DEFAULT_TARGET_PLATFORM_ID],
            *runtime_library_names,
        ]
    else:
        runtime_library_names = RELEASE_RUNTIME_LIBRARY_NAMES_BY_PLATFORM[
            DEFAULT_TARGET_PLATFORM_ID
        ]
    return {
        "target_platform_id": DEFAULT_TARGET_PLATFORM_ID,
        "package_id": metadata["package_id"],
        "package_channel_id": metadata["package_channel_id"],
        "sanitizer_variant": sanitizer_variant,
        "runtime_variant": metadata["runtime_variant"],
        "runtime_library_ids": metadata["runtime_library_ids"],
        "runtime_library_names": runtime_library_names,
        "package_root_layout": required_payload_entries_for_platform(
            sanitizer_variant=sanitizer_variant,
            target_platform_id=target_platform_id,
        ),
        "missing_runtime_behavior": metadata.get(
            "missing_runtime_behavior",
            "fail-closed-before-native-execution-claim",
        ),
        "unsupported_behavior": "fail-closed",
        "support_truth": False,
        "native_execution_claimed": False,
    }


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def installer_signature_payload(installer_archive: Path) -> dict[str, Any]:
    return {
        "signature_format": "objc3c-local-sha256-v1",
        "signing_key_id": "objc3c-release-operations-local-installer-key-v1",
        "subject": "local-installer",
        "artifact": repo_rel(installer_archive),
        "sha256": sha256_file(installer_archive),
        "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
        "trust_scope": "checked-in-artifact-digest",
    }


def archive_digest_record(*, artifact_role: str, artifact_path: Path) -> dict[str, str]:
    return {
        "digest_format": "sha256",
        "artifact_role": artifact_role,
        "artifact": repo_rel(artifact_path),
        "sha256": sha256_file(artifact_path),
        "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
        "trust_scope": "checked-in-artifact-digest",
    }


def archive_digest_payloads(paths: PackageChannelPaths) -> dict[str, dict[str, str]]:
    artifact_paths = {
        "portable_archive": paths.portable_archive,
        "installer_archive": paths.installer_archive,
        "offline_archive": paths.offline_archive,
    }
    return {
        field_name: archive_digest_record(
            artifact_role=artifact_role,
            artifact_path=artifact_paths[field_name],
        )
        for field_name, artifact_role in ARCHIVE_DIGEST_FIELDS.items()
    }


def package_payload_contract(
    paths: PackageChannelPaths,
    *,
    runnable_manifest: dict[str, Any] | None = None,
) -> dict[str, Any]:
    manifest_path = paths.package_root / MANIFEST_RELATIVE_PATH
    resolved_manifest = (
        load_runnable_package_manifest(paths)
        if runnable_manifest is None
        else runnable_manifest
    )
    target_platform_id = target_platform_id_from_manifest(resolved_manifest)
    expected_entries = required_payload_entries_for_platform(
        sanitizer_variant=paths.sanitizer_variant,
        target_platform_id=target_platform_id,
    )
    entry_digests = {
        relative_path: package_payload_entry_digest(
            paths=paths,
            relative_path=relative_path,
        )
        for relative_path in expected_entries
    }
    return {
        "contract_id": "objc3c.packaging.channels.payload-contract.v1",
        "source": "canonical-runnable-toolchain-package",
        "manifest_relative_path": MANIFEST_RELATIVE_PATH,
        "manifest_artifact": repo_rel(manifest_path),
        "manifest_sha256": entry_digests[MANIFEST_RELATIVE_PATH]["sha256"],
        "target_platform_id": target_platform_id,
        "required_entries": expected_entries,
        "entry_digests": entry_digests,
        "clean_room_source_policy": "fresh-owned-tmp-root-only",
    }


def package_payload_entry_digest(
    *,
    paths: PackageChannelPaths,
    relative_path: str,
) -> dict[str, str]:
    artifact_path = paths.package_root / relative_path
    if not artifact_path.is_file():
        raise RuntimeError(f"package-channels payload missing required entry {relative_path}")
    return {
        "digest_format": "sha256",
        "artifact": relative_path,
        "sha256": sha256_file(artifact_path),
    }


def receipt_contract_payloads(
    sanitizer_variant: str = "release",
    *,
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> dict[str, dict[str, Any]]:
    return {
        "install_receipt": receipt_contract_payload(
            channel_id="local-installer",
            emitted_by="Install-objc3c.ps1",
            network_policy="local-filesystem-only",
            sanitizer_variant=sanitizer_variant,
            target_platform_id=target_platform_id,
        ),
        "offline_install_receipt": receipt_contract_payload(
            channel_id="offline-bundle",
            emitted_by="OfflineBootstrap-objc3c.ps1",
            network_policy="no-network",
            delegates_to="local-installer",
            sanitizer_variant=sanitizer_variant,
            target_platform_id=target_platform_id,
        ),
    }


def receipt_contract_payload(
    *,
    channel_id: str,
    emitted_by: str,
    network_policy: str,
    sanitizer_variant: str = "release",
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
    delegates_to: str | None = None,
) -> dict[str, Any]:
    metadata = sanitizer_variant_metadata(sanitizer_variant)
    package_id = str(metadata["package_id"])
    package_channel_id = str(metadata["package_channel_id"])
    if sanitizer_variant == "release":
        package_id = release_package_id_for_platform(target_platform_id)
        package_channel_id = release_package_channel_id_for_platform(target_platform_id)
    elif target_platform_id != DEFAULT_TARGET_PLATFORM_ID:
        raise RuntimeError("sanitizer package channels are currently windows-x64 only")
    required_fields = (
        SANITIZER_REQUIRED_RECEIPT_FIELDS
        if sanitizer_variant != "release"
        else REQUIRED_RECEIPT_FIELDS
    )
    payload: dict[str, Any] = {
        "contract_id": INSTALL_RECEIPT_CONTRACT_ID,
        "schema": INSTALL_RECEIPT_SCHEMA,
        "receipt_path": INSTALL_RECEIPT_PATH,
        "channel_id": channel_id,
        "emitted_by": emitted_by,
        "bootstrap_entrypoint": INSTALL_BOOTSTRAP_ENTRYPOINT,
        "package_bridge": "objc3c",
        "install_command": INSTALL_COMMAND,
        "payload_manifest": MANIFEST_RELATIVE_PATH,
        "payload_required_entries": required_payload_entries_for_platform(
            sanitizer_variant=sanitizer_variant,
            target_platform_id=target_platform_id,
        ),
        "required_fields": required_fields,
        "network_policy": network_policy,
        "rollback_required": True,
        "package_id": package_id,
        "package_channel_id": package_channel_id,
        "sanitizer_variant": sanitizer_variant,
        "target_platform_id": target_platform_id,
        "package_runtime_model": receipt_package_runtime_model(
            sanitizer_variant,
            target_platform_id=target_platform_id,
        ),
        "emitted_platform_fields": list(RECEIPT_PLATFORM_FIELDS),
        "support_truth": False,
        "native_execution_claimed": False,
    }
    if sanitizer_variant != "release":
        payload["sanitizer_install_selector"] = metadata["install_selector"]
        payload["sanitizer_runtime_library_manifest_path"] = metadata[
            "runtime_library_manifest_path"
        ]
        payload["sanitizer_runtime_library_required_entries"] = metadata[
            "runtime_library_payload_entries"
        ]
        payload["missing_runtime_behavior"] = metadata["missing_runtime_behavior"]
    if delegates_to is not None:
        payload["delegates_to"] = delegates_to
    return payload
