#!/usr/bin/env python3
"""Generate offline mirror and local registry metadata from the package lock."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from package_ecosystem_contracts import PACKAGE_LOADER_INTEROP_TAMPER_CODE


ROOT = Path(__file__).resolve().parents[1]
LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
MIRROR_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "offline-mirror-index.json"
REGISTRY_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "local-package-index.json"
PUBLICATION_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "publication-metadata.json"
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


def mirror_package_payload(package: dict[str, Any]) -> dict[str, Any]:
    package_id = str(package["package_id"])
    payload: dict[str, Any] = {
        "package_id": package_id,
        "source": str(package["source"]),
        "cache_path": f"tmp/artifacts/package-ecosystem/mirrors/cache/{package_id.replace(':', '/')}.json",
    }
    interop_metadata = package.get("interop_loader_metadata")
    if isinstance(interop_metadata, dict):
        payload["interop_loader_metadata"] = interop_metadata
    return payload


def registry_package_payload(package: dict[str, Any]) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "package_id": str(package["package_id"]),
        "source": str(package["source"]),
        "provenance_id": str(package["provenance_id"]),
    }
    interop_metadata = package.get("interop_loader_metadata")
    if isinstance(interop_metadata, dict):
        payload["interop_loader_metadata"] = interop_metadata
    return payload


def main() -> int:
    ensure_lock()
    lock = load_json(LOCK_PATH)
    build_lock_command = public_workflow_command("build-package-lock")
    mirror_check_command = public_workflow_command("validate-package-mirror")
    packages = lock.get("packages", [])
    if not isinstance(packages, list):
        raise RuntimeError("lock packages field drifted from a list")

    mirror_packages = [mirror_package_payload(package) for package in packages if isinstance(package, dict)]
    registry_packages = [registry_package_payload(package) for package in packages if isinstance(package, dict)]
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
    registry = {
        "contract_id": "objc3c.package_ecosystem.local_registry_index.v1",
        "source_lock": repo_rel(LOCK_PATH),
        "source_mirror": repo_rel(MIRROR_PATH),
        "support_state": "local-generated-index",
        "hosted_registry_state": "deferred",
        "interop_loader_metadata": interop_loader_metadata if isinstance(interop_loader_metadata, dict) else {},
        "packages": registry_packages,
    }
    publication = {
        "contract_id": "objc3c.package_ecosystem.publication_metadata.v1",
        "source_lock": repo_rel(LOCK_PATH),
        "source_mirror": repo_rel(MIRROR_PATH),
        "source_registry_index": repo_rel(REGISTRY_PATH),
        "publication_state": "generated-local-metadata",
        "hosted_registry_support": "unsupported-fail-closed-if-claimed",
        "network_resolution_support": "unsupported",
        "interop_loader_support": "local-mixed-image-metadata-digest-checked",
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        "package_count": len(packages),
        "interop_loader_metadata_package_count": interop_package_count,
    }

    for path, payload in ((MIRROR_PATH, mirror), (REGISTRY_PATH, registry), (PUBLICATION_PATH, publication)):
        path.parent.mkdir(parents=True, exist_ok=True)
        write_json_file(path, payload)

    summary = {
        "contract_id": "objc3c.package_ecosystem.package_mirror.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "lock_path": repo_rel(LOCK_PATH),
        "mirror_index": repo_rel(MIRROR_PATH),
        "local_registry_index": repo_rel(REGISTRY_PATH),
        "publication_metadata": repo_rel(PUBLICATION_PATH),
        "package_count": len(packages),
        "mirror_package_count": len(mirror_packages),
        "interop_loader_metadata_package_count": interop_package_count,
        "tamper_rejection_diagnostic": PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        "network_policy": mirror["network_policy"],
        "hosted_registry_support": publication["hosted_registry_support"],
        "interop_loader_support": publication["interop_loader_support"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"mirror_path: {repo_rel(MIRROR_PATH)}")
    print(f"registry_path: {repo_rel(REGISTRY_PATH)}")
    print(f"publication_path: {repo_rel(PUBLICATION_PATH)}")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-package-mirror: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
