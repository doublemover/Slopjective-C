"""Hosted registry metadata and deterministic offline resolution."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re
from typing import Any

from objc3c_shared.json_io import load_json_object, validate_json_schema

from .digests import stable_digest
from .model import (
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_HOST_PLATFORM,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    PACKAGE_MANAGER_TAMPER_CODE,
)
from .hosted_service import (
    HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID,
    HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID,
    HOSTED_REGISTRY_SERVICE_ID,
    HOSTED_REGISTRY_LIVE_SERVICE_CONTRACT_ID,
    HOSTED_REGISTRY_LIVE_SERVICE_REQUIRED_DIAGNOSTICS,
    HOSTED_REGISTRY_LIVE_SERVICE_STATE,
    HOSTED_REGISTRY_LIVE_SERVICE_UNSUPPORTED_MODE,
    HOSTED_REGISTRY_LIVE_TRANSPORT_ID,
    HOSTED_REGISTRY_PUBLIC_CAPABILITY_ID,
    HostedRegistryServiceRequest,
    collect_hosted_registry_service_reference_failures,
    collect_hosted_registry_service_request_failures,
)
from .trust import (
    collect_signature_envelope_failures,
    default_trust_policy_payload,
    package_namespace_from_id,
    sign_subject_with_deterministic_test_key,
    signature_subject_payload,
    trust_diagnostic,
    trust_roots_by_id,
)

HOSTED_REGISTRY_CONTRACT_ID = "objc3c.package_ecosystem.hosted_registry_index.v1"
HOSTED_REGISTRY_SCHEMA_KEY = "objc3c-package-hosted-registry-index-v1"
HOSTED_REGISTRY_NETWORK_POLICY = "offline-fixture-metadata-only"
HOSTED_REGISTRY_RESOLVER_ID = "deterministic-hosted-registry-offline-resolver-v1"
HOSTED_REGISTRY_PROVIDER_ID = "schema-backed-hosted-registry-provider-v1"
HOSTED_REGISTRY_TRUST_VALIDATOR_ID = "schema-backed-hosted-registry-trust-validator-v1"
HOSTED_REGISTRY_SNAPSHOT_POLICY = "monotonic-sequence-required"
HOSTED_REGISTRY_LOCK_MATERIALIZATION_POLICY = "lockfile-first-offline-mirror-handoff-v1"
HOSTED_REGISTRY_ENDPOINT_ID = "objc3c-hosted-registry-fixture-endpoint-v1"
HOSTED_REGISTRY_CHANNEL_ID = "stable-fixture"
HOSTED_REGISTRY_BASE_URL = "https://registry.objc3c.invalid/fixture/v1"
OFFLINE_MIRROR_CONTRACT_ID = "objc3c.package_ecosystem.offline_mirror.v1"
OFFLINE_MIRROR_NETWORK_POLICY = "no-network-during-validation"
HOSTED_REGISTRY_FAILURE_MODES = {
    "ambiguous-module-identity",
    "ambiguous-version-selection",
    "cache-identity-drift",
    "dependency-cycle",
    "digest-drift",
    "duplicate-version-entry",
    "endpoint-channel-drift",
    "fallback-registry-success",
    "invalid-semver",
    "live-network-fetch",
    "live-public-service-unavailable",
    "live-transport-disabled",
    "missing-package-provenance",
    "missing-service-auth",
    "offline-mirror-handoff-drift",
    "production-auth-unavailable",
    "production-availability-unavailable",
    "production-moderation-unavailable",
    "production-trust-root-unavailable",
    "registry-trust-mismatch",
    "revoked-subject",
    "rollback-snapshot",
    "service-auth-token-drift",
    "service-availability-unavailable",
    "service-contract-drift",
    "service-index-drift",
    "service-moderation-blocked",
    "service-revocation-unavailable",
    "unavailable-registry",
    "unlocked-version-selection",
    "unpinned-hosted-dependency",
    "unsigned-hosted-artifact",
    "unknown-trust-root",
    "unknown-service-auth-subject",
    "unsupported-live-service-mode",
    "unsupported-platform",
    "yanked-version",
}
HOSTED_REGISTRY_CACHE_POLICY = "offline-cache-required-digest-pinned"
HOSTED_REGISTRY_PROVENANCE_POLICY = "source-owned-package-manifest-required"
HOSTED_REGISTRY_SELECTION_POLICY = "exact-pinned-version-only"
HOSTED_REGISTRY_FIXTURE_AVAILABILITY_STATE = "offline-fixture-available"
HOSTED_REGISTRY_CACHE_ORIGIN = "offline-mirror-lock-materialized"
HOSTED_REGISTRY_SERVICE_BOUNDARY = {
    "support_state": "fixture-only-offline",
    "supported_capability_id": "ecosystem.package-manager.hosted-registry-fixture",
    "reserved_capability_id": "ecosystem.package-manager.public-hosted-registry",
    "availability": "not-claimed",
    "auth": "not-implemented-reserved",
    "moderation": "not-implemented-reserved",
    "live_network_fetch": "forbidden-fail-closed",
    "fallback_registry_success": False,
    "package_manager_parity": "not-claimed",
    "local_offline_replay_preserved": True,
}
HOSTED_REGISTRY_LIVE_SERVICE_BOUNDARY = {
    "contract_id": HOSTED_REGISTRY_LIVE_SERVICE_CONTRACT_ID,
    "capability_id": HOSTED_REGISTRY_PUBLIC_CAPABILITY_ID,
    "claim_state": HOSTED_REGISTRY_LIVE_SERVICE_STATE,
    "activation_state": "inactive-live-service-unavailable",
    "unsupported_mode": HOSTED_REGISTRY_LIVE_SERVICE_UNSUPPORTED_MODE,
    "fallback_registry_success": False,
    "offline_replay_preserved": True,
}
HOSTED_REGISTRY_SCHEMA_PATH = (
    Path(__file__).resolve().parents[2]
    / "schemas"
    / "objc3c-package-hosted-registry-index-v1.schema.json"
)
_SEMVER_RE = re.compile(r"^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$")


@dataclass(frozen=True)
class HostedRegistryResolutionRequest:
    package_id: str
    package_version: str | None = None
    language_version: str = LOCAL_PACKAGE_LANGUAGE_VERSION
    abi_identity: str = LOCAL_PACKAGE_ABI_IDENTITY
    host_platform: str = LOCAL_PACKAGE_HOST_PLATFORM
    endpoint_id: str = HOSTED_REGISTRY_ENDPOINT_ID
    channel_id: str = HOSTED_REGISTRY_CHANNEL_ID
    minimum_snapshot_sequence: int = 1
    allow_network: bool = False
    registry_url: str | None = None
    service_id: str = HOSTED_REGISTRY_SERVICE_ID
    auth_subject_id: str = HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID
    auth_token_id: str = HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID


@dataclass(frozen=True)
class HostedRegistryProviderModel:
    provider_id: str
    network_fetch_policy: str
    resolver_id: str
    trust_validator_id: str
    selection_policy: str


@dataclass(frozen=True)
class HostedRegistrySnapshot:
    snapshot_id: str
    registry_id: str
    sequence: int
    source_lock_digest: str
    rollback_policy: str


@dataclass(frozen=True)
class HostedRegistryPackageVersion:
    package_id: str
    package_version: str
    version_state: str
    yank_state: str
    supported_platforms: tuple[str, ...]


@dataclass(frozen=True)
class HostedRegistryDependencyRecord:
    package_id: str
    depends_on_package_id: str
    version_requirement: str
    resolution_policy: str


@dataclass(frozen=True)
class HostedRegistryTrustResult:
    package_id: str
    package_version: str
    status: str
    trust_root_id: str
    registry_signature_id: str
    package_signature_id: str


@dataclass(frozen=True)
class HostedRegistryCacheIdentity:
    cache_key: str
    package_id: str
    package_version: str
    cache_path: str
    cache_digest: str
    source_digest: str
    manifest_digest: str


@dataclass(frozen=True)
class HostedRegistryOfflineMirrorHandoff:
    handoff_id: str
    package_id: str
    package_version: str
    mirror_path: str
    cache_key: str
    network_required_after_lock: bool


@dataclass(frozen=True)
class HostedRegistryResolution:
    package_id: str
    package_version: str
    source_digest: str
    manifest_digest: str
    cache_path: str
    cache_digest: str
    snapshot_id: str
    cache_key: str
    offline_mirror_path: str
    registry_record_digest: str
    registry_signature_id: str
    trust_result_id: str


class HostedRegistryResolutionError(RuntimeError):
    """Raised when hosted registry resolution must fail closed."""

    def __init__(self, failures: list[str]) -> None:
        self.failures = failures
        super().__init__("\n".join(failures))


def hosted_registry_diagnostic(message: str) -> str:
    return f"{PACKAGE_MANAGER_TAMPER_CODE}: {message}"


def _is_exact_semver(value: str) -> bool:
    return _SEMVER_RE.fullmatch(value) is not None


def _semver_parts(value: str) -> tuple[int, int, int] | None:
    match = _SEMVER_RE.fullmatch(value)
    if match is None:
        return None
    return (int(match.group(1)), int(match.group(2)), int(match.group(3)))


def _package_key(package_id: str, package_version: str) -> str:
    return f"{package_id}@{package_version}"


def _registry_record_key(record: dict[str, Any]) -> tuple[str, str]:
    return (str(record.get("package_id", "")), str(record.get("package_version", "")))


def _version_keys_from_records(records: list[dict[str, Any]]) -> set[tuple[str, str]]:
    return {_registry_record_key(record) for record in records}


def _as_object_list(value: Any) -> list[dict[str, Any]]:
    if not isinstance(value, list):
        return []
    return [item for item in value if isinstance(item, dict)]


def _string_values(value: Any) -> set[str]:
    if not isinstance(value, list):
        return set()
    return {str(item) for item in value if isinstance(item, str)}


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


def collect_offline_mirror_contract_failures(mirror: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    if mirror.get("contract_id") != OFFLINE_MIRROR_CONTRACT_ID:
        failures.append(hosted_registry_diagnostic("offline mirror contract id drifted"))
    if mirror.get("network_policy") != OFFLINE_MIRROR_NETWORK_POLICY:
        failures.append(hosted_registry_diagnostic("offline mirror network policy drifted"))
    packages = mirror.get("packages", [])
    if not isinstance(packages, list):
        failures.append(hosted_registry_diagnostic("offline mirror packages field is not a list"))
    return failures


def _revoked_values(index: dict[str, Any], field_name: str) -> set[str]:
    revocations = index.get("revocations", {})
    if not isinstance(revocations, dict):
        return set()
    values = revocations.get(field_name, [])
    if not isinstance(values, list):
        return set()
    return {str(value) for value in values if isinstance(value, str)}


def collect_endpoint_identity_failures(index: dict[str, Any]) -> list[str]:
    endpoint = index.get("endpoint_identity", {})
    registry_id = str(index.get("registry_id", ""))
    if not isinstance(endpoint, dict):
        return [hosted_registry_diagnostic("missing hosted registry endpoint identity")]
    failures: list[str] = []
    expected = {
        "registry_id": registry_id,
        "endpoint_id": HOSTED_REGISTRY_ENDPOINT_ID,
        "channel_id": HOSTED_REGISTRY_CHANNEL_ID,
        "base_url": HOSTED_REGISTRY_BASE_URL,
        "availability_claim": "fixture-metadata-only",
        "network_fetch": "forbidden-fail-closed",
    }
    for field_name, expected_value in expected.items():
        if endpoint.get(field_name) != expected_value:
            failures.append(hosted_registry_diagnostic(f"hosted registry endpoint {field_name} drifted"))
    if endpoint.get("fallback_registry_success") is not False:
        failures.append(hosted_registry_diagnostic("fallback registry success path is forbidden"))
    return failures


def collect_service_boundary_failures(index: dict[str, Any]) -> list[str]:
    boundary = index.get("service_boundary", {})
    if not isinstance(boundary, dict):
        return [hosted_registry_diagnostic("missing hosted registry service boundary")]
    failures: list[str] = []
    for field_name, expected_value in HOSTED_REGISTRY_SERVICE_BOUNDARY.items():
        if boundary.get(field_name) != expected_value:
            failures.append(
                hosted_registry_diagnostic(
                    f"hosted registry service boundary {field_name} drifted"
                )
            )
    return failures


def collect_live_service_boundary_failures(index: dict[str, Any]) -> list[str]:
    boundary = index.get("live_service_boundary", {})
    if not isinstance(boundary, dict):
        return [
            hosted_registry_diagnostic(
                "missing live public hosted registry service boundary"
            )
        ]

    failures: list[str] = []
    for field_name, expected_value in HOSTED_REGISTRY_LIVE_SERVICE_BOUNDARY.items():
        if boundary.get(field_name) != expected_value:
            failures.append(
                hosted_registry_diagnostic(
                    f"live public hosted registry service boundary {field_name} drifted"
                )
            )

    transport = boundary.get("network_transport", {})
    if not isinstance(transport, dict):
        failures.append(
            hosted_registry_diagnostic(
                "missing live public hosted registry transport boundary"
            )
        )
    else:
        expected_transport = {
            "transport_id": HOSTED_REGISTRY_LIVE_TRANSPORT_ID,
            "mode": "disabled-live-public-transport",
            "request_policy": "fail-closed-before-resolver",
            "unsupported_diagnostic": "live-transport-disabled",
            "fallback_registry_success": False,
        }
        for field_name, expected_value in expected_transport.items():
            if transport.get(field_name) != expected_value:
                failures.append(
                    hosted_registry_diagnostic(
                        f"live public hosted registry transport {field_name} drifted"
                    )
                )
        if transport.get("separated_from_resolver") is not True:
            failures.append(
                hosted_registry_diagnostic(
                    "live public hosted registry transport is not separated from resolver"
                )
            )
        if transport.get("enabled") is not False:
            failures.append(
                hosted_registry_diagnostic(
                    "live public hosted registry transport enabled"
                )
            )

    production_records = boundary.get("production_records", {})
    if not isinstance(production_records, dict):
        failures.append(
            hosted_registry_diagnostic(
                "missing live public hosted registry production records"
            )
        )
    else:
        expected_records = {
            "auth": (
                "production-auth-reserved-fail-closed-v1",
                "production-auth-unavailable",
            ),
            "moderation": (
                "production-moderation-reserved-fail-closed-v1",
                "production-moderation-unavailable",
            ),
            "trust_root": (
                "production-trust-root-reserved-fail-closed-v1",
                "production-trust-root-unavailable",
            ),
            "availability": (
                "production-availability-reserved-fail-closed-v1",
                "production-availability-unavailable",
            ),
        }
        for field_name, (record_id, diagnostic) in expected_records.items():
            record = production_records.get(field_name, {})
            if not isinstance(record, dict):
                failures.append(
                    hosted_registry_diagnostic(
                        f"missing live public hosted registry {field_name} record"
                    )
                )
                continue
            expected = {
                "record_id": record_id,
                "state": HOSTED_REGISTRY_LIVE_SERVICE_STATE,
                "unavailable_behavior": "fail-closed",
                "unsupported_diagnostic": diagnostic,
            }
            for key, expected_value in expected.items():
                if record.get(key) != expected_value:
                    failures.append(
                        hosted_registry_diagnostic(
                            f"live public hosted registry {field_name} {key} drifted"
                        )
                    )
            if record.get("active") is not False:
                failures.append(
                    hosted_registry_diagnostic(
                        f"live public hosted registry {field_name} activated"
                    )
                )

    handoff = boundary.get("lock_offline_handoff", {})
    if not isinstance(handoff, dict):
        failures.append(
            hosted_registry_diagnostic(
                "missing live public hosted registry lock/offline mirror handoff"
            )
        )
    else:
        expected_handoff = {
            "policy": HOSTED_REGISTRY_LOCK_MATERIALIZATION_POLICY,
            "unsupported_live_resolution_behavior": "fail-closed-before-network",
        }
        for field_name, expected_value in expected_handoff.items():
            if handoff.get(field_name) != expected_value:
                failures.append(
                    hosted_registry_diagnostic(
                        f"live public hosted registry handoff {field_name} drifted"
                    )
                )
        for field_name in (
            "lock_required_before_live_resolution",
            "offline_mirror_handoff_required",
            "offline_replay_preserved",
        ):
            if handoff.get(field_name) is not True:
                failures.append(
                    hosted_registry_diagnostic(
                        f"live public hosted registry handoff disabled {field_name}"
                    )
                )
        if handoff.get("network_required_after_lock") is not False:
            failures.append(
                hosted_registry_diagnostic(
                    "live public hosted registry handoff requires network after lock"
                )
            )

    diagnostics = _string_values(boundary.get("diagnostics"))
    missing_diagnostics = sorted(
        HOSTED_REGISTRY_LIVE_SERVICE_REQUIRED_DIAGNOSTICS - diagnostics
    )
    if missing_diagnostics:
        failures.append(
            hosted_registry_diagnostic(
                "live public hosted registry diagnostics missing: "
                + ", ".join(missing_diagnostics)
            )
        )
    return failures


def collect_provider_model_failures(index: dict[str, Any]) -> list[str]:
    provider = index.get("provider_model", {})
    if not isinstance(provider, dict):
        return [hosted_registry_diagnostic("missing hosted registry provider model")]
    failures: list[str] = []
    if provider.get("provider_id") != HOSTED_REGISTRY_PROVIDER_ID:
        failures.append(hosted_registry_diagnostic("hosted registry provider id drifted"))

    network_fetch = provider.get("network_fetch", {})
    if not isinstance(network_fetch, dict):
        failures.append(hosted_registry_diagnostic("hosted registry network fetch layer missing"))
    else:
        if network_fetch.get("separated_from_resolution") is not True:
            failures.append(hosted_registry_diagnostic("hosted registry network fetch is not separated from resolution"))
        if network_fetch.get("policy") != "forbidden-fail-closed":
            failures.append(hosted_registry_diagnostic("hosted registry network fetch policy drifted"))
        if network_fetch.get("live_fetch_enabled") is not False:
            failures.append(hosted_registry_diagnostic("hosted registry live fetch enabled"))

    transport = provider.get("transport", {})
    if not isinstance(transport, dict):
        failures.append(hosted_registry_diagnostic("hosted registry transport layer missing"))
    else:
        expected_transport = {
            "transport_id": HOSTED_REGISTRY_LIVE_TRANSPORT_ID,
            "mode": "disabled-live-public-transport",
            "request_policy": "fail-closed-before-resolver",
            "unsupported_diagnostic": "live-transport-disabled",
            "fallback_registry_success": False,
        }
        for field_name, expected_value in expected_transport.items():
            if transport.get(field_name) != expected_value:
                failures.append(
                    hosted_registry_diagnostic(
                        f"hosted registry transport {field_name} drifted"
                    )
                )
        if transport.get("separated_from_resolver") is not True:
            failures.append(
                hosted_registry_diagnostic(
                    "hosted registry transport is not separated from resolver"
                )
            )
        if transport.get("live_transport_enabled") is not False:
            failures.append(
                hosted_registry_diagnostic("hosted registry live transport enabled")
            )
        if transport.get("resolver_invocation_allowed") is not False:
            failures.append(
                hosted_registry_diagnostic(
                    "hosted registry transport can invoke resolver"
                )
            )

    resolver = provider.get("resolver", {})
    if not isinstance(resolver, dict):
        failures.append(hosted_registry_diagnostic("hosted registry resolver model missing"))
    else:
        if resolver.get("resolver_id") != HOSTED_REGISTRY_RESOLVER_ID:
            failures.append(hosted_registry_diagnostic("hosted registry provider resolver id drifted"))
        if resolver.get("selection_policy") != HOSTED_REGISTRY_SELECTION_POLICY:
            failures.append(hosted_registry_diagnostic("hosted registry selection policy drifted"))
        if resolver.get("allow_unpinned_versions") is not False:
            failures.append(hosted_registry_diagnostic("hosted registry resolver allows unpinned versions"))
        if resolver.get("fallback_registry_success") is not False:
            failures.append(hosted_registry_diagnostic("fallback registry success path is forbidden"))

    trust_validator = provider.get("trust_validator", {})
    if not isinstance(trust_validator, dict):
        failures.append(hosted_registry_diagnostic("hosted registry trust validator missing"))
    else:
        if trust_validator.get("validator_id") != HOSTED_REGISTRY_TRUST_VALIDATOR_ID:
            failures.append(hosted_registry_diagnostic("hosted registry trust validator id drifted"))
        for field_name in (
            "requires_schema",
            "requires_signature",
            "requires_trust_root",
            "requires_revocation_check",
            "requires_cache_identity",
            "requires_offline_mirror_handoff",
        ):
            if trust_validator.get(field_name) is not True:
                failures.append(hosted_registry_diagnostic(f"hosted registry trust validator disabled {field_name}"))
        if trust_validator.get("allows_local_install_fallback") is not False:
            failures.append(hosted_registry_diagnostic("unsigned hosted artifact local install fallback is forbidden"))
    return failures


def collect_registry_snapshot_failures(index: dict[str, Any]) -> list[str]:
    snapshot = index.get("snapshot", {})
    if not isinstance(snapshot, dict):
        return [hosted_registry_diagnostic("missing hosted registry snapshot")]
    failures: list[str] = []
    registry_id = str(index.get("registry_id", ""))
    if snapshot.get("registry_id") != registry_id:
        failures.append(hosted_registry_diagnostic("hosted registry snapshot registry id drifted"))
    sequence = snapshot.get("sequence")
    if not isinstance(sequence, int) or sequence < 1:
        failures.append(hosted_registry_diagnostic("hosted registry snapshot sequence is invalid"))
    expected_snapshot_id = f"{registry_id}@{index.get('registry_version')}"
    if snapshot.get("snapshot_id") != expected_snapshot_id:
        failures.append(hosted_registry_diagnostic("hosted registry snapshot id drifted"))
    lock_trust_material = index.get("lock_trust_material", {})
    expected_source_lock_digest = (
        str(lock_trust_material.get("source_lock_digest", ""))
        if isinstance(lock_trust_material, dict)
        else ""
    )
    if snapshot.get("source_lock_digest") != expected_source_lock_digest:
        failures.append(hosted_registry_diagnostic("hosted registry snapshot source lock digest drifted"))
    if snapshot.get("rollback_policy") != HOSTED_REGISTRY_SNAPSHOT_POLICY:
        failures.append(hosted_registry_diagnostic("hosted registry rollback policy drifted"))
    if snapshot.get("network_fetch_observed") is not False:
        failures.append(hosted_registry_diagnostic("hosted registry snapshot observed a live network fetch"))
    return failures


def collect_service_availability_failures(index: dict[str, Any]) -> list[str]:
    availability = index.get("service_availability", {})
    if not isinstance(availability, dict):
        return [hosted_registry_diagnostic("missing hosted registry service availability")]
    failures: list[str] = []
    if availability.get("state") != HOSTED_REGISTRY_FIXTURE_AVAILABILITY_STATE:
        failures.append(hosted_registry_diagnostic("hosted registry unavailable"))
    if availability.get("live_service_available") is not False:
        failures.append(hosted_registry_diagnostic("hosted registry live service availability is claimed"))
    if availability.get("network_fetch_allowed") is not False:
        failures.append(hosted_registry_diagnostic("hosted registry service allows network fetch"))
    if availability.get("failure_policy") != "unavailable-fails-closed":
        failures.append(hosted_registry_diagnostic("hosted registry service availability failure policy drifted"))
    if availability.get("health_source") != "checked-in-fixture-only":
        failures.append(hosted_registry_diagnostic("hosted registry service availability source drifted"))
    return failures


def collect_lock_materialization_failures(index: dict[str, Any], mirror: dict[str, Any]) -> list[str]:
    policy = index.get("lock_materialization", {})
    if not isinstance(policy, dict):
        return [hosted_registry_diagnostic("missing hosted registry lock materialization policy")]
    failures: list[str] = []
    if policy.get("policy") != HOSTED_REGISTRY_LOCK_MATERIALIZATION_POLICY:
        failures.append(hosted_registry_diagnostic("hosted registry lock materialization policy drifted"))
    if policy.get("source_lock") != mirror.get("source_lock"):
        failures.append(hosted_registry_diagnostic("hosted registry lock materialization source lock drifted"))
    if policy.get("network_required_after_lock") is not False:
        failures.append(hosted_registry_diagnostic("hosted registry lock replay requires network"))
    if policy.get("unsigned_artifact_local_install_fallback") is not False:
        failures.append(hosted_registry_diagnostic("unsigned hosted artifact local install fallback is forbidden"))
    if policy.get("offline_replay_sufficient") is not True:
        failures.append(hosted_registry_diagnostic("hosted registry offline replay is not sufficient after lock materialization"))
    return failures


def collect_lock_trust_material_failures(
    index: dict[str, Any],
    mirror: dict[str, Any],
    *,
    root: Path | None = None,
) -> list[str]:
    material = index.get("lock_trust_material", {})
    if not isinstance(material, dict):
        return [hosted_registry_diagnostic("missing hosted registry lock/trust material")]
    failures: list[str] = []
    if material.get("source_lock") != mirror.get("source_lock"):
        failures.append(hosted_registry_diagnostic("hosted registry source lock drifted"))
    if not isinstance(material.get("offline_mirror_path"), str) or not material.get("offline_mirror_path"):
        failures.append(hosted_registry_diagnostic("hosted registry offline mirror path missing"))
    if material.get("cache_policy") != HOSTED_REGISTRY_CACHE_POLICY:
        failures.append(hosted_registry_diagnostic("hosted registry offline cache policy drifted"))
    if material.get("provenance_policy") != HOSTED_REGISTRY_PROVENANCE_POLICY:
        failures.append(hosted_registry_diagnostic("hosted registry provenance policy drifted"))
    trust_root_id = str(material.get("trust_root_id", ""))
    if not trust_root_id:
        failures.append(hosted_registry_diagnostic("hosted registry trust root missing"))
    if root is not None:
        for label, relative_path in (
            ("source lock", str(material.get("source_lock", ""))),
            ("offline mirror", str(material.get("offline_mirror_path", ""))),
        ):
            normalized = relative_path.replace("\\", "/")
            normalized_path = Path(normalized)
            if (
                not normalized
                or normalized_path.is_absolute()
                or normalized.startswith(("tmp/", "temp/"))
                or ".." in normalized_path.parts
            ):
                failures.append(hosted_registry_diagnostic(f"unsafe hosted registry {label} path"))
                continue
            if not (root / normalized).is_file():
                failures.append(hosted_registry_diagnostic(f"missing hosted registry {label} file"))
    return failures


def collect_failure_mode_failures(index: dict[str, Any]) -> list[str]:
    failure_modes = index.get("failure_modes", [])
    if not isinstance(failure_modes, list):
        return [hosted_registry_diagnostic("hosted registry failure modes field is not a list")]
    actual_modes = {str(mode) for mode in failure_modes if isinstance(mode, str)}
    missing_modes = sorted(HOSTED_REGISTRY_FAILURE_MODES - actual_modes)
    if missing_modes:
        return [hosted_registry_diagnostic("hosted registry failure mode contract missing: " + ", ".join(missing_modes))]
    return []


def collect_package_version_record_failures(index: dict[str, Any]) -> list[str]:
    packages = _as_object_list(index.get("packages", []))
    package_keys = _version_keys_from_records(packages)
    records = index.get("package_versions", [])
    if not isinstance(records, list):
        return [hosted_registry_diagnostic("hosted registry package_versions field is not a list")]

    failures: list[str] = []
    version_records = _as_object_list(records)
    seen_keys: set[tuple[str, str]] = set()
    for record in version_records:
        package_id = str(record.get("package_id", ""))
        package_version = str(record.get("package_version", ""))
        key = (package_id, package_version)
        if key in seen_keys:
            failures.append(hosted_registry_diagnostic(f"duplicate package version entry for {_package_key(*key)}"))
        seen_keys.add(key)
        if key not in package_keys:
            failures.append(hosted_registry_diagnostic(f"hosted registry version record has no package record for {_package_key(*key)}"))
        if not _is_exact_semver(package_version):
            failures.append(hosted_registry_diagnostic(f"invalid semver for {_package_key(package_id, package_version)}"))
        semver = record.get("semver", {})
        parts = _semver_parts(package_version)
        if not isinstance(semver, dict) or parts is None or (
            semver.get("major"),
            semver.get("minor"),
            semver.get("patch"),
        ) != parts:
            failures.append(hosted_registry_diagnostic(f"semver record drift for {_package_key(package_id, package_version)}"))
        if record.get("version_state") != "available":
            failures.append(hosted_registry_diagnostic(f"hosted registry unavailable version {_package_key(package_id, package_version)}"))
        if record.get("yank_state") != "not-yanked":
            failures.append(hosted_registry_diagnostic(f"yanked hosted registry version {_package_key(package_id, package_version)}"))
        if record.get("selection_policy") != HOSTED_REGISTRY_SELECTION_POLICY:
            failures.append(hosted_registry_diagnostic(f"unlocked version selection for {_package_key(package_id, package_version)}"))
        platforms = record.get("supported_platforms", [])
        if not isinstance(platforms, list) or not all(isinstance(platform, str) and platform for platform in platforms):
            failures.append(hosted_registry_diagnostic(f"unsupported platform metadata for {_package_key(package_id, package_version)}"))
        elif LOCAL_PACKAGE_HOST_PLATFORM not in platforms:
            failures.append(hosted_registry_diagnostic(f"unsupported platform {LOCAL_PACKAGE_HOST_PLATFORM} for {_package_key(package_id, package_version)}"))

    for key in sorted(package_keys - seen_keys):
        failures.append(hosted_registry_diagnostic(f"missing package version record for {_package_key(*key)}"))
    return failures


def collect_dependency_record_failures(index: dict[str, Any]) -> list[str]:
    records = index.get("dependency_records", [])
    if not isinstance(records, list):
        return [hosted_registry_diagnostic("hosted registry dependency_records field is not a list")]
    version_keys = {
        (str(record.get("package_id", "")), str(record.get("package_version", "")))
        for record in _as_object_list(index.get("package_versions", []))
    }
    package_ids = {package_id for package_id, _ in version_keys}
    failures: list[str] = []
    graph: dict[str, list[str]] = {}
    for record in _as_object_list(records):
        package_id = str(record.get("package_id", ""))
        depends_on = str(record.get("depends_on_package_id", ""))
        required_version = str(record.get("version_requirement", ""))
        resolved_version = str(record.get("resolved_version", ""))
        graph.setdefault(package_id, []).append(depends_on)
        if package_id not in package_ids:
            failures.append(hosted_registry_diagnostic(f"dependency source is not in hosted registry: {package_id}"))
        if (depends_on, resolved_version) not in version_keys:
            failures.append(hosted_registry_diagnostic(f"dependency target is not pinned in hosted registry: {_package_key(depends_on, resolved_version)}"))
        if not _is_exact_semver(required_version):
            failures.append(hosted_registry_diagnostic(f"invalid semver for dependency {package_id}->{depends_on}"))
        if required_version != resolved_version:
            failures.append(hosted_registry_diagnostic(f"unpinned hosted dependency for {depends_on}"))
        if record.get("resolution_policy") != HOSTED_REGISTRY_SELECTION_POLICY:
            failures.append(hosted_registry_diagnostic(f"unlocked version selection for {depends_on}"))
        if record.get("source") != "hosted-registry-offline-mirror":
            failures.append(hosted_registry_diagnostic(f"hosted dependency source drift for {package_id}->{depends_on}"))

    visiting: set[str] = set()
    visited: set[str] = set()

    def visit(package_id: str) -> None:
        if package_id in visited:
            return
        if package_id in visiting:
            failures.append(hosted_registry_diagnostic(f"dependency cycle detected at {package_id}"))
            return
        visiting.add(package_id)
        for target_id in graph.get(package_id, []):
            visit(target_id)
        visiting.remove(package_id)
        visited.add(package_id)

    for package_id in sorted(graph):
        visit(package_id)
    return failures


def collect_revocation_and_yank_failures(index: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    revocation_state = index.get("revocation_state", {})
    if not isinstance(revocation_state, dict):
        failures.append(hosted_registry_diagnostic("missing hosted registry revocation state"))
        revocation_state = {}
    if revocation_state.get("checked") is not True:
        failures.append(hosted_registry_diagnostic("hosted registry revocation state was not checked"))
    if revocation_state.get("registry_state") != index.get("registry_state"):
        failures.append(hosted_registry_diagnostic("hosted registry revocation state drifted"))
    revocations = index.get("revocations", {})
    if not isinstance(revocations, dict):
        revocations = {}
    for field_name in (
        "revoked_registry_ids",
        "revoked_package_ids",
        "revoked_signature_ids",
        "revoked_trust_root_ids",
    ):
        expected_values = revocations.get(field_name, [])
        actual_values = revocation_state.get(field_name, [])
        if expected_values != actual_values:
            failures.append(hosted_registry_diagnostic(f"hosted registry revocation {field_name} drifted"))

    yank_state = index.get("yank_state", {})
    if not isinstance(yank_state, dict):
        failures.append(hosted_registry_diagnostic("missing hosted registry yank state"))
        return failures
    if yank_state.get("policy") != "yanked-versions-fail-closed":
        failures.append(hosted_registry_diagnostic("hosted registry yank policy drifted"))
    yanked_versions = yank_state.get("yanked_versions", [])
    if not isinstance(yanked_versions, list):
        failures.append(hosted_registry_diagnostic("hosted registry yanked_versions field is not a list"))
        return failures
    yanked_keys = {
        (str(item.get("package_id", "")), str(item.get("package_version", "")))
        for item in yanked_versions
        if isinstance(item, dict)
    }
    for package_id, package_version in sorted(yanked_keys):
        failures.append(hosted_registry_diagnostic(f"yanked hosted registry version {_package_key(package_id, package_version)}"))
    return failures


def collect_trust_result_failures(
    index: dict[str, Any],
    *,
    trust_policy: dict[str, Any] | None = None,
) -> list[str]:
    records = index.get("trust_results", [])
    if not isinstance(records, list):
        return [hosted_registry_diagnostic("hosted registry trust_results field is not a list")]
    policy = trust_policy if isinstance(trust_policy, dict) else default_trust_policy_payload()
    known_roots = set(trust_roots_by_id(policy))
    revoked_roots = _revoked_values(index, "revoked_trust_root_ids") | _revoked_values({"revocations": policy.get("revocations", {})}, "revoked_trust_root_ids")
    package_records = {
        _registry_record_key(record): record
        for record in _as_object_list(index.get("packages", []))
    }
    failures: list[str] = []
    seen_keys: set[tuple[str, str]] = set()
    for result in _as_object_list(records):
        package_id = str(result.get("package_id", ""))
        package_version = str(result.get("package_version", ""))
        key = (package_id, package_version)
        seen_keys.add(key)
        record = package_records.get(key)
        if record is None:
            failures.append(hosted_registry_diagnostic(f"trust result has no package record for {_package_key(*key)}"))
            continue
        trust_root_id = str(result.get("trust_root_id", ""))
        if trust_root_id not in known_roots:
            failures.append(hosted_registry_diagnostic(f"unknown trust root {trust_root_id}"))
        if trust_root_id in revoked_roots:
            failures.append(hosted_registry_diagnostic(f"revoked trust root {trust_root_id}"))
        if result.get("status") != "verified":
            failures.append(hosted_registry_diagnostic(f"unsigned hosted artifact {_package_key(*key)}"))
        if result.get("revocation_checked") is not True:
            failures.append(hosted_registry_diagnostic(f"revocation check missing for {_package_key(*key)}"))
        if result.get("allows_local_install_fallback") is not False:
            failures.append(hosted_registry_diagnostic("unsigned hosted artifact local install fallback is forbidden"))
        package_trust = record.get("trust", {})
        registry_signature = record.get("registry_signature", {})
        package_signature_id = (
            str(package_trust.get("signature_id", ""))
            if isinstance(package_trust, dict)
            else ""
        )
        registry_signature_id = (
            str(registry_signature.get("signature_id", ""))
            if isinstance(registry_signature, dict)
            else ""
        )
        if result.get("package_signature_id") != package_signature_id:
            failures.append(hosted_registry_diagnostic(f"package trust result signature drift for {_package_key(*key)}"))
        if result.get("registry_signature_id") != registry_signature_id:
            failures.append(hosted_registry_diagnostic(f"registry trust result signature drift for {_package_key(*key)}"))
    for key in sorted(set(package_records) - seen_keys):
        failures.append(hosted_registry_diagnostic(f"missing trust result for {_package_key(*key)}"))
    return failures


def collect_cache_identity_failures(index: dict[str, Any]) -> list[str]:
    cache_identities = index.get("cache_identities", [])
    if not isinstance(cache_identities, list):
        return [hosted_registry_diagnostic("hosted registry cache_identities field is not a list")]
    package_records = {
        _registry_record_key(record): record
        for record in _as_object_list(index.get("packages", []))
    }
    failures: list[str] = []
    seen_keys: set[tuple[str, str]] = set()
    for identity in _as_object_list(cache_identities):
        package_id = str(identity.get("package_id", ""))
        package_version = str(identity.get("package_version", ""))
        key = (package_id, package_version)
        seen_keys.add(key)
        record = package_records.get(key)
        if record is None:
            failures.append(hosted_registry_diagnostic(f"cache identity has no package record for {_package_key(*key)}"))
            continue
        offline_mirror = record.get("offline_mirror", {})
        manifest = record.get("package_manifest", {})
        if not isinstance(offline_mirror, dict):
            offline_mirror = {}
        if not isinstance(manifest, dict):
            manifest = {}
        expected_cache_key = _package_key(package_id, package_version)
        expected = {
            "cache_key": expected_cache_key,
            "cache_path": offline_mirror.get("cache_path"),
            "cache_digest": offline_mirror.get("cache_digest"),
            "source_digest": record.get("source_digest"),
            "manifest_digest": manifest.get("digest"),
            "cache_origin": HOSTED_REGISTRY_CACHE_ORIGIN,
        }
        for field_name, expected_value in expected.items():
            if identity.get(field_name) != expected_value:
                failures.append(hosted_registry_diagnostic(f"cache identity drift for {_package_key(*key)}"))
                break
    for key in sorted(set(package_records) - seen_keys):
        failures.append(hosted_registry_diagnostic(f"missing cache identity for {_package_key(*key)}"))
    return failures


def collect_offline_mirror_handoff_failures(index: dict[str, Any], mirror: dict[str, Any]) -> list[str]:
    handoffs = index.get("offline_mirror_handoffs", [])
    if not isinstance(handoffs, list):
        return [hosted_registry_diagnostic("hosted registry offline_mirror_handoffs field is not a list")]
    package_records = {
        _registry_record_key(record): record
        for record in _as_object_list(index.get("packages", []))
    }
    lock_trust_material = index.get("lock_trust_material", {})
    mirror_path = (
        str(lock_trust_material.get("offline_mirror_path", ""))
        if isinstance(lock_trust_material, dict)
        else ""
    )
    failures: list[str] = []
    seen_keys: set[tuple[str, str]] = set()
    for handoff in _as_object_list(handoffs):
        package_id = str(handoff.get("package_id", ""))
        package_version = str(handoff.get("package_version", ""))
        key = (package_id, package_version)
        seen_keys.add(key)
        record = package_records.get(key)
        if record is None:
            failures.append(hosted_registry_diagnostic(f"offline mirror handoff has no package record for {_package_key(*key)}"))
            continue
        offline_mirror = record.get("offline_mirror", {})
        if not isinstance(offline_mirror, dict):
            offline_mirror = {}
        if handoff.get("mirror_path") != mirror_path or offline_mirror.get("index_path") != mirror_path:
            failures.append(hosted_registry_diagnostic(f"offline mirror handoff drift for {_package_key(*key)}"))
        if handoff.get("source_lock") != mirror.get("source_lock"):
            failures.append(hosted_registry_diagnostic(f"offline mirror handoff source lock drift for {_package_key(*key)}"))
        if handoff.get("cache_key") != _package_key(package_id, package_version):
            failures.append(hosted_registry_diagnostic(f"offline mirror handoff cache identity drift for {_package_key(*key)}"))
        if handoff.get("requires_lock_materialization") is not True:
            failures.append(hosted_registry_diagnostic(f"offline mirror handoff did not require lock materialization for {_package_key(*key)}"))
        if handoff.get("network_required_after_lock") is not False:
            failures.append(hosted_registry_diagnostic(f"offline mirror handoff requires network for {_package_key(*key)}"))
        if handoff.get("local_install_fallback_for_unverified_artifacts") is not False:
            failures.append(hosted_registry_diagnostic("unsigned hosted artifact local install fallback is forbidden"))
    for key in sorted(set(package_records) - seen_keys):
        failures.append(hosted_registry_diagnostic(f"missing offline mirror handoff for {_package_key(*key)}"))
    return failures


def collect_source_path_failures(record: dict[str, Any], *, root: Path | None) -> list[str]:
    if root is None:
        return []
    package_id = str(record.get("package_id", ""))
    failures: list[str] = []
    source = str(record.get("source", ""))
    package_manifest = record.get("package_manifest", {})
    offline_mirror = record.get("offline_mirror", {})
    manifest_path = str(package_manifest.get("path", "")) if isinstance(package_manifest, dict) else ""
    cache_path = str(offline_mirror.get("cache_path", "")) if isinstance(offline_mirror, dict) else ""
    for label, relative_path in (
        ("source", source),
        ("package manifest", manifest_path),
        ("offline cache", cache_path),
    ):
        normalized = relative_path.replace("\\", "/")
        normalized_path = Path(normalized)
        if (
            not normalized
            or normalized_path.is_absolute()
            or normalized.startswith(("tmp/", "temp/"))
            or ".." in normalized_path.parts
        ):
            failures.append(hosted_registry_diagnostic(f"unsafe hosted registry {label} path for {package_id}"))
            continue
        if not (root / normalized).is_file():
            failures.append(hosted_registry_diagnostic(f"missing hosted registry {label} file for {package_id}"))
    return failures


def collect_hosted_registry_model_failures(
    index: dict[str, Any],
    mirror: dict[str, Any],
    *,
    root: Path | None = None,
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
    if index.get("language_version") != LOCAL_PACKAGE_LANGUAGE_VERSION:
        failures.append(hosted_registry_diagnostic("hosted registry language version drifted"))
    if index.get("abi_identity") != LOCAL_PACKAGE_ABI_IDENTITY:
        failures.append(hosted_registry_diagnostic("hosted registry ABI identity drifted"))
    registry_id = str(index.get("registry_id", ""))
    failures.extend(collect_provider_model_failures(index))
    failures.extend(collect_registry_snapshot_failures(index))
    failures.extend(collect_service_availability_failures(index))
    failures.extend(collect_service_boundary_failures(index))
    failures.extend(collect_live_service_boundary_failures(index))
    failures.extend(
        collect_hosted_registry_service_reference_failures(index, root=root)
    )
    failures.extend(collect_endpoint_identity_failures(index))
    failures.extend(collect_lock_materialization_failures(index, mirror))
    failures.extend(collect_lock_trust_material_failures(index, mirror, root=root))
    failures.extend(collect_failure_mode_failures(index))
    failures.extend(collect_package_version_record_failures(index))
    failures.extend(collect_dependency_record_failures(index))
    failures.extend(collect_revocation_and_yank_failures(index))
    failures.extend(collect_trust_result_failures(index, trust_policy=policy))
    failures.extend(collect_cache_identity_failures(index))
    failures.extend(collect_offline_mirror_handoff_failures(index, mirror))
    if index.get("registry_state") != "not-revoked":
        failures.append(hosted_registry_diagnostic(f"revoked registry {registry_id}"))
    if registry_id in _revoked_values(index, "revoked_registry_ids"):
        failures.append(hosted_registry_diagnostic(f"revoked registry {registry_id}"))

    failures.extend(collect_offline_mirror_contract_failures(mirror))
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
        failures.extend(collect_source_path_failures(record, root=root))
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
        lock_trust_material = index.get("lock_trust_material", {})
        expected_trust_root = str(lock_trust_material.get("trust_root_id", "")) if isinstance(lock_trust_material, dict) else ""
        if expected_trust_root:
            for envelope_name, envelope in (
                ("package manifest", record.get("trust")),
                ("registry record", registry_signature),
            ):
                if not isinstance(envelope, dict) or envelope.get("trust_root_id") != expected_trust_root:
                    failures.append(hosted_registry_diagnostic(
                        f"hosted registry trust root mismatch for {package_id} {envelope_name}"
                    ))
        package_manifest = record.get("package_manifest", {})
        record_trust = record.get("trust")
        record_provenance = str(record_trust.get("provenance", "")) if isinstance(record_trust, dict) else ""
        registry_provenance = str(registry_signature.get("provenance", "")) if isinstance(registry_signature, dict) else ""
        if (
            not isinstance(package_manifest, dict)
            or not package_manifest.get("path")
            or not package_manifest.get("digest")
            or record_provenance != "deterministic-local-fixture-replay-only"
            or registry_provenance != "deterministic-local-fixture-replay-only"
        ):
            failures.append(hosted_registry_diagnostic(f"missing package provenance for {package_id}"))
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
    failures: list[str] = []
    if request.allow_network or request.registry_url:
        target = request.registry_url or "live registry endpoint"
        failures.append(hosted_registry_diagnostic(f"network fetch request rejected for {target}"))
        failures.append(
            hosted_registry_diagnostic(
                "live public hosted registry service unavailable"
            )
        )
        failures.append(
            hosted_registry_diagnostic(
                "live public hosted registry transport disabled"
            )
        )
        failures.append(
            hosted_registry_diagnostic(
                "unsupported live hosted registry service mode"
            )
        )
    if request.endpoint_id != HOSTED_REGISTRY_ENDPOINT_ID:
        failures.append(hosted_registry_diagnostic(f"hosted registry endpoint mismatch for {request.package_id}"))
    if request.channel_id != HOSTED_REGISTRY_CHANNEL_ID:
        failures.append(hosted_registry_diagnostic(f"hosted registry channel mismatch for {request.package_id}"))
    if request.package_version is None:
        failures.append(hosted_registry_diagnostic(f"unpinned hosted dependency for {request.package_id}"))
    elif not _is_exact_semver(request.package_version):
        failures.append(hosted_registry_diagnostic(f"invalid semver for {_package_key(request.package_id, request.package_version)}"))
    if request.host_platform != LOCAL_PACKAGE_HOST_PLATFORM:
        failures.append(hosted_registry_diagnostic(f"unsupported platform {request.host_platform} for {request.package_id}"))
    if request.minimum_snapshot_sequence < 1:
        failures.append(hosted_registry_diagnostic(f"rollback snapshot request for {request.package_id}"))
    return failures


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
            root=None,
            trust_policy=trust_policy,
        )
    )
    failures.extend(
        collect_hosted_registry_service_request_failures(
            index.get("hosted_service"),
            HostedRegistryServiceRequest(
                package_id=request.package_id,
                package_version=request.package_version,
                service_id=request.service_id,
                endpoint_id=request.endpoint_id,
                channel_id=request.channel_id,
                auth_subject_id=request.auth_subject_id,
                auth_token_id=request.auth_token_id,
                allow_network=request.allow_network or request.registry_url is not None,
            ),
        )
    )
    if request.language_version != LOCAL_PACKAGE_LANGUAGE_VERSION:
        failures.append(hosted_registry_diagnostic(f"language mismatch for {request.package_id}"))
    if request.abi_identity != LOCAL_PACKAGE_ABI_IDENTITY:
        failures.append(hosted_registry_diagnostic(f"ABI mismatch for {request.package_id}"))
    snapshot = index.get("snapshot", {})
    if isinstance(snapshot, dict):
        sequence = snapshot.get("sequence")
        if isinstance(sequence, int) and sequence < request.minimum_snapshot_sequence:
            failures.append(hosted_registry_diagnostic(f"rollback snapshot for {request.package_id}"))
    availability = index.get("service_availability", {})
    if isinstance(availability, dict) and availability.get("state") != HOSTED_REGISTRY_FIXTURE_AVAILABILITY_STATE:
        failures.append(hosted_registry_diagnostic("hosted registry unavailable"))

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
    else:
        record = candidates[0]
        version_records = [
            version_record
            for version_record in _as_object_list(index.get("package_versions", []))
            if str(version_record.get("package_id", "")) == request.package_id
            and str(version_record.get("package_version", "")) == str(record.get("package_version", ""))
        ]
        if len(version_records) != 1:
            failures.append(hosted_registry_diagnostic(f"ambiguous version selection for {request.package_id}"))
        else:
            version_record = version_records[0]
            platforms = version_record.get("supported_platforms", [])
            if not isinstance(platforms, list) or request.host_platform not in platforms:
                failures.append(hosted_registry_diagnostic(f"unsupported platform {request.host_platform} for {request.package_id}"))
            if version_record.get("yank_state") != "not-yanked":
                failures.append(hosted_registry_diagnostic(f"yanked hosted registry version {_package_key(request.package_id, str(record.get('package_version', '')))}"))

    if failures:
        raise HostedRegistryResolutionError(failures)

    record = candidates[0]
    package_manifest = record.get("package_manifest", {})
    offline_mirror = record.get("offline_mirror", {})
    registry_signature = record.get("registry_signature", {})
    snapshot = index.get("snapshot", {})
    cache_identity = next(
        (
            identity
            for identity in _as_object_list(index.get("cache_identities", []))
            if str(identity.get("package_id", "")) == str(record["package_id"])
            and str(identity.get("package_version", "")) == str(record["package_version"])
        ),
        {},
    )
    trust_result = next(
        (
            result
            for result in _as_object_list(index.get("trust_results", []))
            if str(result.get("package_id", "")) == str(record["package_id"])
            and str(result.get("package_version", "")) == str(record["package_version"])
        ),
        {},
    )
    return HostedRegistryResolution(
        package_id=str(record["package_id"]),
        package_version=str(record["package_version"]),
        source_digest=str(record["source_digest"]),
        manifest_digest=str(package_manifest["digest"]),
        cache_path=str(offline_mirror["cache_path"]),
        cache_digest=str(offline_mirror["cache_digest"]),
        snapshot_id=str(snapshot.get("snapshot_id", "")) if isinstance(snapshot, dict) else "",
        cache_key=str(cache_identity.get("cache_key", "")),
        offline_mirror_path=str(offline_mirror["index_path"]),
        registry_record_digest=str(record["metadata_digest"]),
        registry_signature_id=str(registry_signature["signature_id"]),
        trust_result_id=str(trust_result.get("trust_result_id", "")),
    )


__all__ = [
    "HOSTED_REGISTRY_CONTRACT_ID",
    "HOSTED_REGISTRY_BASE_URL",
    "HOSTED_REGISTRY_CHANNEL_ID",
    "HOSTED_REGISTRY_ENDPOINT_ID",
    "HOSTED_REGISTRY_FAILURE_MODES",
    "HOSTED_REGISTRY_NETWORK_POLICY",
    "HOSTED_REGISTRY_PROVIDER_ID",
    "HOSTED_REGISTRY_RESOLVER_ID",
    "HOSTED_REGISTRY_SCHEMA_KEY",
    "HOSTED_REGISTRY_SCHEMA_PATH",
    "HOSTED_REGISTRY_LIVE_SERVICE_BOUNDARY",
    "HOSTED_REGISTRY_SERVICE_BOUNDARY",
    "HOSTED_REGISTRY_TRUST_VALIDATOR_ID",
    "HOSTED_REGISTRY_SERVICE_ID",
    "HostedRegistryCacheIdentity",
    "HostedRegistryDependencyRecord",
    "HostedRegistryOfflineMirrorHandoff",
    "HostedRegistryPackageVersion",
    "HostedRegistryProviderModel",
    "HostedRegistryResolution",
    "HostedRegistryResolutionError",
    "HostedRegistryResolutionRequest",
    "HostedRegistryServiceRequest",
    "HostedRegistrySnapshot",
    "HostedRegistryTrustResult",
    "collect_cache_identity_failures",
    "collect_dependency_record_failures",
    "collect_hosted_registry_model_failures",
    "collect_hosted_registry_service_reference_failures",
    "collect_hosted_registry_service_request_failures",
    "collect_live_service_boundary_failures",
    "collect_lock_materialization_failures",
    "collect_offline_mirror_contract_failures",
    "collect_offline_mirror_handoff_failures",
    "collect_package_version_record_failures",
    "collect_provider_model_failures",
    "collect_registry_snapshot_failures",
    "collect_revocation_and_yank_failures",
    "collect_service_availability_failures",
    "collect_service_boundary_failures",
    "collect_trust_result_failures",
    "hosted_registry_diagnostic",
    "hosted_registry_record_digest",
    "hosted_registry_record_signature_subject",
    "network_fetch_request_failures",
    "resolve_hosted_registry_package",
    "sign_hosted_registry_record",
    "trust_diagnostic",
    "validate_hosted_registry_schema",
]
