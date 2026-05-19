"""Release-claims source-surface payload shaping."""

from __future__ import annotations

from typing import Any

from ...case_result import CaseResult
from ..release_claims_owner_contracts import release_claims_surface_owner_payload
from .assertions import expect_release_claims_source_surface_data
from .catalog import (
    CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SOURCE_SURFACE,
    CLAIMABLE_RESIDUAL_SOURCE_SURFACE,
    STRICT_PROFILE_CLAIM_IMPLEMENTATION_SOURCE_SURFACE,
    STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE,
)
from .data import PayloadField, ReleaseClaimsSourceSurfaceData
from .predicates import authoritative_case_ids


def build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return release_claims_source_surface_payload(
        results,
        CLAIMABLE_RESIDUAL_SOURCE_SURFACE,
    )


def build_runtime_strict_profile_feature_claim_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return release_claims_source_surface_payload(
        results,
        STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE,
    )


def build_runtime_claimability_semantics_release_policy_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return release_claims_source_surface_payload(
        results,
        CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SOURCE_SURFACE,
    )


def build_runtime_strict_profile_claim_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return release_claims_source_surface_payload(
        results,
        STRICT_PROFILE_CLAIM_IMPLEMENTATION_SOURCE_SURFACE,
    )


def release_claims_source_surface_payload(
    results: list[CaseResult],
    surface: ReleaseClaimsSourceSurfaceData,
) -> dict[str, Any]:
    expect_release_claims_source_surface_data(surface)
    payload: dict[str, Any] = {
        "contract_id": surface.contract_id,
        "owner_contract": release_claims_surface_owner_payload(),
    }
    _add_payload_fields(payload, surface.pre_case_fields)
    payload["authoritative_case_ids"] = authoritative_case_ids(
        results,
        surface.case_ids,
    )
    payload["authoritative_fixture_paths"] = _shape_payload_value(
        surface.fixture_paths
    )
    _add_payload_fields(payload, surface.post_fixture_fields)
    return payload


def _add_payload_fields(
    payload: dict[str, Any],
    fields: tuple[PayloadField, ...],
) -> None:
    for key, value in fields:
        payload[key] = _shape_payload_value(value)


def _shape_payload_value(value: Any) -> Any:
    if isinstance(value, tuple):
        return [_shape_payload_value(item) for item in value]
    if isinstance(value, dict):
        return {
            key: _shape_payload_value(nested_value)
            for key, nested_value in value.items()
        }
    return value


__all__ = [
    "build_runtime_claimability_semantics_release_policy_surface",
    "build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
    "build_runtime_strict_profile_claim_implementation_surface",
    "build_runtime_strict_profile_feature_claim_source_surface",
    "release_claims_source_surface_payload",
]
