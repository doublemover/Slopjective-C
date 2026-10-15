#!/usr/bin/env python3
"""Validate package install and distribution credibility from clean local roots."""

from __future__ import annotations

import json
import argparse
import subprocess
import sys
import shutil
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_package_manager.install_distribution import (
    INSTALL_DISTRIBUTION_SUMMARY_CONTRACT_ID,
    INSTALL_DISTRIBUTION_ACTION,
    INSTALL_LOCAL_ARTIFACT_ROOT_REL,
    INSTALL_PROOF_MANIFEST_REL,
    INSTALL_RECEIPT_REL,
    INSTALL_VALIDATION_ROOT_REL,
    INSTALL_VERIFICATION_REL,
    PACKAGE_UNINSTALL_RECEIPT_REL,
    PACKAGE_UPDATE_RECEIPT_REL,
    collect_install_distribution_failures,
    materialize_clean_distribution_install,
    package_ids,
)
from objc3c_package_manager.producer_report import validate_producer_report
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command
from package_ecosystem_contracts import (
    require_package_ecosystem_blocker_metadata,
    require_package_ecosystem_owner_policy,
)
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "install_distribution_credibility_contract.json"
)
LOCK_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "locks" / "objc3c-package-lock.json"
MIRROR_PATH = ROOT / "tmp" / "artifacts" / "package-ecosystem" / "mirrors" / "offline-mirror-index.json"
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
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "package-ecosystem"
    / "install-distribution-credibility-summary.json"
)
FROM_NOTHING_ROOTS = (
    LOCK_PATH,
    MIRROR_PATH.parent,
    REGISTRY_PATH.parent,
    RESTORE_RECEIPT_PATH.parent,
    ROOT / INSTALL_VALIDATION_ROOT_REL,
    MIRROR_SUMMARY_PATH,
    SUMMARY_PATH,
)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate package install and distribution credibility from clean local roots."
    )
    parser.add_argument(
        "--from-nothing",
        action="store_true",
        help=(
            "Remove owned package-ecosystem temp outputs before replaying the "
            "product path so the run cannot depend on preexisting artifacts."
        ),
    )
    parser.add_argument("--producer-report", type=Path,
                        help="validated package producer report for graph consumers")
    parser.add_argument("--output-root", type=Path,
                        help="isolated graph consumer output root; standalone defaults are unchanged")
    parser.add_argument("--producer-node-identity", help=argparse.SUPPRESS)
    parser.add_argument("--build-identity", help=argparse.SUPPRESS)
    return parser.parse_args(argv)


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def run_package_mirror_generator() -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        python_script_command("scripts/build_objc3c_package_mirror.py"),
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def load_if_present(path: Path) -> dict[str, Any]:
    return load_json(path) if path.is_file() else {}


def _ensure_owned_temp_path(path: Path) -> None:
    resolved = path.resolve()
    allowed_roots = (
        (ROOT / "tmp" / "artifacts" / "package-ecosystem").resolve(),
        (ROOT / "tmp" / "reports" / "package-ecosystem").resolve(),
    )
    if not any(resolved == allowed or resolved.is_relative_to(allowed) for allowed in allowed_roots):
        raise RuntimeError(f"refusing to remove non-package temp path: {repo_rel(path)}")


def clean_owned_from_nothing_outputs(*, enabled: bool) -> dict[str, Any]:
    preexisting = {repo_rel(path): path.exists() for path in FROM_NOTHING_ROOTS}
    removed: list[str] = []
    if enabled:
        for path in FROM_NOTHING_ROOTS:
            _ensure_owned_temp_path(path)
            if not path.exists():
                continue
            if path.is_dir():
                shutil.rmtree(path)
            else:
                path.unlink()
            removed.append(repo_rel(path))
    after_clean = {repo_rel(path): path.exists() for path in FROM_NOTHING_ROOTS}
    return {
        "requested": enabled,
        "owned_roots": [repo_rel(path) for path in FROM_NOTHING_ROOTS],
        "preexisting_owned_outputs": preexisting,
        "removed_owned_outputs": sorted(removed),
        "owned_outputs_exist_after_clean": after_clean,
        "generated_from_clean_owned_outputs": enabled and not any(after_clean.values()),
    }


def main(argv: list[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    graph_consumer = args.producer_report is not None or args.output_root is not None
    if graph_consumer and (args.producer_report is None or args.output_root is None):
        raise RuntimeError("graph consumer requires both producer report and isolated output root")
    if graph_consumer and (not args.producer_node_identity or not args.build_identity):
        raise RuntimeError("graph consumer requires producer node and build identities")
    if not graph_consumer and (args.producer_node_identity or args.build_identity):
        raise RuntimeError("producer identities require a graph consumer report")
    output_root = args.output_root.resolve() if args.output_root else ROOT
    summary_path = output_root / "tmp" / "reports" / "package-ecosystem" / "install-distribution-credibility-summary.json"
    contract = load_json(CONTRACT_PATH)
    if graph_consumer:
        runs_root = (ROOT / "tmp").resolve()
        run_output = output_root.parent.resolve()
        if output_root.name != "package-consumer" or not run_output.is_relative_to(runs_root):
            raise RuntimeError("graph consumer output root must be package-consumer within repository tmp")
        report_path = args.producer_report.resolve()
        if not report_path.is_relative_to(runs_root):
            raise RuntimeError("producer report must be contained in repository tmp")
        producer_report = validate_producer_report(
            ROOT, report_path,
            expected_node_identity=args.producer_node_identity,
            expected_build_identity=args.build_identity,
        )
        validation_root = (output_root / INSTALL_VALIDATION_ROOT_REL).resolve()
        try:
            validation_root.relative_to(output_root)
        except ValueError as error:
            raise RuntimeError("refusing to remove an install output outside the isolated consumer root") from error
        if report_path.is_relative_to(validation_root):
            raise RuntimeError("producer report cannot be contained in the install root being cleaned")
        if validation_root.exists():
            shutil.rmtree(validation_root)
        from_nothing_probe = {
            "requested": True, "generated_from_clean_owned_outputs": True,
            "producer_report": repo_rel(report_path),
            "producer_report_digest": producer_report["report_digest"],
            "isolated_output_root": repo_rel(output_root),
        }
    else:
        from_nothing_probe = clean_owned_from_nothing_outputs(enabled=args.from_nothing)
    owner_policy = require_package_ecosystem_owner_policy(
        contract,
        surface_name="package install distribution credibility",
    )
    blocker_metadata = require_package_ecosystem_blocker_metadata(
        contract,
        surface_name="package install distribution credibility",
        required_blockers=(
            "clean install root depends on stale preexisting artifacts",
            "install receipt missing or not machine-owned",
            "package manifest, lock, mirror, registry, publication, or restore metadata disagree",
            "hosted registry or network install claim widened without evidence",
        ),
    )
    result = None if graph_consumer else run_package_mirror_generator()
    if result is not None and result.stdout:
        sys.stdout.write(result.stdout)
    if result is not None and result.stderr:
        sys.stderr.write(result.stderr)

    failures: list[str] = []
    if result is not None:
        expect(result.returncode == 0, "package mirror generator failed", failures)
    for path in (
        LOCK_PATH,
        MIRROR_PATH,
        REGISTRY_PATH,
        PUBLICATION_PATH,
        RESTORE_RECEIPT_PATH,
        MIRROR_SUMMARY_PATH,
    ):
        expect(path.is_file(), f"missing generated package install artifact {repo_rel(path)}", failures)

    lock = load_if_present(LOCK_PATH)
    mirror = load_if_present(MIRROR_PATH)
    registry = load_if_present(REGISTRY_PATH)
    publication = load_if_present(PUBLICATION_PATH)
    restore_receipt = load_if_present(RESTORE_RECEIPT_PATH)
    mirror_summary = load_if_present(MIRROR_SUMMARY_PATH)
    verification = (
        materialize_clean_distribution_install(
            root=ROOT,
            contract=contract,
            lock=lock,
            mirror=mirror,
            registry=registry,
            publication=publication,
            restore_receipt=restore_receipt,
            mirror_path=MIRROR_PATH,
            registry_path=REGISTRY_PATH,
            publication_path=PUBLICATION_PATH,
            restore_receipt_path=RESTORE_RECEIPT_PATH,
            output_root=output_root if graph_consumer else None,
        )
        if not failures
        else {}
    )

    if verification:
        failures.extend(
            collect_install_distribution_failures(
                root=ROOT,
                contract=contract,
                lock=lock,
                mirror=mirror,
                registry=registry,
                publication=publication,
                restore_receipt=restore_receipt,
                verification=verification,
                output_root=output_root if graph_consumer else None,
            )
        )
    action_names = set(public_workflow_action_names())
    required_actions = [
        str(action)
        for action in contract.get("required_public_actions", [])
        if isinstance(action, str)
    ]
    missing_public_actions = sorted(set(required_actions) - action_names)
    expect(
        not missing_public_actions,
        f"missing package install distribution public actions: {', '.join(missing_public_actions)}",
        failures,
    )
    expect(INSTALL_DISTRIBUTION_ACTION in action_names, "install distribution action is not public", failures)

    payload = {
        "contract_id": INSTALL_DISTRIBUTION_SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(CONTRACT_PATH),
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "lock_path": repo_rel(LOCK_PATH),
        "mirror_index": repo_rel(MIRROR_PATH),
        "local_registry_index": repo_rel(REGISTRY_PATH),
        "publication_metadata": repo_rel(PUBLICATION_PATH),
        "restore_receipt": repo_rel(RESTORE_RECEIPT_PATH),
        "install_receipt": repo_rel(output_root / INSTALL_RECEIPT_REL) if graph_consumer else INSTALL_RECEIPT_REL,
        "update_receipt": repo_rel(output_root / PACKAGE_UPDATE_RECEIPT_REL) if graph_consumer else PACKAGE_UPDATE_RECEIPT_REL,
        "uninstall_receipt": repo_rel(output_root / PACKAGE_UNINSTALL_RECEIPT_REL) if graph_consumer else PACKAGE_UNINSTALL_RECEIPT_REL,
        "install_verification": repo_rel(output_root / INSTALL_VERIFICATION_REL) if graph_consumer else INSTALL_VERIFICATION_REL,
        "install_proof_manifest": repo_rel(output_root / INSTALL_PROOF_MANIFEST_REL) if graph_consumer else INSTALL_PROOF_MANIFEST_REL,
        "local_package_artifact_root": repo_rel(output_root / INSTALL_LOCAL_ARTIFACT_ROOT_REL) if graph_consumer else INSTALL_LOCAL_ARTIFACT_ROOT_REL,
        "mirror_summary": repo_rel(MIRROR_SUMMARY_PATH),
        "package_count": len(package_ids(lock)),
        "mirror_package_count": len(package_ids(mirror)),
        "registry_package_count": len(package_ids(registry)),
        "installed_package_count": verification.get("manifest_count", 0),
        "dependency_count": verification.get("dependency_count", 0),
        "cache_entry_count": restore_receipt.get("cache_entry_count"),
        "network_policy": verification.get("network_policy"),
        "hosted_registry_support": verification.get("hosted_registry_support"),
        "offline_restore_support": verification.get("offline_restore_support"),
        "mirror_summary_status": mirror_summary.get("status"),
        "clean_start": verification.get("clean_start", {}),
        "from_nothing_probe": from_nothing_probe,
        "platform_host_evidence": verification.get("platform_host_evidence", {}),
        "generated_paths": verification.get("generated_paths", []),
        "required_public_actions": required_actions,
        "missing_public_actions": missing_public_actions,
        "failures": failures,
    }
    if graph_consumer:
        payload["install_output_root"] = repo_rel(output_root)
        payload["producer_report"] = repo_rel(args.producer_report.resolve())
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(summary_path)}")
    if failures:
        print("objc3c-package-install-distribution-credibility: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-install-distribution-credibility: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
