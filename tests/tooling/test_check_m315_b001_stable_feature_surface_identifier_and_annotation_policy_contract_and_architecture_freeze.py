from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
POLICY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b001_stable_feature_surface_identifier_and_annotation_policy_contract_and_architecture_freeze_policy.json"


def test_policy_freezes_identifier_regex() -> None:
    payload = json.loads(POLICY_JSON.read_text(encoding="utf-8"))
    assert payload["durable_identifier_grammar"]["regex"] == "^objc3c\\.[a-z][a-z0-9]*(\\.[a-z][a-z0-9]*)+\\.v[0-9]+$"


def test_policy_hands_annotation_work_to_b002() -> None:
    payload = json.loads(POLICY_JSON.read_text(encoding="utf-8"))
    assert payload["downstream_ownership"]["product_code_annotation_policy"] == "M315-B002"
