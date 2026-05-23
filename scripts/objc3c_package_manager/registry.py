"""Schema-backed local registry index model for Objective-C 3.0 packages."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_package_manager.model import (
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    PACKAGE_MANAGER_TAMPER_CODE,
    public_workflow_command,
)

LOCAL_REGISTRY_CONTRACT_ID = "objc3c.package_ecosystem.local_registry_index.v1"
LOCAL_REGISTRY_SCHEMA_KEY = "objc3c-package-local-registry-index-v1"

REGISTRY_UNSUPPORTED_OPERATIONS = (
    "hosted-registry-publication",
    "hosted-registry-resolution",
    "network-dependency-fetch",
    "unlocked-version-selection",
    "registry-authentication-service",
    "cross-host-package-restore",
)


def registry_policy_payload() -> dict[str, object]:
    return {
        "source_authority": "generated-from-lock-and-offline-mirror",
        "dependency_resolution": "locked-local-registry-only",
        "version_selection": "exact-locked-version-only",
        "network_resolution": "unsupported-fail-closed",
        "hosted_registry": "unsupported-fail-closed-if-claimed",
        "publication_mode": "local-metadata-only",
        "unsupported_operations": list(REGISTRY_UNSUPPORTED_OPERATIONS),
    }


def registry_error_policy_payload() -> dict[str, object]:
    return {
        "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
        "fail_closed_conditions": [
            "missing locked package",
            "package id drift from lockfile",
            "source digest mismatch",
            "package manifest digest mismatch",
            "missing signature envelope",
            "bad signature",
            "unknown trust root",
            "language requirement mismatch",
            "ABI requirement mismatch",
            "dependency version drift",
            "revoked signing key",
            "revoked package trust envelope",
            "network dependency resolution claim",
            "hosted registry support claim",
            "registry evidence replay command drift",
        ],
    }


def _package_by_id(lock: dict[str, Any]) -> dict[str, dict[str, Any]]:
    packages = lock.get("packages", [])
    if not isinstance(packages, list):
        return {}
    return {
        str(package.get("package_id")): package
        for package in packages
        if isinstance(package, dict)
    }


def dependency_edge_payload(
    dependency: dict[str, Any],
    *,
    packages_by_id: dict[str, dict[str, Any]],
) -> dict[str, str]:
    from_id = str(dependency.get("from"))
    to_id = str(dependency.get("to"))
    target = packages_by_id.get(to_id, {})
    target_manifest = target.get("package_manifest", {})
    if not isinstance(target_manifest, dict):
        target_manifest = {}
    return {
        "from": from_id,
        "to": to_id,
        "source": str(dependency.get("source")),
        "language_requirement": str(dependency.get("language_requirement")),
        "abi_requirement": str(dependency.get("abi_requirement")),
        "resolution": str(dependency.get("resolution")),
        "required_version": str(dependency.get("required_version", target.get("package_version", ""))),
        "resolved_version": str(dependency.get("resolved_version", target.get("package_version", ""))),
        "target_manifest_digest": str(dependency.get("target_manifest_digest", target_manifest.get("digest", ""))),
        "target_source_digest": str(dependency.get("target_source_digest", target.get("source_digest", ""))),
    }


def package_registry_payload(
    package: dict[str, Any],
    *,
    outgoing_dependencies: list[dict[str, str]],
) -> dict[str, Any]:
    package_id = str(package["package_id"])
    manifest = package.get("package_manifest", {})
    if not isinstance(manifest, dict):
        manifest = {}
    trust = package.get("trust", {})
    if not isinstance(trust, dict):
        trust = {}
    payload: dict[str, Any] = {
        "package_id": package_id,
        "source": str(package["source"]),
        "source_digest": str(package["source_digest"]),
        "source_kind": str(package["source_kind"]),
        "package_version": str(package["package_version"]),
        "language_version": str(package["language_version"]),
        "abi_identity": str(package["abi_identity"]),
        "package_manifest": manifest,
        "provenance_id": str(package["provenance_id"]),
        "trust": trust,
        "version": {
            "selected": str(package["package_version"]),
            "selection_policy": "exact-locked-version-only",
            "candidate_versions": [str(package["package_version"])],
        },
        "resolution": {
            "source": "locked-local-registry",
            "network": "unsupported-fail-closed",
            "hosted_registry": "unsupported-fail-closed-if-claimed",
            "offline_mirror_required": True,
        },
        "evidence": {
            "source": str(package["source"]),
            "source_digest": str(package["source_digest"]),
            "manifest_path": str(manifest.get("path", "")),
            "manifest_digest": str(manifest.get("digest", "")),
            "provenance_id": str(package["provenance_id"]),
            "trust_signature": str(trust.get("signature", "")),
            "replay_commands": [
                public_workflow_command("build-package-lock"),
                public_workflow_command("validate-package-manager-model"),
                public_workflow_command("validate-package-mirror"),
            ],
        },
        "dependencies": outgoing_dependencies,
    }
    interop_metadata = package.get("interop_loader_metadata")
    if isinstance(interop_metadata, dict):
        payload["interop_loader_metadata"] = interop_metadata
    return payload


def local_registry_payload(
    lock: dict[str, Any],
    *,
    source_mirror: str,
    source_restore_receipt: str,
) -> dict[str, Any]:
    packages_by_id = _package_by_id(lock)
    raw_dependencies = lock.get("dependencies", [])
    dependencies = [
        dependency
        for dependency in raw_dependencies
        if isinstance(dependency, dict)
    ] if isinstance(raw_dependencies, list) else []
    dependency_edges = [
        dependency_edge_payload(dependency, packages_by_id=packages_by_id)
        for dependency in dependencies
    ]
    outgoing_by_package: dict[str, list[dict[str, str]]] = {
        package_id: [] for package_id in packages_by_id
    }
    for edge in dependency_edges:
        outgoing_by_package.setdefault(edge["from"], []).append(edge)
    package_manager = lock.get("package_manager", {})
    if not isinstance(package_manager, dict):
        package_manager = {}
    interop_loader_metadata = lock.get("interop_loader_metadata")
    resolution_plan = lock.get("resolution_plan")
    payload: dict[str, Any] = {
        "contract_id": LOCAL_REGISTRY_CONTRACT_ID,
        "registry_version": 1,
        "source_lock": "tmp/artifacts/package-ecosystem/locks/objc3c-package-lock.json",
        "source_mirror": source_mirror,
        "source_restore_receipt": source_restore_receipt,
        "support_state": "local-generated-index",
        "hosted_registry_state": "deferred",
        "network_resolution_support": "unsupported-fail-closed",
        "language_version": package_manager.get("language_version"),
        "abi_identity": package_manager.get("abi_identity"),
        "registry_policy": registry_policy_payload(),
        "packages": [
            package_registry_payload(
                package,
                outgoing_dependencies=sorted(
                    outgoing_by_package.get(str(package.get("package_id")), []),
                    key=lambda edge: edge["to"],
                ),
            )
            for package in sorted(
                packages_by_id.values(),
                key=lambda entry: str(entry.get("package_id")),
            )
        ],
        "dependency_edges": sorted(
            dependency_edges,
            key=lambda edge: (edge["from"], edge["to"]),
        ),
        "resolution_plan": resolution_plan if isinstance(resolution_plan, dict) else {},
        "error_policy": registry_error_policy_payload(),
        "replay": {
            "commands": [
                public_workflow_command("build-package-lock"),
                public_workflow_command("validate-package-mirror"),
            ],
        },
    }
    if isinstance(interop_loader_metadata, dict):
        payload["interop_loader_metadata"] = interop_loader_metadata
    return payload


def _expected_package_ids(lock: dict[str, Any]) -> list[str]:
    return sorted(_package_by_id(lock))


def _registry_package_ids(registry: dict[str, Any]) -> list[str]:
    packages = registry.get("packages", [])
    if not isinstance(packages, list):
        return []
    return sorted(
        str(package.get("package_id"))
        for package in packages
        if isinstance(package, dict)
    )


def collect_registry_index_failures(
    registry: dict[str, Any],
    lock: dict[str, Any],
    *,
    root: Path,
) -> list[str]:
    failures: list[str] = []
    if registry.get("contract_id") != LOCAL_REGISTRY_CONTRACT_ID:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry contract_id drifted")
    if registry.get("support_state") != "local-generated-index":
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry support state drifted")
    if registry.get("hosted_registry_state") != "deferred":
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry state must stay deferred")
    if registry.get("network_resolution_support") != "unsupported-fail-closed":
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: registry network resolution must fail closed")
    if registry.get("language_version") != LOCAL_PACKAGE_LANGUAGE_VERSION:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: registry language version drifted")
    if registry.get("abi_identity") != LOCAL_PACKAGE_ABI_IDENTITY:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: registry ABI identity drifted")

    policy = registry.get("registry_policy", {})
    if not isinstance(policy, dict):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry missing policy")
        policy = {}
    expected_policy = registry_policy_payload()
    for field, expected_value in expected_policy.items():
        if policy.get(field) != expected_value:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: registry policy drifted for {field}")

    error_policy = registry.get("error_policy", {})
    if not isinstance(error_policy, dict):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry missing error policy")
    elif error_policy.get("diagnostic_code") != PACKAGE_MANAGER_TAMPER_CODE:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry diagnostic code drifted")

    expected_ids = _expected_package_ids(lock)
    registry_ids = _registry_package_ids(registry)
    if registry_ids != expected_ids:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry package ids drifted from lock")

    lock_packages_by_id = _package_by_id(lock)
    raw_packages = registry.get("packages", [])
    if not isinstance(raw_packages, list):
        return [*failures, f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry packages field is not a list"]
    seen_ids: set[str] = set()
    for raw_package in raw_packages:
        if not isinstance(raw_package, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry package entry is not an object")
            continue
        package_id = str(raw_package.get("package_id"))
        if package_id in seen_ids:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: duplicate local registry package id {package_id}")
        seen_ids.add(package_id)
        locked = lock_packages_by_id.get(package_id)
        if locked is None:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry package is not locked: {package_id}")
            continue
        source_path = root / str(raw_package.get("source", ""))
        if not source_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing local registry source for {package_id}")
        if raw_package.get("source_digest") != locked.get("source_digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry source digest drift for {package_id}")
        if raw_package.get("package_version") != locked.get("package_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry package version drift for {package_id}")
        if raw_package.get("language_version") != locked.get("language_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry language drift for {package_id}")
        if raw_package.get("abi_identity") != locked.get("abi_identity"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry ABI drift for {package_id}")

        version = raw_package.get("version", {})
        if not isinstance(version, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry missing version policy for {package_id}")
        else:
            selected = version.get("selected")
            if selected != locked.get("package_version"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry selected version drift for {package_id}")
            if version.get("selection_policy") != "exact-locked-version-only":
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry version policy drift for {package_id}")
            if version.get("candidate_versions") != [locked.get("package_version")]:
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry candidate versions drift for {package_id}")

        resolution = raw_package.get("resolution", {})
        if not isinstance(resolution, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry missing resolution policy for {package_id}")
        else:
            if resolution.get("source") != "locked-local-registry":
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry source policy drift for {package_id}")
            if resolution.get("network") != "unsupported-fail-closed":
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry network claim drift for {package_id}")
            if resolution.get("hosted_registry") != "unsupported-fail-closed-if-claimed":
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry hosted claim drift for {package_id}")

        locked_manifest = locked.get("package_manifest", {})
        registry_manifest = raw_package.get("package_manifest", {})
        if not isinstance(locked_manifest, dict) or not isinstance(registry_manifest, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry manifest envelope drift for {package_id}")
        elif registry_manifest != locked_manifest:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry manifest drift for {package_id}")

        locked_trust = locked.get("trust", {})
        registry_trust = raw_package.get("trust", {})
        if not isinstance(registry_trust, dict) or registry_trust != locked_trust:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry trust drift for {package_id}")
        elif registry_trust.get("revocation_state") != "not-revoked":
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked local registry package cannot resolve {package_id}")

        evidence = raw_package.get("evidence", {})
        if not isinstance(evidence, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry missing evidence for {package_id}")
        else:
            if evidence.get("source") != locked.get("source"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry evidence source drift for {package_id}")
            if evidence.get("source_digest") != locked.get("source_digest"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry evidence source digest drift for {package_id}")
            if isinstance(locked_manifest, dict) and evidence.get("manifest_path") != locked_manifest.get("path"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry evidence manifest path drift for {package_id}")
            if isinstance(locked_manifest, dict) and evidence.get("manifest_digest") != locked_manifest.get("digest"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry evidence manifest digest drift for {package_id}")
            if evidence.get("provenance_id") != locked.get("provenance_id"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry evidence provenance drift for {package_id}")
            if isinstance(locked_trust, dict) and evidence.get("trust_signature") != locked_trust.get("signature"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry evidence trust drift for {package_id}")
            replay_commands = evidence.get("replay_commands")
            required_replay = [
                public_workflow_command("build-package-lock"),
                public_workflow_command("validate-package-manager-model"),
                public_workflow_command("validate-package-mirror"),
            ]
            if replay_commands != required_replay:
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry evidence replay drift for {package_id}")

    lock_dependencies = lock.get("dependencies", [])
    if not isinstance(lock_dependencies, list):
        lock_dependencies = []
    expected_edges = [
        dependency_edge_payload(dependency, packages_by_id=lock_packages_by_id)
        for dependency in lock_dependencies
        if isinstance(dependency, dict)
    ]
    expected_edges = sorted(expected_edges, key=lambda edge: (edge["from"], edge["to"]))
    raw_edges = registry.get("dependency_edges", [])
    if not isinstance(raw_edges, list):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency_edges field is not a list")
        raw_edges = []
    registry_edges = [
        edge for edge in raw_edges if isinstance(edge, dict)
    ]
    registry_edges_sorted = sorted(
        registry_edges,
        key=lambda edge: (str(edge.get("from")), str(edge.get("to"))),
    )
    if registry_edges_sorted != expected_edges:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency edges drifted from lock")
    for edge in registry_edges_sorted:
        from_id = str(edge.get("from"))
        to_id = str(edge.get("to"))
        target = lock_packages_by_id.get(to_id, {})
        if edge.get("resolution") != "locked-local-registry":
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency resolution drift for {from_id}->{to_id}")
        if edge.get("source") != "checked-in-local-workspace":
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency source drift for {from_id}->{to_id}")
        if edge.get("language_requirement") != target.get("language_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency language mismatch for {from_id}->{to_id}")
        if edge.get("abi_requirement") != target.get("abi_identity"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency ABI mismatch for {from_id}->{to_id}")
        if edge.get("required_version") != target.get("package_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency version mismatch for {from_id}->{to_id}")
        if edge.get("resolved_version") != target.get("package_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency resolved version mismatch for {from_id}->{to_id}")
        if edge.get("target_source_digest") != target.get("source_digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency target source digest mismatch for {from_id}->{to_id}")
        target_manifest = target.get("package_manifest", {})
        if isinstance(target_manifest, dict) and edge.get("target_manifest_digest") != target_manifest.get("digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry dependency target manifest digest mismatch for {from_id}->{to_id}")
    if registry.get("resolution_plan") != lock.get("resolution_plan"):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry resolution plan drifted from lock")
    return failures


__all__ = [
    "LOCAL_REGISTRY_CONTRACT_ID",
    "LOCAL_REGISTRY_SCHEMA_KEY",
    "REGISTRY_UNSUPPORTED_OPERATIONS",
    "collect_registry_index_failures",
    "dependency_edge_payload",
    "local_registry_payload",
    "package_registry_payload",
    "registry_error_policy_payload",
    "registry_policy_payload",
]
