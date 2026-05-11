"""Constants for runnable developer-tooling end-to-end validation."""

from __future__ import annotations

import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "packaged_cli_to_editor_contract.json"
)
REPORT_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "runtime"
    / "runnable-developer-tooling-e2e"
    / "summary.json"
)
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.developer.tooling.e2e.summary.v1"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
RUNNER_PATH = "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py"


__all__ = [
    "CONTRACT_PATH",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "RUNNER_PATH",
    "SUMMARY_CONTRACT_ID",
]
