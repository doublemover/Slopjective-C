"""Paths and contract IDs for runnable release-candidate end-to-end validation."""

from __future__ import annotations

import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "runnable-release-candidate-e2e" / "summary.json"
)
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.release.candidate.e2e.summary.v1"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
RUNNER_PATH = "scripts/check_objc3c_runnable_release_candidate_end_to_end.py"


__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "RUNNER_PATH",
    "SUMMARY_CONTRACT_ID",
]
