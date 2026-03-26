from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MODEL = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_model.json"


def test_model_freezes_pyramid_layer_order() -> None:
    payload = json.loads(MODEL.read_text(encoding="utf-8"))
    assert [entry["layer"] for entry in payload["pyramid_layers"]] == [
        "executable_acceptance_suites",
        "shared_integration_harnesses",
        "retained_static_guards",
        "migration_only_wrappers",
    ]


def test_model_freezes_retained_and_prohibited_classes() -> None:
    payload = json.loads(MODEL.read_text(encoding="utf-8"))
    assert payload["retained_static_guard_classes"] == [
        "schema_contract",
        "inventory_contract",
        "publication_consistency",
        "policy_budget_enforcement",
    ]
    assert payload["migration_only_classes"] == [
        "issue_local_checker",
        "issue_local_readiness_runner",
        "issue_local_pytest_wrapper",
    ]
    assert payload["next_issue"] == "M313-B002"
