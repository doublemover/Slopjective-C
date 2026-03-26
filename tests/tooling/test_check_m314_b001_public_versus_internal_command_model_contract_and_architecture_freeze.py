from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MODEL_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b001_public_versus_internal_command_model_contract_and_architecture_freeze_model.json"


def test_model_freezes_public_families() -> None:
    payload = json.loads(MODEL_JSON.read_text(encoding="utf-8"))
    assert payload["public_operator_layer"]["families"] == [
        "build",
        "compile",
        "lint",
        "test",
        "package",
        "tool",
        "proof",
    ]


def test_model_marks_legacy_aliases_temporary() -> None:
    payload = json.loads(MODEL_JSON.read_text(encoding="utf-8"))
    assert "legacy milestone-local package aliases" in payload["temporary_compatibility_surface"]["categories"]
