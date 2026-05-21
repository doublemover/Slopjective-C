#!/usr/bin/env python3
"""Validate the local package authoring workflow and generated lock."""

from __future__ import annotations

import json
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
    PACKAGE_MANAGER_TAMPER_CODE,
    collect_lock_model_failures,
)
from package_ecosystem_contracts import PACKAGE_LOADER_INTEROP_TAMPER_CODE


CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "package_authoring_workflow_contract.json"
LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
LOCK_SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-lock-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-authoring-workflow-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(ROOT / "package.json")
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    result = subprocess.run(
        python_script_command("scripts/build_objc3c_package_lock.py"),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)

    failures: list[str] = []
    expect(result.returncode == 0, "package lock generator failed", failures)
    expect(LOCK_PATH.is_file(), f"missing generated lock {repo_rel(LOCK_PATH)}", failures)
    expect(LOCK_SUMMARY_PATH.is_file(), f"missing package lock summary {repo_rel(LOCK_SUMMARY_PATH)}", failures)

    lock = load_json(LOCK_PATH) if LOCK_PATH.is_file() else {}
    lock_summary = load_json(LOCK_SUMMARY_PATH) if LOCK_SUMMARY_PATH.is_file() else {}
    packages = lock.get("packages", [])
    dependencies = lock.get("dependencies", [])
    provenance = lock.get("provenance", [])
    digest_inputs = lock.get("digest_inputs", [])
    replay = lock.get("replay", {})
    package_manager = lock.get("package_manager", {})
    interop_loader_metadata = lock.get("interop_loader_metadata", {})
    authoring_check_command = "npm run objc3c -- validate-package-authoring"
    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in contract["required_actions"]]
    missing_actions = [name for name in required_actions if name not in set(public_workflow_action_names())]

    expect(lock.get("contract_id") == "objc3c.package_ecosystem.lockfile.v1", "lock contract id drifted", failures)
    expect(isinstance(packages, list) and len(packages) == lock_summary.get("package_count"), "lock package count drifted", failures)
    expect(isinstance(dependencies, list) and len(dependencies) == lock_summary.get("dependency_count"), "lock dependency count drifted", failures)
    expect(isinstance(provenance, list) and len(provenance) == lock_summary.get("provenance_count"), "lock provenance count drifted", failures)
    expect(isinstance(digest_inputs, list) and digest_inputs == sorted(digest_inputs), "lock digest inputs are not deterministic", failures)
    expect(isinstance(packages, list) and packages == sorted(packages, key=lambda entry: entry["package_id"]), "lock packages are not sorted", failures)
    expect(isinstance(dependencies, list) and dependencies == sorted(dependencies, key=lambda entry: (entry["from"], entry["to"])), "lock dependencies are not sorted", failures)
    expect(isinstance(replay, dict) and authoring_check_command in replay.get("commands", []), "lock replay commands missing authoring check", failures)
    expect(
        isinstance(package_manager, dict)
        and package_manager.get("language_version") == LOCAL_PACKAGE_LANGUAGE_VERSION,
        "package manager language version drifted",
        failures,
    )
    expect(
        isinstance(package_manager, dict)
        and package_manager.get("abi_identity") == LOCAL_PACKAGE_ABI_IDENTITY,
        "package manager ABI identity drifted",
        failures,
    )
    expect(
        isinstance(package_manager, dict)
        and package_manager.get("network_resolution") == "unsupported-fail-closed",
        "package manager network resolution must fail closed",
        failures,
    )
    expect(
        isinstance(package_manager, dict)
        and package_manager.get("hosted_registry") == "unsupported-fail-closed-if-claimed",
        "hosted registry claims must fail closed",
        failures,
    )
    failures.extend(collect_lock_model_failures(lock, root=ROOT))
    expect(
        isinstance(interop_loader_metadata, dict)
        and interop_loader_metadata.get("package_count") == lock_summary.get("interop_loader_metadata_package_count"),
        "lock interop loader metadata count drifted",
        failures,
    )
    expect(
        isinstance(interop_loader_metadata, dict)
        and interop_loader_metadata.get("tamper_rejection_diagnostic") == PACKAGE_LOADER_INTEROP_TAMPER_CODE,
        "lock interop loader tamper diagnostic drifted",
        failures,
    )
    expect(
        isinstance(interop_loader_metadata, dict)
        and interop_loader_metadata.get("objcxx_bridge_surface_count", 0) > 0
        and interop_loader_metadata.get("swift_bridge_surface_count", 0) > 0,
        "lock interop loader ObjC++/Swift bridge surface counts drifted",
        failures,
    )
    expect(package_bridge_exists, f"package authoring workflow missing package bridge {package_bridge}", failures)
    expect(not missing_actions, "package authoring workflow missing required actions", failures)

    payload = {
        "contract_id": "objc3c.package_ecosystem.package_authoring_workflow.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(CONTRACT_PATH),
        "lock_path": repo_rel(LOCK_PATH),
        "lock_summary_path": repo_rel(LOCK_SUMMARY_PATH),
        "package_count": len(packages) if isinstance(packages, list) else 0,
        "dependency_count": len(dependencies) if isinstance(dependencies, list) else 0,
        "provenance_count": len(provenance) if isinstance(provenance, list) else 0,
        "package_manifest_count": (
            len(package_manager.get("package_manifest_paths", []))
            if isinstance(package_manager, dict) and isinstance(package_manager.get("package_manifest_paths"), list)
            else 0
        ),
        "language_version": package_manager.get("language_version") if isinstance(package_manager, dict) else None,
        "abi_identity": package_manager.get("abi_identity") if isinstance(package_manager, dict) else None,
        "network_resolution": package_manager.get("network_resolution") if isinstance(package_manager, dict) else None,
        "hosted_registry": package_manager.get("hosted_registry") if isinstance(package_manager, dict) else None,
        "package_manager_tamper_diagnostic": PACKAGE_MANAGER_TAMPER_CODE,
        "interop_loader_metadata_package_count": (
            interop_loader_metadata.get("package_count") if isinstance(interop_loader_metadata, dict) else 0
        ),
        "interop_loader_metadata_bridge_surface_count": (
            interop_loader_metadata.get("bridge_surface_count") if isinstance(interop_loader_metadata, dict) else 0
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
        "tamper_rejection_diagnostic": (
            interop_loader_metadata.get("tamper_rejection_diagnostic")
            if isinstance(interop_loader_metadata, dict)
            else None
        ),
        "digest_input_count": len(digest_inputs) if isinstance(digest_inputs, list) else 0,
        "package_bridge": package_bridge,
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "required_actions": required_actions,
        "missing_actions": missing_actions,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-authoring-workflow: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-authoring-workflow: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
