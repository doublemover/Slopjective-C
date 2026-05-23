"""Deterministic local package manager model for Objective-C 3.0.

The package manager intentionally starts with checked-in local package roots.
Network resolution and hosted registry behavior remain fail-closed until they
have their own replayable evidence.
"""

from __future__ import annotations

from dataclasses import dataclass
import json
from pathlib import Path
from typing import Any, Iterable

from .digests import file_digest, stable_digest
from .trust import (
    LOCAL_PACKAGE_SIGNATURE_FORMAT,
    LOCAL_PACKAGE_TRUST_KEY_ID,
    PACKAGE_MANAGER_TAMPER_CODE,
    collect_lock_package_trust_failures,
    collect_manifest_trust_failures,
    default_trust_policy_payload,
    sign_manifest_trust_envelope,
)

PACKAGE_MANIFEST_CONTRACT_ID = "objc3c.package_ecosystem.package_manifest.v1"
PACKAGE_MODULE_GRAPH_CONTRACT_ID = "objc3c.package_ecosystem.module_graph.v1"
LOCAL_PACKAGE_LANGUAGE_VERSION = "3.0"
LOCAL_PACKAGE_LANGUAGE_MODE = "strict"
LOCAL_PACKAGE_ABI_IDENTITY = "objc3-abi-2025Q4"
LOCAL_PACKAGE_HOST_PLATFORM = "windows-x64"
DIRECT_IMPORT_SYNTAX_SUPPORT = "reserved-fail-closed"
LOCAL_MODULE_GRAPH_RESOLVER = "checked-in-local-registry"


@dataclass(frozen=True)
class PackageManagerPaths:
    lock_path: Path
    manifest_root: Path


def public_workflow_command(action: str) -> str:
    return f"npm run objc3c -- {action}"


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


def trust_payload(package_id: str, signing_material: dict[str, Any]) -> dict[str, Any]:
    package_version = str(signing_material.get("package_version", ""))
    source_digest = str(signing_material.get("source_digest", ""))
    manifest_digest = str(signing_material.get("manifest_digest", source_digest))
    abi_identity = str(signing_material.get("abi_identity", LOCAL_PACKAGE_ABI_IDENTITY))
    language_version = str(signing_material.get("language_version", LOCAL_PACKAGE_LANGUAGE_VERSION))
    return sign_manifest_trust_envelope(
        {
            "package_id": package_id,
            "package_namespace": package_namespace(package_id),
            "package_version": package_version,
            "source_digest": source_digest,
            "language": {"version": language_version},
            "abi": {"identity": abi_identity},
        },
        manifest_digest=manifest_digest,
    )


def dependency_payload(package_id: str, *, required_version: str) -> dict[str, str]:
    return {
        "package_id": package_id,
        "source": "checked-in-local-workspace",
        "version_requirement": required_version,
        "language_requirement": LOCAL_PACKAGE_LANGUAGE_VERSION,
        "abi_requirement": LOCAL_PACKAGE_ABI_IDENTITY,
    }


def manifest_signing_material(manifest: dict[str, Any]) -> dict[str, Any]:
    return {
        "package_id": manifest.get("package_id"),
        "source": manifest.get("source"),
        "source_digest": manifest.get("source_digest"),
        "package_version": manifest.get("package_version"),
        "language_version": manifest.get("language", {}).get("version")
        if isinstance(manifest.get("language"), dict)
        else None,
        "abi_identity": manifest.get("abi", {}).get("identity")
        if isinstance(manifest.get("abi"), dict)
        else None,
        "manifest_digest": manifest.get("manifest_digest"),
        "dependencies": manifest.get("dependencies"),
    }


def package_manifest_digest(manifest: dict[str, Any]) -> str:
    payload = dict(manifest)
    payload.pop("manifest_digest", None)
    payload.pop("trust", None)
    return stable_digest(payload)


def module_import_edges(
    *,
    from_module: str,
    dependencies: list[dict[str, str]],
) -> list[dict[str, str]]:
    return sorted(
        [
            {
                "from_module": from_module,
                "to_module": package_name(str(dependency["package_id"])),
                "to_package_id": str(dependency["package_id"]),
                "source": str(dependency["source"]),
                "resolution": "locked-local-registry",
                "required_version": str(dependency["version_requirement"]),
            }
            for dependency in dependencies
        ],
        key=lambda edge: (edge["to_package_id"], edge["required_version"]),
    )


def module_graph_payload(
    *,
    package_id: str,
    source_kind: str,
    source: str,
    source_digest: str,
    module_id: str,
    implementation_module: str,
    source_authority: str,
    source_authority_digest: str,
    dependencies: list[dict[str, str]],
) -> dict[str, Any]:
    return {
        "contract_id": PACKAGE_MODULE_GRAPH_CONTRACT_ID,
        "package_id": package_id,
        "package_source_kind": source_kind,
        "source": source,
        "source_digest": source_digest,
        "source_authority": source_authority,
        "source_authority_digest": source_authority_digest,
        "module_name": {
            "canonical": module_id,
            "implementation": implementation_module,
            "case_sensitive": True,
        },
        "resolver": LOCAL_MODULE_GRAPH_RESOLVER,
        "direct_import_syntax": DIRECT_IMPORT_SYNTAX_SUPPORT,
        "import_edges": module_import_edges(
            from_module=module_id,
            dependencies=dependencies,
        ),
        "unsafe_metadata_policy": {
            "missing_module_graph": "fail-closed",
            "missing_package_manifest": "fail-closed",
            "manual_sidecar_manifest": "fail-closed",
            "network_or_hosted_metadata": "fail-closed",
        },
    }


def package_manifest_payload(
    *,
    package_id: str,
    source: str,
    source_kind: str,
    package_version: str,
    source_digest: str,
    module_graph: dict[str, Any],
    dependencies: list[dict[str, str]],
    runtime_symbols: list[str],
    replay_actions: list[str],
) -> dict[str, Any]:
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
        "module_graph": module_graph,
        "dependencies": dependencies,
        "registry": {
            "resolution": "checked-in-local-registry",
            "network_resolution": "unsupported-fail-closed",
            "offline_mirror_required": True,
        },
        "replay": {
            "commands": [public_workflow_command(action) for action in replay_actions],
        },
    }
    payload["manifest_digest"] = package_manifest_digest(payload)
    payload["trust"] = sign_manifest_trust_envelope(
        payload,
        manifest_digest=str(payload["manifest_digest"]),
    )
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
        "module_graph": manifest["module_graph"],
        "provenance_id": provenance_id(str(manifest["package_id"])),
        "trust": manifest["trust"],
    }


def lock_dependency_payload(
    *,
    from_package_id: str,
    manifest_dependency: dict[str, str],
    target_package: dict[str, Any],
) -> dict[str, str]:
    target_manifest = target_package.get("package_manifest", {})
    if not isinstance(target_manifest, dict):
        target_manifest = {}
    target_version = str(target_package.get("package_version", ""))
    return {
        "from": from_package_id,
        "to": str(manifest_dependency["package_id"]),
        "source": str(manifest_dependency["source"]),
        "language_requirement": str(manifest_dependency["language_requirement"]),
        "abi_requirement": str(manifest_dependency["abi_requirement"]),
        "resolution": "locked-local-registry",
        "required_version": str(manifest_dependency.get("version_requirement", target_version)),
        "resolved_version": target_version,
        "target_source_digest": str(target_package.get("source_digest", "")),
        "target_manifest_digest": str(target_manifest.get("digest", "")),
    }


def package_resolution_plan(
    *,
    packages: list[dict[str, Any]],
    dependencies: list[dict[str, Any]],
) -> dict[str, Any]:
    package_ids = sorted(str(package.get("package_id")) for package in packages)
    graph: dict[str, list[str]] = {package_id: [] for package_id in package_ids}
    edge_by_pair: dict[tuple[str, str], dict[str, Any]] = {}
    for dependency in dependencies:
        from_id = str(dependency.get("from"))
        to_id = str(dependency.get("to"))
        graph.setdefault(from_id, []).append(to_id)
        edge_by_pair[(from_id, to_id)] = dependency
    for targets in graph.values():
        targets.sort()

    def closure_for(package_id: str) -> list[str]:
        closure: list[str] = []
        seen: set[str] = set()
        visiting: set[str] = set()

        def visit(current_id: str) -> None:
            for target_id in graph.get(current_id, []):
                if target_id in visiting:
                    continue
                if target_id in seen:
                    continue
                visiting.add(target_id)
                visit(target_id)
                visiting.remove(target_id)
                seen.add(target_id)
                closure.append(target_id)

        visiting.add(package_id)
        visit(package_id)
        return closure

    install_order: list[str] = []
    installed: set[str] = set()
    visiting_install: set[str] = set()

    def visit_install(package_id: str) -> None:
        if package_id in installed or package_id in visiting_install:
            return
        visiting_install.add(package_id)
        for target_id in graph.get(package_id, []):
            visit_install(target_id)
        visiting_install.remove(package_id)
        installed.add(package_id)
        install_order.append(package_id)

    for package_id in package_ids:
        visit_install(package_id)

    resolution_edges = [
        {
            "from": from_id,
            "to": to_id,
            "required_version": str(edge.get("required_version", "")),
            "resolved_version": str(edge.get("resolved_version", "")),
            "target_source_digest": str(edge.get("target_source_digest", "")),
            "target_manifest_digest": str(edge.get("target_manifest_digest", "")),
        }
        for (from_id, to_id), edge in sorted(edge_by_pair.items())
    ]
    return {
        "contract_id": "objc3c.package_ecosystem.resolution_plan.v1",
        "resolver": "deterministic-local-registry",
        "selection_policy": "exact-locked-version-only",
        "network_resolution": "unsupported-fail-closed",
        "hosted_registry": "unsupported-fail-closed-if-claimed",
        "install_order": install_order,
        "dependency_closures": [
            {
                "package_id": package_id,
                "dependencies_first": closure_for(package_id),
            }
            for package_id in package_ids
        ],
        "resolution_edges": resolution_edges,
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
    manifest_dependencies_by_package: dict[str, list[dict[str, str]]] = {}
    digest_inputs: list[str] = []
    generator = "scripts/build_objc3c_package_lock.py"
    stdlib_module_inventory_source = "stdlib/module_inventory.json"
    showcase_portfolio_source = "showcase/portfolio.json"
    stdlib_module_inventory_digest = file_digest(root / stdlib_module_inventory_source)
    showcase_portfolio_digest = file_digest(root / showcase_portfolio_source)

    stdlib_versions: dict[str, str] = {}
    for module in sorted((entry for entry in modules if isinstance(entry, dict)), key=lambda entry: str(entry.get("module", ""))):
        module_id = str(module["module"])
        package_id = f"stdlib:{module_id}"
        package_version = package_version_from_module(module)
        stdlib_versions[package_id] = package_version
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
            package_version=package_version,
            source_digest=file_digest(root / source),
            module_graph=module_graph_payload(
                package_id=package_id,
                source=source,
                source_kind="stdlib-module-manifest",
                source_digest=file_digest(root / source),
                module_id=module_id,
                implementation_module=str(module.get("implementation_module", "")),
                source_authority=stdlib_module_inventory_source,
                source_authority_digest=stdlib_module_inventory_digest,
                dependencies=[],
            ),
            dependencies=[],
            runtime_symbols=runtime_symbols,
            replay_actions=[
                "build-package-lock",
                "validate-package-manager-model",
                "validate-package-authoring",
            ],
        )
        package_manifests.append(manifest)
        manifest_dependencies_by_package[package_id] = []
        digest_inputs.append(source)

    for example in sorted((entry for entry in examples if isinstance(entry, dict)), key=lambda entry: str(entry.get("id", ""))):
        example_id = str(example["id"])
        package_id = f"showcase:{example_id}"
        source = str(example["workspace_manifest"])
        package_dependencies = [
            dependency_payload(
                f"stdlib:{name}",
                required_version=stdlib_versions.get(f"stdlib:{name}", "0.0.0"),
            )
            for name in sorted(str(name) for name in example.get("stdlib_followup_modules", []) if isinstance(name, str))
        ]
        manifest = package_manifest_payload(
            package_id=package_id,
            source=source,
            source_kind="showcase-workspace-manifest",
            package_version="1.0.0",
            source_digest=file_digest(root / source),
            module_graph=module_graph_payload(
                package_id=package_id,
                source=source,
                source_kind="showcase-workspace-manifest",
                source_digest=file_digest(root / source),
                module_id=f"showcase.{example_id}",
                implementation_module=example_id,
                source_authority=showcase_portfolio_source,
                source_authority_digest=showcase_portfolio_digest,
                dependencies=package_dependencies,
            ),
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
        manifest_dependencies_by_package[package_id] = package_dependencies
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
    packages_by_id = {str(package["package_id"]): package for package in packages}
    dependencies = []
    for from_package_id, package_dependencies in sorted(manifest_dependencies_by_package.items()):
        for manifest_dependency in package_dependencies:
            target_package = packages_by_id.get(str(manifest_dependency["package_id"]))
            if target_package is None:
                dependencies.append(
                    {
                        "from": from_package_id,
                        "to": str(manifest_dependency["package_id"]),
                        "source": str(manifest_dependency["source"]),
                        "language_requirement": str(manifest_dependency["language_requirement"]),
                        "abi_requirement": str(manifest_dependency["abi_requirement"]),
                        "resolution": "locked-local-registry",
                        "required_version": str(manifest_dependency["version_requirement"]),
                        "resolved_version": "",
                        "target_source_digest": "",
                        "target_manifest_digest": "",
                    }
                )
                continue
            dependencies.append(
                lock_dependency_payload(
                    from_package_id=from_package_id,
                    manifest_dependency=manifest_dependency,
                    target_package=target_package,
                )
            )
    dependencies = sorted(dependencies, key=lambda entry: (str(entry["from"]), str(entry["to"])))
    digest_inputs.extend(manifest_paths)
    return {
        "package_manifests": sorted(package_manifests, key=lambda entry: str(entry["package_id"])),
        "packages": sorted(packages, key=lambda entry: str(entry["package_id"])),
        "dependencies": dependencies,
        "provenance": sorted(provenance, key=lambda entry: str(entry["provenance_id"])),
        "resolution_plan": package_resolution_plan(
            packages=sorted(packages, key=lambda entry: str(entry["package_id"])),
            dependencies=dependencies,
        ),
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


def collect_package_module_graph_failures(
    package: dict[str, Any],
    *,
    dependencies: list[dict[str, Any]],
    root: Path,
) -> list[str]:
    package_id = str(package.get("package_id", ""))
    graph = package.get("module_graph")
    if not isinstance(graph, dict):
        return [f"{PACKAGE_MANAGER_TAMPER_CODE}: missing module graph metadata for {package_id}"]

    failures: list[str] = []
    if graph.get("contract_id") != PACKAGE_MODULE_GRAPH_CONTRACT_ID:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph contract drift for {package_id}")
    if graph.get("package_id") != package_id:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph package id drift for {package_id}")
    if graph.get("package_source_kind") != package.get("source_kind"):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph source kind drift for {package_id}")
    if graph.get("source") != package.get("source"):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph source drift for {package_id}")
    if graph.get("source_digest") != package.get("source_digest"):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph source digest drift for {package_id}")

    source_authority = str(graph.get("source_authority", ""))
    if source_authority not in {"stdlib/module_inventory.json", "showcase/portfolio.json"}:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: unsafe module graph source authority for {package_id}")
    source_authority_path = root / source_authority
    if not source_authority_path.is_file():
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing module graph source authority for {package_id}")
    elif graph.get("source_authority_digest") != file_digest(source_authority_path):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph source authority digest drift for {package_id}")

    module_name = graph.get("module_name", {})
    if not isinstance(module_name, dict):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing module graph module name for {package_id}")
        from_module = ""
    else:
        from_module = str(module_name.get("canonical", ""))
        if not from_module:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing canonical module name for {package_id}")
        if not str(module_name.get("implementation", "")):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing implementation module name for {package_id}")
        if module_name.get("case_sensitive") is not True:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph case sensitivity drift for {package_id}")

    if graph.get("resolver") != LOCAL_MODULE_GRAPH_RESOLVER:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph resolver drift for {package_id}")
    if graph.get("direct_import_syntax") != DIRECT_IMPORT_SYNTAX_SUPPORT:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: direct @import support claim widened for {package_id}")

    unsafe_policy = graph.get("unsafe_metadata_policy", {})
    expected_unsafe_policy = {
        "missing_module_graph": "fail-closed",
        "missing_package_manifest": "fail-closed",
        "manual_sidecar_manifest": "fail-closed",
        "network_or_hosted_metadata": "fail-closed",
    }
    if not isinstance(unsafe_policy, dict):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing unsafe metadata policy for {package_id}")
    elif unsafe_policy != expected_unsafe_policy:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: unsafe metadata policy drift for {package_id}")

    expected_edges: list[dict[str, str]] = []
    for dependency in dependencies:
        to_package_id = str(dependency.get("to", ""))
        expected_edges.append(
            {
                "from_module": from_module,
                "to_module": package_name(to_package_id) if ":" in to_package_id else "",
                "to_package_id": to_package_id,
                "source": str(dependency.get("source", "")),
                "resolution": str(dependency.get("resolution", "")),
                "required_version": str(dependency.get("required_version", "")),
            }
        )
    expected_edges = sorted(
        expected_edges,
        key=lambda edge: (edge["to_package_id"], edge["required_version"]),
    )
    actual_edges = graph.get("import_edges", [])
    if actual_edges != expected_edges:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: module graph import edges drift for {package_id}")
    return failures


def collect_lock_model_failures(lock: dict[str, Any], *, root: Path) -> list[str]:
    failures: list[str] = []
    raw_packages = lock.get("packages", [])
    raw_dependencies = lock.get("dependencies", [])
    raw_provenance = lock.get("provenance", [])
    raw_trust_policy = lock.get("trust_policy")
    trust_policy = raw_trust_policy if isinstance(raw_trust_policy, dict) else default_trust_policy_payload()
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
    dependencies = [dependency for dependency in raw_dependencies if isinstance(dependency, dict)]
    expected_resolution_plan = package_resolution_plan(
        packages=packages,
        dependencies=dependencies,
    )
    resolution_plan = lock.get("resolution_plan")
    if not isinstance(resolution_plan, dict):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing package resolution plan")
    elif resolution_plan != expected_resolution_plan:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package resolution plan drifted")

    provenance_by_id = {
        str(entry.get("provenance_id")): entry
        for entry in raw_provenance
        if isinstance(entry, dict)
    }
    for package in packages:
        package_id = str(package.get("package_id"))
        source = str(package.get("source", ""))
        package_dependencies = [
            dependency
            for dependency in dependencies
            if str(dependency.get("from")) == package_id
        ]
        source_path = root / source
        if not source_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing package source for {package_id}")
        elif package.get("source_digest") != file_digest(source_path):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: source digest mismatch for {package_id}")
        if package.get("language_version") != LOCAL_PACKAGE_LANGUAGE_VERSION:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: language version mismatch for {package_id}")
        if package.get("abi_identity") != LOCAL_PACKAGE_ABI_IDENTITY:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: ABI identity mismatch for {package_id}")
        failures.extend(
            collect_package_module_graph_failures(
                package,
                dependencies=package_dependencies,
                root=root,
            )
        )
        trust = package.get("trust", {})
        if not isinstance(trust, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing trust envelope for {package_id}")
        else:
            failures.extend(
                collect_lock_package_trust_failures(
                    package,
                    trust_policy=trust_policy,
                )
            )
            if trust.get("signing_key_id") != LOCAL_PACKAGE_TRUST_KEY_ID:
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: signing key drift for {package_id}")
            if trust.get("revocation_state") != "not-revoked":
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: revoked package cannot resolve {package_id}")
        manifest = package.get("package_manifest", {})
        if not isinstance(manifest, dict) or not manifest:
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
            absolute_manifest_path = root / manifest_path
            if not absolute_manifest_path.is_file():
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing package manifest for {package_id}")
            else:
                manifest_payload = json.loads(absolute_manifest_path.read_text(encoding="utf-8"))
                manifest_digest = package_manifest_digest(manifest_payload)
                if manifest_payload.get("manifest_digest") != manifest.get("digest"):
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package manifest digest field drift for {package_id}")
                if manifest_digest != manifest.get("digest"):
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package manifest digest mismatch for {package_id}")
                if manifest_payload.get("module_graph") != package.get("module_graph"):
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package manifest module graph drift for {package_id}")
                failures.extend(
                    collect_manifest_trust_failures(
                        manifest_payload,
                        manifest_digest=str(manifest.get("digest", "")),
                        trust_policy=trust_policy,
                    )
                )
                expected_trust = sign_manifest_trust_envelope(
                    manifest_payload,
                    manifest_digest=str(manifest.get("digest", "")),
                    trust_policy=trust_policy,
                )
                if manifest_payload.get("trust") != expected_trust:
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package manifest trust signature drift for {package_id}")
                if isinstance(trust, dict) and trust != expected_trust:
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: lock trust signature drift for {package_id}")
                expected_manifest_dependencies = sorted(
                    [
                        {
                            "package_id": str(dependency.get("to")),
                            "source": str(dependency.get("source")),
                            "version_requirement": str(dependency.get("required_version")),
                            "language_requirement": str(dependency.get("language_requirement")),
                            "abi_requirement": str(dependency.get("abi_requirement")),
                        }
                        for dependency in dependencies
                        if str(dependency.get("from")) == package_id
                    ],
                    key=lambda dependency: str(dependency["package_id"]),
                )
                actual_manifest_dependencies = manifest_payload.get("dependencies", [])
                if isinstance(actual_manifest_dependencies, list):
                    actual_manifest_dependencies = sorted(
                        [
                            dependency
                            for dependency in actual_manifest_dependencies
                            if isinstance(dependency, dict)
                        ],
                        key=lambda dependency: str(dependency.get("package_id")),
                    )
                if actual_manifest_dependencies != expected_manifest_dependencies:
                    failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package manifest dependency closure drift for {package_id}")

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
        if target_package is not None and dependency.get("required_version") != target_package.get("package_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: version requirement mismatch for {from_id}->{to_id}")
        if target_package is not None and dependency.get("resolved_version") != target_package.get("package_version"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: resolved version mismatch for {from_id}->{to_id}")
        if target_package is not None and dependency.get("target_source_digest") != target_package.get("source_digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: target source digest mismatch for {from_id}->{to_id}")
        target_manifest = target_package.get("package_manifest") if target_package is not None else None
        if isinstance(target_manifest, dict) and dependency.get("target_manifest_digest") != target_manifest.get("digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: target manifest digest mismatch for {from_id}->{to_id}")

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
