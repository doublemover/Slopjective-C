#!/usr/bin/env python3
"""Validate deterministic local package operation plans and receipts."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_package_manager.operations import (
    PACKAGE_OPERATION_SUMMARY_CONTRACT_ID,
    PACKAGE_OPERATIONS,
    PackageOperationError,
    PackageOperationRequest,
    collect_package_operation_receipt_failures,
    package_operation_artifact_paths,
    package_operation_plan,
    package_operation_receipt,
)
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command


LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
MIRROR_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "offline-mirror-index.json"
REGISTRY_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "registry" / "local-package-index.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-operations-summary.json"


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate deterministic package operation plans and receipts."
    )
    parser.add_argument(
        "--operation",
        choices=(*PACKAGE_OPERATIONS, "all"),
        default="all",
        help="Operation to validate. Defaults to every package operation.",
    )
    parser.add_argument(
        "--package-id",
        default="",
        help="Locked package id to operate on. Defaults to the first package with dependencies.",
    )
    parser.add_argument(
        "--allow-network",
        action="store_true",
        help="Request live network support; this must fail closed.",
    )
    parser.add_argument("--registry-url", default="", help="Live registry URL; must fail closed.")
    return parser.parse_args(argv)


def run_package_mirror_generator() -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        python_script_command("scripts/build_objc3c_package_mirror.py"),
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def package_ids(lock: dict[str, Any]) -> list[str]:
    packages = lock.get("packages", [])
    if not isinstance(packages, list):
        return []
    return sorted(
        str(package.get("package_id"))
        for package in packages
        if isinstance(package, dict)
    )


def package_ids_with_dependencies(lock: dict[str, Any]) -> list[str]:
    dependencies = lock.get("dependencies", [])
    if not isinstance(dependencies, list):
        return []
    return sorted(
        {
            str(edge.get("from"))
            for edge in dependencies
            if isinstance(edge, dict) and edge.get("from")
        }
    )


def default_package_id(lock: dict[str, Any]) -> str:
    with_dependencies = package_ids_with_dependencies(lock)
    if with_dependencies:
        return with_dependencies[0]
    ids = package_ids(lock)
    if not ids:
        raise RuntimeError("package lock contains no package ids")
    return ids[0]


def synthetic_installed_state(lock: dict[str, Any]) -> dict[str, Any]:
    packages = lock.get("packages", [])
    installed_packages: list[dict[str, str]] = []
    for package in packages if isinstance(packages, list) else []:
        if not isinstance(package, dict):
            continue
        package_id = str(package.get("package_id", ""))
        namespace, _, name = package_id.partition(":")
        safe_name = name.replace(".", "_")
        installed_packages.append(
            {
                "package_id": package_id,
                "installed_manifest": (
                    "tmp/artifacts/package-ecosystem/install-validation/clean-root/"
                    f"objc3c/packages/{namespace}/{safe_name}/package-manifest.json"
                ),
                "local_install_artifact": (
                    "tmp/artifacts/package-ecosystem/install-validation/"
                    f"local-package-artifacts/{namespace}/{safe_name}.json"
                ),
            }
        )
    return {"installed_packages": installed_packages}


def operations_from_args(args: argparse.Namespace) -> tuple[str, ...]:
    if args.operation == "all":
        return PACKAGE_OPERATIONS
    return (str(args.operation),)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    result = run_package_mirror_generator()
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)

    failures: list[str] = []
    if result.returncode != 0:
        failures.append("package mirror generator failed")

    lock = load_json(LOCK_PATH) if LOCK_PATH.is_file() else {}
    mirror = load_json(MIRROR_PATH) if MIRROR_PATH.is_file() else {}
    registry = load_json(REGISTRY_PATH) if REGISTRY_PATH.is_file() else {}
    package_id = args.package_id or default_package_id(lock)
    installed_state = synthetic_installed_state(lock)
    operation_records: list[dict[str, Any]] = []

    for operation in operations_from_args(args):
        request = PackageOperationRequest(
            operation=operation,
            package_id=package_id,
            allow_network=bool(args.allow_network),
            live_registry_url=str(args.registry_url or "") or None,
        )
        try:
            plan = package_operation_plan(
                root=ROOT,
                lock=lock,
                mirror=mirror,
                registry=registry,
                request=request,
                installed_state=installed_state,
            )
            receipt = package_operation_receipt(plan)
        except PackageOperationError as exc:
            failures.extend(exc.failures)
            continue
        receipt_failures = collect_package_operation_receipt_failures(
            plan=plan,
            receipt=receipt,
        )
        failures.extend(receipt_failures)
        plan_rel, receipt_rel = package_operation_artifact_paths(operation, package_id)
        write_json_file(ROOT / plan_rel, plan, sort_keys=True)
        write_json_file(ROOT / receipt_rel, receipt, sort_keys=True)
        operation_records.append(
            {
                "operation": operation,
                "package_id": package_id,
                "plan": plan_rel,
                "receipt": receipt_rel,
                "plan_digest": plan["plan_digest"],
                "receipt_digest": receipt["receipt_digest"],
                "rollback_token": receipt["rollback_token"],
                "receipt_failures": receipt_failures,
            }
        )

    summary = {
        "contract_id": PACKAGE_OPERATION_SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "package_id": package_id,
        "operations": operation_records,
        "lock_path": repo_rel(LOCK_PATH),
        "mirror_path": repo_rel(MIRROR_PATH),
        "registry_path": repo_rel(REGISTRY_PATH),
        "network_request": bool(args.allow_network or args.registry_url),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-operations: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-operations: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
