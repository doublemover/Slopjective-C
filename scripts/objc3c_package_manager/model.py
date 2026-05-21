"""Deterministic local package manager model for Objective-C 3.0.

The package manager intentionally starts with checked-in local package roots.
Network resolution and hosted registry behavior remain fail-closed until they
have their own replayable evidence.
"""

from __future__ import annotations

from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
from typing import Any, Iterable

PACKAGE_MANIFEST_CONTRACT_ID = "objc3c.package_ecosystem.package_manifest.v1"
LOCAL_PACKAGE_LANGUAGE_VERSION = "3.0"
LOCAL_PACKAGE_LANGUAGE_MODE = "strict"
LOCAL_PACKAGE_ABI_IDENTITY = "objc3-abi-2025Q4"
LOCAL_PACKAGE_TRUST_KEY_ID = "objc3c-local-package-key-v1"
LOCAL_PACKAGE_SIGNATURE_FORMAT = "objc3c-local-sha256-v1"
LOCAL_PACKAGE_HOST_PLATFORM = "windows-x64"
PACKAGE_MANAGER_TAMPER_CODE = "O3PKG8055"


@dataclass(frozen=True)
class PackageManagerPaths:
    lock_path: Path
    manifest_root: Path


def public_workflow_command(action: str) -> str:
    return f"npm run objc3c -- {action}"


def stable_digest(payload: Any) -> str:
    encoded = json.dumps(
        payload,
        sort_keys=True,
        separators=(",", ":"),
        ensure_ascii=True,
        allow_nan=False,
    ).encode("utf-8")
    return "sha256:" + hashlib.sha256(encoded).hexdigest()


def file_digest(path: Path) -> str:
    return "sha256:" + hashlib.sha256(path.read_bytes()).hexdigest()


def package_namespace(package_id: str) -> str:
    namespace, _, _ = package_id.partition(":")
    if not namespace:
        raise RuntimeError(f"package id is not namespace-qualified: {package_id}")
    return namespace


def package_name(package_id: str) -> str:
    _, _, name = package_id.partition(":")
    if not name:
        raise RuntimeError(f"package id is not namespace-qualified: {package_id}")
    return name


def provenance_id(package_id: str) -> str:
    return "prov-" + package_id.replace(":", "-").replace(".", "-")


def package_manifest_rel_path(package_id: str) -> str:
    namespace = package_namespace(package_id)
    name = package_name(package_id).replace(".", "_")
    return f"tmp/artifacts/package-ecosystem/manifests/{namespace}/{name}.json"


def package_manifest_paths(packages: Iterable[dict[str, Any]]) -> list[str]:
    return sorted(
        str(package.get("package_manifest", {}).get("path"))
        for package in packages
        if isinstance(package, dict) and isinstance(package.get("package_manifest"), dict)
    )


def package_version_from_module(module: dict[str, Any]) -> str:
    semver = module.get("module_semver", {})
    if not isinstance(semver, dict):
        return "1.0.0"
    major = int(semver.get("major", 1))
    minor = int(semver.get("minor", 0))
    patch = int(semver.get("patch", 0))
    return f"{major}.{minor}.{patch}"


def trust_payload(package_id: str, signing_material: dict[str, Any]) -> dict[str, str]:
    return {
        "signature_format": LOCAL_PACKAGE_SIGNATURE_FORMAT,
        "signing_key_id": LOCAL_PACKAGE_TRUST_KEY_ID,
        "subject": package_id,
        "signature": stable_digest(signing_material),
        "trust_scope": "checked-in-local-package-source",
        "revocation_state": "not-revoked",
        "revocation_policy": "revoked-package-ids-fail-resolution",
    }


def dependency_payload(package_id: str) -> dict[str, str]:
    return {
        "package_id": package_id,
        "source": "checked-in-local-workspace",
        "language_requirement": LOCAL_PACKAGE_LANGUAGE_VERSION,
        "abi_requirement": LOCAL_PACKAGE_ABI_IDENTITY,
    }


def package_manifest_payload(
    *,
    package_id: str,
    source: str,
    source_kind: str,
    package_version: str,
    source_digest: str,
    dependencies: list[dict[str, str]],
    runtime_symbols: list[str],
    replay_actions: list[str],
) -> dict[str, Any]:
    signing_material = {
        "package_id": package_id,
        "source": source,
        "source_digest": source_digest,
        "package_version": package_version,
        "language_version": LOCAL_PACKAGE_LANGUAGE_VERSION,
        "abi_identity": LOCAL_PACKAGE_ABI_IDENTITY,
        "dependencies": dependencies,
    }
    payload: dict[str, Any] = {
        "contract_id": PACKAGE_MANIFEST_CONTRACT_ID,
        "package_id": package_id,
        "package_namespace": package_namespace(package_id),
        "package_name": package_name(package_id),
        "package_version": package_version,
        "source_kind": source_kind,
        "source": source,
        "source_digest": source_digest,
        "language": {
            "version": LOCAL_PACKAGE_LANGUAGE_VERSION,
            "mode": LOCAL_PACKAGE_LANGUAGE_MODE,
        },
        "abi": {
            "identity": LOCAL_PACKAGE_ABI_IDENTITY,
            "minimum": LOCAL_PACKAGE_ABI_IDENTITY,
            "runtime_symbols": sorted(runtime_symbols),
        },
        "dependencies": dependencies,
        "registry": {
            "resolution": "checked-in-local-registry",
            "network_resolution": "unsupported-fail-closed",
            "offline_mirror_required": True,
        },
        "trust": trust_payload(package_id, signing_material),
        "replay": {
            "commands": [public_workflow_command(action) for action in replay_actions],
        },
    }
    payload["manifest_digest"] = stable_digest(payload)
    return payload


def package_lock_entry(
    *,
    manifest: dict[str, Any],
    manifest_path: str,
) -> dict[str, Any]:
    return {
        "package_id": manifest["package_id"],
        "source": manifest["source"],
        "source_kind": manifest["source_kind"],
        "package_version": manifest["package_version"],
        "language_version": manifest["language"]["version"],
        "abi_identity": manifest["abi"]["identity"],
        "source_digest": manifest["source_digest"],
        "package_manifest": {
            "path": manifest_path,
            "contract_id": manifest["contract_id"],
            "digest": manifest["manifest_digest"],
        },
        "provenance_id": provenance_id(str(manifest["package_id"])),
        "trust": manifest["trust"],
    }


def provenance_entry(
    *,
    manifest: dict[str, Any],
    generator: str,
    manifest_path: str,
) -> dict[str, str]:
    return {
        "provenance_id": provenance_id(str(manifest["package_id"])),
        "source_path": str(manifest["source"]),
        "generator": generator,
        "replay_command": public_workflow_command("build-package-lock"),
        "package_manifest": manifest_path,
        "source_digest": str(manifest["source_digest"]),
        "manifest_digest": str(manifest["manifest_digest"]),
        "trust_signature": str(manifest["trust"]["signature"]),
    }


def build_lock_components(
    *,
    root: Path,
    module_inventory: dict[str, Any],
    showcase_portfolio: dict[str, Any],
) -> dict[str, Any]:
    modules = module_inventory.get("canonical_modules", [])
    examples = showcase_portfolio.get("examples", [])
    if not isinstance(modules, list) or not isinstance(examples, list):
        raise RuntimeError("package source inventories drifted from list shapes")

    package_manifests: list[dict[str, Any]] = []
    dependencies: list[dict[str, str]] = []
    digest_inputs: list[str] = []
    generator = "scripts/build_objc3c_package_lock.py"

    for module in sorted((entry for entry in modules if isinstance(entry, dict)), key=lambda entry: str(entry.get("module", ""))):
        module_id = str(module["module"])
        package_id = f"stdlib:{module_id}"
        source = str(module["manifest"])
        runtime_symbols = [
            str(symbol)
            for symbol in module.get("runtime_abi", [])
            if isinstance(symbol, str)
        ]
        manifest = package_manifest_payload(
            package_id=package_id,
            source=source,
            source_kind="stdlib-module-manifest",
            package_version=package_version_from_module(module),
            source_digest=file_digest(root / source),
            dependencies=[],
            runtime_symbols=runtime_symbols,
            replay_actions=[
                "build-package-lock",
                "validate-package-manager-model",
                "validate-package-authoring",
            ],
        )
        package_manifests.append(manifest)
        digest_inputs.append(source)

    for example in sorted((entry for entry in examples if isinstance(entry, dict)), key=lambda entry: str(entry.get("id", ""))):
        example_id = str(example["id"])
        package_id = f"showcase:{example_id}"
        source = str(example["workspace_manifest"])
        package_dependencies = [
            dependency_payload(f"stdlib:{name}")
            for name in sorted(str(name) for name in example.get("stdlib_followup_modules", []) if isinstance(name, str))
        ]
        for dependency in package_dependencies:
            dependencies.append(
                {
                    "from": package_id,
                    "to": dependency["package_id"],
                    "source": dependency["source"],
                    "language_requirement": dependency["language_requirement"],
                    "abi_requirement": dependency["abi_requirement"],
                    "resolution": "locked-local-registry",
                }
            )
        manifest = package_manifest_payload(
            package_id=package_id,
            source=source,
            source_kind="showcase-workspace-manifest",
            package_version="1.0.0",
            source_digest=file_digest(root / source),
            dependencies=package_dependencies,
            runtime_symbols=[],
            replay_actions=[
                "build-package-lock",
                "validate-package-manager-model",
                "validate-package-authoring",
                "validate-package-ecosystem",
            ],
        )
        package_manifests.append(manifest)
        digest_inputs.append(source)

    packages = [
        package_lock_entry(
            manifest=manifest,
            manifest_path=package_manifest_rel_path(str(manifest["package_id"])),
        )
        for manifest in package_manifests
    ]
    provenance = [
        provenance_entry(
            manifest=manifest,
            generator=generator,
            manifest_path=package_manifest_rel_path(str(manifest["package_id"])),
        )
        for manifest in package_manifests
    ]
    manifest_paths = [
        package_manifest_rel_path(str(manifest["package_id"]))
        for manifest in package_manifests
    ]
    digest_inputs.extend(manifest_paths)
    return {
        "package_manifests": sorted(package_manifests, key=lambda entry: str(entry["package_id"])),
        "packages": sorted(packages, key=lambda entry: str(entry["package_id"])),
        "dependencies": sorted(dependencies, key=lambda entry: (str(entry["from"]), str(entry["to"]))),
        "provenance": sorted(provenance, key=lambda entry: str(entry["provenance_id"])),
        "digest_inputs": sorted(set(digest_inputs)),
    }


def cache_payload_from_mirror_package(mirror_package: dict[str, Any]) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "contract_id": "objc3c.package_ecosystem.offline_mirror.cache_entry.v1",
        "package_id": str(mirror_package["package_id"]),
        "source": str(mirror_package["source"]),
        "source_digest": str(mirror_package["source_digest"]),
        "package_manifest": mirror_package["package_manifest"],
        "package_version": str(mirror_package["package_version"]),
        "language_version": str(mirror_package["language_version"]),
        "abi_identity": str(mirror_package["abi_identity"]),
        "trust": mirror_package["trust"],
        "network_policy": "no-network-during-validation",
        "restore_failure_mode": "reject-package-metadata-digest-mismatch",
    }
    interop_metadata = mirror_package.get("interop_loader_metadata")
    if isinstance(interop_metadata, dict):
        payload["interop_loader_metadata"] = interop_metadata
        payload["interop_loader_metadata_digest"] = str(interop_metadata.get("digest", ""))
    return payload


def collect_lock_model_failures(lock: dict[str, Any], *, root: Path) -> list[str]:
    failures: list[str] = []
    raw_packages = lock.get("packages", [])
    raw_dependencies = lock.get("dependencies", [])
    raw_provenance = lock.get("provenance", [])
    if not isinstance(raw_packages, list):
        return [f"{PACKAGE_MANAGER_TAMPER_CODE}: lock packages field is not a list"]
    if not isinstance(raw_dependencies, list):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: lock dependencies field is not a list")
        raw_dependencies = []
    if not isinstance(raw_provenance, list):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: lock provenance field is not a list")
        raw_provenance = []

    packages = [package for package in raw_packages if isinstance(package, dict)]
    package_ids = [str(package.get("package_id")) for package in packages]
    if package_ids != sorted(package_ids):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: lock packages are not sorted by package_id")
    duplicate_ids = sorted({package_id for package_id in package_ids if package_ids.count(package_id) > 1})
    for package_id in duplicate_ids:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: duplicate package id {package_id}")

    package_by_id = {str(package.get("package_id")): package for package in packages}
    provenance_by_id = {
        str(entry.get("provenance_id")): entry
        for entry in raw_provenance
        if isinstance(entry, dict)
    }
    for package in packages:
        package_id = str(package.get("package_id"))
        source = str(package.get("source", ""))
        source_path = root / source
        if not source_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing package source for {package_id}")
        elif package.get("source_digest") != file_digest(source_path):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: source digest mismatch for {package_id}")
        if package.get("language_version") != LOCAL_PACKAGE_LANGUAGE_VERSION:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: language version mismatch for {package_id}")
        if package.get("abi_identity") != LOCAL_PACKAGE_ABI_IDENTITY:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: ABI identity mismatch for {package_id}")
        trust = package.get("trust", {})
        if not isinstance(trust, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing trust envelope for {package_id}")
        else:
            if trust.get("signing_key_id") != LOCAL_PACKAGE_TRUST_KEY_ID:
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: signing key drift for {package_id}")
            if trust.get("revocation_state") != "not-revoked":
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked package cannot resolve {package_id}")
        manifest = package.get("package_manifest", {})
        if not isinstance(manifest, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing package manifest envelope for {package_id}")
        else:
            manifest_path = str(manifest.get("path", ""))
            if not manifest_path.startswith("tmp/artifacts/package-ecosystem/manifests/"):
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package manifest escaped generated root for {package_id}")
            provenance = provenance_by_id.get(str(package.get("provenance_id")))
            if provenance is None:
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing provenance for {package_id}")
            else:
                if provenance.get("package_manifest") != manifest_path:
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: provenance manifest drift for {package_id}")
                if provenance.get("manifest_digest") != manifest.get("digest"):
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: provenance manifest digest drift for {package_id}")
                if isinstance(trust, dict) and provenance.get("trust_signature") != trust.get("signature"):
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: provenance trust signature drift for {package_id}")

    graph: dict[str, list[str]] = {package_id: [] for package_id in package_ids}
    for dependency in raw_dependencies:
        if not isinstance(dependency, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: dependency entry is not an object")
            continue
        from_id = str(dependency.get("from"))
        to_id = str(dependency.get("to"))
        graph.setdefault(from_id, []).append(to_id)
        if from_id not in package_by_id:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: dependency source is not locked: {from_id}")
        if to_id not in package_by_id:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: dependency target is not locked: {to_id}")
        if dependency.get("source") != "checked-in-local-workspace":
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: unsupported dependency source for {from_id}->{to_id}")
        if dependency.get("resolution") != "locked-local-registry":
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: unsupported dependency resolution for {from_id}->{to_id}")
        target_package = package_by_id.get(to_id)
        if target_package is not None and dependency.get("abi_requirement") != target_package.get("abi_identity"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: ABI requirement mismatch for {from_id}->{to_id}")
        if target_package is not None and dependency.get("language_requirement") != target_package.get("language_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: language requirement mismatch for {from_id}->{to_id}")

    visiting: set[str] = set()
    visited: set[str] = set()

    def visit(package_id: str) -> None:
        if package_id in visited:
            return
        if package_id in visiting:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: dependency cycle detected at {package_id}")
            return
        visiting.add(package_id)
        for target_id in graph.get(package_id, []):
            visit(target_id)
        visiting.remove(package_id)
        visited.add(package_id)

    for package_id in sorted(graph):
        visit(package_id)
    return failures
