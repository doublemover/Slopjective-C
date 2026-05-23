"""Hermetic hosted-registry service contract for Objective-C 3.0 packages."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object, validate_json_schema

from .model import PACKAGE_MANAGER_TAMPER_CODE
from .trust import LOCAL_PACKAGE_TRUST_ROOT_ID

HOSTED_REGISTRY_SERVICE_CONTRACT_ID = (
    "objc3c.package_ecosystem.hosted_registry_service.v1"
)
HOSTED_REGISTRY_SERVICE_SCHEMA_KEY = "objc3c-package-hosted-registry-service-v1"
HOSTED_REGISTRY_SERVICE_ID = "objc3c-hermetic-hosted-registry-service-v1"
HOSTED_REGISTRY_SERVICE_MODE = "hermetic-local-service"
HOSTED_REGISTRY_SERVICE_NETWORK_POLICY = "hermetic-no-network-service"
HOSTED_REGISTRY_SERVICE_ENDPOINT_ID = "objc3c-hosted-registry-fixture-endpoint-v1"
HOSTED_REGISTRY_SERVICE_CHANNEL_ID = "stable-fixture"
HOSTED_REGISTRY_SERVICE_BASE_URL = "https://registry.objc3c.invalid/fixture/v1"
HOSTED_REGISTRY_SERVICE_PROVIDER_ID = "schema-backed-hosted-registry-provider-v1"
HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID = "fixture-developer"
HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID = "fixture-developer-token-v1"
HOSTED_REGISTRY_SERVICE_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/service/"
    "hosted-registry-service.json"
)
HOSTED_REGISTRY_SERVICE_SCHEMA_PATH = (
    Path(__file__).resolve().parents[2]
    / "schemas"
    / "objc3c-package-hosted-registry-service-v1.schema.json"
)
HOSTED_REGISTRY_SERVICE_REQUIRED_FAILURE_MODES = {
    "fallback-registry-success",
    "live-network-fetch",
    "missing-service-auth",
    "service-auth-token-drift",
    "service-availability-unavailable",
    "service-contract-drift",
    "service-index-drift",
    "service-moderation-blocked",
    "service-revocation-unavailable",
    "unknown-service-auth-subject",
}


@dataclass(frozen=True)
class HostedRegistryServiceRequest:
    package_id: str
    package_version: str | None
    operation: str = "resolve-package"
    service_id: str = HOSTED_REGISTRY_SERVICE_ID
    endpoint_id: str = HOSTED_REGISTRY_SERVICE_ENDPOINT_ID
    channel_id: str = HOSTED_REGISTRY_SERVICE_CHANNEL_ID
    auth_subject_id: str = HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID
    auth_token_id: str = HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID
    allow_network: bool = False


@dataclass(frozen=True)
class HostedRegistryServiceDecision:
    service_id: str
    package_id: str
    package_version: str
    operation: str
    registry_index_path: str
    offline_mirror_path: str
    auth_subject_id: str


class HostedRegistryServiceError(RuntimeError):
    """Raised when the hosted-registry service contract fails closed."""

    def __init__(self, failures: list[str]) -> None:
        self.failures = failures
        super().__init__("\n".join(failures))


def hosted_registry_service_diagnostic(message: str) -> str:
    return f"{PACKAGE_MANAGER_TAMPER_CODE}: {message}"


def _as_object_list(value: Any) -> list[dict[str, Any]]:
    if not isinstance(value, list):
        return []
    return [item for item in value if isinstance(item, dict)]


def _string_values(value: Any) -> set[str]:
    if not isinstance(value, list):
        return set()
    return {str(item) for item in value if isinstance(item, str)}


def _path_is_safe_repo_relative(raw_path: str) -> bool:
    normalized = raw_path.replace("\\", "/")
    path = Path(normalized)
    return (
        bool(normalized)
        and not path.is_absolute()
        and not normalized.startswith(("tmp/", "temp/", "~/", "/"))
        and ".." not in path.parts
        and not (len(normalized) >= 2 and normalized[1] == ":")
    )


def validate_hosted_registry_service_schema(service: dict[str, Any]) -> list[str]:
    try:
        schema = load_json_object(HOSTED_REGISTRY_SERVICE_SCHEMA_PATH)
        validate_json_schema(
            service,
            schema,
            label=HOSTED_REGISTRY_SERVICE_SCHEMA_KEY,
        )
    except RuntimeError as exc:
        return [
            hosted_registry_service_diagnostic(
                f"hosted registry service schema validation failed: {exc}"
            )
        ]
    return []


def _collect_provider_failures(
    service: dict[str, Any],
    *,
    index: dict[str, Any] | None,
    root: Path | None,
) -> list[str]:
    provider = service.get("provider", {})
    if not isinstance(provider, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service provider")]

    failures: list[str] = []
    expected = {
        "provider_id": HOSTED_REGISTRY_SERVICE_PROVIDER_ID,
        "service_id": HOSTED_REGISTRY_SERVICE_ID,
        "endpoint_id": HOSTED_REGISTRY_SERVICE_ENDPOINT_ID,
        "channel_id": HOSTED_REGISTRY_SERVICE_CHANNEL_ID,
        "base_url": HOSTED_REGISTRY_SERVICE_BASE_URL,
        "transport": "disabled-network-transport",
        "request_source": "checked-in-local-service-fixture",
        "response_source": "checked-in-hosted-registry-index",
    }
    for field_name, expected_value in expected.items():
        if provider.get(field_name) != expected_value:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service provider {field_name} drifted"
                )
            )
    if provider.get("network_fetch_allowed") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service provider allows network fetch"
            )
        )
    registry_index_path = str(provider.get("registry_index_path", ""))
    if not _path_is_safe_repo_relative(registry_index_path):
        failures.append(
            hosted_registry_service_diagnostic(
                "unsafe hosted registry service index path"
            )
        )
    elif root is not None and not (root / registry_index_path).is_file():
        failures.append(
            hosted_registry_service_diagnostic(
                "missing hosted registry service index path"
            )
        )
    if index is not None:
        endpoint = index.get("endpoint_identity", {})
        if not isinstance(endpoint, dict):
            endpoint = {}
        for field_name in ("endpoint_id", "channel_id", "base_url"):
            if provider.get(field_name) != endpoint.get(field_name):
                failures.append(
                    hosted_registry_service_diagnostic(
                        f"hosted registry service endpoint {field_name} drifted"
                    )
                )
    return failures


def _collect_auth_failures(service: dict[str, Any]) -> list[str]:
    auth = service.get("auth_policy", {})
    if not isinstance(auth, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service auth policy")]

    failures: list[str] = []
    expected = {
        "policy_id": "hermetic-fixture-token-auth-v1",
        "token_transport": "fixture-header",
        "missing_token_policy": "fail-closed",
        "unknown_subject_policy": "fail-closed",
        "token_replay_policy": "fail-closed",
    }
    for field_name, expected_value in expected.items():
        if auth.get(field_name) != expected_value:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service auth {field_name} drifted"
                )
            )
    if auth.get("auth_required") is not True:
        failures.append(hosted_registry_service_diagnostic("hosted registry service auth disabled"))
    if auth.get("anonymous_access") is not False:
        failures.append(hosted_registry_service_diagnostic("hosted registry service anonymous access enabled"))

    subjects = _as_object_list(auth.get("accepted_subjects", []))
    if not subjects:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service has no accepted auth subject"
            )
        )
    for subject in subjects:
        subject_id = str(subject.get("subject_id", ""))
        if not subject_id:
            failures.append(hosted_registry_service_diagnostic("hosted registry service auth subject is empty"))
        if subject.get("token_id") != HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service auth token drift for {subject_id}"
                )
            )
        if subject.get("revocation_checked") is not True:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service auth revocation not checked for {subject_id}"
                )
            )
        namespaces = _string_values(subject.get("package_namespaces"))
        if "fixture" not in namespaces:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service auth namespace drift for {subject_id}"
                )
            )
    return failures


def _collect_trust_failures(service: dict[str, Any]) -> list[str]:
    trust = service.get("trust_root_operation", {})
    if not isinstance(trust, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service trust-root operation")]

    failures: list[str] = []
    expected = {
        "policy_id": "hermetic-hosted-registry-trust-root-operation-v1",
        "active_trust_root_id": LOCAL_PACKAGE_TRUST_ROOT_ID,
        "active_root_source": "checked-in-local-deterministic-trust-policy",
        "production_registry_root_state": "reserved-fail-closed",
        "unknown_root_policy": "fail-closed",
    }
    for field_name, expected_value in expected.items():
        if trust.get(field_name) != expected_value:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service trust {field_name} drifted"
                )
            )
    for field_name in (
        "requires_signature",
        "requires_revocation_check",
        "reserved_root_use_fails_closed",
    ):
        if trust.get(field_name) is not True:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service trust disabled {field_name}"
                )
            )
    if trust.get("fallback_trust_root_allowed") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service fallback trust root allowed"
            )
        )
    return failures


def _collect_revocation_failures(
    service: dict[str, Any],
    *,
    index: dict[str, Any] | None,
) -> list[str]:
    revocation = service.get("revocation_service", {})
    if not isinstance(revocation, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service revocation service")]

    failures: list[str] = []
    expected = {
        "service_id": "hermetic-registry-revocation-service-v1",
        "source": "checked-in-revocation-list",
        "unavailable_behavior": "fail-closed",
    }
    for field_name, expected_value in expected.items():
        if revocation.get(field_name) != expected_value:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service revocation {field_name} drifted"
                )
            )
    if revocation.get("checked_before_resolution") is not True:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service revocation is not checked before resolution"
            )
        )
    if revocation.get("available") is not True:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service revocation unavailable"
            )
        )
    if index is not None:
        index_revocations = index.get("revocations", {})
        if not isinstance(index_revocations, dict):
            index_revocations = {}
        service_lists = revocation.get("revocations", {})
        if not isinstance(service_lists, dict):
            service_lists = {}
        for field_name in (
            "revoked_registry_ids",
            "revoked_package_ids",
            "revoked_signature_ids",
            "revoked_trust_root_ids",
        ):
            if service_lists.get(field_name) != index_revocations.get(field_name):
                failures.append(
                    hosted_registry_service_diagnostic(
                        f"hosted registry service revocation {field_name} drifted from index"
                    )
                )
    return failures


def _collect_moderation_failures(service: dict[str, Any]) -> list[str]:
    moderation = service.get("moderation_policy", {})
    if not isinstance(moderation, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service moderation policy")]

    failures: list[str] = []
    expected = {
        "policy_id": "hermetic-registry-moderation-policy-v1",
        "source": "checked-in-moderation-policy",
        "unavailable_behavior": "fail-closed",
        "unknown_package_policy": "fail-closed",
    }
    for field_name, expected_value in expected.items():
        if moderation.get(field_name) != expected_value:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service moderation {field_name} drifted"
                )
            )
    for field_name in ("checked_before_publication", "checked_before_resolution"):
        if moderation.get(field_name) is not True:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service moderation disabled {field_name}"
                )
            )
    if moderation.get("available") is not True:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service moderation unavailable"
            )
        )
    if _string_values(moderation.get("blocked_packages", [])):
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service moderation blocked package"
            )
        )
    if _as_object_list(moderation.get("blocked_versions", [])):
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service moderation blocked package"
            )
        )
    return failures


def _collect_availability_failures(service: dict[str, Any]) -> list[str]:
    availability = service.get("availability", {})
    if not isinstance(availability, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service availability")]

    failures: list[str] = []
    expected = {
        "state": "hermetic-local-service-available",
        "health_source": "checked-in-service-fixture",
        "unavailable_behavior": "fail-closed",
    }
    for field_name, expected_value in expected.items():
        if availability.get(field_name) != expected_value:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service availability {field_name} drifted"
                )
            )
    if availability.get("live_service_available") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service claims live availability"
            )
        )
    if availability.get("network_fetch_allowed") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service availability allows network fetch"
            )
        )
    return failures


def _collect_request_policy_failures(service: dict[str, Any]) -> list[str]:
    request_policy = service.get("request_policy", {})
    if not isinstance(request_policy, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service request policy")]

    failures: list[str] = []
    allowed_operations = _string_values(request_policy.get("allowed_operations"))
    if "resolve-package" not in allowed_operations:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service resolve-package operation missing"
            )
        )
    for field_name in (
        "require_exact_version",
        "require_lock_materialization",
        "require_offline_mirror_handoff",
    ):
        if request_policy.get(field_name) is not True:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service request policy disabled {field_name}"
                )
            )
    if request_policy.get("live_network_fetch") != "fail-closed":
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service live network fetch policy drifted"
            )
        )
    if request_policy.get("fallback_registry_success") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service fallback registry success allowed"
            )
        )
    return failures


def _collect_output_failures(
    service: dict[str, Any],
    *,
    index: dict[str, Any] | None,
) -> list[str]:
    output = service.get("resolution_output", {})
    if not isinstance(output, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service resolution output")]

    failures: list[str] = []
    for field_name in (
        "lock_materialization_required",
        "offline_mirror_handoff_required",
        "offline_replay_sufficient",
    ):
        if output.get(field_name) is not True:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service output disabled {field_name}"
                )
            )
    if output.get("network_required_after_lock") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service output requires network after lock"
            )
        )
    provider = service.get("provider", {})
    provider_index_path = (
        str(provider.get("registry_index_path", ""))
        if isinstance(provider, dict)
        else ""
    )
    if output.get("registry_index_path") != provider_index_path:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service index path drifted"
            )
        )
    if index is not None:
        material = index.get("lock_trust_material", {})
        if not isinstance(material, dict):
            material = {}
        if output.get("offline_mirror_path") != material.get("offline_mirror_path"):
            failures.append(
                hosted_registry_service_diagnostic(
                    "hosted registry service offline mirror path drifted"
                )
            )
    return failures


def _collect_negative_case_failures(service: dict[str, Any]) -> list[str]:
    negative_cases = service.get("negative_cases", [])
    if not isinstance(negative_cases, list):
        return [hosted_registry_service_diagnostic("hosted registry service negative cases is not a list")]
    actual_modes = {
        str(case.get("failure_mode"))
        for case in negative_cases
        if isinstance(case, dict)
    }
    missing = sorted(HOSTED_REGISTRY_SERVICE_REQUIRED_FAILURE_MODES - actual_modes)
    if missing:
        return [
            hosted_registry_service_diagnostic(
                "hosted registry service failure mode contract missing: "
                + ", ".join(missing)
            )
        ]
    return []


def collect_hosted_registry_service_failures(
    service: dict[str, Any],
    *,
    index: dict[str, Any] | None = None,
    root: Path | None = None,
) -> list[str]:
    failures = validate_hosted_registry_service_schema(service)
    if service.get("contract_id") != HOSTED_REGISTRY_SERVICE_CONTRACT_ID:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service contract id drifted"
            )
        )
    if service.get("service_id") != HOSTED_REGISTRY_SERVICE_ID:
        failures.append(hosted_registry_service_diagnostic("hosted registry service id drifted"))
    if service.get("service_mode") != HOSTED_REGISTRY_SERVICE_MODE:
        failures.append(hosted_registry_service_diagnostic("hosted registry service mode drifted"))
    if service.get("network_policy") != HOSTED_REGISTRY_SERVICE_NETWORK_POLICY:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service network policy drifted"
            )
        )
    if service.get("fallback_registry_success") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service fallback registry success allowed"
            )
        )

    failures.extend(_collect_provider_failures(service, index=index, root=root))
    failures.extend(_collect_auth_failures(service))
    failures.extend(_collect_trust_failures(service))
    failures.extend(_collect_revocation_failures(service, index=index))
    failures.extend(_collect_moderation_failures(service))
    failures.extend(_collect_availability_failures(service))
    failures.extend(_collect_request_policy_failures(service))
    failures.extend(_collect_output_failures(service, index=index))
    failures.extend(_collect_negative_case_failures(service))
    return failures


def collect_hosted_registry_service_reference_failures(
    index: dict[str, Any],
    *,
    root: Path | None = None,
) -> list[str]:
    service_ref = index.get("hosted_service", {})
    if not isinstance(service_ref, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service contract")]

    failures: list[str] = []
    expected = {
        "service_contract_id": HOSTED_REGISTRY_SERVICE_CONTRACT_ID,
        "service_id": HOSTED_REGISTRY_SERVICE_ID,
        "schema": HOSTED_REGISTRY_SERVICE_SCHEMA_KEY,
        "service_fixture_path": HOSTED_REGISTRY_SERVICE_FIXTURE_PATH,
        "network_policy": HOSTED_REGISTRY_SERVICE_NETWORK_POLICY,
        "public_service_claim": "reserved-live-public-service",
        "validation_behavior": "service-contract-required-before-resolution",
        "no_network_fallback": "fail-closed",
        "default_auth_subject_id": HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID,
        "default_auth_token_id": HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID,
    }
    for field_name, expected_value in expected.items():
        if service_ref.get(field_name) != expected_value:
            failures.append(
                hosted_registry_service_diagnostic(
                    f"hosted registry service reference {field_name} drifted"
                )
            )
    if service_ref.get("fallback_registry_success") is not False:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service fallback registry success allowed"
            )
        )

    fixture_path = str(service_ref.get("service_fixture_path", ""))
    if not _path_is_safe_repo_relative(fixture_path):
        failures.append(
            hosted_registry_service_diagnostic(
                "unsafe hosted registry service fixture path"
            )
        )
        return failures
    if root is None:
        return failures

    absolute_fixture = root / fixture_path
    if not absolute_fixture.is_file():
        failures.append(
            hosted_registry_service_diagnostic(
                "missing hosted registry service fixture"
            )
        )
        return failures
    try:
        service = load_json_object(absolute_fixture)
    except RuntimeError as exc:
        failures.append(
            hosted_registry_service_diagnostic(
                f"hosted registry service fixture load failed: {exc}"
            )
        )
        return failures
    failures.extend(collect_hosted_registry_service_failures(service, index=index, root=root))
    return failures


def collect_hosted_registry_service_request_failures(
    service_ref: Any,
    request: HostedRegistryServiceRequest,
) -> list[str]:
    if not isinstance(service_ref, dict):
        return [hosted_registry_service_diagnostic("missing hosted registry service contract")]

    failures: list[str] = []
    if request.service_id != HOSTED_REGISTRY_SERVICE_ID:
        failures.append(hosted_registry_service_diagnostic("hosted registry service id mismatch"))
    if request.endpoint_id != HOSTED_REGISTRY_SERVICE_ENDPOINT_ID:
        failures.append(hosted_registry_service_diagnostic("hosted registry service endpoint mismatch"))
    if request.channel_id != HOSTED_REGISTRY_SERVICE_CHANNEL_ID:
        failures.append(hosted_registry_service_diagnostic("hosted registry service channel mismatch"))
    if request.allow_network:
        failures.append(
            hosted_registry_service_diagnostic(
                "hosted registry service network request rejected"
            )
        )
    if request.package_version is None:
        failures.append(
            hosted_registry_service_diagnostic(
                f"hosted registry service requires exact package version for {request.package_id}"
            )
        )
    if request.auth_subject_id != service_ref.get("default_auth_subject_id"):
        failures.append(
            hosted_registry_service_diagnostic(
                f"unknown hosted registry service auth subject {request.auth_subject_id}"
            )
        )
    if request.auth_token_id != service_ref.get("default_auth_token_id"):
        failures.append(
            hosted_registry_service_diagnostic(
                f"hosted registry service auth token drift for {request.auth_subject_id}"
            )
        )
    if request.operation != "resolve-package":
        failures.append(
            hosted_registry_service_diagnostic(
                f"unsupported hosted registry service operation {request.operation}"
            )
        )
    return failures


def resolve_hosted_registry_service_request(
    service: dict[str, Any],
    request: HostedRegistryServiceRequest,
    *,
    index: dict[str, Any],
    root: Path | None = None,
) -> HostedRegistryServiceDecision:
    failures = collect_hosted_registry_service_failures(service, index=index, root=root)
    service_ref = index.get("hosted_service", {})
    failures.extend(collect_hosted_registry_service_request_failures(service_ref, request))
    if failures:
        raise HostedRegistryServiceError(failures)

    output = service.get("resolution_output", {})
    provider = service.get("provider", {})
    assert isinstance(output, dict)
    assert isinstance(provider, dict)
    return HostedRegistryServiceDecision(
        service_id=HOSTED_REGISTRY_SERVICE_ID,
        package_id=request.package_id,
        package_version=str(request.package_version),
        operation=request.operation,
        registry_index_path=str(provider["registry_index_path"]),
        offline_mirror_path=str(output["offline_mirror_path"]),
        auth_subject_id=request.auth_subject_id,
    )


__all__ = [
    "HOSTED_REGISTRY_SERVICE_CONTRACT_ID",
    "HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID",
    "HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID",
    "HOSTED_REGISTRY_SERVICE_FIXTURE_PATH",
    "HOSTED_REGISTRY_SERVICE_ID",
    "HOSTED_REGISTRY_SERVICE_NETWORK_POLICY",
    "HOSTED_REGISTRY_SERVICE_REQUIRED_FAILURE_MODES",
    "HOSTED_REGISTRY_SERVICE_SCHEMA_KEY",
    "HOSTED_REGISTRY_SERVICE_SCHEMA_PATH",
    "HostedRegistryServiceDecision",
    "HostedRegistryServiceError",
    "HostedRegistryServiceRequest",
    "collect_hosted_registry_service_failures",
    "collect_hosted_registry_service_reference_failures",
    "collect_hosted_registry_service_request_failures",
    "hosted_registry_service_diagnostic",
    "resolve_hosted_registry_service_request",
    "validate_hosted_registry_service_schema",
]
