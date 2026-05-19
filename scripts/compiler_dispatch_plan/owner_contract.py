from __future__ import annotations

from typing import Any

from compiler_dispatch_plan.constants import COMPILER_DISPATCH_ARTIFACT_OWNER
from compiler_dispatch_plan.constants import COMPILER_DISPATCH_FIXTURE_CONTRACT_ID
from compiler_dispatch_plan.constants import COMPILER_DISPATCH_OWNER
from compiler_dispatch_plan.constants import COMPILER_DISPATCH_RESULT_OWNER
from compiler_dispatch_plan.constants import COMPILER_DISPATCH_SNAPSHOT_OWNER
from compiler_dispatch_plan.constants import COMPILER_DISPATCH_STATUS_OWNER
from compiler_dispatch_plan.constants import OWNER_CONTRACT_FIELDS


def dispatch_owner_contract() -> dict[str, Any]:
    return {
        "contract_id": COMPILER_DISPATCH_FIXTURE_CONTRACT_ID,
        "dispatch_owner": COMPILER_DISPATCH_OWNER,
        "snapshot_owner": COMPILER_DISPATCH_SNAPSHOT_OWNER,
        "result_owner": COMPILER_DISPATCH_RESULT_OWNER,
        "artifact_owner": COMPILER_DISPATCH_ARTIFACT_OWNER,
        "status_owner": COMPILER_DISPATCH_STATUS_OWNER,
        "no_retired_route_or_evidence_log_claims": True,
    }


def validate_fixture_owner_contract(payload: Any) -> None:
    if not isinstance(payload, dict):
        return
    contract = payload.get("fixture_contract")
    if contract is None:
        return
    if not isinstance(contract, dict):
        raise ValueError("fixture_contract must be an object when present")

    missing = [field for field in OWNER_CONTRACT_FIELDS if field not in contract]
    if missing:
        raise ValueError("fixture_contract missing owner fields: " + ", ".join(missing))

    expected = dispatch_owner_contract()
    for field in OWNER_CONTRACT_FIELDS:
        if contract.get(field) != expected[field]:
            raise ValueError(
                f"fixture_contract field {field} drifted from dispatch owner"
            )
