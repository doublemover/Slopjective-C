#!/usr/bin/env python3
"""Generate the local objc3c package lock from checked-in package surfaces."""

from __future__ import annotations

import hashlib
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from package_ecosystem_contracts import (
    load_package_loader_interop_metadata,
    normalize_package_loader_interop_metadata,
    package_loader_metadata_by_package,
    package_loader_metadata_digest_inputs,
    package_loader_metadata_summary,
)


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "package_authoring_workflow_contract.json"
LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-lock-summary.json"




def digest_for_paths(paths: list[str]) -> str:
    digest = hashlib.sha256()
    for raw_path in sorted(paths):
        path = ROOT / raw_path
        digest.update(raw_path.encode("utf-8"))
        digest.update(b"\0")
        digest.update(path.read_bytes())
        digest.update(b"\0")
    return digest.hexdigest()


def provenance_id(package_id: str) -> str:
    return "prov-" + package_id.replace(":", "-").replace(".", "-")


def public_workflow_command(action: str) -> str:
    return f"npm run objc3c -- {action}"


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    build_lock_command = public_workflow_command("build-package-lock")
    authoring_check_command = public_workflow_command("validate-package-authoring")
    sources = contract["package_sources"]
    module_inventory = load_json(ROOT / str(sources["stdlib_module_inventory"]))
    package_surface = load_json(ROOT / str(sources["stdlib_package_surface"]))
    showcase_portfolio = load_json(ROOT / str(sources["showcase_portfolio"]))

    modules = module_inventory.get("canonical_modules", [])
    examples = showcase_portfolio.get("examples", [])
    if not isinstance(modules, list) or not isinstance(examples, list):
        raise RuntimeError("package source inventories drifted from list shapes")

    interop_metadata = load_package_loader_interop_metadata(ROOT)
    packages: list[dict[str, Any]] = []
    dependencies: list[dict[str, str]] = []
    provenance: list[dict[str, str]] = []
    digest_inputs: list[str] = [
        str(sources["stdlib_module_inventory"]),
        str(sources["stdlib_package_surface"]),
        str(sources["showcase_portfolio"]),
    ]

    for module in sorted((entry for entry in modules if isinstance(entry, dict)), key=lambda entry: str(entry.get("module", ""))):
        module_id = str(module["module"])
        package_id = f"stdlib:{module_id}"
        source = str(module["manifest"])
        packages.append(
            {
                "package_id": package_id,
                "source": source,
                "provenance_id": provenance_id(package_id),
            }
        )
        provenance.append(
            {
                "provenance_id": provenance_id(package_id),
                "source_path": source,
                "generator": "scripts/build_objc3c_package_lock.py",
                "replay_command": build_lock_command,
            }
        )
        digest_inputs.append(source)

    for example in sorted((entry for entry in examples if isinstance(entry, dict)), key=lambda entry: str(entry.get("id", ""))):
        example_id = str(example["id"])
        package_id = f"showcase:{example_id}"
        source = str(example["workspace_manifest"])
        packages.append(
            {
                "package_id": package_id,
                "source": source,
                "provenance_id": provenance_id(package_id),
            }
        )
        provenance.append(
            {
                "provenance_id": provenance_id(package_id),
                "source_path": source,
                "generator": "scripts/build_objc3c_package_lock.py",
                "replay_command": build_lock_command,
            }
        )
        digest_inputs.append(source)
        for dependency in sorted(str(name) for name in example.get("stdlib_followup_modules", []) if isinstance(name, str)):
            dependencies.append(
                {
                    "from": package_id,
                    "to": f"stdlib:{dependency}",
                    "source": "checked-in-local-workspace",
                }
            )

    interop_by_package = package_loader_metadata_by_package(
        interop_metadata,
        root=ROOT,
        package_ids=(entry["package_id"] for entry in packages),
    )
    for package in packages:
        metadata_entry = interop_by_package.get(str(package["package_id"]))
        if metadata_entry is None:
            continue
        package["interop_loader_metadata"] = normalize_package_loader_interop_metadata(metadata_entry)
        digest_inputs.extend(package_loader_metadata_digest_inputs(metadata_entry))

    packages = sorted(packages, key=lambda entry: entry["package_id"])
    dependencies = sorted(dependencies, key=lambda entry: (entry["from"], entry["to"]))
    provenance = sorted(provenance, key=lambda entry: entry["provenance_id"])
    digest_inputs = sorted(set(digest_inputs))
    lock = {
        "contract_id": "objc3c.package_ecosystem.lockfile.v1",
        "lockfile_version": 1,
        "workspace": {
            "workspace_id": "objc3c-local-package-workspace",
            "source": "tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json",
        },
        "packages": packages,
        "dependencies": dependencies,
        "provenance": provenance,
        "interop_loader_metadata": package_loader_metadata_summary(interop_metadata, interop_by_package),
        "digest_inputs": digest_inputs,
        "replay": {
            "commands": [
                build_lock_command,
                authoring_check_command,
            ]
        },
    }

    LOCK_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(LOCK_PATH, lock)
    interop_summary = lock["interop_loader_metadata"]
    summary = {
        "contract_id": "objc3c.package_ecosystem.package_lock.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "lock_path": repo_rel(LOCK_PATH),
        "lock_digest": digest_for_paths(digest_inputs),
        "package_count": len(packages),
        "dependency_count": len(dependencies),
        "provenance_count": len(provenance),
        "interop_loader_metadata_package_count": len(interop_by_package),
        "interop_loader_metadata_source": interop_summary["source"],
        "interop_loader_metadata_bridge_surface_count": interop_summary["bridge_surface_count"],
        "interop_loader_metadata_objcxx_bridge_surface_count": interop_summary["objcxx_bridge_surface_count"],
        "interop_loader_metadata_swift_bridge_surface_count": interop_summary["swift_bridge_surface_count"],
        "tamper_rejection_diagnostic": interop_summary["tamper_rejection_diagnostic"],
        "digest_input_count": len(digest_inputs),
        "source_package_surface_contract_id": package_surface.get("contract_id"),
        "showcase_portfolio_contract_id": showcase_portfolio.get("contract_id"),
        "stdlib_module_inventory_contract_id": module_inventory.get("contract_id"),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"lock_path: {repo_rel(LOCK_PATH)}")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-package-lock: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
