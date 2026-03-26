from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_c004_synthetic_fixture_relocation_labeling_and_parity_test_updates_core_feature_expansion.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-C004"
    / "synthetic_fixture_labeling_summary.json"
)


def test_m315_c004_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["relocation_decision"] == "filesystem_root_retained_labeling_promoted_to_authenticity_contract"
    assert summary["committed_manifest_authenticity_class"] == "synthetic_fixture"
    assert summary["golden_summary_authenticity_class"] == "synthetic_fixture"
    assert summary["a003_checker_ok"] is True
    assert summary["b004_checker_ok"] is True
    assert summary["parity_golden_ok"] is True
    assert summary["tampered_fail_closed"] is True
    assert summary["ok"] is True
