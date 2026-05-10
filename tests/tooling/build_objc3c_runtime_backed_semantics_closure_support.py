from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any, cast

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "build_objc3c_runtime_backed_semantics_closure.py"
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "runtime-backed-semantics-closure"
    / "runtime_backed_semantics_closure_summary.json"
)


def run_runtime_backed_semantics_closure_check() -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )


def load_runtime_backed_semantics_closure_summary() -> dict[str, Any]:
    return cast(dict[str, Any], json.loads(SUMMARY.read_text(encoding="utf-8")))
