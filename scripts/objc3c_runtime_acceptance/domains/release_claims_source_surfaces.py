"""Release-claims source and policy runtime acceptance surfaces."""

from __future__ import annotations

from pathlib import Path


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .release_claims_source_surfaces.payloads import (  # noqa: E402
    build_runtime_claimability_semantics_release_policy_surface,
    build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface,
    build_runtime_strict_profile_claim_implementation_surface,
    build_runtime_strict_profile_feature_claim_source_surface,
)


__all__ = [
    "build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
    "build_runtime_strict_profile_feature_claim_source_surface",
    "build_runtime_claimability_semantics_release_policy_surface",
    "build_runtime_strict_profile_claim_implementation_surface",
]

for _exported_name in __all__:
    globals()[_exported_name].__module__ = __name__

del _exported_name
