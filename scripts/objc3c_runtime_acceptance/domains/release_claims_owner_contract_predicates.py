"""Release-claims owner contract lookup predicates."""

from __future__ import annotations

from .release_claims_owner_contract_data import (
    RELEASE_CLAIMS_OWNER_CONTRACTS,
    ReleaseClaimsOwnerContract,
)


_CASE_OWNER_CONTRACTS: dict[str, ReleaseClaimsOwnerContract] = {
    case_id: contract
    for contract in RELEASE_CLAIMS_OWNER_CONTRACTS
    for case_id in contract.case_ids
}


def release_claims_owner_contract_for_case(
    case_id: str,
) -> ReleaseClaimsOwnerContract | None:
    return _CASE_OWNER_CONTRACTS.get(case_id)


def release_claims_case_has_owner_contract(case_id: str) -> bool:
    return case_id in _CASE_OWNER_CONTRACTS


__all__ = [
    "release_claims_case_has_owner_contract",
    "release_claims_owner_contract_for_case",
]
