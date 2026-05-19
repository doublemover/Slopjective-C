"""Configuration for the runnable storage/reflection E2E checker."""

from __future__ import annotations

import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "runtime"
    / "runnable-storage-reflection-e2e"
    / "summary.json"
)
RUNNER_PATH = "scripts/check_objc3c_runnable_storage_reflection_end_to_end.py"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.storage.reflection.e2e.summary.v1"
PACKAGE_CONTRACT_ID = (
    "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
)
MANIFEST_KEYS = (
    "compile_wrapper",
    "runtime_library",
    "execution_smoke_script",
    "execution_replay_script",
    "runtime_public_header",
    "runtime_internal_header",
    "storage_reflection_fixture",
    "storage_reflection_probe",
)

__all__ = [
    "MANIFEST_KEYS",
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "RUNNER_PATH",
    "SUMMARY_CONTRACT_ID",
]
