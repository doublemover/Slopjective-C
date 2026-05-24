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
    contract = load_json(CONTRACT_PATH)
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
    result = run_package_mirror_generator()
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)

    failures: list[str] = []
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
        "install_receipt": INSTALL_RECEIPT_REL,
        "update_receipt": PACKAGE_UPDATE_RECEIPT_REL,
        "uninstall_receipt": PACKAGE_UNINSTALL_RECEIPT_REL,
        "install_verification": INSTALL_VERIFICATION_REL,
        "install_proof_manifest": INSTALL_PROOF_MANIFEST_REL,
        "local_package_artifact_root": INSTALL_LOCAL_ARTIFACT_ROOT_REL,
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
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-install-distribution-credibility: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-install-distribution-credibility: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
