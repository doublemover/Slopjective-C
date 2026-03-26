from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PLAN = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d002_ci_migration_to_acceptance_first_validation_core_feature_implementation_plan.json"


def test_plan_tracks_expected_runner_stages() -> None:
    payload = json.loads(PLAN.read_text(encoding="utf-8"))
    assert payload["supported_stages"] == [
        "static-guards",
        "acceptance-suites",
        "compatibility-bridges",
        "topology",
    ]


def test_plan_freezes_expected_workflow_path() -> None:
    payload = json.loads(PLAN.read_text(encoding="utf-8"))
    assert payload["workflow_path"] == ".github/workflows/m313-validation-acceptance-first.yml"
