from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_e001_validation_noise_reduction_gate_contract_and_architecture_freeze_contract.json"


def test_gate_contract_tracks_closeout_maximums() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert payload["gate_threshold_maximums"] == {
        "check_scripts": 558,
        "readiness_runners": 179,
        "pytest_check_files": 555,
    }


def test_gate_contract_tracks_expected_bridge_owners() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert payload["required_bridge_deprecation_owners"]["native_fixture_corpus_and_runtime_probes_bridge"] == "M313-E002"
