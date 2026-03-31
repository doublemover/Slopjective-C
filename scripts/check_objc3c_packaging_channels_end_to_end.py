#!/usr/bin/env python3
"""Validate objc3c package channels end to end from the generated artifacts."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
import zipfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
BUILD_PACKAGE_CHANNELS_PY = ROOT / "scripts" / "build_objc3c_package_channels.py"
REPORT_PATH = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
INSTALL_RECEIPT_SCHEMA = ROOT / "schemas" / "objc3c-package-install-receipt-v1.schema.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "end-to-end-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run_capture(
    command: Sequence[str],
    *,
    cwd: Path,
    capture_output: bool = True,
) -> subprocess.CompletedProcess[str]:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(
        list(command),
        cwd=cwd,
        text=True,
        capture_output=capture_output,
        check=False,
        env=env,
    )
    if capture_output and result.stdout:
        sys.stdout.write(result.stdout)
    if capture_output and result.stderr:
        sys.stderr.write(result.stderr)
    return result


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def extract_zip(zip_path: Path, destination: Path) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path) as archive:
        archive.extractall(destination)


def extract_output_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return None


def main() -> int:
    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    work_root = ROOT / "tmp" / "pkg" / "objc3c-package-channels-e2e" / run_id
    portable_extract_root = work_root / "portable-extract"
    installer_extract_root = work_root / "installer-extract"
    offline_extract_root = work_root / "offline-extract"
    install_root = work_root / "install-root"
    offline_install_root = work_root / "offline-install-root"

    build_result = run_capture([sys.executable, str(BUILD_PACKAGE_CHANNELS_PY)], cwd=ROOT, capture_output=False)
    if build_result.returncode != 0:
        raise RuntimeError("package-channels build failed")

    summary = load_json(REPORT_PATH)
    manifest_path = ROOT / str(summary["manifest_path"]).replace("/", os.sep)
    manifest = load_json(manifest_path)

    portable_archive = ROOT / str(summary["portable_archive"]).replace("/", os.sep)
    installer_archive = ROOT / str(summary["installer_archive"]).replace("/", os.sep)
    offline_archive = ROOT / str(summary["offline_archive"]).replace("/", os.sep)
    expect(portable_archive.is_file(), "portable archive was not published")
    expect(installer_archive.is_file(), "installer archive was not published")
    expect(offline_archive.is_file(), "offline archive was not published")

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

    receipt = load_json(receipt_path)
    receipt_schema = load_json(INSTALL_RECEIPT_SCHEMA)
    for required_key in receipt_schema["required"]:
        expect(required_key in receipt, f"install receipt missing required field {required_key}")

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
    expect((offline_install_root / "objc3c-install-receipt.json").is_file(), "offline bootstrap did not publish install receipt")
    expect((offline_install_root / "objc3c" / "artifacts" / "bin" / "objc3c-native.exe").is_file(), "offline bootstrap did not install native executable")

    end_to_end_summary = {
        "contract_id": "objc3c.packaging.channels.end-to-end.summary.v1",
        "status": "PASS",
        "build_report": repo_rel(REPORT_PATH),
        "manifest_path": repo_rel(manifest_path),
        "package_root": manifest["package_root"],
        "portable_archive": repo_rel(portable_archive),
        "installer_archive": repo_rel(installer_archive),
        "offline_archive": repo_rel(offline_archive),
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
