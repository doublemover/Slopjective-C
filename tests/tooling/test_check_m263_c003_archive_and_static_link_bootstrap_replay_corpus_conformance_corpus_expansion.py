from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m263_c003_archive_and_static_link_bootstrap_replay_corpus_conformance_corpus_expansion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-C003" / "pytest_archive_static_link_bootstrap_replay_corpus_summary.json"


def test_m263_c003_static_contract_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1"
    assert payload["dynamic_probes_executed"] is False
