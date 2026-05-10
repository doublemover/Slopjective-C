from __future__ import annotations

import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "runnable-end-to-end-summary.json"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.foundation.runnable.end.to.end.summary.v1"
