#!/usr/bin/env python3
"""Validate the Objective-C 3.0 package manager model contract."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command
from objc3c_package_manager.model import (
    DIRECT_IMPORT_SYNTAX_SUPPORT,
    LOCAL_PACKAGE_ABI_IDENTITY,
    LOCAL_PACKAGE_LANGUAGE_VERSION,
    LOCAL_PACKAGE_TRUST_KEY_ID,
    PACKAGE_MANAGER_TAMPER_CODE,
    PACKAGE_MANIFEST_CONTRACT_ID,
    collect_lock_model_failures,
)
from objc3c_shared.schema_registry import validate_registered_schema


CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "package_manager_model_contract.json"
LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-manager-model-summary.json"


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def load_manifest(path: str) -> dict[str, Any]:
    return load_json(ROOT / path)


def main() -> int:
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

    contract = load_json(CONTRACT_PATH)
    lock = load_json(LOCK_PATH) if LOCK_PATH.is_file() else {}
    package_manager = lock.get("package_manager", {})
    trust_policy = lock.get("trust_policy", {})
    packages = lock.get("packages", [])
    dependencies = lock.get("dependencies", [])
    manifests = []
    manifest_paths = []
    if isinstance(package_manager, dict):
        manifest_paths = [
            str(path)
            for path in package_manager.get("package_manifest_paths", [])
            if isinstance(path, str)
        ]
    for raw_path in manifest_paths:
        if not (ROOT / raw_path).is_file():
            failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: missing package manifest {raw_path}")
            continue
        manifests.append(load_manifest(raw_path))

    failures.extend(collect_lock_model_failures(lock, root=ROOT))
    expect(package_manager.get("language_version") == LOCAL_PACKAGE_LANGUAGE_VERSION, "language version drifted", failures)
    expect(package_manager.get("abi_identity") == LOCAL_PACKAGE_ABI_IDENTITY, "ABI identity drifted", failures)
    expect(package_manager.get("network_resolution") == "unsupported-fail-closed", "network resolution must fail closed", failures)
    expect(package_manager.get("hosted_registry") == "unsupported-fail-closed-if-claimed", "hosted registry must fail closed", failures)
    try:
        validate_registered_schema(
            trust_policy,
            "objc3c-package-signing-trust-v1",
            label="package signing trust policy",
        )
    except (KeyError, RuntimeError) as exc:
        failures.append(f"{PACKAGE_MANAGER_TAMPER_CODE}: package signing trust policy schema validation failed: {exc}")
    expect(len(manifests) == len(packages), "manifest count must match package count", failures)
    expect(len(packages) >= int(contract["minimum_package_count"]), "package graph lost package roots", failures)
    expect(len(dependencies) >= int(contract["minimum_dependency_count"]), "package graph lost dependency edges", failures)

    for manifest in manifests:
        package_id = str(manifest.get("package_id"))
        expect(manifest.get("contract_id") == PACKAGE_MANIFEST_CONTRACT_ID, f"manifest contract drifted for {package_id}", failures)
        expect(manifest.get("language", {}).get("version") == LOCAL_PACKAGE_LANGUAGE_VERSION, f"manifest language drifted for {package_id}", failures)
        expect(manifest.get("abi", {}).get("identity") == LOCAL_PACKAGE_ABI_IDENTITY, f"manifest ABI drifted for {package_id}", failures)
        module_graph = manifest.get("module_graph", {})
        expect(isinstance(module_graph, dict), f"manifest module graph missing for {package_id}", failures)
        if not isinstance(module_graph, dict):
            module_graph = {}
        expect(module_graph.get("direct_import_syntax") == DIRECT_IMPORT_SYNTAX_SUPPORT, f"manifest direct import syntax drifted for {package_id}", failures)
        expect(manifest.get("registry", {}).get("network_resolution") == "unsupported-fail-closed", f"manifest network support widened for {package_id}", failures)
        trust = manifest.get("trust", {})
        expect(isinstance(trust, dict) and trust.get("signing_key_id") == LOCAL_PACKAGE_TRUST_KEY_ID, f"manifest signing key drifted for {package_id}", failures)
        expect(isinstance(trust, dict) and trust.get("trust_root_id") == "objc3c-local-deterministic-trust-root-v1", f"manifest trust root drifted for {package_id}", failures)
        expect(isinstance(trust, dict) and trust.get("signing_backend") == "deterministic-test-replay", f"manifest signing backend drifted for {package_id}", failures)
        expect(isinstance(trust, dict) and trust.get("revocation_state") == "not-revoked", f"manifest revocation drifted for {package_id}", failures)

    action_names = {
        str(command).removeprefix("npm run objc3c -- ")
        for command in lock.get("replay", {}).get("commands", [])
        if isinstance(command, str)
    }
    missing_actions = [
        action
        for action in contract["required_public_actions"]
        if action not in action_names
    ]
    expect(not missing_actions, f"missing package manager replay actions: {', '.join(missing_actions)}", failures)

    payload = {
        "contract_id": "objc3c.package_ecosystem.package_manager_model.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(CONTRACT_PATH),
        "lock_path": repo_rel(LOCK_PATH),
        "package_count": len(packages) if isinstance(packages, list) else 0,
        "dependency_count": len(dependencies) if isinstance(dependencies, list) else 0,
        "package_manifest_count": len(manifests),
        "module_graph_count": (
            sum(
                1
                for package in packages
                if isinstance(package, dict)
                and isinstance(package.get("module_graph"), dict)
            )
            if isinstance(packages, list)
            else 0
        ),
        "language_version": package_manager.get("language_version") if isinstance(package_manager, dict) else None,
        "abi_identity": package_manager.get("abi_identity") if isinstance(package_manager, dict) else None,
        "trust_key_id": LOCAL_PACKAGE_TRUST_KEY_ID,
        "trust_root_id": "objc3c-local-deterministic-trust-root-v1",
        "production_signing_backend": trust_policy.get("production_signing_backend") if isinstance(trust_policy, dict) else None,
        "network_resolution": package_manager.get("network_resolution") if isinstance(package_manager, dict) else None,
        "hosted_registry": package_manager.get("hosted_registry") if isinstance(package_manager, dict) else None,
        "tamper_diagnostic": PACKAGE_MANAGER_TAMPER_CODE,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-manager-model: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-manager-model: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
