from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / 'scripts' / 'check_m270_e001_strict_concurrency_conformance_gate_contract_and_architecture_freeze.py'
SUMMARY = ROOT / 'tmp' / 'reports' / 'm270' / 'M270-E001' / 'strict_concurrency_conformance_gate_summary.json'


def test_m270_e001_checker_writes_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), '--skip-dynamic-probes'],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding='utf-8'))
    assert payload['contract_id'] == 'objc3c-strict-concurrency-conformance-gate/m270-e001-v1'
    assert payload['mode'] == 'm270-e001-strict-concurrency-conformance-gate-v1'
    assert payload['next_closeout_issue'] == 'M270-E002'
    assert payload['dynamic']['skipped'] is True
    assert payload['upstream']['M270-D003']['contract_id'] == 'objc3c-cross-module-actor-isolation-metadata-hardening/m270-d003-v1'
    assert payload['ok'] is True
