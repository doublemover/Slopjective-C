from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
POLICY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b002_product_code_annotation_and_provenance_policy_contract_and_architecture_freeze_policy.json"


def test_policy_freezes_provenance_class_vocabulary() -> None:
    payload = json.loads(POLICY_JSON.read_text(encoding="utf-8"))
    assert payload["provenance_classes"] == [
        "synthetic_fixture",
        "sample_or_example",
        "generated_report",
        "generated_replay",
        "schema_policy_contract",
        "historical_archive",
    ]


def test_policy_hands_native_source_removal_to_b003() -> None:
    payload = json.loads(POLICY_JSON.read_text(encoding="utf-8"))
    assert payload["downstream_ownership"]["native_source_marker_removal"] == "M315-B003"
