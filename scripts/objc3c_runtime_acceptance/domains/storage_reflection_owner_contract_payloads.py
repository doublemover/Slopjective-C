"""Storage/reflection owner contract payload helpers."""

from __future__ import annotations

from typing import Any

from .storage_reflection_owner_contract_catalog import (
    STORAGE_REFLECTION_OWNER_CONTRACTS,
    _CASE_OWNER_CONTRACTS,
)
from .storage_reflection_owner_contract_inventory import (
    STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS,
    STORAGE_REFLECTION_OWNER_CASE_IDS,
    STORAGE_REFLECTION_OWNER_CONTRACT_ID,
    STORAGE_REFLECTION_REQUIRED_OWNER_IDS,
    STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID,
    STRICT_STATUS_OWNER,
)


def storage_reflection_owner_contract_payloads() -> list[dict[str, Any]]:
    return [contract.payload() for contract in STORAGE_REFLECTION_OWNER_CONTRACTS]


def storage_reflection_strict_status_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID,
        "owner_id": STRICT_STATUS_OWNER,
        "owner_surface": "storage-reflection runtime acceptance status",
        "case_ids": list(STORAGE_REFLECTION_OWNER_CASE_IDS),
        "workflow_actions": [
            "validate-storage-reflection-conformance",
            "validate-runnable-storage-reflection",
        ],
        "status_policy": (
            "case status is derived from CaseResult pass/fail data and missing "
            "owner coverage raises immediately"
        ),
    }


def storage_reflection_case_owner_payload(case_id: str) -> dict[str, Any]:
    contract = _CASE_OWNER_CONTRACTS.get(case_id)
    if contract is None:
        raise ValueError(f"unowned storage/reflection runtime acceptance case: {case_id}")
    payload = contract.payload()
    payload["strict_status_owner"] = storage_reflection_strict_status_owner_payload()
    return payload


def storage_reflection_case_summary(
    case_id: str,
    summary: dict[str, Any],
) -> dict[str, Any]:
    return {
        "owner_contract": storage_reflection_case_owner_payload(case_id),
        **summary,
    }


def storage_reflection_surface_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": STORAGE_REFLECTION_OWNER_CONTRACT_ID,
        "owner_ids": list(STORAGE_REFLECTION_REQUIRED_OWNER_IDS),
        "owner_contracts": storage_reflection_owner_contract_payloads(),
        "strict_status_owner": storage_reflection_strict_status_owner_payload(),
    }


def assert_storage_reflection_owner_case_ids(case_ids: tuple[str, ...]) -> None:
    expected = set(STORAGE_REFLECTION_OWNER_CASE_IDS)
    actual = set(case_ids)
    if actual != expected:
        missing = sorted(expected - actual)
        extra = sorted(actual - expected)
        raise ValueError(
            "storage/reflection owner case inventory drift: "
            f"missing={missing} extra={extra}"
        )


def assert_storage_reflection_direct_factory_case_ids(
    case_ids: tuple[str, ...],
) -> None:
    expected = set(STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS)
    actual = set(case_ids)
    if actual != expected:
        missing = sorted(expected - actual)
        extra = sorted(actual - expected)
        raise ValueError(
            "storage/reflection direct factory owner drift: "
            f"missing={missing} extra={extra}"
        )


__all__ = [
    "assert_storage_reflection_direct_factory_case_ids",
    "assert_storage_reflection_owner_case_ids",
    "storage_reflection_case_owner_payload",
    "storage_reflection_case_summary",
    "storage_reflection_owner_contract_payloads",
    "storage_reflection_strict_status_owner_payload",
    "storage_reflection_surface_owner_payload",
]
