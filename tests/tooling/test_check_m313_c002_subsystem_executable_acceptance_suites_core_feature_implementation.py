from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PLAN = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c002_subsystem_executable_acceptance_suites_core_feature_implementation_plan.json"


def test_plan_freezes_required_harness_modes() -> None:
    payload = json.loads(PLAN.read_text(encoding="utf-8"))
    assert payload["required_harness_modes"] == ["list-suites", "show-suite", "check-roots", "run-suite"]


def test_plan_tracks_all_a003_suite_ids() -> None:
    payload = json.loads(PLAN.read_text(encoding="utf-8"))
    assert [entry["suite_id"] for entry in payload["suite_execution_plan"]] == [
        "runtime_bootstrap_dispatch",
        "frontend_split_recovery",
        "module_parity_packaging",
        "native_fixture_corpus_and_runtime_probes",
    ]
