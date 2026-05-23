"""Hosted registry metadata and deterministic offline resolution."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object, validate_json_schema

from .digests import stable_digest
from .model import (
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    PACKAGE_MANAGER_TAMPER_CODE,
)
from .trust import (
    collect_signature_envelope_failures,
    default_trust_policy_payload,
    package_namespace_from_id,
    sign_subject_with_deterministic_test_key,
    signature_subject_payload,
    trust_diagnostic,
)

HOSTED_REGISTRY_CONTRACT_ID = "objc3c.package_ecosystem.hosted_registry_index.v1"
HOSTED_REGISTRY_SCHEMA_KEY = "objc3c-package-hosted-registry-index-v1"
HOSTED_REGISTRY_NETWORK_POLICY = "offline-fixture-metadata-only"
HOSTED_REGISTRY_RESOLVER_ID = "deterministic-hosted-registry-offline-resolver-v1"
HOSTED_REGISTRY_SCHEMA_PATH = (
    Path(__file__).resolve().parents[2]
    / "schemas"
    / "objc3c-package-hosted-registry-index-v1.schema.json"
)


@dataclass(frozen=True)
class HostedRegistryResolutionRequest:
    package_id: str
    package_version: str | None = None
    language_version: str = LOCAL_PACKAGE_LANGUAGE_VERSION
    abi_identity: str = LOCAL_PACKAGE_ABI_IDENTITY
    allow_network: bool = False
    registry_url: str | None = None


@dataclass(frozen=True)
class HostedRegistryResolution:
    package_id: str
    package_version: str
    source_digest: str
    manifest_digest: str
    cache_path: str
    cache_digest: str
    registry_record_digest: str
    registry_signature_id: str


class HostedRegistryResolutionError(RuntimeError):
    """Raised when hosted registry resolution must fail closed."""

    def __init__(self, failures: list[str]) -> None:
        self.failures = failures
        super().__init__("\n".join(failures))


def hosted_registry_diagnostic(message: str) -> str:
    return f"{PACKAGE_MANAGER_TAMPER_CODE}: {message}"


def _record_without_integrity_fields(record: dict[str, Any]) -> dict[str, Any]:
    payload = dict(record)
    payload.pop("metadata_digest", None)
    payload.pop("registry_signature", None)
    return payload


def hosted_registry_record_digest(record: dict[str, Any]) -> str:
    return stable_digest(_record_without_integrity_fields(record))


def hosted_registry_record_signature_subject(record: dict[str, Any]) -> dict[str, str]:
    package_manifest = record.get("package_manifest", {})
    if not isinstance(package_manifest, dict):
        package_manifest = {}
    package_id = str(record.get("package_id", ""))
    return signature_subject_payload(
        subject_kind="hosted-registry-record",
        package_id=package_id,
        package_version=str(record.get("package_version", "")),
        package_namespace=package_namespace_from_id(package_id),
        artifact_digest=str(record.get("source_digest", "")),
        manifest_digest=str(record.get("metadata_digest", "")),
        abi_identity=str(record.get("abi_identity", "")),
        language_version=str(record.get("language_version", "")),
        lock_digest=str(package_manifest.get("digest", "")),
    )


def package_manifest_signature_subject_from_record(record: dict[str, Any]) -> dict[str, str]:
    package_manifest = record.get("package_manifest", {})
    if not isinstance(package_manifest, dict):
        package_manifest = {}
    package_id = str(record.get("package_id", ""))
    return signature_subject_payload(
        subject_kind="package-manifest",
        package_id=package_id,
        package_version=str(record.get("package_version", "")),
        package_namespace=package_namespace_from_id(package_id),
        artifact_digest=str(record.get("source_digest", "")),
        manifest_digest=str(package_manifest.get("digest", "")),
        abi_identity=str(record.get("abi_identity", "")),
        language_version=str(record.get("language_version", "")),
    )


def sign_hosted_registry_record(
    record: dict[str, Any],
    *,
    trust_policy: dict[str, Any] | None = None,
) -> dict[str, Any]:
    signed = _record_without_integrity_fields(record)
    signed["metadata_digest"] = hosted_registry_record_digest(signed)
    signed["registry_signature"] = sign_subject_with_deterministic_test_key(
        hosted_registry_record_signature_subject(signed),
        trust_policy=trust_policy,
    )
    return signed


def validate_hosted_registry_schema(index: dict[str, Any]) -> list[str]:
    try:
        schema = load_json_object(HOSTED_REGISTRY_SCHEMA_PATH)
        validate_json_schema(index, schema, label=HOSTED_REGISTRY_SCHEMA_KEY)
    except RuntimeError as exc:
        return [hosted_registry_diagnostic(f"hosted registry schema validation failed: {exc}")]
    return []


def mirror_packages_by_key(mirror: dict[str, Any]) -> dict[tuple[str, str], dict[str, Any]]:
    packages = mirror.get("packages", [])
    if not isinstance(packages, list):
        return {}
    return {
        (str(package.get("package_id")), str(package.get("package_version"))): package
        for package in packages
        if isinstance(package, dict)
    }


def _revoked_values(index: dict[str, Any], field_name: str) -> set[str]:
    revocations = index.get("revocations", {})
    if not isinstance(revocations, dict):
        return set()
    values = revocations.get(field_name, [])
    if not isinstance(values, list):
        return set()
    return {str(value) for value in values if isinstance(value, str)}


def collect_hosted_registry_model_failures(
    index: dict[str, Any],
    mirror: dict[str, Any],
    *,
    trust_policy: dict[str, Any] | None = None,
) -> list[str]:
    failures = validate_hosted_registry_schema(index)
    policy = trust_policy if isinstance(trust_policy, dict) else default_trust_policy_payload()

    if index.get("contract_id") != HOSTED_REGISTRY_CONTRACT_ID:
        failures.append(hosted_registry_diagnostic("hosted registry contract id drifted"))
    if index.get("network_policy") != HOSTED_REGISTRY_NETWORK_POLICY:
        failures.append(hosted_registry_diagnostic("network fetch request rejected by hosted registry policy"))
    if index.get("resolver") != HOSTED_REGISTRY_RESOLVER_ID:
        failures.append(hosted_registry_diagnostic("hosted registry resolver id drifted"))
    registry_id = str(index.get("registry_id", ""))
    if index.get("registry_state") != "not-revoked":
        failures.append(hosted_registry_diagnostic(f"revoked registry {registry_id}"))
    if registry_id in _revoked_values(index, "revoked_registry_ids"):
        failures.append(hosted_registry_diagnostic(f"revoked registry {registry_id}"))

    mirror_packages = mirror_packages_by_key(mirror)
    packages = index.get("packages", [])
    if not isinstance(packages, list):
        return [*failures, hosted_registry_diagnostic("hosted registry packages field is not a list")]

    seen_keys: set[tuple[str, str]] = set()
    for record in packages:
        if not isinstance(record, dict):
            failures.append(hosted_registry_diagnostic("hosted registry package record is not an object"))
            continue
        package_id = str(record.get("package_id", ""))
        package_version = str(record.get("package_version", ""))
        record_key = (package_id, package_version)
        if record_key in seen_keys:
            failures.append(hosted_registry_diagnostic(f"nondeterministic candidates for {package_id}@{package_version}"))
        seen_keys.add(record_key)

        if package_id in _revoked_values(index, "revoked_package_ids"):
            failures.append(hosted_registry_diagnostic(f"revoked package {package_id}"))
        metadata_digest = str(record.get("metadata_digest", ""))
        expected_metadata_digest = hosted_registry_record_digest(record)
        if metadata_digest != expected_metadata_digest:
            failures.append(hosted_registry_diagnostic(f"registry metadata digest mismatch for {package_id}"))

        registry_signature = record.get("registry_signature")
        if isinstance(registry_signature, dict):
            signature_id = str(registry_signature.get("signature_id", ""))
            if signature_id in _revoked_values(index, "revoked_signature_ids"):
                failures.append(hosted_registry_diagnostic(f"revoked signature {signature_id}"))
        failures.extend(
            collect_signature_envelope_failures(
                registry_signature,
                expected_subject=hosted_registry_record_signature_subject(record),
                trust_policy=policy,
            )
        )
        failures.extend(
            collect_signature_envelope_failures(
                record.get("trust"),
                expected_subject=package_manifest_signature_subject_from_record(record),
                trust_policy=policy,
            )
        )

        if record.get("language_version") != LOCAL_PACKAGE_LANGUAGE_VERSION:
            failures.append(hosted_registry_diagnostic(f"language mismatch for {package_id}"))
        if record.get("abi_identity") != LOCAL_PACKAGE_ABI_IDENTITY:
            failures.append(hosted_registry_diagnostic(f"ABI mismatch for {package_id}"))

        mirror_package = mirror_packages.get(record_key)
        if mirror_package is None:
            failures.append(hosted_registry_diagnostic(f"missing offline mirror metadata for {package_id}@{package_version}"))
            continue
        offline_mirror = record.get("offline_mirror", {})
        if not isinstance(offline_mirror, dict):
            failures.append(hosted_registry_diagnostic(f"missing offline mirror pin for {package_id}"))
            continue
        mirror_manifest = mirror_package.get("package_manifest", {})
        record_manifest = record.get("package_manifest", {})
        if not isinstance(mirror_manifest, dict):
            mirror_manifest = {}
        if not isinstance(record_manifest, dict):
            record_manifest = {}
        expected_pins = {
            "source_digest": mirror_package.get("source_digest"),
            "manifest_digest": mirror_manifest.get("digest"),
            "cache_path": mirror_package.get("cache_path"),
            "cache_digest": mirror_package.get("cache_digest"),
        }
        actual_pins = {
            "source_digest": record.get("source_digest"),
            "manifest_digest": record_manifest.get("digest"),
            "cache_path": offline_mirror.get("cache_path"),
            "cache_digest": offline_mirror.get("cache_digest"),
        }
        if actual_pins != expected_pins:
            failures.append(hosted_registry_diagnostic(f"cache/offline mirror pin mismatch for {package_id}"))
    return failures


def network_fetch_request_failures(request: HostedRegistryResolutionRequest) -> list[str]:
    if request.allow_network or request.registry_url:
        target = request.registry_url or "live registry endpoint"
        return [hosted_registry_diagnostic(f"network fetch request rejected for {target}")]
    return []


def resolve_hosted_registry_package(
    index: dict[str, Any],
    mirror: dict[str, Any],
    request: HostedRegistryResolutionRequest,
    *,
    trust_policy: dict[str, Any] | None = None,
) -> HostedRegistryResolution:
    failures = network_fetch_request_failures(request)
    failures.extend(
        collect_hosted_registry_model_failures(
            index,
            mirror,
            trust_policy=trust_policy,
        )
    )
    if request.language_version != LOCAL_PACKAGE_LANGUAGE_VERSION:
        failures.append(hosted_registry_diagnostic(f"language mismatch for {request.package_id}"))
    if request.abi_identity != LOCAL_PACKAGE_ABI_IDENTITY:
        failures.append(hosted_registry_diagnostic(f"ABI mismatch for {request.package_id}"))

    packages = index.get("packages", [])
    candidates = [
        package
        for package in packages
        if isinstance(package, dict)
        and str(package.get("package_id")) == request.package_id
        and (
            request.package_version is None
            or str(package.get("package_version")) == request.package_version
        )
    ] if isinstance(packages, list) else []
    if not candidates:
        suffix = f"@{request.package_version}" if request.package_version else ""
        failures.append(hosted_registry_diagnostic(f"missing hosted registry metadata for {request.package_id}{suffix}"))
    elif len(candidates) != 1:
        failures.append(hosted_registry_diagnostic(f"nondeterministic candidates for {request.package_id}"))

    if failures:
        raise HostedRegistryResolutionError(failures)

    record = candidates[0]
    package_manifest = record.get("package_manifest", {})
    offline_mirror = record.get("offline_mirror", {})
    registry_signature = record.get("registry_signature", {})
    return HostedRegistryResolution(
        package_id=str(record["package_id"]),
        package_version=str(record["package_version"]),
        source_digest=str(record["source_digest"]),
        manifest_digest=str(package_manifest["digest"]),
        cache_path=str(offline_mirror["cache_path"]),
        cache_digest=str(offline_mirror["cache_digest"]),
        registry_record_digest=str(record["metadata_digest"]),
        registry_signature_id=str(registry_signature["signature_id"]),
    )


__all__ = [
    "HOSTED_REGISTRY_CONTRACT_ID",
    "HOSTED_REGISTRY_NETWORK_POLICY",
    "HOSTED_REGISTRY_RESOLVER_ID",
    "HOSTED_REGISTRY_SCHEMA_KEY",
    "HOSTED_REGISTRY_SCHEMA_PATH",
    "HostedRegistryResolution",
    "HostedRegistryResolutionError",
    "HostedRegistryResolutionRequest",
    "collect_hosted_registry_model_failures",
    "hosted_registry_diagnostic",
    "hosted_registry_record_digest",
    "hosted_registry_record_signature_subject",
    "network_fetch_request_failures",
    "resolve_hosted_registry_package",
    "sign_hosted_registry_record",
    "trust_diagnostic",
    "validate_hosted_registry_schema",
]
