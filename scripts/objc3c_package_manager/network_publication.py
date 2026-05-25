"""Network dependency resolution and package release-channel contracts."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object, validate_json_schema

from .hosted_registry import (
    HOSTED_REGISTRY_BASE_URL,
    HOSTED_REGISTRY_CACHE_POLICY,
    HOSTED_REGISTRY_CHANNEL_ID,
    HOSTED_REGISTRY_ENDPOINT_ID,
    HOSTED_REGISTRY_PROVENANCE_POLICY,
)
from .model import PACKAGE_MANAGER_TAMPER_CODE
from .trust import LOCAL_PACKAGE_TRUST_ROOT_ID

NETWORK_DEPENDENCY_RESOLUTION_CONTRACT_ID = (
    "objc3c.package_ecosystem.network_dependency_resolution.v1"
)
PACKAGE_RELEASE_CHANNEL_PUBLICATION_CONTRACT_ID = (
    "objc3c.package_ecosystem.package_release_channel_publication.v1"
)
NETWORK_DEPENDENCY_RESOLUTION_SCHEMA_KEY = "objc3c-package-network-resolution-v1"
PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA_KEY = (
    "objc3c-package-release-channel-publication-v1"
)
NETWORK_RESOLVER_ID = "deterministic-network-dependency-offline-fixture-resolver-v1"
NETWORK_RESOLUTION_POLICY = "offline-fixture-backed-network-resolution"
NETWORK_RESOLUTION_MODE = "offline-hosted-registry-fixture"
NETWORK_FETCH_POLICY = "forbidden-fail-closed"
NETWORK_VERSION_SELECTION_POLICY = "exact-locked-version-only"
NETWORK_LOCK_POLICY = "fixture-lock-required"
PACKAGE_PUBLICATION_MODE = "source-owned-offline-fixture-channel"
PACKAGE_RELEASE_CHANNEL_ID = "stable"
PACKAGE_RELEASE_CHANNEL_STATE = "current"
PACKAGE_RELEASE_CHANNEL_FRESHNESS_POLICY = "fail-closed"
NETWORK_DEPENDENCY_RESOLUTION_SCHEMA_PATH = (
    Path(__file__).resolve().parents[2]
    / "schemas"
    / "objc3c-package-network-resolution-v1.schema.json"
)
PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA_PATH = (
    Path(__file__).resolve().parents[2]
    / "schemas"
    / "objc3c-package-release-channel-publication-v1.schema.json"
)
NETWORK_PUBLICATION_FAILURE_MODES = {
    "dependency-identity-drift",
    "fallback-registry-success",
    "hosted-registry-endpoint-mismatch",
    "live-network-fetch",
    "lock-mismatch",
    "stale-release-channel",
    "unsigned-or-tampered-cache",
    "unpinned-hosted-dependency",
}


def network_publication_diagnostic(message: str) -> str:
    return f"{PACKAGE_MANAGER_TAMPER_CODE}: {message}"


def _validate_schema(
    payload: dict[str, Any],
    *,
    schema_key: str,
    schema_path: Path,
) -> list[str]:
    try:
        validate_json_schema(payload, load_json_object(schema_path), label=schema_key)
    except RuntimeError as exc:
        return [network_publication_diagnostic(f"{schema_key} schema validation failed: {exc}")]
    return []


def validate_network_resolution_schema(payload: dict[str, Any]) -> list[str]:
    return _validate_schema(
        payload,
        schema_key=NETWORK_DEPENDENCY_RESOLUTION_SCHEMA_KEY,
        schema_path=NETWORK_DEPENDENCY_RESOLUTION_SCHEMA_PATH,
    )


def validate_release_channel_publication_schema(payload: dict[str, Any]) -> list[str]:
    return _validate_schema(
        payload,
        schema_key=PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA_KEY,
        schema_path=PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA_PATH,
    )


def _hosted_records_by_key(index: dict[str, Any]) -> dict[tuple[str, str], dict[str, Any]]:
    packages = index.get("packages", [])
    if not isinstance(packages, list):
        return {}
    return {
        (str(record.get("package_id", "")), str(record.get("package_version", ""))): record
        for record in packages
        if isinstance(record, dict)
    }


def _mirror_records_by_key(mirror: dict[str, Any]) -> dict[tuple[str, str], dict[str, Any]]:
    packages = mirror.get("packages", [])
    if not isinstance(packages, list):
        return {}
    return {
        (str(record.get("package_id", "")), str(record.get("package_version", ""))): record
        for record in packages
        if isinstance(record, dict)
    }


def _endpoint_tuple(endpoint: dict[str, Any]) -> tuple[str, str, str]:
    return (
        str(endpoint.get("endpoint_id", "")),
        str(endpoint.get("channel_id", "")),
        str(endpoint.get("base_url", "")),
    )


def _expected_endpoint_tuple() -> tuple[str, str, str]:
    return (
        HOSTED_REGISTRY_ENDPOINT_ID,
        HOSTED_REGISTRY_CHANNEL_ID,
        HOSTED_REGISTRY_BASE_URL,
    )


def _collect_endpoint_failures(endpoint: Any, *, package_id: str | None = None) -> list[str]:
    label = f" for {package_id}" if package_id else ""
    if not isinstance(endpoint, dict):
        return [network_publication_diagnostic(f"hosted registry endpoint mismatch{label}")]
    failures: list[str] = []
    if _endpoint_tuple(endpoint) != _expected_endpoint_tuple():
        failures.append(network_publication_diagnostic(f"hosted registry endpoint mismatch{label}"))
    if endpoint.get("fallback_registry_success") is not False:
        failures.append(network_publication_diagnostic(f"fallback registry success is forbidden{label}"))
    if endpoint.get("network_fetch") != NETWORK_FETCH_POLICY:
        failures.append(network_publication_diagnostic(f"live network fetch is forbidden{label}"))
    return failures


def _collect_lock_trust_failures(
    material: Any,
    *,
    hosted_index: dict[str, Any] | None,
) -> list[str]:
    if not isinstance(material, dict):
        return [network_publication_diagnostic("network resolution lock/trust material mismatch")]
    failures: list[str] = []
    if material.get("trust_root_id") != LOCAL_PACKAGE_TRUST_ROOT_ID:
        failures.append(network_publication_diagnostic("network resolution lock/trust material mismatch"))
    if material.get("cache_policy") != HOSTED_REGISTRY_CACHE_POLICY:
        failures.append(network_publication_diagnostic("network resolution cache policy drifted"))
    if material.get("provenance_policy") != HOSTED_REGISTRY_PROVENANCE_POLICY:
        failures.append(network_publication_diagnostic("network resolution provenance policy drifted"))
    if hosted_index is not None:
        hosted_material = hosted_index.get("lock_trust_material", {})
        if not isinstance(hosted_material, dict):
            failures.append(network_publication_diagnostic("network resolution lock/trust material mismatch"))
        else:
            for field_name in (
                "source_lock",
                "source_lock_digest",
                "trust_root_id",
                "offline_mirror_path",
                "cache_policy",
                "provenance_policy",
            ):
                if material.get(field_name) != hosted_material.get(field_name):
                    failures.append(network_publication_diagnostic("network resolution lock/trust material mismatch"))
                    break
    return failures


def _collect_network_policy_failures(policy: Any) -> list[str]:
    if not isinstance(policy, dict):
        return [network_publication_diagnostic("network dependency resolution policy missing")]
    failures: list[str] = []
    expected = {
        "dependency_resolution": NETWORK_RESOLUTION_POLICY,
        "live_network_fetch": NETWORK_FETCH_POLICY,
        "version_selection": NETWORK_VERSION_SELECTION_POLICY,
        "lock_policy": NETWORK_LOCK_POLICY,
        "cache_policy": HOSTED_REGISTRY_CACHE_POLICY,
        "provenance_policy": HOSTED_REGISTRY_PROVENANCE_POLICY,
    }
    for field_name, expected_value in expected.items():
        if policy.get(field_name) != expected_value:
            failures.append(network_publication_diagnostic(f"network dependency resolution {field_name} drifted"))
    if policy.get("fallback_registry_success") is not False:
        failures.append(network_publication_diagnostic("fallback registry success is forbidden"))
    for field_name in ("lockfile_required", "trust_required", "provenance_required", "cache_signature_required"):
        if policy.get(field_name) is not True:
            failures.append(network_publication_diagnostic(f"network dependency resolution disabled {field_name}"))
    return failures


def _collect_required_failure_mode_failures(payload: dict[str, Any]) -> list[str]:
    modes = payload.get("failure_modes", [])
    if not isinstance(modes, list):
        return [network_publication_diagnostic("network/publication failure modes field is not a list")]
    actual = {str(mode) for mode in modes if isinstance(mode, str)}
    missing = sorted(NETWORK_PUBLICATION_FAILURE_MODES - actual)
    if missing:
        return [network_publication_diagnostic("network/publication failure modes missing: " + ", ".join(missing))]
    return []


def _collect_dependency_record_failures(
    dependency: dict[str, Any],
    *,
    hosted_records: dict[tuple[str, str], dict[str, Any]],
    mirror_records: dict[tuple[str, str], dict[str, Any]],
) -> list[str]:
    package_id = str(dependency.get("dependency_id", ""))
    requested_version = str(dependency.get("requested_version", ""))
    resolved_version = str(dependency.get("resolved_version", ""))
    failures: list[str] = []
    if not requested_version or requested_version != resolved_version:
        failures.append(network_publication_diagnostic(f"unpinned hosted dependency for {package_id}"))
    if dependency.get("resolution") != NETWORK_RESOLUTION_MODE:
        failures.append(network_publication_diagnostic(f"network dependency resolution mode drifted for {package_id}"))
    if dependency.get("fallback_used") is not False:
        failures.append(network_publication_diagnostic(f"fallback registry success is forbidden for {package_id}"))
    failures.extend(_collect_endpoint_failures(dependency.get("endpoint"), package_id=package_id))

    record = hosted_records.get((package_id, resolved_version))
    if record is None:
        failures.append(network_publication_diagnostic(f"missing hosted registry record for {package_id}@{resolved_version}"))
        return failures
    mirror_record = mirror_records.get((package_id, resolved_version))
    if mirror_record is None:
        failures.append(network_publication_diagnostic(f"missing offline mirror record for {package_id}@{resolved_version}"))
        return failures

    manifest = record.get("package_manifest", {})
    if not isinstance(manifest, dict):
        manifest = {}
    expected_identity = {
        "package_id": package_id,
        "package_version": resolved_version,
        "source_digest": str(record.get("source_digest", "")),
        "manifest_digest": str(manifest.get("digest", "")),
        "registry_record_digest": str(record.get("metadata_digest", "")),
        "endpoint_id": HOSTED_REGISTRY_ENDPOINT_ID,
        "channel_id": HOSTED_REGISTRY_CHANNEL_ID,
    }
    identity = dependency.get("dependency_identity", {})
    if not isinstance(identity, dict) or any(identity.get(name) != value for name, value in expected_identity.items()):
        failures.append(network_publication_diagnostic(f"dependency identity drift for {package_id}"))

    lock = dependency.get("lock", {})
    if not isinstance(lock, dict) or lock.get("manifest_digest") != expected_identity["manifest_digest"]:
        failures.append(network_publication_diagnostic(f"lock mismatch for {package_id}"))
    if isinstance(lock, dict) and lock.get("trust_root_id") != LOCAL_PACKAGE_TRUST_ROOT_ID:
        failures.append(network_publication_diagnostic(f"lock mismatch for {package_id}"))

    cache = dependency.get("cache", {})
    offline_mirror = record.get("offline_mirror", {})
    if not isinstance(cache, dict) or not isinstance(offline_mirror, dict):
        failures.append(network_publication_diagnostic(f"unsigned or tampered cache for {package_id}"))
    else:
        expected_cache_path = str(offline_mirror.get("cache_path", ""))
        expected_cache_digest = str(offline_mirror.get("cache_digest", ""))
        mirror_cache_digest = str(mirror_record.get("cache_digest", ""))
        registry_signature = record.get("registry_signature", {})
        expected_signature_id = (
            str(registry_signature.get("signature_id", ""))
            if isinstance(registry_signature, dict)
            else ""
        )
        if (
            cache.get("cache_path") != expected_cache_path
            or cache.get("cache_digest") != expected_cache_digest
            or cache.get("cache_digest") != mirror_cache_digest
            or cache.get("cache_policy") != HOSTED_REGISTRY_CACHE_POLICY
            or cache.get("signature_required") is not True
            or cache.get("registry_signature_id") != expected_signature_id
        ):
            failures.append(network_publication_diagnostic(f"unsigned or tampered cache for {package_id}"))

    trust = dependency.get("trust", {})
    registry_signature = record.get("registry_signature", {})
    record_trust = record.get("trust", {})
    if not isinstance(trust, dict):
        failures.append(network_publication_diagnostic(f"network dependency trust drift for {package_id}"))
    else:
        expected_package_sig = str(record_trust.get("signature_id", "")) if isinstance(record_trust, dict) else ""
        expected_registry_sig = str(registry_signature.get("signature_id", "")) if isinstance(registry_signature, dict) else ""
        if (
            trust.get("trust_root_id") != LOCAL_PACKAGE_TRUST_ROOT_ID
            or trust.get("package_manifest_signature_id") != expected_package_sig
            or trust.get("registry_record_signature_id") != expected_registry_sig
            or trust.get("provenance") != "deterministic-local-fixture-replay-only"
        ):
            failures.append(network_publication_diagnostic(f"network dependency trust drift for {package_id}"))
    return failures


def collect_package_network_resolution_failures(
    payload: dict[str, Any],
    *,
    hosted_index: dict[str, Any] | None = None,
    mirror: dict[str, Any] | None = None,
) -> list[str]:
    failures = validate_network_resolution_schema(payload)
    if payload.get("contract_id") != NETWORK_DEPENDENCY_RESOLUTION_CONTRACT_ID:
        failures.append(network_publication_diagnostic("network dependency resolution contract id drifted"))
    if 8221 not in payload.get("issue_refs", []):
        failures.append(network_publication_diagnostic("network dependency resolution issue mapping missing #8221"))
    if 8204 not in payload.get("umbrella_issue_refs", []):
        failures.append(network_publication_diagnostic("network dependency resolution umbrella mapping missing #8204"))
    if payload.get("resolver_id") != NETWORK_RESOLVER_ID:
        failures.append(network_publication_diagnostic("network dependency resolver id drifted"))
    failures.extend(_collect_network_policy_failures(payload.get("network_policy")))
    failures.extend(_collect_endpoint_failures(payload.get("hosted_registry_endpoint")))
    failures.extend(
        _collect_lock_trust_failures(
            payload.get("lock_trust_material"),
            hosted_index=hosted_index,
        )
    )
    failures.extend(_collect_required_failure_mode_failures(payload))

    hosted_records = _hosted_records_by_key(hosted_index or {})
    mirror_records = _mirror_records_by_key(mirror or {})
    dependencies = payload.get("dependencies", [])
    if not isinstance(dependencies, list) or not dependencies:
        failures.append(network_publication_diagnostic("network dependency resolution has no dependency records"))
        return failures
    for dependency in dependencies:
        if not isinstance(dependency, dict):
            failures.append(network_publication_diagnostic("network dependency record is not an object"))
            continue
        failures.extend(
            _collect_dependency_record_failures(
                dependency,
                hosted_records=hosted_records,
                mirror_records=mirror_records,
            )
        )
    return failures


def _network_dependencies_by_key(payload: dict[str, Any]) -> dict[tuple[str, str], dict[str, Any]]:
    dependencies = payload.get("dependencies", [])
    if not isinstance(dependencies, list):
        return {}
    return {
        (str(dependency.get("dependency_id", "")), str(dependency.get("resolved_version", ""))): dependency
        for dependency in dependencies
        if isinstance(dependency, dict)
    }


def _collect_publication_policy_failures(policy: Any) -> list[str]:
    if not isinstance(policy, dict):
        return [network_publication_diagnostic("package release-channel publication policy missing")]
    failures: list[str] = []
    expected = {
        "publication_mode": PACKAGE_PUBLICATION_MODE,
        "release_channel_id": PACKAGE_RELEASE_CHANNEL_ID,
        "package_registry_channel_id": HOSTED_REGISTRY_CHANNEL_ID,
        "channel_state": PACKAGE_RELEASE_CHANNEL_STATE,
        "stale_behavior": PACKAGE_RELEASE_CHANNEL_FRESHNESS_POLICY,
        "live_network_publication": NETWORK_FETCH_POLICY,
        "cache_policy": HOSTED_REGISTRY_CACHE_POLICY,
        "provenance_policy": HOSTED_REGISTRY_PROVENANCE_POLICY,
    }
    for field_name, expected_value in expected.items():
        if policy.get(field_name) != expected_value:
            failures.append(network_publication_diagnostic(f"package release-channel publication {field_name} drifted"))
    if policy.get("fallback_publication_success") is not False:
        failures.append(network_publication_diagnostic("fallback publication success is forbidden"))
    for field_name in ("lock_required", "trust_required", "provenance_required", "cache_signature_required"):
        if policy.get(field_name) is not True:
            failures.append(network_publication_diagnostic(f"package release-channel publication disabled {field_name}"))
    return failures


def _collect_channel_freshness_failures(freshness: Any) -> list[str]:
    if not isinstance(freshness, dict):
        return [network_publication_diagnostic("stale release channel stable blocks package publication")]
    if (
        freshness.get("channel_id") != PACKAGE_RELEASE_CHANNEL_ID
        or freshness.get("status") != PACKAGE_RELEASE_CHANNEL_STATE
        or freshness.get("stale_behavior") != PACKAGE_RELEASE_CHANNEL_FRESHNESS_POLICY
        or freshness.get("blocks_publication_on_stale") is not True
    ):
        return [network_publication_diagnostic("stale release channel stable blocks package publication")]
    return []


def _collect_publication_package_failures(
    package: dict[str, Any],
    *,
    network_dependencies: dict[tuple[str, str], dict[str, Any]],
    hosted_records: dict[tuple[str, str], dict[str, Any]],
) -> list[str]:
    package_id = str(package.get("package_id", ""))
    package_version = str(package.get("package_version", ""))
    failures: list[str] = []
    if package.get("release_channel_id") != PACKAGE_RELEASE_CHANNEL_ID or package.get("channel_status") != PACKAGE_RELEASE_CHANNEL_STATE:
        failures.append(network_publication_diagnostic(f"stale release channel stable blocks package publication for {package_id}"))
    if package.get("registry_channel_id") != HOSTED_REGISTRY_CHANNEL_ID:
        failures.append(network_publication_diagnostic(f"hosted registry endpoint mismatch for {package_id}"))
    dependency = network_dependencies.get((package_id, package_version))
    if dependency is None:
        failures.append(network_publication_diagnostic(f"missing network resolution proof for {package_id}@{package_version}"))
        return failures
    record = hosted_records.get((package_id, package_version))
    if record is None:
        failures.append(network_publication_diagnostic(f"missing hosted registry record for {package_id}@{package_version}"))
        return failures
    manifest = record.get("package_manifest", {})
    offline_mirror = record.get("offline_mirror", {})
    registry_signature = record.get("registry_signature", {})
    record_trust = record.get("trust", {})
    expected = {
        "source_lock": str(dependency.get("lock", {}).get("source_lock", "")) if isinstance(dependency.get("lock"), dict) else "",
        "lock_digest": str(dependency.get("lock", {}).get("source_lock_digest", "")) if isinstance(dependency.get("lock"), dict) else "",
        "manifest_digest": str(manifest.get("digest", "")) if isinstance(manifest, dict) else "",
        "cache_path": str(offline_mirror.get("cache_path", "")) if isinstance(offline_mirror, dict) else "",
        "cache_digest": str(offline_mirror.get("cache_digest", "")) if isinstance(offline_mirror, dict) else "",
        "registry_record_digest": str(record.get("metadata_digest", "")),
        "package_manifest_signature_id": str(record_trust.get("signature_id", "")) if isinstance(record_trust, dict) else "",
        "registry_signature_id": str(registry_signature.get("signature_id", "")) if isinstance(registry_signature, dict) else "",
        "trust_root_id": LOCAL_PACKAGE_TRUST_ROOT_ID,
        "provenance": "deterministic-local-fixture-replay-only",
    }
    for field_name, expected_value in expected.items():
        if package.get(field_name) != expected_value:
            if field_name in {"cache_path", "cache_digest", "registry_signature_id"}:
                failures.append(network_publication_diagnostic(f"unsigned or tampered cache for {package_id}"))
            elif field_name in {"source_lock", "lock_digest"}:
                failures.append(network_publication_diagnostic(f"lock mismatch for {package_id}"))
            else:
                failures.append(network_publication_diagnostic(f"dependency identity drift for {package_id}"))
            break
    return failures


def collect_package_release_channel_publication_failures(
    payload: dict[str, Any],
    *,
    network_resolution: dict[str, Any] | None = None,
    hosted_index: dict[str, Any] | None = None,
) -> list[str]:
    failures = validate_release_channel_publication_schema(payload)
    if payload.get("contract_id") != PACKAGE_RELEASE_CHANNEL_PUBLICATION_CONTRACT_ID:
        failures.append(network_publication_diagnostic("package release-channel publication contract id drifted"))
    if 8222 not in payload.get("issue_refs", []):
        failures.append(network_publication_diagnostic("package release-channel publication issue mapping missing #8222"))
    if 8204 not in payload.get("umbrella_issue_refs", []):
        failures.append(network_publication_diagnostic("package release-channel publication umbrella mapping missing #8204"))
    failures.extend(_collect_publication_policy_failures(payload.get("publication_policy")))
    failures.extend(_collect_channel_freshness_failures(payload.get("channel_freshness")))
    failures.extend(_collect_endpoint_failures(payload.get("registry_endpoint")))
    failures.extend(_collect_required_failure_mode_failures(payload))

    network_dependencies = _network_dependencies_by_key(network_resolution or {})
    hosted_records = _hosted_records_by_key(hosted_index or {})
    packages = payload.get("packages", [])
    if not isinstance(packages, list) or not packages:
        failures.append(network_publication_diagnostic("package release-channel publication has no package records"))
        return failures
    for package in packages:
        if not isinstance(package, dict):
            failures.append(network_publication_diagnostic("package release-channel publication package record is not an object"))
            continue
        failures.extend(
            _collect_publication_package_failures(
                package,
                network_dependencies=network_dependencies,
                hosted_records=hosted_records,
            )
        )
    return failures


def collect_package_network_publication_failures(
    network_resolution: dict[str, Any],
    publication: dict[str, Any],
    *,
    hosted_index: dict[str, Any],
    mirror: dict[str, Any],
) -> list[str]:
    failures = collect_package_network_resolution_failures(
        network_resolution,
        hosted_index=hosted_index,
        mirror=mirror,
    )
    failures.extend(
        collect_package_release_channel_publication_failures(
            publication,
            network_resolution=network_resolution,
            hosted_index=hosted_index,
        )
    )
    return failures


__all__ = [
    "NETWORK_DEPENDENCY_RESOLUTION_CONTRACT_ID",
    "NETWORK_DEPENDENCY_RESOLUTION_SCHEMA_KEY",
    "NETWORK_DEPENDENCY_RESOLUTION_SCHEMA_PATH",
    "NETWORK_FETCH_POLICY",
    "NETWORK_LOCK_POLICY",
    "NETWORK_PUBLICATION_FAILURE_MODES",
    "NETWORK_RESOLUTION_MODE",
    "NETWORK_RESOLUTION_POLICY",
    "NETWORK_RESOLVER_ID",
    "NETWORK_VERSION_SELECTION_POLICY",
    "PACKAGE_PUBLICATION_MODE",
    "PACKAGE_RELEASE_CHANNEL_FRESHNESS_POLICY",
    "PACKAGE_RELEASE_CHANNEL_ID",
    "PACKAGE_RELEASE_CHANNEL_PUBLICATION_CONTRACT_ID",
    "PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA_KEY",
    "PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA_PATH",
    "PACKAGE_RELEASE_CHANNEL_STATE",
    "collect_package_network_publication_failures",
    "collect_package_network_resolution_failures",
    "collect_package_release_channel_publication_failures",
    "network_publication_diagnostic",
    "validate_network_resolution_schema",
    "validate_release_channel_publication_schema",
]
