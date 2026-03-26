from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
POLICY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_policy.json"
REGISTRY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_exception_registry.json"


def test_policy_freezes_required_exception_fields() -> None:
    payload = json.loads(POLICY.read_text(encoding="utf-8"))
    assert payload["exception_record_required_fields"] == [
        "id",
        "reason",
        "owner_issue",
        "created_by_issue",
        "expiry_issue",
        "replacement_target",
        "approved_by",
        "validation_class",
        "status",
    ]


def test_registry_is_empty_by_default() -> None:
    payload = json.loads(REGISTRY.read_text(encoding="utf-8"))
    assert payload["exceptions"] == []
