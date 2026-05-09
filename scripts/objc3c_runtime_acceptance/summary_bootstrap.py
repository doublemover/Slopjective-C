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
from objc3c_runtime_acceptance.c_api import PUBLIC_RUNTIME_ABI_BOUNDARY
from objc3c_runtime_acceptance.summary_owner_contracts import (
    build_domain_summary_owner_payload,
)


def build_bootstrap_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_bootstrap_summary_owner": build_domain_summary_owner_payload(
            owner_module="summary_bootstrap",
            domain="bootstrap",
        ),
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
