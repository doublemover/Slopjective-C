"""Clean install distribution credibility checks for local package artifacts."""

from __future__ import annotations

import os
from pathlib import Path
import shutil
from typing import Any

from objc3c_package_manager.model import (
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    LOCAL_PACKAGE_TRUST_KEY_ID,
    PACKAGE_MANAGER_TAMPER_CODE,
    cache_payload_from_mirror_package,
    collect_lock_model_failures,
    file_digest,
    stable_digest,
)
from objc3c_package_manager.trust import (
    collect_extraction_plan_failures,
    collect_filesystem_extraction_plan_failures,
    package_extraction_plan_payload,
)
from objc3c_shared.schema_registry import validate_registered_schema
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel


INSTALL_DISTRIBUTION_CONTRACT_ID = (
    "objc3c.package_ecosystem.install_distribution_credibility.v1"
)
INSTALL_DISTRIBUTION_SUMMARY_CONTRACT_ID = (
    "objc3c.package_ecosystem.install_distribution_credibility.summary.v1"
)
INSTALL_DISTRIBUTION_ACTION = "validate-package-install-distribution"
INSTALL_VALIDATION_ROOT_REL = "tmp/artifacts/package-ecosystem/install-validation"
INSTALL_ROOT_REL = f"{INSTALL_VALIDATION_ROOT_REL}/clean-root"
INSTALL_HOME_REL = f"{INSTALL_ROOT_REL}/objc3c"
INSTALL_RECEIPT_REL = f"{INSTALL_ROOT_REL}/objc3c-install-receipt.json"
PACKAGE_RECEIPT_ROOT_REL = f"{INSTALL_HOME_REL}/receipts"
PACKAGE_UPDATE_RECEIPT_REL = (
    f"{PACKAGE_RECEIPT_ROOT_REL}/objc3c-update-plan-receipt.json"
)
PACKAGE_UNINSTALL_RECEIPT_REL = (
    f"{PACKAGE_RECEIPT_ROOT_REL}/objc3c-uninstall-plan-receipt.json"
)
INSTALL_VERIFICATION_REL = (
    f"{INSTALL_VALIDATION_ROOT_REL}/objc3c-install-distribution-verification.json"
)
PLATFORM_CLEAN_INSTALL_RECEIPT_NAME = "clean-install-distribution-receipt.json"
INSTALL_LOCAL_ARTIFACT_ROOT_REL = f"{INSTALL_VALIDATION_ROOT_REL}/local-package-artifacts"
INSTALL_PROOF_MANIFEST_REL = f"{INSTALL_VALIDATION_ROOT_REL}/objc3c-install-proof-manifest.json"
INSTALL_BOOTSTRAP_ENTRYPOINT = "Bootstrap-objc3cEnvironment.ps1"
INSTALL_PACKAGE_BRIDGE = "objc3c"
INSTALL_RECEIPT_CONTRACT_ID = (
    "objc3c.package_ecosystem.from_nothing_install_receipt.v1"
)
INSTALL_RECEIPT_SCHEMA = (
    "schemas/objc3c-package-install-distribution-receipt-v1.schema.json"
)
INSTALL_RECEIPT_SCHEMA_ID = "objc3c-package-install-distribution-receipt-v1"
PACKAGE_OPERATION_RECEIPT_CONTRACT_ID = (
    "objc3c.package_ecosystem.install_distribution_operation_receipt.v1"
)
PACKAGE_OPERATION_RECEIPT_SCHEMA = (
    "schemas/objc3c-package-install-distribution-operation-receipt-v1.schema.json"
)
PACKAGE_OPERATION_RECEIPT_SCHEMA_ID = (
    "objc3c-package-install-distribution-operation-receipt-v1"
)
INSTALL_PROOF_CONTRACT_ID = "objc3c.package_ecosystem.from_nothing_install_proof.v1"
NO_NETWORK_POLICY = "no-network-during-validation"
HOSTED_REGISTRY_FAIL_CLOSED = "unsupported-fail-closed-if-claimed"
NETWORK_RESOLUTION_UNSUPPORTED = "unsupported"
OFFLINE_RESTORE_SUPPORT = "local-cache-digest-checked"


def package_ids(payload: dict[str, Any]) -> list[str]:
    packages = payload.get("packages", [])
    if not isinstance(packages, list):
        return []
    return sorted(
        str(package.get("package_id"))
        for package in packages
        if isinstance(package, dict)
    )


def _namespace_and_name(package_id: str) -> tuple[str, str]:
    namespace, separator, name = package_id.partition(":")
    if not separator or not namespace or not name:
        raise RuntimeError(f"package id is not namespace-qualified: {package_id}")
    return namespace, name.replace(".", "_")


def _layout_path(root: Path, output_root: Path | None, relative: str) -> Path:
    return (output_root or root) / relative


def _layout_rel(root: Path, output_root: Path | None, relative: str) -> str:
    return repo_rel(_layout_path(root, output_root, relative))


def _require_clean_install_path(root: Path, install_root: Path, *, output_root: Path | None = None) -> None:
    base = (output_root or root).resolve()
    try:
        base.relative_to(root.resolve())
    except ValueError as exc:
        raise RuntimeError(f"install output root escaped repository root: {base}") from exc
    allowed_root = (base / INSTALL_VALIDATION_ROOT_REL).resolve()
    if not allowed_root.is_relative_to(base):
        raise RuntimeError(f"install validation root escaped output root: {allowed_root}")
    resolved = install_root.resolve()
    try:
        resolved.relative_to(allowed_root)
    except ValueError as exc:
        raise RuntimeError(
            f"install validation root escaped package ecosystem tmp root: {install_root}"
        ) from exc


def reset_clean_install_root(root: Path, *, output_root: Path | None = None) -> bool:
    install_root = _layout_path(root, output_root, INSTALL_ROOT_REL)
    _require_clean_install_path(root, install_root, output_root=output_root)
    existed = install_root.exists()
    if existed:
        shutil.rmtree(install_root)
    install_root.mkdir(parents=True, exist_ok=True)
    return existed


def package_manifest_digest(manifest: dict[str, Any]) -> str:
    payload = dict(manifest)
    payload.pop("manifest_digest", None)
    payload.pop("trust", None)
    return stable_digest(payload)


def package_manifest_install_path(root: Path, package_id: str, *, output_root: Path | None = None) -> Path:
    namespace, name = _namespace_and_name(package_id)
    return _layout_path(root, output_root, INSTALL_HOME_REL) / "packages" / namespace / name / "package-manifest.json"


def local_package_artifact_path(root: Path, package_id: str, *, output_root: Path | None = None) -> Path:
    namespace, name = _namespace_and_name(package_id)
    return _layout_path(root, output_root, INSTALL_LOCAL_ARTIFACT_ROOT_REL) / namespace / f"{name}.json"


def _copy_json_payload(source: Path, target: Path) -> dict[str, Any]:
    payload = load_json(source)
    write_json_file(target, payload)
    return payload


def install_receipt_payload(root: Path, *, output_root: Path | None = None) -> dict[str, Any]:
    return {
        "contract_id": INSTALL_RECEIPT_CONTRACT_ID,
        "schema": INSTALL_RECEIPT_SCHEMA,
        "install_root": _layout_rel(root, output_root, INSTALL_ROOT_REL),
        "install_home": _layout_rel(root, output_root, INSTALL_HOME_REL),
        "bootstrap_entrypoint": INSTALL_BOOTSTRAP_ENTRYPOINT,
        "package_bridge": INSTALL_PACKAGE_BRIDGE,
        "install_command": f"npm run objc3c -- {INSTALL_DISTRIBUTION_ACTION} --from-nothing",
        "machine_owned": True,
        "installed_at_utc": "omitted-for-deterministic-replay",
    }


def platform_host_evidence_root(root: Path) -> tuple[str, Path] | None:
    platform_id = os.environ.get("OBJC3C_PLATFORM_ID", "")
    evidence_root = os.environ.get("OBJC3C_PLATFORM_EVIDENCE_ROOT", "")
    if platform_id not in {"linux-x64", "darwin-arm64"} or not evidence_root:
        return None
    resolved_root = (root / evidence_root).resolve()
    expected_root = (root / "tmp" / "reports" / "platform-host-evidence" / platform_id).resolve()
    if resolved_root != expected_root:
        raise RuntimeError(
            "platform install evidence root must be platform-scoped: "
            f"{evidence_root}"
        )
    return platform_id, resolved_root


def publish_platform_install_receipt(
    *,
    root: Path,
    install_receipt_path: Path,
) -> dict[str, Any] | None:
    config = platform_host_evidence_root(root)
    if config is None:
        return None
    platform_id, evidence_root = config
    target_path = evidence_root / "install" / PLATFORM_CLEAN_INSTALL_RECEIPT_NAME
    target_path.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(install_receipt_path, target_path)
    return {
        "platform_id": platform_id,
        "source_receipt": repo_rel(install_receipt_path),
        "platform_scoped_clean_install_receipt": repo_rel(target_path),
        "source_receipt_sha256": file_digest(install_receipt_path),
        "host_promotion_receipt_path_reserved": (
            f"tmp/reports/platform-host-evidence/{platform_id}/install/install-receipt.json"
        ),
        "support_truth": False,
    }


def package_operation_receipt_payload(
    *,
    root: Path,
    lock: dict[str, Any],
    operation: str,
    package_order: list[str],
    output_root: Path | None = None,
) -> dict[str, Any]:
    packages_by_id = {
        str(entry.get("package_id")): entry
        for entry in lock.get("packages", [])
        if isinstance(entry, dict)
    }
    package_digests: list[dict[str, str]] = []
    for package_id in package_order:
        package = packages_by_id.get(package_id, {})
        manifest = package.get("package_manifest", {})
        if not isinstance(manifest, dict):
            manifest = {}
        trust = package.get("trust", {})
        if not isinstance(trust, dict):
            trust = {}
        package_digests.append(
            {
                "package_id": package_id,
                "source_digest": str(package.get("source_digest", "")),
                "manifest_digest": str(manifest.get("digest", "")),
                "trust_signature": str(trust.get("signature", "")),
            }
        )
    operation_mode = (
        "locked-local-update-noop"
        if operation == "update"
        else "reverse-dependency-order-uninstall-plan"
    )
    action = "validate-package-install-distribution --from-nothing"
    return {
        "contract_id": PACKAGE_OPERATION_RECEIPT_CONTRACT_ID,
        "schema": PACKAGE_OPERATION_RECEIPT_SCHEMA,
        "operation": operation,
        "operation_mode": operation_mode,
        "install_root": _layout_rel(root, output_root, INSTALL_ROOT_REL),
        "install_home": _layout_rel(root, output_root, INSTALL_HOME_REL),
        "receipt_root": _layout_rel(root, output_root, PACKAGE_RECEIPT_ROOT_REL),
        "package_bridge": INSTALL_PACKAGE_BRIDGE,
        "command": f"npm run objc3c -- {action}",
        "network_policy": NO_NETWORK_POLICY,
        "hosted_registry_support": HOSTED_REGISTRY_FAIL_CLOSED,
        "language_version": lock.get("package_manager", {}).get("language_version"),
        "abi_identity": lock.get("package_manager", {}).get("abi_identity"),
        "selection_policy": "exact-locked-version-only",
        "state_mutation": "record-only-deterministic-plan",
        "timestamp": "omitted-for-deterministic-replay",
        "package_count": len(package_order),
        "package_order": package_order,
        "package_digests": package_digests,
    }


def bridge_payload(contract: dict[str, Any]) -> dict[str, Any]:
    actions = [
        str(action)
        for action in contract.get("required_public_actions", [])
        if isinstance(action, str)
    ]
    return {
        "contract_id": "objc3c.package_ecosystem.install_bridge.v1",
        "package_bridge": INSTALL_PACKAGE_BRIDGE,
        "network_policy": NO_NETWORK_POLICY,
        "public_actions": sorted(actions),
    }


def install_artifact_payload(record: dict[str, Any]) -> dict[str, Any]:
    payload = {
        "contract_id": "objc3c.package_ecosystem.local_install_artifact.v1",
        "artifact_kind": "from-nothing-local-package-install-proof",
        "package_id": str(record["package_id"]),
        "source": str(record["source"]),
        "source_digest": str(record["source_digest"]),
        "source_manifest": str(record["source_manifest"]),
        "installed_manifest": str(record["installed_manifest"]),
        "manifest_digest": str(record["manifest_digest"]),
        "lock_manifest_digest": str(record["lock_manifest_digest"]),
        "trust_signature": str(record["trust_signature"]),
        "generated_report_source": False,
    }
    payload["artifact_digest"] = stable_digest(payload)
    return payload


def install_proof_payload(
    *,
    root: Path,
    contract: dict[str, Any],
    bridge_path: Path,
    registry_copy: Path,
    publication_copy: Path,
    mirror_copy: Path,
    restore_copy: Path,
    local_artifacts: list[dict[str, str]],
    output_root: Path | None = None,
) -> dict[str, Any]:
    release_input_paths = sorted(
        [
            _layout_rel(root, output_root, INSTALL_RECEIPT_REL),
            _layout_rel(root, output_root, PACKAGE_UPDATE_RECEIPT_REL),
            _layout_rel(root, output_root, PACKAGE_UNINSTALL_RECEIPT_REL),
            repo_rel(bridge_path),
            repo_rel(registry_copy),
            repo_rel(publication_copy),
            repo_rel(mirror_copy),
            repo_rel(restore_copy),
            *[artifact["artifact_path"] for artifact in local_artifacts],
        ]
    )
    return {
        "contract_id": INSTALL_PROOF_CONTRACT_ID,
        "source_contract": repo_rel(
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "package_ecosystem"
            / "install_distribution_credibility_contract.json"
        ),
        "proof_contract": repo_rel(
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "package_ecosystem"
            / "from_nothing_install_proof_contract.json"
        ),
        "install_verification": _layout_rel(root, output_root, INSTALL_VERIFICATION_REL),
        "install_receipt": _layout_rel(root, output_root, INSTALL_RECEIPT_REL),
        "local_package_artifact_root": _layout_rel(root, output_root, INSTALL_LOCAL_ARTIFACT_ROOT_REL),
        "package_bridge": INSTALL_PACKAGE_BRIDGE,
        "network_policy": NO_NETWORK_POLICY,
        "hosted_registry_support": HOSTED_REGISTRY_FAIL_CLOSED,
        "required_public_actions": sorted(
            str(action)
            for action in contract.get("required_public_actions", [])
            if isinstance(action, str)
        ),
        "release_manifest_validation": {
            "required_release_manifest_command": "npm run objc3c -- build-release-manifest",
            "generated_report_inputs_allowed": False,
            "forbidden_input_prefixes": ["tmp/reports/"],
            "generated_report_inputs": [],
            "release_manifest_input_paths": release_input_paths,
            "non_source_report_outputs": [
                "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
            ],
        },
        "local_package_artifacts": local_artifacts,
    }


def _artifact_digest(payload: dict[str, Any]) -> str:
    normalized = dict(payload)
    normalized.pop("artifact_digest", None)
    return stable_digest(normalized)


def install_extraction_plan_payload(
    *,
    root: Path,
    install_order: list[str],
    registry_copy: Path,
    publication_copy: Path,
    mirror_copy: Path,
    restore_copy: Path,
    bootstrap_path: Path,
    bridge_path: Path,
    install_receipt_path: Path,
    update_receipt_path: Path,
    uninstall_receipt_path: Path,
    install_proof_path: Path,
    install_verification_path: Path,
    output_root: Path | None = None,
) -> dict[str, Any]:
    entries: list[dict[str, str | int]] = []
    for index, package_id in enumerate(install_order):
        entries.extend(
            [
                {
                    "path": repo_rel(package_manifest_install_path(root, package_id, output_root=output_root)),
                    "entry_type": "file",
                    "mutation": "copy",
                    "package_id": package_id,
                    "order": index,
                },
                {
                    "path": repo_rel(local_package_artifact_path(root, package_id, output_root=output_root)),
                    "entry_type": "file",
                    "mutation": "write",
                    "package_id": package_id,
                    "order": index,
                },
            ]
        )
    for order, path in enumerate(
        (
            registry_copy,
            publication_copy,
            mirror_copy,
            restore_copy,
            bootstrap_path,
            bridge_path,
            install_receipt_path,
            update_receipt_path,
            uninstall_receipt_path,
            install_proof_path,
            install_verification_path,
        ),
        start=len(entries),
    ):
        entries.append(
            {
                "path": repo_rel(path),
                "entry_type": "file",
                "mutation": "write",
                "package_id": "installer/update-policy",
                "order": order,
            }
        )
    return package_extraction_plan_payload(
        plan_id=f"install-distribution-{stable_digest(install_order)}",
        entries=entries,
        provenance="install-distribution-before-filesystem-mutation",
    )


def materialize_clean_distribution_install(
    *,
    root: Path,
    contract: dict[str, Any],
    lock: dict[str, Any],
    mirror: dict[str, Any],
    registry: dict[str, Any],
    publication: dict[str, Any],
    restore_receipt: dict[str, Any],
    mirror_path: Path,
    registry_path: Path,
    publication_path: Path,
    restore_receipt_path: Path,
    output_root: Path | None = None,
) -> dict[str, Any]:
    output_root = output_root or root
    preexisting_root_removed = reset_clean_install_root(root, output_root=output_root)
    install_home = _layout_path(root, output_root, INSTALL_HOME_REL)
    packages_dir = install_home / "packages"
    registry_dir = install_home / "registry"
    mirror_dir = install_home / "offline-mirror"
    receipt_dir = install_home / "receipts"
    bin_dir = install_home / "bin"
    artifact_root = _layout_path(root, output_root, INSTALL_LOCAL_ARTIFACT_ROOT_REL)

    packages_by_id = {
        str(entry.get("package_id")): entry
        for entry in lock.get("packages", [])
        if isinstance(entry, dict)
    }
    raw_install_order = lock.get("resolution_plan", {}).get("install_order", [])
    install_order = [
        str(package_id)
        for package_id in raw_install_order
        if isinstance(package_id, str) and package_id in packages_by_id
    ] if isinstance(raw_install_order, list) else []
    if not install_order:
        install_order = sorted(packages_by_id)

    registry_copy = registry_dir / "local-package-index.json"
    publication_copy = registry_dir / "publication-metadata.json"
    mirror_copy = mirror_dir / "offline-mirror-index.json"
    restore_copy = receipt_dir / "objc3c-offline-mirror-restore-receipt.json"
    bootstrap_path = install_home / INSTALL_BOOTSTRAP_ENTRYPOINT
    bridge_path = bin_dir / "objc3c-package-bridge.json"
    install_receipt_path = _layout_path(root, output_root, INSTALL_RECEIPT_REL)
    update_receipt_path = _layout_path(root, output_root, PACKAGE_UPDATE_RECEIPT_REL)
    uninstall_receipt_path = _layout_path(root, output_root, PACKAGE_UNINSTALL_RECEIPT_REL)
    install_proof_path = _layout_path(root, output_root, INSTALL_PROOF_MANIFEST_REL)
    install_verification_path = _layout_path(root, output_root, INSTALL_VERIFICATION_REL)
    extraction_plan = install_extraction_plan_payload(
        root=root,
        install_order=install_order,
        registry_copy=registry_copy,
        publication_copy=publication_copy,
        mirror_copy=mirror_copy,
        restore_copy=restore_copy,
        bootstrap_path=bootstrap_path,
        bridge_path=bridge_path,
        install_receipt_path=install_receipt_path,
        update_receipt_path=update_receipt_path,
        uninstall_receipt_path=uninstall_receipt_path,
        install_proof_path=install_proof_path,
        install_verification_path=install_verification_path,
        output_root=output_root,
    )
    extraction_failures = collect_filesystem_extraction_plan_failures(
        root=root,
        extraction_plan=extraction_plan,
    )
    if extraction_failures:
        raise RuntimeError("\n".join(extraction_failures))

    for directory in (packages_dir, registry_dir, mirror_dir, receipt_dir, bin_dir, artifact_root):
        directory.mkdir(parents=True, exist_ok=True)

    installed_packages: list[dict[str, Any]] = []
    local_artifacts: list[dict[str, str]] = []
    for package in [packages_by_id[package_id] for package_id in install_order]:
        package_id = str(package.get("package_id"))
        manifest_ref = package.get("package_manifest", {})
        if not isinstance(manifest_ref, dict):
            continue
        source_manifest_path = root / str(manifest_ref.get("path", ""))
        installed_manifest_path = package_manifest_install_path(root, package_id, output_root=output_root)
        manifest = _copy_json_payload(source_manifest_path, installed_manifest_path)
        installed_record = {
            "package_id": package_id,
            "source": str(package.get("source")),
            "source_digest": str(package.get("source_digest")),
            "source_manifest": repo_rel(source_manifest_path),
            "installed_manifest": repo_rel(installed_manifest_path),
            "manifest_digest": str(manifest.get("manifest_digest")),
            "lock_manifest_digest": str(manifest_ref.get("digest")),
            "trust_signature": str(package.get("trust", {}).get("signature")),
        }
        artifact_path = local_package_artifact_path(root, package_id, output_root=output_root)
        artifact_payload = install_artifact_payload(installed_record)
        write_json_file(artifact_path, artifact_payload)
        installed_record["local_install_artifact"] = repo_rel(artifact_path)
        installed_record["local_install_artifact_digest"] = str(artifact_payload["artifact_digest"])
        installed_packages.append(installed_record)
        local_artifacts.append(
            {
                "package_id": package_id,
                "artifact_path": repo_rel(artifact_path),
                "artifact_digest": str(artifact_payload["artifact_digest"]),
                "installed_manifest": repo_rel(installed_manifest_path),
                "source_manifest": repo_rel(source_manifest_path),
            }
        )

    _copy_json_payload(registry_path, registry_copy)
    _copy_json_payload(publication_path, publication_copy)
    _copy_json_payload(mirror_path, mirror_copy)
    _copy_json_payload(restore_receipt_path, restore_copy)

    bootstrap_path.write_text(
        "\n".join(
            [
                "$env:OBJC3C_PACKAGE_HOME = $PSScriptRoot",
                "$env:OBJC3C_PACKAGE_BRIDGE = \"objc3c\"",
                "Write-Output \"objc3c package environment ready\"",
                "",
            ]
        ),
        encoding="utf-8",
    )
    write_json_file(bridge_path, bridge_payload(contract))
    write_json_file(install_receipt_path, install_receipt_payload(root, output_root=output_root))
    platform_install_receipt = (
        publish_platform_install_receipt(root=root, install_receipt_path=install_receipt_path)
        if output_root.resolve() == root.resolve()
        else {"status": "SKIPPED", "reason": "isolated graph consumer owns its install receipt"}
    )
    write_json_file(
        update_receipt_path,
        package_operation_receipt_payload(
            root=root,
            lock=lock,
            operation="update",
            package_order=install_order,
            output_root=output_root,
        ),
    )
    write_json_file(
        uninstall_receipt_path,
        package_operation_receipt_payload(
            root=root,
            lock=lock,
            operation="uninstall",
            package_order=list(reversed(install_order)),
            output_root=output_root,
        ),
    )
    install_proof = install_proof_payload(
        root=root,
        contract=contract,
        bridge_path=bridge_path,
        registry_copy=registry_copy,
        publication_copy=publication_copy,
        mirror_copy=mirror_copy,
        restore_copy=restore_copy,
        local_artifacts=sorted(local_artifacts, key=lambda entry: entry["package_id"]),
        output_root=output_root,
    )
    write_json_file(install_proof_path, install_proof)

    generated_paths = [
        repo_rel(_layout_path(root, output_root, INSTALL_ROOT_REL)),
        repo_rel(install_home),
        repo_rel(install_receipt_path),
        repo_rel(update_receipt_path),
        repo_rel(uninstall_receipt_path),
        repo_rel(install_proof_path),
        repo_rel(install_verification_path),
        repo_rel(artifact_root),
        repo_rel(bootstrap_path),
        repo_rel(bridge_path),
        repo_rel(registry_copy),
        repo_rel(publication_copy),
        repo_rel(mirror_copy),
        repo_rel(restore_copy),
    ]
    generated_paths.extend(record["installed_manifest"] for record in installed_packages)
    generated_paths.extend(record["local_install_artifact"] for record in installed_packages)
    verification = {
        "contract_id": INSTALL_DISTRIBUTION_CONTRACT_ID,
        "contract": repo_rel(
            root
            / "tests"
            / "tooling"
            / "fixtures"
            / "package_ecosystem"
            / "install_distribution_credibility_contract.json"
        ),
        "install_root": repo_rel(_layout_path(root, output_root, INSTALL_ROOT_REL)),
        "install_home": repo_rel(install_home),
        "install_receipt": repo_rel(install_receipt_path),
        "update_receipt": repo_rel(update_receipt_path),
        "uninstall_receipt": repo_rel(uninstall_receipt_path),
        "install_proof_manifest": repo_rel(install_proof_path),
        "local_package_artifact_root": _layout_rel(root, output_root, INSTALL_LOCAL_ARTIFACT_ROOT_REL),
        "bootstrap_entrypoint": INSTALL_BOOTSTRAP_ENTRYPOINT,
        "package_bridge": INSTALL_PACKAGE_BRIDGE,
        "clean_start": {
            "validation_root": _layout_rel(root, output_root, INSTALL_VALIDATION_ROOT_REL),
            "preexisting_root_removed": preexisting_root_removed,
            "stale_artifacts_allowed": False,
        },
        "source_lock": str(contract.get("generated_outputs", {}).get("lock")),
        "source_mirror": repo_rel(mirror_path),
        "source_registry_index": repo_rel(registry_path),
        "source_publication_metadata": repo_rel(publication_path),
        "source_restore_receipt": repo_rel(restore_receipt_path),
        "installed_registry_index": repo_rel(registry_copy),
        "installed_publication_metadata": repo_rel(publication_copy),
        "installed_mirror_index": repo_rel(mirror_copy),
        "installed_restore_receipt": repo_rel(restore_copy),
        "language_version": lock.get("package_manager", {}).get("language_version"),
        "abi_identity": lock.get("package_manager", {}).get("abi_identity"),
        "trust_key_id": publication.get("trust_key_id"),
        "network_policy": mirror.get("network_policy"),
        "network_resolution_support": publication.get("network_resolution_support"),
        "hosted_registry_support": publication.get("hosted_registry_support"),
        "offline_restore_support": publication.get("offline_restore_support"),
        "package_count": len(package_ids(lock)),
        "dependency_count": len(lock.get("dependencies", []))
        if isinstance(lock.get("dependencies"), list)
        else 0,
        "install_order": install_order,
        "update_plan_order": install_order,
        "uninstall_plan_order": list(reversed(install_order)),
        "manifest_count": len(installed_packages),
        "cache_entry_count": restore_receipt.get("cache_entry_count"),
        "installed_packages": installed_packages,
        "release_manifest_validation": install_proof["release_manifest_validation"],
        "public_actions": bridge_payload(contract)["public_actions"],
        "extraction_plan": extraction_plan,
        "extraction_plan_digest": extraction_plan["plan_digest"],
        "platform_host_evidence": {
            "install_receipt": platform_install_receipt,
        },
        "generated_paths": sorted(generated_paths),
    }
    write_json_file(install_verification_path, verification)
    return verification


def collect_registered_schema_failures(
    payload: dict[str, Any],
    schema_id: str,
    label: str,
    *,
    root: Path | None = None,
    output_root: Path | None = None,
) -> list[str]:
    schema_payload = payload
    if root is not None and output_root is not None and output_root.resolve() != root.resolve():
        # Receipt schemas specify the standalone layout. Validate the same
        # structure after translating only exact, already expected graph paths;
        # the callers also compare the original receipt with the actual layout.
        schema_payload = dict(payload)
        for key, relative in (("install_root", INSTALL_ROOT_REL), ("install_home", INSTALL_HOME_REL),
                              ("receipt_root", PACKAGE_RECEIPT_ROOT_REL)):
            if schema_payload.get(key) == _layout_rel(root, output_root, relative):
                schema_payload[key] = relative
    try:
        validate_registered_schema(schema_payload, schema_id, label=label)
    except Exception as exc:  # schema failures must become package diagnostics
        return [f"{PACKAGE_MANAGER_TAMPER_CODE}: {label} schema validation failed: {exc}"]
    return []


def collect_install_distribution_failures(
    *,
    root: Path,
    contract: dict[str, Any],
    lock: dict[str, Any],
    mirror: dict[str, Any],
    registry: dict[str, Any],
    publication: dict[str, Any],
    restore_receipt: dict[str, Any],
    verification: dict[str, Any],
    output_root: Path | None = None,
) -> list[str]:
    output_root = output_root or root
    failures: list[str] = []
    if verification.get("contract_id") != INSTALL_DISTRIBUTION_CONTRACT_ID:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install distribution contract drifted")
    if verification.get("package_bridge") != INSTALL_PACKAGE_BRIDGE:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install package bridge drifted")
    extraction_plan = verification.get("extraction_plan")
    extraction_failures = collect_extraction_plan_failures(extraction_plan)
    failures.extend(extraction_failures)
    if (
        isinstance(extraction_plan, dict)
        and verification.get("extraction_plan_digest") != extraction_plan.get("plan_digest")
    ):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install extraction plan digest drifted")
    if verification.get("network_policy") != NO_NETWORK_POLICY:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install network policy drifted")
    if verification.get("network_resolution_support") != NETWORK_RESOLUTION_UNSUPPORTED:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install network resolution support drifted")
    if verification.get("hosted_registry_support") != HOSTED_REGISTRY_FAIL_CLOSED:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry support widened")
    if verification.get("offline_restore_support") != OFFLINE_RESTORE_SUPPORT:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: offline restore support drifted")
    if verification.get("language_version") != LOCAL_PACKAGE_LANGUAGE_VERSION:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install language version drifted")
    if verification.get("abi_identity") != LOCAL_PACKAGE_ABI_IDENTITY:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install ABI identity drifted")
    if verification.get("trust_key_id") != LOCAL_PACKAGE_TRUST_KEY_ID:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install trust key drifted")

    lock_ids = package_ids(lock)
    mirror_ids = package_ids(mirror)
    registry_ids = package_ids(registry)
    installed_ids = sorted(
        str(record.get("package_id"))
        for record in verification.get("installed_packages", [])
        if isinstance(record, dict)
    )
    installed_order = [
        str(record.get("package_id"))
        for record in verification.get("installed_packages", [])
        if isinstance(record, dict)
    ]
    expected_install_order = []
    resolution_plan = lock.get("resolution_plan", {})
    if isinstance(resolution_plan, dict):
        raw_install_order = resolution_plan.get("install_order", [])
        if isinstance(raw_install_order, list):
            expected_install_order = [
                str(package_id)
                for package_id in raw_install_order
                if isinstance(package_id, str)
            ]
    minimum_package_count = int(contract.get("minimum_package_count", 0))
    minimum_dependency_count = int(contract.get("minimum_dependency_count", 0))
    if len(lock_ids) < minimum_package_count:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package count below install credibility floor")
    if int(verification.get("dependency_count", 0)) < minimum_dependency_count:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: dependency count below install credibility floor")
    if lock_ids != mirror_ids:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install mirror package ids drifted from lock")
    if lock_ids != registry_ids:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install registry package ids drifted from lock")
    if lock_ids != installed_ids:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed package ids drifted from lock")
    if expected_install_order and verification.get("install_order") != expected_install_order:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install order drifted from package resolution plan")
    if expected_install_order and installed_order != expected_install_order:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed package order drifted from package resolution plan")

    packages_by_id = {
        str(package.get("package_id")): package
        for package in lock.get("packages", [])
        if isinstance(package, dict)
    }
    for record in verification.get("installed_packages", []):
        if not isinstance(record, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed package record is not an object")
            continue
        package_id = str(record.get("package_id"))
        package = packages_by_id.get(package_id, {})
        source_path = root / str(record.get("source", ""))
        source_manifest_path = root / str(record.get("source_manifest", ""))
        installed_manifest_path = root / str(record.get("installed_manifest", ""))
        if not source_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed package source missing for {package_id}")
        elif record.get("source_digest") != file_digest(source_path):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed package source digest drifted for {package_id}")
        if not source_manifest_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: source package manifest missing for {package_id}")
            continue
        if not installed_manifest_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed package manifest missing for {package_id}")
            continue
        source_manifest = load_json(source_manifest_path)
        installed_manifest = load_json(installed_manifest_path)
        expected_digest = package.get("package_manifest", {}).get("digest")
        if installed_manifest != source_manifest:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed manifest payload drifted for {package_id}")
        if record.get("manifest_digest") != expected_digest:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed manifest digest drifted for {package_id}")
        if record.get("lock_manifest_digest") != expected_digest:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: lock manifest digest drifted for {package_id}")
        if package_manifest_digest(source_manifest) != expected_digest:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: recomputed manifest digest drifted for {package_id}")
        if record.get("trust_signature") != package.get("trust", {}).get("signature"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: installed trust signature drifted for {package_id}")

    for raw_package in mirror.get("packages", []):
        if not isinstance(raw_package, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: mirror package entry is not an object")
            continue
        package_id = str(raw_package.get("package_id"))
        cache_path = root / str(raw_package.get("cache_path", ""))
        if not cache_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install mirror cache missing for {package_id}")
            continue
        cache_payload = load_json(cache_path)
        expected_cache = cache_payload_from_mirror_package(raw_package)
        if cache_payload != expected_cache:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install mirror cache payload drifted for {package_id}")
        if stable_digest(cache_payload) != raw_package.get("cache_digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install mirror cache digest drifted for {package_id}")

    if restore_receipt.get("network_policy") != NO_NETWORK_POLICY:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: restore receipt network policy drifted")
    if restore_receipt.get("cache_entry_count") != len(lock_ids):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: restore receipt cache entry count drifted")
    if publication.get("hosted_registry_support") != HOSTED_REGISTRY_FAIL_CLOSED:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: publication hosted registry support widened")
    if registry.get("network_resolution_support") != "unsupported-fail-closed":
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: registry network resolution support widened")

    receipt_path = root / str(verification.get("install_receipt", ""))
    if not receipt_path.is_file():
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing install receipt")
    else:
        receipt = load_json(receipt_path)
        failures.extend(
            collect_registered_schema_failures(
                receipt,
                INSTALL_RECEIPT_SCHEMA_ID,
                "package install distribution receipt",
                root=root,
                output_root=output_root,
            )
        )
        expected_receipt = install_receipt_payload(root, output_root=output_root)
        for field_name in contract.get("required_install_receipt_fields", []):
            if field_name not in receipt:
                failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install receipt missing {field_name}")
        if receipt != expected_receipt:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install receipt payload drifted")

    update_receipt_path = root / str(verification.get("update_receipt", ""))
    uninstall_receipt_path = root / str(verification.get("uninstall_receipt", ""))
    failures.extend(
        collect_package_operation_receipt_failures(
            root=root,
            lock=lock,
            verification=verification,
            receipt_path=update_receipt_path,
            operation="update",
            expected_order=expected_install_order,
            output_root=output_root,
        )
    )
    failures.extend(
        collect_package_operation_receipt_failures(
            root=root,
            lock=lock,
            verification=verification,
            receipt_path=uninstall_receipt_path,
            operation="uninstall",
            expected_order=list(reversed(expected_install_order)),
            output_root=output_root,
        )
    )

    proof_path = root / str(verification.get("install_proof_manifest", ""))
    if not proof_path.is_file():
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing from-nothing install proof manifest")
    else:
        proof = load_json(proof_path)
        failures.extend(
            collect_install_proof_failures(
                root=root,
                proof=proof,
                verification=verification,
                output_root=output_root,
            )
        )

    for raw_path in verification.get("generated_paths", []):
        if not isinstance(raw_path, str):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: generated install path is not a string")
            continue
        validation_prefix = _layout_rel(root, output_root, INSTALL_VALIDATION_ROOT_REL) + "/"
        if not raw_path.startswith(validation_prefix):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install generated path escaped validation root: {raw_path}")
        if not (root / raw_path).exists():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: generated install path missing: {raw_path}")

    required_actions = [
        str(action)
        for action in contract.get("required_public_actions", [])
        if isinstance(action, str)
    ]
    public_actions = set(
        str(action)
        for action in verification.get("public_actions", [])
        if isinstance(action, str)
    )
    missing_actions = sorted(set(required_actions) - public_actions)
    if missing_actions:
        failures.append(
            f"{PACKAGE_MANAGER_TAMPER_CODE}: install bridge missing actions: {', '.join(missing_actions)}"
        )
    failures.extend(collect_lock_model_failures(lock, root=root))
    return failures


def collect_package_operation_receipt_failures(
    *,
    root: Path,
    lock: dict[str, Any],
    verification: dict[str, Any],
    receipt_path: Path,
    operation: str,
    expected_order: list[str],
    output_root: Path | None = None,
) -> list[str]:
    failures: list[str] = []
    if not receipt_path.is_file():
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing {operation} receipt")
        return failures

    receipt = load_json(receipt_path)
    failures.extend(
        collect_registered_schema_failures(
            receipt,
            PACKAGE_OPERATION_RECEIPT_SCHEMA_ID,
            f"package install distribution {operation} receipt",
            root=root,
            output_root=output_root,
        )
    )
    expected_receipt = package_operation_receipt_payload(
        root=root,
        lock=lock,
        operation=operation,
        package_order=expected_order,
        output_root=output_root,
    )
    if receipt != expected_receipt:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt payload drifted")
    if receipt.get("contract_id") != PACKAGE_OPERATION_RECEIPT_CONTRACT_ID:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt contract drifted")
    if receipt.get("network_policy") != NO_NETWORK_POLICY:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt network policy drifted")
    if receipt.get("hosted_registry_support") != HOSTED_REGISTRY_FAIL_CLOSED:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt hosted registry widened")
    if receipt.get("package_bridge") != INSTALL_PACKAGE_BRIDGE:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt package bridge drifted")
    if receipt.get("language_version") != LOCAL_PACKAGE_LANGUAGE_VERSION:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt language drifted")
    if receipt.get("abi_identity") != LOCAL_PACKAGE_ABI_IDENTITY:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt ABI drifted")
    if receipt.get("selection_policy") != "exact-locked-version-only":
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt selection policy drifted")
    if receipt.get("package_order") != expected_order:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt package order drifted")
    if receipt.get("package_count") != len(expected_order):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt package count drifted")
    package_digests = receipt.get("package_digests", [])
    if not isinstance(package_digests, list):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt package digests missing")
        return failures
    digest_ids = [
        str(entry.get("package_id"))
        for entry in package_digests
        if isinstance(entry, dict)
    ]
    if digest_ids != expected_order:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: {operation} receipt digest order drifted")
    if operation == "update" and verification.get("update_plan_order") != expected_order:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: update plan order drifted")
    if operation == "uninstall" and verification.get("uninstall_plan_order") != expected_order:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: uninstall plan order drifted")
    return failures


def collect_install_proof_failures(
    *,
    root: Path,
    proof: dict[str, Any],
    verification: dict[str, Any],
    output_root: Path | None = None,
) -> list[str]:
    output_root = output_root or root
    failures: list[str] = []
    if proof.get("contract_id") != INSTALL_PROOF_CONTRACT_ID:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof contract drifted")
    if proof.get("package_bridge") != INSTALL_PACKAGE_BRIDGE:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof package bridge drifted")
    if proof.get("network_policy") != NO_NETWORK_POLICY:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof network policy drifted")
    if proof.get("hosted_registry_support") != HOSTED_REGISTRY_FAIL_CLOSED:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof hosted registry support widened")
    artifact_prefix = _layout_rel(root, output_root, INSTALL_LOCAL_ARTIFACT_ROOT_REL)
    if proof.get("local_package_artifact_root") != artifact_prefix:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact root drifted")

    release_validation = proof.get("release_manifest_validation")
    if not isinstance(release_validation, dict):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof missing release manifest validation")
        release_validation = {}
    if release_validation.get("generated_report_inputs_allowed") is not False:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: generated reports allowed as release inputs")
    if release_validation.get("generated_report_inputs") != []:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: generated report input list is not empty")
    forbidden_prefixes = [
        str(prefix)
        for prefix in release_validation.get("forbidden_input_prefixes", [])
        if isinstance(prefix, str)
    ]
    if "tmp/reports/" not in forbidden_prefixes:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: generated report exclusion prefix drifted")
    release_input_paths = release_validation.get("release_manifest_input_paths")
    if not isinstance(release_input_paths, list) or not release_input_paths:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof release input paths missing")
        release_input_paths = []
    for raw_path in release_input_paths:
        if not isinstance(raw_path, str) or not raw_path:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof release input path is invalid")
            continue
        if any(raw_path.startswith(prefix) for prefix in forbidden_prefixes):
            failures.append(
                f"{PACKAGE_MANAGER_TAMPER_CODE}: generated report used as release input: {raw_path}"
            )
        if not (root / raw_path).exists():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof release input missing: {raw_path}")

    installed_by_id = {
        str(record.get("package_id")): record
        for record in verification.get("installed_packages", [])
        if isinstance(record, dict)
    }
    proof_artifacts = proof.get("local_package_artifacts")
    if not isinstance(proof_artifacts, list) or not proof_artifacts:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof local artifacts missing")
        return failures
    artifact_ids = sorted(
        str(artifact.get("package_id"))
        for artifact in proof_artifacts
        if isinstance(artifact, dict)
    )
    if artifact_ids != sorted(installed_by_id):
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact ids drifted")
    for artifact in proof_artifacts:
        if not isinstance(artifact, dict):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact entry is not an object")
            continue
        package_id = str(artifact.get("package_id"))
        installed_record = installed_by_id.get(package_id, {})
        artifact_path = str(artifact.get("artifact_path", ""))
        if not artifact_path.startswith(artifact_prefix + "/"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact escaped root for {package_id}")
            continue
        payload_path = root / artifact_path
        if not payload_path.is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact missing for {package_id}")
            continue
        payload = load_json(payload_path)
        if payload.get("contract_id") != "objc3c.package_ecosystem.local_install_artifact.v1":
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact contract drifted for {package_id}")
        if payload.get("generated_report_source") is not False:
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact used report source for {package_id}")
        if payload.get("artifact_digest") != artifact.get("artifact_digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact digest record drifted for {package_id}")
        if _artifact_digest(payload) != artifact.get("artifact_digest"):
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact digest drifted for {package_id}")
        for field_name in (
            "source_manifest",
            "installed_manifest",
            "manifest_digest",
            "lock_manifest_digest",
            "trust_signature",
        ):
            if payload.get(field_name) != installed_record.get(field_name):
                failures.append(
                    f"{PACKAGE_MANAGER_TAMPER_CODE}: install proof artifact {field_name} drifted for {package_id}"
                )
    return failures


__all__ = [
    "HOSTED_REGISTRY_FAIL_CLOSED",
    "INSTALL_DISTRIBUTION_ACTION",
    "INSTALL_DISTRIBUTION_CONTRACT_ID",
    "INSTALL_DISTRIBUTION_SUMMARY_CONTRACT_ID",
    "INSTALL_HOME_REL",
    "INSTALL_LOCAL_ARTIFACT_ROOT_REL",
    "INSTALL_PACKAGE_BRIDGE",
    "INSTALL_PROOF_CONTRACT_ID",
    "INSTALL_PROOF_MANIFEST_REL",
    "INSTALL_RECEIPT_REL",
    "INSTALL_ROOT_REL",
    "INSTALL_VALIDATION_ROOT_REL",
    "INSTALL_VERIFICATION_REL",
    "NO_NETWORK_POLICY",
    "PACKAGE_OPERATION_RECEIPT_CONTRACT_ID",
    "PACKAGE_RECEIPT_ROOT_REL",
    "PACKAGE_UNINSTALL_RECEIPT_REL",
    "PACKAGE_UPDATE_RECEIPT_REL",
    "PLATFORM_CLEAN_INSTALL_RECEIPT_NAME",
    "collect_install_distribution_failures",
    "collect_install_proof_failures",
    "collect_package_operation_receipt_failures",
    "install_extraction_plan_payload",
    "install_receipt_payload",
    "materialize_clean_distribution_install",
    "package_operation_receipt_payload",
    "package_ids",
    "package_manifest_digest",
    "publish_platform_install_receipt",
    "reset_clean_install_root",
]
