from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCHEMA = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_schema.json"


def test_schema_freezes_required_envelope_fields() -> None:
    payload = json.loads(SCHEMA.read_text(encoding="utf-8"))
    assert payload["required_envelope_fields"] == [
        "schema_version",
        "contract_id",
        "suite_id",
        "artifact_class",
        "producer",
        "ok",
        "inputs",
        "outputs",
        "replay",
        "measurements",
    ]


def test_schema_reserves_compatibility_bridge_artifact_fields() -> None:
    payload = json.loads(SCHEMA.read_text(encoding="utf-8"))
    assert payload["artifact_classes"]["compatibility_bridge_summary"]["required_measurement_fields"] == [
        "legacy_wrappers_observed",
        "legacy_wrappers_remaining",
        "deprecation_owner_issue",
    ]
