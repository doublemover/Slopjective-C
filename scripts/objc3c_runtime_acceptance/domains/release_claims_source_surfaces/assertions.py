"""Release-claims source-surface catalog assertions."""

from __future__ import annotations

from collections.abc import Iterable

from .data import PayloadField, ReleaseClaimsSourceSurfaceData


_RESERVED_PAYLOAD_KEYS = frozenset(
    {
        "contract_id",
        "owner_contract",
        "authoritative_case_ids",
        "authoritative_fixture_paths",
    }
)


def expect_release_claims_source_surface_data(
    surface: ReleaseClaimsSourceSurfaceData,
) -> None:
    if not surface.contract_id:
        raise RuntimeError("release-claims source surface is missing a contract id")
    if not surface.case_ids:
        raise RuntimeError("release-claims source surface is missing case ids")
    _expect_payload_fields(surface.pre_case_fields, "pre-case")
    _expect_payload_fields(surface.post_fixture_fields, "post-fixture")


def _expect_payload_fields(fields: Iterable[PayloadField], field_group: str) -> None:
    seen: set[str] = set()
    for key, _ in fields:
        if key in _RESERVED_PAYLOAD_KEYS:
            raise RuntimeError(
                f"release-claims source surface {field_group} field {key!r} is reserved"
            )
        if key in seen:
            raise RuntimeError(
                f"release-claims source surface {field_group} field {key!r} is duplicated"
            )
        seen.add(key)


__all__ = [
    "expect_release_claims_source_surface_data",
]
