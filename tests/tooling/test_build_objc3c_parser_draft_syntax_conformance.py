from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def test_parser_draft_syntax_conformance_report_is_current() -> None:
    completed = subprocess.run(
        [sys.executable, "scripts/build_objc3c_parser_draft_syntax_conformance.py", "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
