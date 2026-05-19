from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from scripts.objc3c_workflow.actions.performance_owner_contracts import (
    OWNER_CONTRACT_ID,
    forbidden_performance_claim_classes,
    load_performance_owner_contracts,
    owner_contract_index,
    required_public_claim_inputs,
    validate_performance_owner_contract_shape,
)

ROOT = Path(__file__).resolve().parents[2]

BUDGET_MODEL_PATH = "tests/tooling/fixtures/performance_governance/budget_model.json"
BREACH_TRIAGE_POLICY_PATH = (
    "tests/tooling/fixtures/performance_governance/breach_triage_policy.json"
)
LAB_POLICY_PATH = "tests/tooling/fixtures/performance_governance/lab_policy.json"
CLAIM_POLICY_PATH = "tests/tooling/fixtures/performance_governance/claim_policy.json"
MEASUREMENT_POLICY_PATH = "tests/tooling/fixtures/performance/measurement_policy.json"


def load_json(relative_path: str) -> dict[str, Any]:
    return json.loads((ROOT / relative_path).read_text(encoding="utf-8"))


def performance_owner_contract() -> dict[str, Any]:
    return load_performance_owner_contracts()


def budget_model_fixture() -> dict[str, Any]:
    return load_json(BUDGET_MODEL_PATH)


def breach_triage_policy_fixture() -> dict[str, Any]:
    return load_json(BREACH_TRIAGE_POLICY_PATH)


def lab_policy_fixture() -> dict[str, Any]:
    return load_json(LAB_POLICY_PATH)


def claim_policy_fixture() -> dict[str, Any]:
    return load_json(CLAIM_POLICY_PATH)


def measurement_policy_fixture() -> dict[str, Any]:
    return load_json(MEASUREMENT_POLICY_PATH)
