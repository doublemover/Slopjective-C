"""Release-claims source-surface catalog assembly."""

from __future__ import annotations

from collections.abc import Iterable

from .models import ReleaseClaimGroup, ReleaseClaimsSourceSurfaceData


def assemble_release_claim_source_surface(
    group: ReleaseClaimGroup,
) -> ReleaseClaimsSourceSurfaceData:
    return ReleaseClaimsSourceSurfaceData(
        contract_id=group.contract_id,
        case_ids=group.case_ids,
        pre_case_fields=group.pre_case_fields,
        fixture_paths=group.fixture_paths,
        post_fixture_fields=group.post_fixture_fields,
    )


def assemble_release_claim_source_surface_catalog(
    groups: Iterable[ReleaseClaimGroup],
) -> tuple[ReleaseClaimsSourceSurfaceData, ...]:
    return tuple(assemble_release_claim_source_surface(group) for group in groups)


__all__ = [
    "assemble_release_claim_source_surface",
    "assemble_release_claim_source_surface_catalog",
]
