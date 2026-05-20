from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "runtime_import_interop_bridge_metadata"
CHECKER = ROOT / "scripts" / "check_runtime_import_interop_bridge_metadata.py"


def _run_checker(fixture_name: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(CHECKER), str(FIXTURE_ROOT / fixture_name)],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def test_runtime_import_interop_bridge_metadata_accepts_canonical_surface() -> None:
    result = _run_checker("valid_bridge_surface.json")

    assert result.returncode == 0
    assert result.stderr == ""


def test_runtime_import_interop_bridge_metadata_rejects_tampered_surface() -> None:
    result = _run_checker("tampered_bridge_surface.json")

    assert result.returncode == 1
    assert "header artifact path traverses directories" in result.stderr
    assert "active bridge packet has no C++ or Swift-facing metadata" in result.stderr
