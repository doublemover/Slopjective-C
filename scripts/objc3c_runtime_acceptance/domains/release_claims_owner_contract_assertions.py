"""Release-claims owner contract validation helpers."""

from __future__ import annotations

from collections.abc import Iterable

from .release_claims_owner_contract_data import (
    RELEASE_CLAIMS_OWNER_CASE_IDS,
    RELEASE_CLAIMS_REQUIRED_OWNER_IDS,
    ReleaseClaimsOwnerContract,
)
from .release_claims_owner_contract_predicates import (
    release_claims_owner_contract_for_case,
)


def require_release_claims_owner_contract(
    case_id: str,
) -> ReleaseClaimsOwnerContract:
    contract = release_claims_owner_contract_for_case(case_id)
    if contract is None:
        raise ValueError(f"unowned release-claims runtime acceptance case: {case_id}")
    return contract


def assert_release_claims_owner_case_ids(case_ids: Iterable[str]) -> None:
    expected = set(RELEASE_CLAIMS_OWNER_CASE_IDS)
    actual = set(case_ids)
    if actual != expected:
        missing = sorted(expected - actual)
        extra = sorted(actual - expected)
        raise ValueError(
            "release-claims owner case inventory drift: "
            f"missing={missing} extra={extra}"
        )


def assert_release_claims_required_owner_ids(owner_ids: Iterable[str]) -> None:
    expected = set(RELEASE_CLAIMS_REQUIRED_OWNER_IDS)
    actual = set(owner_ids)
    if actual != expected:
        missing = sorted(expected - actual)
        extra = sorted(actual - expected)
        raise ValueError(
            "release-claims required owner inventory drift: "
            f"missing={missing} extra={extra}"
        )


__all__ = [
    "assert_release_claims_owner_case_ids",
    "assert_release_claims_required_owner_ids",
    "require_release_claims_owner_contract",
]
