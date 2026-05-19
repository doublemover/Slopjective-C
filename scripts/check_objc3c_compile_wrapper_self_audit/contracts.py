"""Contract payloads for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

from typing import Any

from .config import (
    PROVENANCE_CONTRACT_ID,
    TRUTHFULNESS_CONTRACT_ID,
    WRAPPER_ARTIFACT_OWNER,
    WRAPPER_RESULT_OWNER,
    WRAPPER_STATUS_OWNER,
    WRAPPER_TRUTH_OWNER,
)


def wrapper_truth_owner_contract() -> dict[str, Any]:
    return {
        "wrapper_truth_owner": WRAPPER_TRUTH_OWNER,
        "result_owner": WRAPPER_RESULT_OWNER,
        "artifact_owner": WRAPPER_ARTIFACT_OWNER,
        "status_owner": WRAPPER_STATUS_OWNER,
        "truthfulness_contract_id": TRUTHFULNESS_CONTRACT_ID,
        "provenance_contract_id": PROVENANCE_CONTRACT_ID,
        "no_retired_route_or_evidence_log_claims": True,
    }


__all__ = ["wrapper_truth_owner_contract"]
