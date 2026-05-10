from __future__ import annotations

import json
from pathlib import Path
from typing import Any, cast

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "manifest-object-ir-truth-gate"
    / "manifest_object_ir_truth_gate_summary.json"
)


def load_manifest_object_ir_truth_gate_summary() -> dict[str, Any]:
    return cast(dict[str, Any], json.loads(SUMMARY.read_text(encoding="utf-8")))
