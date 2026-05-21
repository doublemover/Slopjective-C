#!/usr/bin/env python3
"""Validate package mirror, local registry, and publication metadata reproducibility."""

from __future__ import annotations

import json
import hashlib
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names
from objc3c_tooling.subprocesses import python_script_command
from objc3c_package_manager.model import (
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    LOCAL_PACKAGE_TRUST_KEY_ID,
    PACKAGE_MANAGER_TAMPER_CODE,
    collect_lock_model_failures,
)
from objc3c_package_manager.registry import (
    LOCAL_REGISTRY_CONTRACT_ID,
    LOCAL_REGISTRY_SCHEMA_KEY,
    collect_registry_index_failures,
)
from objc3c_shared.schema_registry import validate_registered_schema
from package_ecosystem_contracts import (
    PACKAGE_LOADER_INTEROP_TAMPER_CODE,
    collect_normalized_package_loader_interop_metadata_failures,
    require_package_ecosystem_blocker_metadata,
    require_package_ecosystem_owner_policy,
)


CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "registry_mirror_reproducibility_contract.json"
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
MIRROR_SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-mirror-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "registry-mirror-reproducibility-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def package_ids(payload: dict[str, Any]) -> list[str]:
    packages = payload.get("packages", [])
    if not isinstance(packages, list):
        return []
    return sorted(str(package.get("package_id")) for package in packages if isinstance(package, dict))


def package_interop_metadata(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    packages = payload.get("packages", [])
    if not isinstance(packages, list):
        return {}
    metadata_by_package: dict[str, dict[str, Any]] = {}
    for package in packages:
        if not isinstance(package, dict):
            continue
        interop_metadata = package.get("interop_loader_metadata")
        if isinstance(interop_metadata, dict):
            metadata_by_package[str(package.get("package_id"))] = interop_metadata
    return dict(sorted(metadata_by_package.items()))


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


def expected_cache_payload(mirror_package: dict[str, Any]) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "contract_id": "objc3c.package_ecosystem.offline_mirror.cache_entry.v1",
        "package_id": str(mirror_package.get("package_id")),
        "source": str(mirror_package.get("source")),
        "source_digest": str(mirror_package.get("source_digest")),
        "package_manifest": mirror_package.get("package_manifest"),
        "package_version": str(mirror_package.get("package_version")),
        "language_version": str(mirror_package.get("language_version")),
        "abi_identity": str(mirror_package.get("abi_identity")),
        "trust": mirror_package.get("trust"),
        "network_policy": "no-network-during-validation",
        "restore_failure_mode": "reject-package-metadata-digest-mismatch",
    }
    interop_metadata = mirror_package.get("interop_loader_metadata")
    if isinstance(interop_metadata, dict):
        payload["interop_loader_metadata"] = interop_metadata
        payload["interop_loader_metadata_digest"] = str(interop_metadata.get("digest", ""))
    return payload


def collect_offline_mirror_cache_failures(
    mirror: dict[str, Any],
    *,
    root: Path = ROOT,
    cache_root: Path = CACHE_ROOT,
) -> list[str]:
    packages = mirror.get("packages", [])
    if not isinstance(packages, list):
        return [f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror packages field is not a list"]

    failures: list[str] = []
    expected_paths: set[str] = set()
    for raw_package in packages:
        if not isinstance(raw_package, dict):
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror package entry is not an object")
            continue
        package_id = str(raw_package.get("package_id"))
        cache_path = str(raw_package.get("cache_path", ""))
        if not cache_path.startswith("tmp/artifacts/package-ecosystem/mirrors/cache/"):
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: cache path escaped mirror cache root for {package_id}")
            continue
        expected_paths.add(cache_path)
        absolute_cache_path = root / cache_path
        if not absolute_cache_path.is_file():
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: missing offline mirror cache entry for {package_id}")
            continue
        cache_payload = load_json(absolute_cache_path)
        expected_payload = expected_cache_payload(raw_package)
        expected_digest = str(raw_package.get("cache_digest", ""))
        source_path = root / str(raw_package.get("source", ""))
        if not source_path.is_file():
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: missing package source for {package_id}")
        elif raw_package.get("source_digest") != file_digest(source_path):
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: source digest mismatch for {package_id}")
        if stable_digest(expected_payload) != expected_digest:
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror cache digest metadata drifted for {package_id}")
        if cache_payload != expected_payload:
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror cache payload mismatch for {package_id}")
        elif stable_digest(cache_payload) != expected_digest:
            failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror cache digest mismatch for {package_id}")

    extra_paths = sorted(
        path.relative_to(root).as_posix()
        for path in cache_root.rglob("*.json")
        if path.relative_to(root).as_posix() not in expected_paths
    ) if cache_root.is_dir() else []
    for extra_path in extra_paths:
        failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: extra offline mirror cache entry {extra_path}")
    return failures


def collect_interop_loader_metadata_failures(
    lock: dict[str, Any],
    mirror: dict[str, Any],
    registry: dict[str, Any],
    publication: dict[str, Any],
    *,
    root: Path = ROOT,
) -> list[str]:
    lock_metadata = package_interop_metadata(lock)
    if not lock_metadata:
        return []

    failures: list[str] = []
    mirror_metadata = package_interop_metadata(mirror)
    registry_metadata = package_interop_metadata(registry)
    lock_ids = set(lock_metadata)
    if set(mirror_metadata) != lock_ids:
        failures.append(
            f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror interop metadata package ids drifted from lock"
        )
    if set(registry_metadata) != lock_ids:
        failures.append(
            f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: registry interop metadata package ids drifted from lock"
        )

    for package_id in sorted(lock_ids):
        expected_metadata = lock_metadata[package_id]
        for failure in collect_normalized_package_loader_interop_metadata_failures(
            expected_metadata,
            root=root,
        ):
            failures.append(
                f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: lock interop metadata {failure} for {package_id}"
            )
        expected_digest = expected_metadata.get("digest")
        mirror_payload = mirror_metadata.get(package_id, {})
        registry_payload = registry_metadata.get(package_id, {})
        mirror_digest = mirror_payload.get("digest")
        registry_digest = registry_payload.get("digest")
        if mirror_digest != expected_digest:
            failures.append(
                f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror interop metadata digest mismatch for {package_id}"
            )
        elif mirror_payload != expected_metadata:
            failures.append(
                f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror interop metadata payload mismatch for {package_id}"
            )
        if registry_digest != expected_digest:
            failures.append(
                f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: registry interop metadata digest mismatch for {package_id}"
            )
        elif registry_payload != expected_metadata:
            failures.append(
                f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: registry interop metadata payload mismatch for {package_id}"
            )

    mirror_policy = mirror.get("integrity_policy", {})
    if not isinstance(mirror_policy, dict) or mirror_policy.get("tamper_rejection_diagnostic") != PACKAGE_LOADER_INTEROP_TAMPER_CODE:
        failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: mirror integrity policy diagnostic drifted")
    if publication.get("tamper_rejection_diagnostic") != PACKAGE_LOADER_INTEROP_TAMPER_CODE:
        failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: publication tamper diagnostic drifted")
    if publication.get("interop_loader_support") != "local-mixed-image-metadata-digest-checked":
        failures.append(f"{PACKAGE_LOADER_INTEROP_TAMPER_CODE}: publication interop loader support claim drifted")
    return failures


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    owner_policy = require_package_ecosystem_owner_policy(contract, surface_name="package ecosystem registry mirror reproducibility")
    blocker_metadata = require_package_ecosystem_blocker_metadata(
        contract,
        surface_name="package ecosystem registry mirror reproducibility",
        required_blockers=(
            "hosted registry claim did not fail closed",
            "offline mirror metadata digest mismatch did not fail closed",
            "offline mirror metadata payload mismatch did not fail closed",
        ),
    )
    package = load_json(ROOT / "package.json")
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    result = subprocess.run(
        python_script_command("scripts/build_objc3c_package_mirror.py"),
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)

    failures: list[str] = []
    expect(result.returncode == 0, "package mirror generator failed", failures)
    for path in (LOCK_PATH, MIRROR_PATH, REGISTRY_PATH, PUBLICATION_PATH, RESTORE_RECEIPT_PATH, MIRROR_SUMMARY_PATH):
        expect(path.is_file(), f"missing generated package ecosystem artifact {repo_rel(path)}", failures)

    lock = load_json(LOCK_PATH) if LOCK_PATH.is_file() else {}
    mirror = load_json(MIRROR_PATH) if MIRROR_PATH.is_file() else {}
    registry = load_json(REGISTRY_PATH) if REGISTRY_PATH.is_file() else {}
    publication = load_json(PUBLICATION_PATH) if PUBLICATION_PATH.is_file() else {}
    restore_receipt = load_json(RESTORE_RECEIPT_PATH) if RESTORE_RECEIPT_PATH.is_file() else {}
    mirror_summary = load_json(MIRROR_SUMMARY_PATH) if MIRROR_SUMMARY_PATH.is_file() else {}
    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in contract["required_actions"]]
    missing_actions = [name for name in required_actions if name not in set(public_workflow_action_names())]

    lock_ids = package_ids(lock)
    mirror_ids = package_ids(mirror)
    registry_ids = package_ids(registry)
    expect(lock_ids == mirror_ids, "mirror package ids drifted from lock package ids", failures)
    expect(lock_ids == registry_ids, "registry package ids drifted from lock package ids", failures)
    expect(mirror.get("network_policy") == "no-network-during-validation", "mirror network policy drifted", failures)
    expect(publication.get("hosted_registry_support") == "unsupported-fail-closed-if-claimed", "hosted registry support claim drifted", failures)
    expect(publication.get("network_resolution_support") == "unsupported", "network resolution support claim drifted", failures)
    expect(publication.get("offline_restore_support") == "local-cache-digest-checked", "offline restore support claim drifted", failures)
    expect(restore_receipt.get("network_policy") == "no-network-during-validation", "offline restore receipt network policy drifted", failures)
    expect(restore_receipt.get("cache_entry_count") == len(mirror_ids), "offline restore cache entry count drifted", failures)
    expect(mirror_summary.get("status") == "PASS", "mirror summary did not report PASS", failures)
    expect(mirror_summary.get("package_count") == len(lock_ids), "mirror summary package count drifted", failures)
    expect(mirror_summary.get("cache_entry_count") == len(lock_ids), "mirror summary cache entry count drifted", failures)
    expect(
        mirror_summary.get("interop_loader_metadata_objcxx_bridge_surface_count", 0) > 0
        and mirror_summary.get("interop_loader_metadata_swift_bridge_surface_count", 0) > 0,
        "mirror summary ObjC++/Swift interop bridge surface counts drifted",
        failures,
    )
    expect(package_bridge_exists, f"registry/mirror workflow missing package bridge {package_bridge}", failures)
    expect(not missing_actions, "registry/mirror workflow missing required actions", failures)
    interop_failures = collect_interop_loader_metadata_failures(lock, mirror, registry, publication)
    failures.extend(interop_failures)
    cache_failures = collect_offline_mirror_cache_failures(mirror)
    failures.extend(cache_failures)
    failures.extend(collect_lock_model_failures(lock, root=ROOT))
    registry_failures = collect_registry_index_failures(registry, lock, root=ROOT)
    failures.extend(registry_failures)
    try:
        validate_registered_schema(registry, LOCAL_REGISTRY_SCHEMA_KEY, label=repo_rel(REGISTRY_PATH))
    except (KeyError, RuntimeError) as exc:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: local registry schema validation failed: {exc}")
    expect(registry.get("contract_id") == LOCAL_REGISTRY_CONTRACT_ID, "registry contract id drifted", failures)
    expect(registry.get("network_resolution_support") == "unsupported-fail-closed", "registry network resolution must fail closed", failures)
    expect(registry.get("language_version") == LOCAL_PACKAGE_LANGUAGE_VERSION, "registry language version drifted", failures)
    expect(registry.get("abi_identity") == LOCAL_PACKAGE_ABI_IDENTITY, "registry ABI identity drifted", failures)
    expect(publication.get("package_manager_tamper_diagnostic") == PACKAGE_MANAGER_TAMPER_CODE, "publication package manager diagnostic drifted", failures)
    expect(publication.get("trust_key_id") == LOCAL_PACKAGE_TRUST_KEY_ID, "publication trust key drifted", failures)

    payload = {
        "contract_id": "objc3c.package_ecosystem.registry_mirror_reproducibility.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(CONTRACT_PATH),
        "lock_path": repo_rel(LOCK_PATH),
        "mirror_index": repo_rel(MIRROR_PATH),
        "local_registry_index": repo_rel(REGISTRY_PATH),
        "local_registry_contract_id": registry.get("contract_id"),
        "publication_metadata": repo_rel(PUBLICATION_PATH),
        "restore_receipt": repo_rel(RESTORE_RECEIPT_PATH),
        "mirror_summary": repo_rel(MIRROR_SUMMARY_PATH),
        "package_count": len(lock_ids),
        "cache_entry_count": restore_receipt.get("cache_entry_count"),
        "interop_loader_metadata_package_count": len(package_interop_metadata(lock)),
        "interop_loader_metadata_bridge_surface_count": mirror_summary.get("interop_loader_metadata_bridge_surface_count"),
        "interop_loader_metadata_objcxx_bridge_surface_count": mirror_summary.get(
            "interop_loader_metadata_objcxx_bridge_surface_count"
        ),
        "interop_loader_metadata_swift_bridge_surface_count": mirror_summary.get(
            "interop_loader_metadata_swift_bridge_surface_count"
        ),
        "network_policy": mirror.get("network_policy"),
        "hosted_registry_support": publication.get("hosted_registry_support"),
        "offline_restore_support": publication.get("offline_restore_support"),
        "interop_loader_support": publication.get("interop_loader_support"),
        "tamper_rejection_diagnostic": publication.get("tamper_rejection_diagnostic"),
        "package_manager_tamper_diagnostic": publication.get("package_manager_tamper_diagnostic"),
        "language_version": publication.get("language_version"),
        "abi_identity": publication.get("abi_identity"),
        "trust_key_id": publication.get("trust_key_id"),
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "package_bridge": package_bridge,
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "required_actions": required_actions,
        "missing_actions": missing_actions,
        "interop_integrity_failures": interop_failures,
        "offline_mirror_cache_failures": cache_failures,
        "local_registry_failures": registry_failures,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-registry-mirror-reproducibility: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-registry-mirror-reproducibility: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
