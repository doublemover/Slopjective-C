"""Runtime bootstrap summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.probe_helpers import (
    build_runtime_bootstrap_lowering_registration_artifact_surface,
    build_runtime_bootstrap_registration_source_surface,
    build_runtime_state_publication_surface,
)
from objc3c_runtime_acceptance.runtime_contracts import PUBLIC_RUNTIME_ABI_BOUNDARY


def build_bootstrap_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_state_publication_surface": build_runtime_state_publication_surface(
            PUBLIC_RUNTIME_ABI_BOUNDARY
        ),
        "runtime_bootstrap_registration_source_surface": build_runtime_bootstrap_registration_source_surface(),
        "runtime_bootstrap_lowering_registration_artifact_surface": (
            build_runtime_bootstrap_lowering_registration_artifact_surface()
        ),
        "runtime_multi_image_startup_ordering_source_surface": (
            domains.registration.build_runtime_multi_image_startup_ordering_source_surface(
                results
            )
        ),
    }


__all__ = ["build_bootstrap_summary_sections"]
