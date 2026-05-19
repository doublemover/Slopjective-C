#!/usr/bin/env python3
"""Generate offline mirror and local registry metadata from the package lock."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import command_text, python_script_command
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


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


def main() -> int:
    ensure_lock()
    lock = load_json(LOCK_PATH)
    build_lock_command = command_text(python_script_command("scripts/build_objc3c_package_lock.py"))
    build_mirror_command = command_text(python_script_command("scripts/build_objc3c_package_mirror.py"))
    mirror_check_command = command_text(
        python_script_command("scripts/check_objc3c_package_registry_mirror_reproducibility.py")
    )
    packages = lock.get("packages", [])
    if not isinstance(packages, list):
        raise RuntimeError("lock packages field drifted from a list")

    mirror_packages = [
        {
            "package_id": str(package["package_id"]),
            "source": str(package["source"]),
            "cache_path": f"tmp/artifacts/package-ecosystem/mirrors/cache/{package['package_id'].replace(':', '/')}.json",
        }
        for package in packages
        if isinstance(package, dict)
    ]
    mirror = {
        "contract_id": "objc3c.package_ecosystem.offline_mirror.v1",
        "mirror_version": 1,
        "source_lock": repo_rel(LOCK_PATH),
        "packages": mirror_packages,
        "network_policy": "no-network-during-validation",
        "replay": {
            "commands": [
                build_lock_command,
                build_mirror_command,
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
        "packages": [
            {
                "package_id": package["package_id"],
                "source": package["source"],
                "provenance_id": package["provenance_id"],
            }
            for package in packages
            if isinstance(package, dict)
        ],
    }
    publication = {
        "contract_id": "objc3c.package_ecosystem.publication_metadata.v1",
        "source_lock": repo_rel(LOCK_PATH),
        "source_mirror": repo_rel(MIRROR_PATH),
        "source_registry_index": repo_rel(REGISTRY_PATH),
        "publication_state": "generated-local-metadata",
        "hosted_registry_support": "unsupported-fail-closed-if-claimed",
        "network_resolution_support": "unsupported",
        "package_count": len(packages),
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
        "network_policy": mirror["network_policy"],
        "hosted_registry_support": publication["hosted_registry_support"],
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
