"""Deterministic package publish/install/update/uninstall/rollback operations."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json

from .hosted_registry import (
    HostedRegistryResolutionRequest,
    collect_hosted_registry_model_failures,
    hosted_registry_diagnostic,
    resolve_hosted_registry_package,
)
from .model import (
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    PACKAGE_MANAGER_TAMPER_CODE,
    cache_payload_from_mirror_package,
    collect_lock_model_failures,
    stable_digest,
)
from .registry import collect_registry_index_failures
from .trust import LOCAL_PACKAGE_TRUST_KEY_ID
from .trust import collect_extraction_plan_failures, package_extraction_plan_payload

PACKAGE_OPERATION_PLAN_CONTRACT_ID = "objc3c.package_ecosystem.operation_plan.v1"
PACKAGE_OPERATION_RECEIPT_CONTRACT_ID = "objc3c.package_ecosystem.operation_receipt.v1"
PACKAGE_OPERATION_SUMMARY_CONTRACT_ID = "objc3c.package_ecosystem.operation_summary.v1"
PACKAGE_OPERATION_NETWORK_POLICY = "no-network-during-validation"
PACKAGE_OPERATION_HOSTED_SUPPORT = "offline-metadata-only-live-network-fail-closed"
PACKAGE_OPERATION_CACHE_ROOT = "tmp/artifacts/package-ecosystem/mirrors/cache"
PACKAGE_OPERATION_OWNED_INSTALL_ROOT = (
    "tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c/packages"
)
PACKAGE_OPERATION_LOCAL_ARTIFACT_ROOT = (
    "tmp/artifacts/package-ecosystem/install-validation/local-package-artifacts"
)
PACKAGE_OPERATION_RECEIPT_ROOT = "tmp/artifacts/package-ecosystem/operations"

PACKAGE_OPERATION_PUBLIC_ACTIONS = (
    "package-publish",
    "package-install",
    "package-update",
    "package-uninstall",
    "package-rollback",
    "validate-package-operations",
)
PACKAGE_OPERATIONS = (
    "publish",
    "install",
    "update",
    "uninstall",
    "rollback",
)


@dataclass(frozen=True)
class PackageOperationRequest:
    operation: str
    package_id: str
    allow_network: bool = False
    live_registry_url: str | None = None


class PackageOperationError(RuntimeError):
    """Raised when an operation plan must fail closed."""

    def __init__(self, failures: list[str]) -> None:
        self.failures = failures
        super().__init__("\n".join(failures))


def package_operation_diagnostic(message: str) -> str:
    return f"{PACKAGE_MANAGER_TAMPER_CODE}: {message}"


def _package_by_id(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    packages = payload.get("packages", [])
    if not isinstance(packages, list):
        return {}
    return {
        str(package.get("package_id")): package
        for package in packages
        if isinstance(package, dict)
    }


def _registry_by_id(registry: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return _package_by_id(registry)


def _dependency_edges(lock: dict[str, Any], package_id: str) -> list[dict[str, str]]:
    dependencies = lock.get("dependencies", [])
    if not isinstance(dependencies, list):
        return []
    return [
        {
            "from": str(edge.get("from")),
            "to": str(edge.get("to")),
            "source_authority": str(edge.get("source_authority", "")),
            "source_authority_digest": str(edge.get("source_authority_digest", "")),
            "required_version": str(edge.get("required_version", "")),
            "resolved_version": str(edge.get("resolved_version", "")),
            "language_requirement": str(edge.get("language_requirement", "")),
            "abi_requirement": str(edge.get("abi_requirement", "")),
            "target_source_digest": str(edge.get("target_source_digest", "")),
            "target_manifest_digest": str(edge.get("target_manifest_digest", "")),
        }
        for edge in dependencies
        if isinstance(edge, dict) and str(edge.get("from")) == package_id
    ]


def _dependency_closure(lock: dict[str, Any], package_id: str) -> list[str]:
    resolution_plan = lock.get("resolution_plan", {})
    if not isinstance(resolution_plan, dict):
        return []
    closures = resolution_plan.get("dependency_closures", [])
    if not isinstance(closures, list):
        return []
    for closure in closures:
        if not isinstance(closure, dict) or closure.get("package_id") != package_id:
            continue
        dependencies_first = closure.get("dependencies_first", [])
        if isinstance(dependencies_first, list):
            return [str(value) for value in dependencies_first if isinstance(value, str)]
    return []


def _manifest_ref(package: dict[str, Any]) -> dict[str, Any]:
    manifest = package.get("package_manifest", {})
    return manifest if isinstance(manifest, dict) else {}


def _trust_ref(package: dict[str, Any]) -> dict[str, Any]:
    trust = package.get("trust", {})
    return trust if isinstance(trust, dict) else {}


def _package_identity(package: dict[str, Any]) -> dict[str, str]:
    manifest = _manifest_ref(package)
    trust = _trust_ref(package)
    return {
        "package_id": str(package.get("package_id", "")),
        "package_version": str(package.get("package_version", "")),
        "source": str(package.get("source", "")),
        "source_digest": str(package.get("source_digest", "")),
        "manifest_path": str(manifest.get("path", "")),
        "manifest_digest": str(manifest.get("digest", "")),
        "language_version": str(package.get("language_version", "")),
        "abi_identity": str(package.get("abi_identity", "")),
        "trust_key_id": str(trust.get("signing_key_id", "")),
        "trust_signature_id": str(trust.get("signature_id", "")),
        "trust_signature": str(trust.get("signature", "")),
    }


def _offline_pin(mirror_package: dict[str, Any]) -> dict[str, str]:
    manifest = _manifest_ref(mirror_package)
    return {
        "cache_path": str(mirror_package.get("cache_path", "")),
        "cache_digest": str(mirror_package.get("cache_digest", "")),
        "source_digest": str(mirror_package.get("source_digest", "")),
        "manifest_digest": str(manifest.get("digest", "")),
        "network_policy": PACKAGE_OPERATION_NETWORK_POLICY,
    }


def _registry_digest(registry_package: dict[str, Any]) -> dict[str, str]:
    manifest = _manifest_ref(registry_package)
    trust = _trust_ref(registry_package)
    return {
        "registry_source": "locked-local-registry",
        "source_digest": str(registry_package.get("source_digest", "")),
        "manifest_digest": str(manifest.get("digest", "")),
        "trust_signature": str(trust.get("signature", "")),
        "record_digest": stable_digest(registry_package),
    }


def _state_label(operation: str) -> tuple[str, str]:
    return {
        "publish": ("local-package-built-unpublished", "local-package-published-metadata"),
        "install": ("not-installed", "installed-from-verified-local-cache"),
        "update": ("installed", "installed-compatible-update-applied"),
        "uninstall": ("installed", "removed-owned-package-root"),
        "rollback": ("failed-update-staged", "previous-installed-state-restored"),
    }[operation]


def _rollback_token(
    *,
    operation: str,
    package_identity: dict[str, str],
    previous_state: str,
    next_state: str,
    offline_pin: dict[str, str],
) -> str:
    return stable_digest(
        {
            "operation": operation,
            "package_identity": package_identity,
            "previous_state": previous_state,
            "next_state": next_state,
            "offline_pin": offline_pin,
        }
    )


def _owned_package_root(package_id: str) -> str:
    namespace, separator, name = package_id.partition(":")
    if not separator or not namespace or not name:
        return ""
    return f"{PACKAGE_OPERATION_OWNED_INSTALL_ROOT}/{namespace}/{name.replace('.', '_')}"


def _local_artifact_path(package_id: str) -> str:
    namespace, separator, name = package_id.partition(":")
    if not separator or not namespace or not name:
        return ""
    return f"{PACKAGE_OPERATION_LOCAL_ARTIFACT_ROOT}/{namespace}/{name.replace('.', '_')}.json"


def _operation_extraction_plan(
    *,
    operation: str,
    package_order: list[str],
) -> dict[str, Any]:
    mutation = "record" if operation == "uninstall" else "write"
    entries: list[dict[str, str | int]] = []
    for index, package_id in enumerate(package_order):
        owned_root = _owned_package_root(package_id)
        local_artifact = _local_artifact_path(package_id)
        if owned_root:
            entries.append(
                {
                    "path": f"{owned_root}/package-manifest.json",
                    "entry_type": "file",
                    "mutation": mutation,
                    "package_id": package_id,
                    "order": index,
                }
            )
        if local_artifact:
            entries.append(
                {
                    "path": local_artifact,
                    "entry_type": "file",
                    "mutation": mutation,
                    "package_id": package_id,
                    "order": index,
                }
            )
    return package_extraction_plan_payload(
        plan_id=f"package-operation-{operation}-{stable_digest(package_order)}",
        entries=entries,
        provenance="package-operation-plan",
    )


def _installed_records_by_id(installed_state: dict[str, Any] | None) -> dict[str, dict[str, Any]]:
    if not isinstance(installed_state, dict):
        return {}
    records = installed_state.get("installed_packages", [])
    if not isinstance(records, list):
        return {}
    return {
        str(record.get("package_id")): record
        for record in records
        if isinstance(record, dict)
    }


def _collect_ownership_failures(
    *,
    package_id: str,
    operation: str,
    installed_state: dict[str, Any] | None,
) -> list[str]:
    if operation not in {"uninstall", "rollback", "update"}:
        return []
    records = _installed_records_by_id(installed_state)
    record = records.get(package_id)
    if record is None:
        return [package_operation_diagnostic(f"missing installed state for {operation} {package_id}")]
    owned_root = _owned_package_root(package_id)
    if not owned_root:
        return [package_operation_diagnostic(f"invalid package id for ownership proof {package_id}")]
    owned_paths = [
        str(record.get("installed_manifest", "")),
        str(record.get("local_install_artifact", "")),
    ]
    failures: list[str] = []
    for raw_path in owned_paths:
        if not raw_path:
            failures.append(package_operation_diagnostic(f"missing owned path for {operation} {package_id}"))
            continue
        if raw_path.startswith(PACKAGE_OPERATION_OWNED_INSTALL_ROOT + "/"):
            if not raw_path.startswith(owned_root + "/"):
                failures.append(package_operation_diagnostic(f"owned package path mismatch for {package_id}: {raw_path}"))
            continue
        if raw_path.startswith("tmp/artifacts/package-ecosystem/install-validation/local-package-artifacts/"):
            continue
        failures.append(package_operation_diagnostic(f"{operation} would remove outside package-owned roots: {raw_path}"))
    return failures


def _collect_mirror_failures(
    *,
    root: Path,
    mirror: dict[str, Any],
    lock: dict[str, Any],
    package_id: str,
) -> list[str]:
    failures: list[str] = []
    lock_package = _package_by_id(lock).get(package_id, {})
    mirror_package = _package_by_id(mirror).get(package_id)
    if mirror_package is None:
        return [package_operation_diagnostic(f"missing offline mirror pin for {package_id}")]
    if mirror.get("network_policy") != PACKAGE_OPERATION_NETWORK_POLICY:
        failures.append(package_operation_diagnostic("offline mirror network policy drifted"))
    manifest = _manifest_ref(lock_package)
    mirror_manifest = _manifest_ref(mirror_package)
    expected = {
        "source_digest": lock_package.get("source_digest"),
        "manifest_digest": manifest.get("digest"),
        "package_version": lock_package.get("package_version"),
        "language_version": LOCAL_PACKAGE_LANGUAGE_VERSION,
        "abi_identity": LOCAL_PACKAGE_ABI_IDENTITY,
        "trust_key_id": LOCAL_PACKAGE_TRUST_KEY_ID,
    }
    actual = {
        "source_digest": mirror_package.get("source_digest"),
        "manifest_digest": mirror_manifest.get("digest"),
        "package_version": mirror_package.get("package_version"),
        "language_version": mirror_package.get("language_version"),
        "abi_identity": mirror_package.get("abi_identity"),
        "trust_key_id": _trust_ref(mirror_package).get("signing_key_id"),
    }
    if actual != expected:
        failures.append(package_operation_diagnostic(f"offline mirror identity pin mismatch for {package_id}"))
    cache_path = str(mirror_package.get("cache_path", ""))
    if not cache_path.startswith(PACKAGE_OPERATION_CACHE_ROOT + "/"):
        failures.append(package_operation_diagnostic(f"offline mirror cache path escaped root for {package_id}"))
        return failures
    cache_file = root / cache_path
    if not cache_file.is_file():
        failures.append(package_operation_diagnostic(f"missing offline mirror cache entry for {package_id}"))
        return failures
    cache_payload = load_json(cache_file)
    expected_cache = cache_payload_from_mirror_package(mirror_package)
    if cache_payload != expected_cache:
        failures.append(package_operation_diagnostic(f"offline mirror cache payload mismatch for {package_id}"))
    if stable_digest(cache_payload) != mirror_package.get("cache_digest"):
        failures.append(package_operation_diagnostic(f"offline mirror cache digest mismatch for {package_id}"))
    return failures


def collect_package_operation_failures(
    *,
    root: Path,
    lock: dict[str, Any],
    mirror: dict[str, Any],
    registry: dict[str, Any],
    request: PackageOperationRequest,
    hosted_registry: dict[str, Any] | None = None,
    hosted_mirror: dict[str, Any] | None = None,
    installed_state: dict[str, Any] | None = None,
) -> list[str]:
    operation = request.operation
    package_id = request.package_id
    failures: list[str] = []
    if operation not in PACKAGE_OPERATIONS:
        return [package_operation_diagnostic(f"unsupported package operation {operation}")]
    if request.allow_network or request.live_registry_url:
        target = request.live_registry_url or "live registry endpoint"
        failures.append(package_operation_diagnostic(f"live network package {operation} rejected for {target}"))
    failures.extend(collect_lock_model_failures(lock, root=root))
    failures.extend(collect_registry_index_failures(registry, lock, root=root))

    lock_package = _package_by_id(lock).get(package_id)
    mirror_package = _package_by_id(mirror).get(package_id)
    registry_package = _registry_by_id(registry).get(package_id)
    if lock_package is None:
        failures.append(package_operation_diagnostic(f"package is not locked: {package_id}"))
        return failures
    if mirror_package is None:
        failures.append(package_operation_diagnostic(f"package missing from offline mirror: {package_id}"))
    if registry_package is None:
        failures.append(package_operation_diagnostic(f"package missing from local registry: {package_id}"))

    identity = _package_identity(lock_package)
    if identity["language_version"] != LOCAL_PACKAGE_LANGUAGE_VERSION:
        failures.append(package_operation_diagnostic(f"language mismatch for {package_id}"))
    if identity["abi_identity"] != LOCAL_PACKAGE_ABI_IDENTITY:
        failures.append(package_operation_diagnostic(f"ABI mismatch for {package_id}"))
    if identity["trust_key_id"] != LOCAL_PACKAGE_TRUST_KEY_ID:
        failures.append(package_operation_diagnostic(f"trust key mismatch for {package_id}"))

    if mirror_package is not None:
        failures.extend(
            _collect_mirror_failures(
                root=root,
                mirror=mirror,
                lock=lock,
                package_id=package_id,
            )
        )
    if registry_package is not None:
        if _package_identity(registry_package) != identity:
            failures.append(package_operation_diagnostic(f"local registry identity drift for {package_id}"))

    package_ids = set(_package_by_id(lock))
    for edge in _dependency_edges(lock, package_id):
        target_id = edge["to"]
        if target_id not in package_ids:
            failures.append(package_operation_diagnostic(f"dependency target is not locked: {package_id}->{target_id}"))
        if edge["language_requirement"] != LOCAL_PACKAGE_LANGUAGE_VERSION:
            failures.append(package_operation_diagnostic(f"dependency language mismatch for {package_id}->{target_id}"))
        if edge["abi_requirement"] != LOCAL_PACKAGE_ABI_IDENTITY:
            failures.append(package_operation_diagnostic(f"dependency ABI mismatch for {package_id}->{target_id}"))
    failures.extend(
        _collect_ownership_failures(
            package_id=package_id,
            operation=operation,
            installed_state=installed_state,
        )
    )
    package_order = [
        *_dependency_closure(lock, package_id),
        package_id,
    ]
    if operation in {"uninstall", "rollback"}:
        package_order = list(reversed(package_order))
    failures.extend(
        collect_extraction_plan_failures(
            _operation_extraction_plan(
                operation=operation,
                package_order=package_order,
            )
        )
    )

    if hosted_registry is not None or hosted_mirror is not None:
        if not isinstance(hosted_registry, dict) or not isinstance(hosted_mirror, dict):
            failures.append(package_operation_diagnostic("hosted registry inputs are incomplete"))
        else:
            failures.extend(
                collect_hosted_registry_model_failures(
                    hosted_registry,
                    hosted_mirror,
                    root=root,
                )
            )
            try:
                resolve_hosted_registry_package(
                    hosted_registry,
                    hosted_mirror,
                    HostedRegistryResolutionRequest(
                        package_id=package_id,
                        package_version=identity["package_version"],
                        allow_network=False,
                    ),
                )
            except RuntimeError as exc:
                if hasattr(exc, "failures"):
                    failures.extend(getattr(exc, "failures"))
                else:
                    failures.append(hosted_registry_diagnostic(str(exc)))
    return failures


def package_operation_plan(
    *,
    root: Path,
    lock: dict[str, Any],
    mirror: dict[str, Any],
    registry: dict[str, Any],
    request: PackageOperationRequest,
    hosted_registry: dict[str, Any] | None = None,
    hosted_mirror: dict[str, Any] | None = None,
    installed_state: dict[str, Any] | None = None,
) -> dict[str, Any]:
    failures = collect_package_operation_failures(
        root=root,
        lock=lock,
        mirror=mirror,
        registry=registry,
        request=request,
        hosted_registry=hosted_registry,
        hosted_mirror=hosted_mirror,
        installed_state=installed_state,
    )
    if failures:
        raise PackageOperationError(failures)

    lock_package = _package_by_id(lock)[request.package_id]
    mirror_package = _package_by_id(mirror)[request.package_id]
    registry_package = _registry_by_id(registry)[request.package_id]
    previous_state, next_state = _state_label(request.operation)
    identity = _package_identity(lock_package)
    offline_pin = _offline_pin(mirror_package)
    package_order = [
        *_dependency_closure(lock, request.package_id),
        request.package_id,
    ]
    if request.operation in {"uninstall", "rollback"}:
        package_order = list(reversed(package_order))
    extraction_plan = _operation_extraction_plan(
        operation=request.operation,
        package_order=package_order,
    )
    rollback_token = _rollback_token(
        operation=request.operation,
        package_identity=identity,
        previous_state=previous_state,
        next_state=next_state,
        offline_pin=offline_pin,
    )
    plan = {
        "contract_id": PACKAGE_OPERATION_PLAN_CONTRACT_ID,
        "operation": request.operation,
        "operation_action": f"package-{request.operation}",
        "package_id": request.package_id,
        "package_order": package_order,
        "package_identity": identity,
        "manifest_digest": identity["manifest_digest"],
        "registry_digest": _registry_digest(registry_package),
        "trust_digest": stable_digest(_trust_ref(lock_package)),
        "offline_mirror_pin": offline_pin,
        "dependency_edges": _dependency_edges(lock, request.package_id),
        "previous_state": previous_state,
        "next_state": next_state,
        "rollback_token": rollback_token,
        "rollback_state": {
            "token": rollback_token,
            "cache_path": offline_pin["cache_path"],
            "cache_digest": offline_pin["cache_digest"],
            "previous_manifest_digest": identity["manifest_digest"],
            "owned_package_root": _owned_package_root(request.package_id),
        },
        "extraction_plan": extraction_plan,
        "extraction_plan_digest": extraction_plan["plan_digest"],
        "network_policy": PACKAGE_OPERATION_NETWORK_POLICY,
        "hosted_registry_support": PACKAGE_OPERATION_HOSTED_SUPPORT,
        "live_network_publication": "fail-closed",
        "language_version": LOCAL_PACKAGE_LANGUAGE_VERSION,
        "abi_identity": LOCAL_PACKAGE_ABI_IDENTITY,
        "diagnostics": [],
    }
    plan["plan_digest"] = stable_digest(plan)
    return plan


def package_operation_receipt(plan: dict[str, Any]) -> dict[str, Any]:
    receipt = {
        "contract_id": PACKAGE_OPERATION_RECEIPT_CONTRACT_ID,
        "operation": str(plan["operation"]),
        "operation_action": str(plan["operation_action"]),
        "package_id": str(plan["package_id"]),
        "package_order": list(plan["package_order"]),
        "package_identity": dict(plan["package_identity"]),
        "manifest_digest": str(plan["manifest_digest"]),
        "registry_digest": dict(plan["registry_digest"]),
        "trust_digest": str(plan["trust_digest"]),
        "offline_mirror_pin": dict(plan["offline_mirror_pin"]),
        "previous_state": str(plan["previous_state"]),
        "next_state": str(plan["next_state"]),
        "rollback_token": str(plan["rollback_token"]),
        "rollback_state": dict(plan["rollback_state"]),
        "extraction_plan_digest": str(plan["extraction_plan_digest"]),
        "network_policy": str(plan["network_policy"]),
        "hosted_registry_support": str(plan["hosted_registry_support"]),
        "live_network_publication": str(plan["live_network_publication"]),
        "language_version": str(plan["language_version"]),
        "abi_identity": str(plan["abi_identity"]),
        "plan_digest": str(plan["plan_digest"]),
        "receipt_timestamp": "omitted-for-deterministic-replay",
        "machine_owned": True,
        "diagnostics": [],
    }
    receipt["receipt_digest"] = stable_digest(receipt)
    return receipt


def collect_package_operation_receipt_failures(
    *,
    plan: dict[str, Any],
    receipt: dict[str, Any],
) -> list[str]:
    expected = package_operation_receipt(plan)
    failures: list[str] = []
    if receipt != expected:
        failures.append(package_operation_diagnostic(f"{plan.get('operation')} receipt payload drifted"))
    if receipt.get("contract_id") != PACKAGE_OPERATION_RECEIPT_CONTRACT_ID:
        failures.append(package_operation_diagnostic("operation receipt contract id drifted"))
    if receipt.get("plan_digest") != plan.get("plan_digest"):
        failures.append(package_operation_diagnostic("operation receipt plan digest drifted"))
    if receipt.get("extraction_plan_digest") != plan.get("extraction_plan_digest"):
        failures.append(package_operation_diagnostic("operation receipt extraction plan digest drifted"))
    if receipt.get("machine_owned") is not True:
        failures.append(package_operation_diagnostic("operation receipt is not machine-owned"))
    if receipt.get("network_policy") != PACKAGE_OPERATION_NETWORK_POLICY:
        failures.append(package_operation_diagnostic("operation receipt network policy drifted"))
    if receipt.get("hosted_registry_support") != PACKAGE_OPERATION_HOSTED_SUPPORT:
        failures.append(package_operation_diagnostic("operation receipt hosted registry claim widened"))
    if stable_digest({key: value for key, value in receipt.items() if key != "receipt_digest"}) != receipt.get("receipt_digest"):
        failures.append(package_operation_diagnostic("operation receipt digest drifted"))
    return failures


def package_operation_artifact_paths(operation: str, package_id: str) -> tuple[str, str]:
    safe_id = package_id.replace(":", "__").replace(".", "_")
    root = f"{PACKAGE_OPERATION_RECEIPT_ROOT}/{operation}"
    return f"{root}/{safe_id}.plan.json", f"{root}/{safe_id}.receipt.json"


__all__ = [
    "PACKAGE_OPERATION_HOSTED_SUPPORT",
    "PACKAGE_OPERATION_LOCAL_ARTIFACT_ROOT",
    "PACKAGE_OPERATION_NETWORK_POLICY",
    "PACKAGE_OPERATION_PLAN_CONTRACT_ID",
    "PACKAGE_OPERATION_PUBLIC_ACTIONS",
    "PACKAGE_OPERATION_RECEIPT_CONTRACT_ID",
    "PACKAGE_OPERATION_RECEIPT_ROOT",
    "PACKAGE_OPERATION_SUMMARY_CONTRACT_ID",
    "PACKAGE_OPERATIONS",
    "PackageOperationError",
    "PackageOperationRequest",
    "collect_package_operation_failures",
    "collect_package_operation_receipt_failures",
    "package_operation_artifact_paths",
    "package_operation_diagnostic",
    "package_operation_plan",
    "package_operation_receipt",
]
