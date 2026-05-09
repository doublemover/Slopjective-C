"""Acceptance suite and runtime ABI summary sections."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.dispatch import (
    build_dispatch_accessor_runtime_abi_surface,
    build_property_ivar_accessor_reflection_implementation_surface,
    build_storage_accessor_runtime_abi_surface,
)
from objc3c_runtime_acceptance.compile_truth import (
    COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
)
from objc3c_runtime_acceptance.compile_truth import COMPILE_PROVENANCE_CONTRACT_ID
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.domains.probe_helpers import (
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_registration import (
    RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.surfaces import build_acceptance_suite_surface
from objc3c_runtime_acceptance.summary_owner_contracts import (
    SUMMARY_OWNER_SURFACE,
    build_summary_section_owner_payload,
)


def build_suite_and_abi_summary_sections(
    *,
    results: list[CaseResult],
    report_path: Path,
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_suite_summary_owner": build_summary_section_owner_payload(
            owner_module="summary_suite_sections",
            owner_surface=SUMMARY_OWNER_SURFACE,
            section_group="suite-and-runtime-abi",
        ),
        "acceptance_suite_surface": build_acceptance_suite_surface(
            results,
            report_path,
            root=ROOT,
            runtime_acceptance_suite_surface_contract_id=(
                RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID
            ),
            runtime_state_publication_surface_contract_id=(
                RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID
            ),
            compile_provenance_contract_id=COMPILE_PROVENANCE_CONTRACT_ID,
            compile_output_truthfulness_contract_id=(
                COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
            ),
        ),
        "runtime_installation_abi_surface": (
            domains.registration.build_runtime_installation_abi_surface()
        ),
        "runtime_loader_lifecycle_surface": (
            domains.registration.build_runtime_loader_lifecycle_surface(results)
        ),
        "dispatch_accessor_runtime_abi_surface": (
            build_dispatch_accessor_runtime_abi_surface()
        ),
        "storage_accessor_runtime_abi_surface": (
            build_storage_accessor_runtime_abi_surface()
        ),
        "runtime_property_ivar_accessor_reflection_implementation_surface": (
            build_property_ivar_accessor_reflection_implementation_surface()
        ),
    }


__all__ = ["build_suite_and_abi_summary_sections"]
