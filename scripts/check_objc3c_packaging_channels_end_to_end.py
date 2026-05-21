#!/usr/bin/env python3
"""Validate objc3c package channels end to end from the generated artifacts."""

from __future__ import annotations

import json
import os
import hashlib
import shutil
import subprocess
import zipfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import validate_json_schema
from objc3c_tooling.subprocesses import python_script_command, run_capture
from objc3c_tooling.public_workflow_output import extract_output_value

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
BUILD_PACKAGE_CHANNELS_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
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


def extract_zip(zip_path: Path, destination: Path) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path) as archive:
        archive.extractall(destination)


def load_valid_install_receipt(receipt_path: Path, receipt_schema: dict[str, Any], install_root: Path) -> dict[str, Any]:
    receipt = load_json(receipt_path)
    validate_json_schema(receipt, receipt_schema, label=repo_rel(receipt_path))
    expect(receipt["install_root"] == str(install_root), "install receipt root drifted from requested install root")
    expect(receipt["install_home"] == str(install_root / "objc3c"), "install receipt home drifted from requested install root")
    expect(receipt["bootstrap_entrypoint"] == "Bootstrap-objc3cEnvironment.ps1", "install receipt bootstrap entrypoint drifted")
    expect(receipt["package_bridge"] == "objc3c", "install receipt package bridge drifted")
    expect(receipt["install_command"] == "npm run objc3c -- build-package-channels", "install receipt command drifted")
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




def main() -> int:
    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    work_root = ROOT / "tmp" / "pkg" / "objc3c-package-channels-e2e" / run_id
    portable_extract_root = work_root / "portable-extract"
    installer_extract_root = work_root / "installer-extract"
    offline_extract_root = work_root / "offline-extract"
    install_root = work_root / "install-root"
    offline_install_root = work_root / "offline-install-root"

    build_result = run_capture(python_script_command(BUILD_PACKAGE_CHANNELS_PY), cwd=ROOT, capture_output=False)
    if build_result.returncode != 0:
        raise RuntimeError("package-channels build failed")

    source_surface = load_json(SOURCE_SURFACE)
    owner_policy = source_surface.get("owner_policy")
    if not isinstance(owner_policy, dict) or owner_policy.get("evidence_log_allowed") is not False:
        raise RuntimeError("packaging-channel source surface missing source-owned owner_policy")
    blocker_metadata = source_surface.get("blocker_metadata")
    if not isinstance(blocker_metadata, dict) or blocker_metadata.get("blocker_owner") != "packaging-channels-blockers":
        raise RuntimeError("packaging-channel source surface missing blocker metadata")

    summary = load_json(REPORT_PATH)
    manifest_path = ROOT / str(summary["manifest_path"]).replace("/", os.sep)
    manifest = load_json(manifest_path)

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
    installed_exe = install_root / "objc3c" / "artifacts" / "bin" / "objc3c-native.exe"
    expect(receipt_path.is_file(), "installer did not publish install receipt")
    expect(bootstrap_script.is_file(), "installer did not publish bootstrap script")
    expect(installed_exe.is_file(), "installer did not publish installed native executable")

    receipt_schema = load_json(INSTALL_RECEIPT_SCHEMA)
    load_valid_install_receipt(receipt_path, receipt_schema, install_root)

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
    load_valid_install_receipt(offline_receipt_path, receipt_schema, offline_install_root)
    expect((offline_install_root / "objc3c" / "artifacts" / "bin" / "objc3c-native.exe").is_file(), "offline bootstrap did not install native executable")

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
        "install_root": repo_rel(install_root),
        "offline_install_root": repo_rel(offline_install_root),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(end_to_end_summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-packaging-channels-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
