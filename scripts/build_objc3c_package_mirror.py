#!/usr/bin/env python3
"""Generate offline mirror and local registry metadata from the package lock."""

from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_package_manager.model import cache_payload_from_mirror_package
from objc3c_package_manager.registry import local_registry_payload
from package_ecosystem_contracts import PACKAGE_LOADER_INTEROP_TAMPER_CODE


ROOT = Path(__file__).resolve().parents[1]
LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
MIRROR_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "offline-mirror-index.json"
CACHE_ROOT = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "cache"
REGISTRY_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "local-package-index.json"
PUBLICATION_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "publication-metadata.json"
RESTORE_RECEIPT_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "package-ecosystem"
    / "offline-install"
    / "objc3c-offline-mirror-restore-receipt.json"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-mirror-summary.json"




def ensure_lock() -> None:
    result = subprocess.run(
        python_script_command("scripts/build_objc3c_package_lock.py"),
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        raise RuntimeError("package lock generation failed")


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


def source_digest(raw_path: str) -> str:
    path = ROOT / raw_path
    return "sha256:" + hashlib.sha256(path.read_bytes()).hexdigest()


def cache_path_for_package(package_id: str) -> Path:
    namespace, _, name = package_id.partition(":")
    if not namespace or not name:
        raise RuntimeError(f"package id is not namespace-qualified: {package_id}")
    return CACHE_ROOT / namespace / f"{name}.json"


def mirror_package_payload(package: dict[str, Any]) -> dict[str, Any]:
    package_id = str(package["package_id"])
    source = str(package["source"])
    cache_path = cache_path_for_package(package_id)
    payload: dict[str, Any] = {
        "package_id": package_id,
        "source": source,
        "source_digest": source_digest(source),
        "source_kind": str(package["source_kind"]),
        "package_version": str(package["package_version"]),
        "language_version": str(package["language_version"]),
        "abi_identity": str(package["abi_identity"]),
        "package_manifest": package["package_manifest"],
        "trust": package["trust"],
        "cache_path": repo_rel(cache_path),
    }
    interop_metadata = package.get("interop_loader_metadata")
    if isinstance(interop_metadata, dict):
        payload["interop_loader_metadata"] = interop_metadata
    payload["cache_digest"] = stable_digest(cache_payload_from_mirror_package(payload))
    return payload


def write_cache_entries(mirror_packages: list[dict[str, Any]]) -> list[str]:
    if CACHE_ROOT.is_dir():
        shutil.rmtree(CACHE_ROOT)
    written_paths: list[str] = []
    for package in mirror_packages:
        cache_path = ROOT / str(package["cache_path"])
        cache_payload = cache_payload_from_mirror_package(package)
        expected_digest = str(package["cache_digest"])
        actual_digest = stable_digest(cache_payload)
        if actual_digest != expected_digest:
            raise RuntimeError(
                f"offline mirror cache digest drifted for {package['package_id']}: "
                f"{actual_digest} != {expected_digest}"
            )
        write_json_file(cache_path, cache_payload)
        written_paths.append(repo_rel(cache_path))
    return sorted(written_paths)


def main() -> int:
    ensure_lock()
    lock = load_json(LOCK_PATH)
    build_lock_command = public_workflow_command("build-package-lock")
    mirror_check_command = public_workflow_command("validate-package-mirror")
    packages = lock.get("packages", [])
    if not isinstance(packages, list):
        raise RuntimeError("lock packages field drifted from a list")

    mirror_packages = [mirror_package_payload(package) for package in packages if isinstance(package, dict)]
    cache_paths = write_cache_entries(mirror_packages)
    interop_loader_metadata = lock.get("interop_loader_metadata")
    interop_package_count = sum(
        1
        for package in packages
        if isinstance(package, dict) and isinstance(package.get("interop_loader_metadata"), dict)
    )
    mirror = {
        "contract_id": "objc3c.package_ecosystem.offline_mirror.v1",
        "mirror_version": 1,
        "source_lock": repo_rel(LOCK_PATH),
        "packages": mirror_packages,
        "network_policy": "no-network-during-validation",
        "interop_loader_metadata": interop_loader_metadata if isinstance(interop_loader_metadata, dict) else {},
        "integrity_policy": {
            "metadata_digest_required": True,
            "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
            "offline_restore_failure_mode": "reject-package-metadata-digest-mismatch",
        },
        "replay": {
            "commands": [
                build_lock_command,
                mirror_check_command,
            ]
        },
    }
    restore_receipt = {
        "contract_id": "objc3c.package_ecosystem.offline_mirror.restore_receipt.v1",
        "source_lock": repo_rel(LOCK_PATH),
        "source_mirror": repo_rel(MIRROR_PATH),
        "cache_root": repo_rel(CACHE_ROOT),
        "cache_paths": cache_paths,
        "package_count": len(mirror_packages),
        "cache_entry_count": len(cache_paths),
        "network_policy": "no-network-during-validation",
        "package_bridge": "objc3c",
        "install_command": mirror_check_command,
        "language_version": lock.get("package_manager", {}).get("language_version"),
        "abi_identity": lock.get("package_manager", {}).get("abi_identity"),
        "trust_key_id": "objc3c-local-package-key-v1",
        "restore_failure_mode": "reject-package-metadata-digest-mismatch",
        "restore_timestamp_policy": "omitted-for-deterministic-replay",
    }
    registry = local_registry_payload(
        lock,
        source_mirror=repo_rel(MIRROR_PATH),
        source_restore_receipt=repo_rel(RESTORE_RECEIPT_PATH),
    )
    publication = {
        "contract_id": "objc3c.package_ecosystem.publication_metadata.v1",
        "source_lock": repo_rel(LOCK_PATH),
        "source_mirror": repo_rel(MIRROR_PATH),
        "source_registry_index": repo_rel(REGISTRY_PATH),
        "source_restore_receipt": repo_rel(RESTORE_RECEIPT_PATH),
        "publication_state": "generated-local-metadata",
        "hosted_registry_support": "unsupported-fail-closed-if-claimed",
        "network_resolution_support": "unsupported",
        "offline_restore_support": "local-cache-digest-checked",
        "interop_loader_support": "local-mixed-image-metadata-digest-checked",
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        "package_manager_tamper_diagnostic": "O3PKG8055",
        "language_version": lock.get("package_manager", {}).get("language_version"),
        "abi_identity": lock.get("package_manager", {}).get("abi_identity"),
        "trust_key_id": "objc3c-local-package-key-v1",
        "package_count": len(packages),
        "cache_entry_count": len(cache_paths),
        "interop_loader_metadata_package_count": interop_package_count,
    }

    for path, payload in (
        (MIRROR_PATH, mirror),
        (RESTORE_RECEIPT_PATH, restore_receipt),
        (REGISTRY_PATH, registry),
        (PUBLICATION_PATH, publication),
    ):
        path.parent.mkdir(parents=True, exist_ok=True)
        write_json_file(path, payload)

    summary = {
        "contract_id": "objc3c.package_ecosystem.package_mirror.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "lock_path": repo_rel(LOCK_PATH),
        "mirror_index": repo_rel(MIRROR_PATH),
        "cache_root": repo_rel(CACHE_ROOT),
        "cache_paths": cache_paths,
        "local_registry_index": repo_rel(REGISTRY_PATH),
        "local_registry_contract_id": registry["contract_id"],
        "local_registry_dependency_edge_count": len(registry["dependency_edges"]),
        "publication_metadata": repo_rel(PUBLICATION_PATH),
        "restore_receipt": repo_rel(RESTORE_RECEIPT_PATH),
        "package_count": len(packages),
        "mirror_package_count": len(mirror_packages),
        "cache_entry_count": len(cache_paths),
        "offline_restore_support": publication["offline_restore_support"],
        "interop_loader_metadata_package_count": interop_package_count,
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        "network_policy": mirror["network_policy"],
        "hosted_registry_support": publication["hosted_registry_support"],
        "interop_loader_support": publication["interop_loader_support"],
        "interop_loader_metadata_bridge_surface_count": (
            interop_loader_metadata.get("bridge_surface_count")
            if isinstance(interop_loader_metadata, dict)
            else 0
        ),
        "interop_loader_metadata_objcxx_bridge_surface_count": (
            interop_loader_metadata.get("objcxx_bridge_surface_count")
            if isinstance(interop_loader_metadata, dict)
            else 0
        ),
        "interop_loader_metadata_swift_bridge_surface_count": (
            interop_loader_metadata.get("swift_bridge_surface_count")
            if isinstance(interop_loader_metadata, dict)
            else 0
        ),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"mirror_path: {repo_rel(MIRROR_PATH)}")
    print(f"cache_root: {repo_rel(CACHE_ROOT)}")
    print(f"registry_path: {repo_rel(REGISTRY_PATH)}")
    print(f"publication_path: {repo_rel(PUBLICATION_PATH)}")
    print(f"restore_receipt: {repo_rel(RESTORE_RECEIPT_PATH)}")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-package-mirror: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
