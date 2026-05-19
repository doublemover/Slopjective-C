from __future__ import annotations

import json
from pathlib import Path
from typing import Any, cast

ROOT = Path(__file__).resolve().parents[2]
REPORT = (
    ROOT
    / "reports"
    / "claimability"
    / "cross-module-semantic-contracts-diagnostics"
    / "cross_module_semantic_contracts_diagnostics_summary.json"
)


def load_cross_module_semantic_contracts_diagnostics_report() -> dict[str, Any]:
    return cast(dict[str, Any], json.loads(REPORT.read_text(encoding="utf-8")))
