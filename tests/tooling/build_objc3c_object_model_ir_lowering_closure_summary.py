from __future__ import annotations

import json
from pathlib import Path
from typing import Any, cast

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "object-model-ir-lowering"
    / "object_model_ir_lowering_summary.json"
)


def load_object_model_ir_lowering_closure_summary() -> dict[str, Any]:
    return cast(dict[str, Any], json.loads(SUMMARY.read_text(encoding="utf-8")))
