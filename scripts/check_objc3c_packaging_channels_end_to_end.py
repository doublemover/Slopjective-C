#!/usr/bin/env python3
"""Validate objc3c package channels end to end from the generated artifacts."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import zipfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import validate_json_schema
from objc3c_tooling.subprocesses import python_script_command, run_capture
from objc3c_workflow.commands import workflow_command
from objc3c_package_channels.model import (
    MANIFEST_RELATIVE_PATH,
    required_payload_entries_for_platform,
)

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
BUILD_PACKAGE_CHANNELS_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
RELEASE_FOUNDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_release_foundation_integration.py"
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "source_surface.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
INSTALL_RECEIPT_SCHEMA = ROOT / "schemas" / "objc3c-package-install-receipt-v1.schema.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "end-to-end-summary.json"
ARCHIVE_DIGEST_FIELDS = {
    "portable_archive": "portable-archive",
    "installer_archive": "local-installer",
    "offline_archive": "offline-bundle",
}






def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def file_artifact(path: Path) -> dict[str, Any]:
    if not path.is_file():
        return {
            "path": repo_rel(path),
            "exists": False,
        }
    return {
        "path": repo_rel(path),
        "exists": True,
        "size_bytes": path.stat().st_size,
        "sha256": sha256_file(path),
    }


def platform_host_evidence_root() -> tuple[str, Path] | None:
    platform_id = os.environ.get("OBJC3C_PLATFORM_ID", "")
    evidence_root = os.environ.get("OBJC3C_PLATFORM_EVIDENCE_ROOT", "")
    if platform_id not in {"linux-x64", "darwin-arm64"} or not evidence_root:
        return None
    resolved_root = (ROOT / evidence_root).resolve()
    expected_root = (ROOT / "tmp" / "reports" / "platform-host-evidence" / platform_id).resolve()
    if resolved_root != expected_root:
        raise RuntimeError(
            "platform package-channel install evidence root must be platform-scoped: "
            f"{evidence_root}"
        )
    return platform_id, resolved_root


def platform_install_receipt_status(
    *,
    receipt_exists: bool,
    receipt: dict[str, Any],
    platform_id: str,
) -> str:
    if not receipt_exists:
        return "missing-source-generated-fail-closed"
    receipt_target = str(receipt.get("target_platform_id", ""))
    if not receipt_target:
        package_runtime_model = receipt.get("package_runtime_model", {})
        if isinstance(package_runtime_model, dict):
            receipt_target = str(package_runtime_model.get("target_platform_id", ""))
    if receipt_target and receipt_target != platform_id:
        return "install-receipt-target-mismatch-generated-fail-closed"
    return "generated-host-artifact-present"


def publish_platform_host_install_receipt(
    *,
    manifest: dict[str, Any],
    manifest_path: Path,
    receipt: dict[str, Any],
    receipt_path: Path,
    receipt_artifact: dict[str, Any],
) -> None:
    config = platform_host_evidence_root()
    if config is None:
        return
    platform_id, evidence_root = config
    package_runtime_model = receipt.get("package_runtime_model", {})
    if not isinstance(package_runtime_model, dict):
        package_runtime_model = {}
    target_triple_by_platform = {
        "linux-x64": "x86_64-unknown-linux-gnu",
        "darwin-arm64": "aarch64-apple-darwin",
    }
    issue_ref_by_platform = {
        "linux-x64": 8228,
        "darwin-arm64": 8229,
    }
    record_id_by_platform = {
        "linux-x64": "objc3c.package-install-identity.linux-x64.release.missing",
        "darwin-arm64": "objc3c.package-install-identity.darwin-arm64.release.missing",
    }
    producer_contract_id_by_platform = {
        "linux-x64": "objc3c.platform.linux.install-receipt.v1",
        "darwin-arm64": "objc3c.platform.darwin.install-receipt.v1",
    }
    payload = {
        "contract_id": "objc3c.platform.hosted-install-receipt.generated.v1",
        "schema_version": 1,
        "platform_id": platform_id,
        "issue_ref": issue_ref_by_platform[platform_id],
        "record_id": record_id_by_platform[platform_id],
        "generated_report_path": (
            f"tmp/reports/platform-host-evidence/{platform_id}/install/install-receipt.json"
        ),
        "source_summary_path": repo_rel(SUMMARY_PATH),
        "source_install_receipt_path": repo_rel(receipt_path),
        "reviewed_source_required": True,
        "support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": platform_install_receipt_status(
            receipt_exists=receipt_artifact.get("exists") is True,
            receipt=receipt,
            platform_id=platform_id,
        ),
        "target_platform_id": platform_id,
        "target_triple": target_triple_by_platform[platform_id],
        "package_root": str(manifest.get("package_root", "")),
        "package_root_layout": list(package_runtime_model.get("package_root_layout", [])),
        "package_manifest": MANIFEST_RELATIVE_PATH,
        "package_manifest_artifact": file_artifact(manifest_path),
        "package_channels_summary_artifact": file_artifact(SUMMARY_PATH),
        "source_install_receipt_artifact": receipt_artifact,
        "source_install_receipt": receipt,
        "producer_evidence": {
            "contract_id": producer_contract_id_by_platform[platform_id],
            "status": "INSTALL_RECEIPT_ROUTED"
            if receipt_artifact.get("exists") is True
            else "INSTALL_RECEIPT_MISSING",
            "target_platform_id": str(package_runtime_model.get("target_platform_id", "")),
            "source_summary": repo_rel(SUMMARY_PATH),
        },
        "source_artifacts": [
            file_artifact(SUMMARY_PATH),
            file_artifact(manifest_path),
            receipt_artifact,
        ],
    }
    target_path = evidence_root / "install" / "install-receipt.json"
    target_path.parent.mkdir(parents=True, exist_ok=True)
    target_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def extract_zip(zip_path: Path, destination: Path) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path) as archive:
        archive.extractall(destination)


def load_valid_install_receipt(
    receipt_path: Path,
    receipt_schema: dict[str, Any],
    install_root: Path,
    *,
    expected_channel_id: str,
    expected_payload_manifest_sha256: str,
    expected_payload_entries: list[str],
) -> dict[str, Any]:
    receipt = load_json(receipt_path)
    validate_json_schema(receipt, receipt_schema, label=repo_rel(receipt_path))
    expect(receipt["install_root"] == str(install_root), "install receipt root drifted from requested install root")
    expect(receipt["install_home"] == str(install_root / "objc3c"), "install receipt home drifted from requested install root")
    expect(receipt["channel_id"] == expected_channel_id, "install receipt channel id drifted")
    expect(receipt["bootstrap_entrypoint"] == "Bootstrap-objc3cEnvironment.ps1", "install receipt bootstrap entrypoint drifted")
    expect(receipt["package_bridge"] == "objc3c", "install receipt package bridge drifted")
    expect(receipt["install_command"] == "npm run objc3c -- build-package-channels", "install receipt command drifted")
    expect(receipt["payload_manifest"] == MANIFEST_RELATIVE_PATH, "install receipt payload manifest drifted")
    expect(
        receipt["payload_manifest_sha256"] == expected_payload_manifest_sha256,
        "install receipt payload manifest digest drifted",
    )
    expect(receipt["payload_required_entries"] == expected_payload_entries, "install receipt payload entries drifted")
    installed_manifest = install_root / "objc3c" / MANIFEST_RELATIVE_PATH
    expect(installed_manifest.is_file(), "installed payload manifest missing")
    expect(
        sha256_file(installed_manifest) == expected_payload_manifest_sha256,
        "installed payload manifest digest drifted from receipt",
    )
    return receipt


def validate_archive_digest(
    *,
    manifest: dict[str, Any],
    archive_digests: dict[str, Any],
    archive_field: str,
    archive_path: Path,
    artifact_role: str,
) -> dict[str, Any]:
    digest_record = archive_digests.get(archive_field)
    expect(isinstance(digest_record, dict), f"archive digest missing {archive_field}")
    expect(digest_record.get("digest_format") == "sha256", f"{archive_field} digest format drifted")
    expect(digest_record.get("artifact_role") == artifact_role, f"{archive_field} digest role drifted")
    expect(digest_record.get("artifact") == manifest.get(archive_field), f"{archive_field} digest artifact drifted")
    expect(digest_record.get("artifact") == repo_rel(archive_path), f"{archive_field} digest artifact path drifted")
    expect(digest_record.get("sha256") == sha256_file(archive_path), f"{archive_field} digest drifted")
    expect(
        digest_record.get("verification_command") == "npm run objc3c -- validate-packaging-channels-end-to-end",
        f"{archive_field} digest verification command drifted",
    )
    expect(digest_record.get("trust_scope") == "checked-in-artifact-digest", f"{archive_field} digest trust scope drifted")
    return digest_record


def validate_payload_contract(
    manifest: dict[str, Any],
    package_root: Path,
    expected_payload_entries: list[str],
) -> dict[str, Any]:
    payload_contract = manifest.get("payload_contract")
    expect(isinstance(payload_contract, dict), "payload_contract missing from package channels manifest")
    expect(
        payload_contract.get("contract_id") == "objc3c.packaging.channels.payload-contract.v1",
        "payload contract identity drifted",
    )
    expect(
        payload_contract.get("source") == "canonical-runnable-toolchain-package",
        "payload contract source drifted",
    )
    expect(payload_contract.get("manifest_relative_path") == MANIFEST_RELATIVE_PATH, "payload contract manifest path drifted")
    payload_manifest = package_root / MANIFEST_RELATIVE_PATH
    expect(payload_manifest.is_file(), "package root missing payload manifest")
    expect(payload_contract.get("manifest_artifact") == repo_rel(payload_manifest), "payload contract manifest artifact drifted")
    expect(payload_contract.get("manifest_sha256") == sha256_file(payload_manifest), "payload contract manifest digest drifted")
    expect(payload_contract.get("required_entries") == expected_payload_entries, "payload contract required entries drifted")
    expect(
        payload_contract.get("clean_room_source_policy") == "fresh-owned-tmp-root-only",
        "payload contract clean-room policy drifted",
    )

    entry_digests = payload_contract.get("entry_digests")
    expect(isinstance(entry_digests, dict), "payload contract entry digests missing")
    for relative_path in expected_payload_entries:
        payload_entry = package_root / relative_path
        expect(payload_entry.is_file(), f"package root missing required payload entry {relative_path}")
        digest_record = entry_digests.get(relative_path)
        expect(isinstance(digest_record, dict), f"payload contract missing digest for {relative_path}")
        expect(digest_record.get("digest_format") == "sha256", f"payload digest format drifted for {relative_path}")
        expect(digest_record.get("artifact") == relative_path, f"payload digest artifact drifted for {relative_path}")
        expect(digest_record.get("sha256") == sha256_file(payload_entry), f"payload digest drifted for {relative_path}")
    return payload_contract


def validate_receipt_contracts(
    manifest: dict[str, Any],
    expected_payload_entries: list[str],
    target_platform_id: str,
) -> dict[str, Any]:
    receipt_contracts = manifest.get("receipt_contracts")
    expect(isinstance(receipt_contracts, dict), "receipt_contracts missing from package channels manifest")
    expected = {
        "install_receipt": ("local-installer", "local-filesystem-only"),
        "offline_install_receipt": ("offline-bundle", "no-network"),
    }
    for contract_name, (channel_id, network_policy) in expected.items():
        receipt_contract = receipt_contracts.get(contract_name)
        expect(isinstance(receipt_contract, dict), f"receipt contract missing {contract_name}")
        expect(
            receipt_contract.get("contract_id") == "objc3c.packaging.channels.install-receipt.v1",
            f"{contract_name} identity drifted",
        )
        expect(receipt_contract.get("channel_id") == channel_id, f"{contract_name} channel id drifted")
        expect(receipt_contract.get("target_platform_id") == target_platform_id, f"{contract_name} target platform drifted")
        expect(receipt_contract.get("payload_manifest") == MANIFEST_RELATIVE_PATH, f"{contract_name} payload manifest drifted")
        expect(
            receipt_contract.get("payload_required_entries") == expected_payload_entries,
            f"{contract_name} payload entries drifted",
        )
        expect(receipt_contract.get("network_policy") == network_policy, f"{contract_name} network policy drifted")
        expect(receipt_contract.get("rollback_required") is True, f"{contract_name} rollback requirement drifted")
    expect(
        receipt_contracts["offline_install_receipt"].get("delegates_to") == "local-installer",
        "offline receipt contract delegate drifted",
    )
    return receipt_contracts




def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--use-existing-build-report",
        action="store_true",
        help="Validate the current package-channel build report instead of rebuilding package channels.",
    )
    return parser.parse_args(argv)


def build_package_channels_from_fresh_release_foundation() -> None:
    release_foundation_result = run_capture(
        workflow_command("validate-release-foundation"),
        cwd=ROOT,
        capture_output=False,
    )
    if release_foundation_result.returncode != 0:
        raise RuntimeError("release-foundation validation failed")

    release_foundation_integration_result = run_capture(
        python_script_command(RELEASE_FOUNDATION_INTEGRATION_PY),
        cwd=ROOT,
        capture_output=False,
    )
    if release_foundation_integration_result.returncode != 0:
        raise RuntimeError("release-foundation integration check failed")

    build_result = run_capture(
        python_script_command(
            BUILD_PACKAGE_CHANNELS_PY,
            "--reuse-release-foundation-artifacts",
        ),
        cwd=ROOT,
        capture_output=False,
    )
    if build_result.returncode != 0:
        raise RuntimeError("package-channels build failed")


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    work_root = ROOT / "tmp" / "pkg" / "objc3c-package-channels-e2e" / run_id
    portable_extract_root = work_root / "portable-extract"
    installer_extract_root = work_root / "installer-extract"
    offline_extract_root = work_root / "offline-extract"
    install_root = work_root / "install-root"
    offline_install_root = work_root / "offline-install-root"
    if work_root.exists():
        shutil.rmtree(work_root)
    SUMMARY_PATH.unlink(missing_ok=True)

    if args.use_existing_build_report:
        expect(REPORT_PATH.is_file(), "existing package-channels build report is missing")
    else:
        REPORT_PATH.unlink(missing_ok=True)
        build_package_channels_from_fresh_release_foundation()

    source_surface = load_json(SOURCE_SURFACE)
    owner_policy = source_surface.get("owner_policy")
    if not isinstance(owner_policy, dict) or owner_policy.get("evidence_log_allowed") is not False:
        raise RuntimeError("packaging-channel source surface missing source-owned owner_policy")
    blocker_metadata = source_surface.get("blocker_metadata")
    if not isinstance(blocker_metadata, dict) or blocker_metadata.get("blocker_owner") != "packaging-channels-blockers":
        raise RuntimeError("packaging-channel source surface missing blocker metadata")

    summary = load_json(REPORT_PATH)
    expect(summary.get("status") == "PASS", "package-channels build report did not pass")
    manifest_path = ROOT / str(summary["manifest_path"]).replace("/", os.sep)
    manifest = load_json(manifest_path)
    expect(manifest.get("support_truth") is False, "package channels manifest promoted support truth")
    expect(manifest.get("native_execution_claimed") is False, "package channels manifest claimed native execution")
    package_root = ROOT / str(manifest["package_root"]).replace("/", os.sep)
    target_platform_id = str(manifest.get("platform_id", ""))
    expected_payload_entries = required_payload_entries_for_platform(
        sanitizer_variant=str(manifest.get("sanitizer_variant", "release")),
        target_platform_id=target_platform_id,
    )
    native_executable_entry = next(
        (
            entry
            for entry in expected_payload_entries
            if entry == "artifacts/bin/objc3c-native.exe"
            or entry == "artifacts/bin/objc3c-native"
        ),
        "",
    )
    expect(native_executable_entry != "", "package payload missing native executable entry")
    payload_contract = validate_payload_contract(manifest, package_root, expected_payload_entries)
    receipt_contracts = validate_receipt_contracts(
        manifest,
        expected_payload_entries,
        target_platform_id,
    )

    portable_archive = ROOT / str(summary["portable_archive"]).replace("/", os.sep)
    installer_archive = ROOT / str(summary["installer_archive"]).replace("/", os.sep)
    offline_archive = ROOT / str(summary["offline_archive"]).replace("/", os.sep)
    expect(portable_archive.is_file(), "portable archive was not published")
    expect(installer_archive.is_file(), "installer archive was not published")
    expect(offline_archive.is_file(), "offline archive was not published")
    installer_signature = manifest.get("installer_signature", {})
    expect(installer_signature.get("signature_format") == "objc3c-local-sha256-v1", "installer signature format drifted")
    expect(installer_signature.get("artifact") == repo_rel(installer_archive), "installer signature artifact drifted")
    expect(installer_signature.get("sha256") == sha256_file(installer_archive), "installer signature digest drifted")
    expect(installer_signature.get("verification_command") == "npm run objc3c -- validate-packaging-channels-end-to-end", "installer signature verification command drifted")
    archive_digests = manifest.get("archive_digests", {})
    expect(isinstance(archive_digests, dict), "archive_digests missing from package channels manifest")
    archive_paths = {
        "portable_archive": portable_archive,
        "installer_archive": installer_archive,
        "offline_archive": offline_archive,
    }
    validated_archive_digests = {
        archive_field: validate_archive_digest(
            manifest=manifest,
            archive_digests=archive_digests,
            archive_field=archive_field,
            archive_path=archive_paths[archive_field],
            artifact_role=artifact_role,
        )
        for archive_field, artifact_role in ARCHIVE_DIGEST_FIELDS.items()
    }
    expect(
        installer_signature.get("sha256") == validated_archive_digests["installer_archive"]["sha256"],
        "installer signature digest drifted from installer archive digest",
    )

    extract_zip(portable_archive, portable_extract_root)
    expect((portable_extract_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json").is_file(), "portable archive missing runnable package manifest")

    extract_zip(installer_archive, installer_extract_root)
    installer_script = installer_extract_root / "Install-objc3c.ps1"
    uninstall_script = installer_extract_root / "Uninstall-objc3c.ps1"
    expect(installer_script.is_file(), "installer archive missing Install-objc3c.ps1")
    expect(uninstall_script.is_file(), "installer archive missing Uninstall-objc3c.ps1")

    install_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(installer_script), "-InstallRoot", str(install_root), "-Force"],
        cwd=installer_extract_root,
    )
    if install_result.returncode != 0:
        raise RuntimeError("installer archive failed to install under temp root")

    receipt_path = install_root / "objc3c-install-receipt.json"
    bootstrap_script = install_root / "Bootstrap-objc3cEnvironment.ps1"
    installed_exe = install_root / "objc3c" / native_executable_entry
    expect(receipt_path.is_file(), "installer did not publish install receipt")
    expect(bootstrap_script.is_file(), "installer did not publish bootstrap script")
    expect(installed_exe.is_file(), "installer did not publish installed native executable")

    receipt_schema = load_json(INSTALL_RECEIPT_SCHEMA)
    install_receipt = load_valid_install_receipt(
        receipt_path,
        receipt_schema,
        install_root,
        expected_channel_id="local-installer",
        expected_payload_manifest_sha256=payload_contract["manifest_sha256"],
        expected_payload_entries=expected_payload_entries,
    )
    install_receipt_artifact = file_artifact(receipt_path)

    bootstrap_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(bootstrap_script)],
        cwd=install_root,
    )
    if bootstrap_result.returncode != 0:
        raise RuntimeError("installed bootstrap script failed")
    expect("objc3c_home:" in bootstrap_result.stdout, "installed bootstrap script did not publish objc3c_home")

    uninstall_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(uninstall_script), "-InstallRoot", str(install_root)],
        cwd=installer_extract_root,
    )
    if uninstall_result.returncode != 0:
        raise RuntimeError("installer archive failed to roll back under temp root")
    expect(not (install_root / "objc3c").exists(), "rollback left the installed toolchain behind")
    expect(not receipt_path.exists(), "rollback left the install receipt behind")

    extract_zip(offline_archive, offline_extract_root)
    offline_bootstrap_script = offline_extract_root / "OfflineBootstrap-objc3c.ps1"
    expect(offline_bootstrap_script.is_file(), "offline bundle missing OfflineBootstrap-objc3c.ps1")
    offline_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(offline_bootstrap_script), "-InstallRoot", str(offline_install_root)],
        cwd=offline_extract_root,
    )
    if offline_result.returncode != 0:
        raise RuntimeError("offline bundle bootstrap failed")
    offline_receipt_path = offline_install_root / "objc3c-install-receipt.json"
    expect(offline_receipt_path.is_file(), "offline bootstrap did not publish install receipt")
    load_valid_install_receipt(
        offline_receipt_path,
        receipt_schema,
        offline_install_root,
        expected_channel_id="offline-bundle",
        expected_payload_manifest_sha256=payload_contract["manifest_sha256"],
        expected_payload_entries=expected_payload_entries,
    )
    expect((offline_install_root / "objc3c" / native_executable_entry).is_file(), "offline bootstrap did not install native executable")

    end_to_end_summary = {
        "contract_id": "objc3c.packaging.channels.end-to-end.summary.v1",
        "status": "PASS",
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "build_report": repo_rel(REPORT_PATH),
        "manifest_path": repo_rel(manifest_path),
        "package_root": manifest["package_root"],
        "portable_archive": repo_rel(portable_archive),
        "installer_archive": repo_rel(installer_archive),
        "offline_archive": repo_rel(offline_archive),
        "installer_signature": installer_signature,
        "archive_digests": validated_archive_digests,
        "payload_contract": payload_contract,
        "receipt_contracts": receipt_contracts,
        "install_root": repo_rel(install_root),
        "offline_install_root": repo_rel(offline_install_root),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(end_to_end_summary, indent=2) + "\n", encoding="utf-8")
    publish_platform_host_install_receipt(
        manifest=manifest,
        manifest_path=manifest_path,
        receipt=install_receipt,
        receipt_path=receipt_path,
        receipt_artifact=install_receipt_artifact,
    )
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-packaging-channels-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
