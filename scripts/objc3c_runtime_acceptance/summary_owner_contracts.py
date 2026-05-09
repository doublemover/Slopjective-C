"""Runtime acceptance reporting owner contracts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


RUNTIME_ACCEPTANCE_REPORTING_OWNER_CONTRACT_ID = (
    "objc3c.runtime.acceptance.reporting.owner.contract.v1"
)
RUNTIME_ACCEPTANCE_SUMMARY_CONTRACT_ID = (
    "objc3c.runtime.acceptance.summary.owned.sections.v1"
)
RUNTIME_ACCEPTANCE_RESULT_NORMALIZATION_CONTRACT_ID = (
    "objc3c.runtime.acceptance.result.normalization.owner.v1"
)
RUNTIME_ACCEPTANCE_PROGRESS_SNAPSHOT_CONTRACT_ID = (
    "objc3c.runtime.acceptance.progress.snapshot.owner.v1"
)
RUNTIME_ACCEPTANCE_FINAL_STATUS_CONTRACT_ID = (
    "objc3c.runtime.acceptance.final.status.decision.owner.v1"
)
RUNTIME_ACCEPTANCE_REPORT_PERSISTENCE_CONTRACT_ID = (
    "objc3c.runtime.acceptance.report.persistence.owner.v1"
)
RUNTIME_ACCEPTANCE_ARTIFACT_EVIDENCE_CONTRACT_ID = (
    "objc3c.runtime.acceptance.artifact.evidence.owner.v1"
)

REPORTING_OWNER_SURFACE = "runtime-acceptance-reporting"
SUMMARY_OWNER_SURFACE = "runtime-acceptance-summary-sections"
DOMAIN_SUMMARY_OWNER_SURFACE = "runtime-acceptance-domain-summary-sections"
RESULT_NORMALIZATION_OWNER_SURFACE = "runtime-acceptance-result-normalization"
PROGRESS_OWNER_SURFACE = "runtime-acceptance-progress-snapshots"
REPORT_ASSEMBLY_OWNER_SURFACE = "runtime-acceptance-report-assembly"
ARTIFACT_EVIDENCE_OWNER_SURFACE = "runtime-acceptance-artifact-evidence"
FINAL_STATUS_OWNER_SURFACE = "runtime-acceptance-final-status-decision"

SUMMARY_SECTION_OWNER_MODULES: tuple[str, ...] = (
    "summary_base",
    "summary_bootstrap",
    "summary_metaprogramming_sections",
    "summary_concurrency_sections",
    "summary_error_sections",
    "summary_interop_sections",
    "summary_release_sections",
    "summary_object_model_sections",
    "summary_storage_block_sections",
    "summary_suite_sections",
)

DOMAIN_SUMMARY_SECTION_OWNER_MODULES: tuple[str, ...] = (
    "summary_bootstrap",
    "summary_metaprogramming_sections",
    "summary_concurrency_sections",
    "summary_error_sections",
    "summary_interop_sections",
    "summary_release_sections",
    "summary_object_model_sections",
    "summary_storage_block_sections",
)

REPORTING_NON_AUTHORITATIVE_INPUTS: tuple[str, ...] = (
    "summary sections without a CaseResult-backed runtime acceptance case",
    "progress snapshots that do not include the progress owner contract",
    "artifact paths missing from the runtime acceptance artifact registry",
    "case outcomes without an explicit final status decision",
)


@dataclass(frozen=True)
class RuntimeAcceptanceOwnerContract:
    contract_id: str
    owner_surface: str
    owner_modules: tuple[str, ...]
    owned_decisions: tuple[str, ...]

    def payload(self) -> dict[str, Any]:
        return {
            "contract_id": self.contract_id,
            "owner_surface": self.owner_surface,
            "owner_modules": list(self.owner_modules),
            "owned_decisions": list(self.owned_decisions),
        }


SUMMARY_OWNER_CONTRACT = RuntimeAcceptanceOwnerContract(
    contract_id=RUNTIME_ACCEPTANCE_SUMMARY_CONTRACT_ID,
    owner_surface=SUMMARY_OWNER_SURFACE,
    owner_modules=SUMMARY_SECTION_OWNER_MODULES,
    owned_decisions=(
        "base run metadata",
        "domain summary section publication",
        "suite and runtime abi summary publication",
    ),
)

RESULT_NORMALIZATION_OWNER_CONTRACT = RuntimeAcceptanceOwnerContract(
    contract_id=RUNTIME_ACCEPTANCE_RESULT_NORMALIZATION_CONTRACT_ID,
    owner_surface=RESULT_NORMALIZATION_OWNER_SURFACE,
    owner_modules=("case_result", "result_normalization"),
    owned_decisions=(
        "case identity normalization",
        "claim class preservation",
        "pass/fail status derivation",
    ),
)

PROGRESS_OWNER_CONTRACT = RuntimeAcceptanceOwnerContract(
    contract_id=RUNTIME_ACCEPTANCE_PROGRESS_SNAPSHOT_CONTRACT_ID,
    owner_surface=PROGRESS_OWNER_SURFACE,
    owner_modules=("progress_state", "progress_snapshots", "progress_format"),
    owned_decisions=(
        "running progress publication",
        "current command snapshot publication",
        "progress write overhead measurement",
    ),
)

REPORT_ASSEMBLY_OWNER_CONTRACT = RuntimeAcceptanceOwnerContract(
    contract_id=RUNTIME_ACCEPTANCE_REPORT_PERSISTENCE_CONTRACT_ID,
    owner_surface=REPORT_ASSEMBLY_OWNER_SURFACE,
    owner_modules=("report_assembly", "reports"),
    owned_decisions=(
        "summary assembly",
        "progress report persistence",
        "runtime acceptance summary persistence",
    ),
)

ARTIFACT_EVIDENCE_OWNER_CONTRACT = RuntimeAcceptanceOwnerContract(
    contract_id=RUNTIME_ACCEPTANCE_ARTIFACT_EVIDENCE_CONTRACT_ID,
    owner_surface=ARTIFACT_EVIDENCE_OWNER_SURFACE,
    owner_modules=("summary_base", "runtime_artifact_registry"),
    owned_decisions=(
        "native compiler artifact evidence publication",
        "runtime library artifact evidence publication",
        "case compile artifact evidence publication",
    ),
)

FINAL_STATUS_OWNER_CONTRACT = RuntimeAcceptanceOwnerContract(
    contract_id=RUNTIME_ACCEPTANCE_FINAL_STATUS_CONTRACT_ID,
    owner_surface=FINAL_STATUS_OWNER_SURFACE,
    owner_modules=("case_result", "progress_snapshots", "summary_base"),
    owned_decisions=(
        "case status derivation",
        "final progress status derivation",
        "summary status derivation",
    ),
)


def build_reporting_owner_contract() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_ACCEPTANCE_REPORTING_OWNER_CONTRACT_ID,
        "owner_surface": REPORTING_OWNER_SURFACE,
        "summary_owner_contract": SUMMARY_OWNER_CONTRACT.payload(),
        "domain_summary_owner_modules": list(DOMAIN_SUMMARY_SECTION_OWNER_MODULES),
        "result_normalization_owner_contract": (
            RESULT_NORMALIZATION_OWNER_CONTRACT.payload()
        ),
        "progress_owner_contract": PROGRESS_OWNER_CONTRACT.payload(),
        "report_assembly_owner_contract": REPORT_ASSEMBLY_OWNER_CONTRACT.payload(),
        "artifact_evidence_owner_contract": ARTIFACT_EVIDENCE_OWNER_CONTRACT.payload(),
        "final_status_owner_contract": FINAL_STATUS_OWNER_CONTRACT.payload(),
        "non_authoritative_inputs": list(REPORTING_NON_AUTHORITATIVE_INPUTS),
    }


def build_summary_section_owner_payload(
    *,
    owner_module: str,
    owner_surface: str,
    section_group: str,
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_ACCEPTANCE_SUMMARY_CONTRACT_ID,
        "owner_surface": owner_surface,
        "owner_module": owner_module,
        "section_group": section_group,
    }


def build_domain_summary_owner_payload(
    *,
    owner_module: str,
    domain: str,
) -> dict[str, Any]:
    return build_summary_section_owner_payload(
        owner_module=owner_module,
        owner_surface=DOMAIN_SUMMARY_OWNER_SURFACE,
        section_group=domain,
    )


__all__ = [
    "ARTIFACT_EVIDENCE_OWNER_CONTRACT",
    "ARTIFACT_EVIDENCE_OWNER_SURFACE",
    "DOMAIN_SUMMARY_OWNER_SURFACE",
    "DOMAIN_SUMMARY_SECTION_OWNER_MODULES",
    "FINAL_STATUS_OWNER_CONTRACT",
    "FINAL_STATUS_OWNER_SURFACE",
    "PROGRESS_OWNER_CONTRACT",
    "PROGRESS_OWNER_SURFACE",
    "REPORT_ASSEMBLY_OWNER_CONTRACT",
    "REPORT_ASSEMBLY_OWNER_SURFACE",
    "REPORTING_OWNER_SURFACE",
    "RESULT_NORMALIZATION_OWNER_CONTRACT",
    "RESULT_NORMALIZATION_OWNER_SURFACE",
    "RUNTIME_ACCEPTANCE_ARTIFACT_EVIDENCE_CONTRACT_ID",
    "RUNTIME_ACCEPTANCE_FINAL_STATUS_CONTRACT_ID",
    "RUNTIME_ACCEPTANCE_PROGRESS_SNAPSHOT_CONTRACT_ID",
    "RUNTIME_ACCEPTANCE_REPORTING_OWNER_CONTRACT_ID",
    "RUNTIME_ACCEPTANCE_REPORT_PERSISTENCE_CONTRACT_ID",
    "RUNTIME_ACCEPTANCE_RESULT_NORMALIZATION_CONTRACT_ID",
    "RUNTIME_ACCEPTANCE_SUMMARY_CONTRACT_ID",
    "SUMMARY_OWNER_CONTRACT",
    "SUMMARY_OWNER_SURFACE",
    "SUMMARY_SECTION_OWNER_MODULES",
    "RuntimeAcceptanceOwnerContract",
    "build_domain_summary_owner_payload",
    "build_reporting_owner_contract",
    "build_summary_section_owner_payload",
]
