from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PLAN_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_b003_build_test_doc_workflow_runner_unification_core_feature_implementation_plan.json"


def test_plan_routes_all_public_scripts_through_runner() -> None:
    payload = json.loads(PLAN_JSON.read_text(encoding="utf-8"))
    assert len(payload["public_script_routes"]) == 17
    assert payload["runner_script"] == "scripts/objc3c_public_workflow_runner.py"


def test_plan_keeps_build_spec_under_runner() -> None:
    payload = json.loads(PLAN_JSON.read_text(encoding="utf-8"))
    assert payload["public_script_routes"]["build:spec"] == "build-spec"
