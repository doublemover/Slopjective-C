"""Release-claims owner contract evidence payload helpers."""

from __future__ import annotations

from typing import Any

from .release_claims_owner_contract_assertions import (
    require_release_claims_owner_contract,
)
from .release_claims_owner_contract_data import (
    RELEASE_CLAIMS_OWNER_CASE_IDS,
    RELEASE_CLAIMS_OWNER_CONTRACT_ID,
    RELEASE_CLAIMS_OWNER_CONTRACTS,
    RELEASE_CLAIMS_REQUIRED_OWNER_IDS,
    RELEASE_CLAIMS_STRICT_STATUS_CONTRACT_ID,
    STRICT_STATUS_OWNER,
)


def release_claims_owner_contract_payloads() -> list[dict[str, Any]]:
    return [contract.payload() for contract in RELEASE_CLAIMS_OWNER_CONTRACTS]


def release_claims_strict_status_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": RELEASE_CLAIMS_STRICT_STATUS_CONTRACT_ID,
        "owner_id": STRICT_STATUS_OWNER,
        "owner_surface": "release-claims runtime acceptance status",
        "case_ids": list(RELEASE_CLAIMS_OWNER_CASE_IDS),
        "status_policy": (
            "release-claim acceptance status is derived from current CaseResult "
            "payloads and owner coverage raises on unmapped cases"
        ),
    }


def release_claims_case_owner_payload(case_id: str) -> dict[str, Any]:
    contract = require_release_claims_owner_contract(case_id)
    payload = contract.payload()
    payload["strict_status_owner"] = release_claims_strict_status_owner_payload()
    return payload


def release_claims_case_summary(
    case_id: str,
    summary: dict[str, Any],
) -> dict[str, Any]:
    return {
        "owner_contract": release_claims_case_owner_payload(case_id),
        **summary,
    }


def release_claims_surface_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": RELEASE_CLAIMS_OWNER_CONTRACT_ID,
        "owner_ids": list(RELEASE_CLAIMS_REQUIRED_OWNER_IDS),
        "owner_contracts": release_claims_owner_contract_payloads(),
        "strict_status_owner": release_claims_strict_status_owner_payload(),
    }


__all__ = [
    "release_claims_case_owner_payload",
    "release_claims_case_summary",
    "release_claims_owner_contract_payloads",
    "release_claims_strict_status_owner_payload",
    "release_claims_surface_owner_payload",
]
