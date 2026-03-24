from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / 'scripts' / 'check_m274_c003_metadata_and_interface_preservation_across_ffi_boundaries_core_feature_expansion.py'


def test_checker_passes_static(tmp_path: Path) -> None:
    summary_out = tmp_path / 'summary-static.json'
    completed = subprocess.run(
        [sys.executable, str(CHECKER), '--skip-dynamic-probes', '--summary-out', str(summary_out)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    assert summary_out.exists()


def test_checker_passes_dynamic(tmp_path: Path) -> None:
    summary_out = tmp_path / 'summary-dynamic.json'
    completed = subprocess.run(
        [sys.executable, str(CHECKER), '--summary-out', str(summary_out)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    assert summary_out.exists()
