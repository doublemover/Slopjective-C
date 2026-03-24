from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m273_c002_synthesized_ast_and_ir_emission_for_derives_macros_and_behaviors_core_feature_implementation.py"


def test_m273_c002_checker_static_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
