from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PLAN = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_plan.json"


def test_plan_tracks_expected_bridge_families() -> None:
    payload = json.loads(PLAN.read_text(encoding="utf-8"))
    assert [entry["bridge_id"] for entry in payload["bridge_families"]] == [
        "frontend_split_recovery_bridge",
        "runtime_bootstrap_dispatch_bridge",
        "module_parity_packaging_bridge",
        "native_fixture_corpus_and_runtime_probes_bridge",
    ]


def test_plan_freezes_default_report_root() -> None:
    payload = json.loads(PLAN.read_text(encoding="utf-8"))
    assert payload["default_report_root"] == "tmp/reports/m313/compatibility/<bridge_id>/summary.json"
