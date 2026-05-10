from scripts.objc3c_runtime_acceptance.summary_owner_contracts import (
    ARTIFACT_EVIDENCE_OWNER_SURFACE,
    FINAL_STATUS_OWNER_SURFACE,
    PROGRESS_OWNER_SURFACE,
    REPORT_ASSEMBLY_OWNER_SURFACE,
    REPORTING_OWNER_SURFACE,
    RESULT_NORMALIZATION_OWNER_SURFACE,
    RUNTIME_ACCEPTANCE_REPORTING_OWNER_CONTRACT_ID,
    SUMMARY_OWNER_SURFACE,
    build_reporting_owner_contract,
)


def runtime_acceptance_reporting_owner_contract_covers_surfaces() -> None:
    contract = build_reporting_owner_contract()

    assert contract["contract_id"] == RUNTIME_ACCEPTANCE_REPORTING_OWNER_CONTRACT_ID
    assert contract["owner_surface"] == REPORTING_OWNER_SURFACE
    assert contract["summary_owner_contract"]["owner_surface"] == SUMMARY_OWNER_SURFACE
    assert (
        contract["result_normalization_owner_contract"]["owner_surface"]
        == RESULT_NORMALIZATION_OWNER_SURFACE
    )
    assert contract["progress_owner_contract"]["owner_surface"] == PROGRESS_OWNER_SURFACE
    assert (
        contract["report_assembly_owner_contract"]["owner_surface"]
        == REPORT_ASSEMBLY_OWNER_SURFACE
    )
    assert (
        contract["artifact_evidence_owner_contract"]["owner_surface"]
        == ARTIFACT_EVIDENCE_OWNER_SURFACE
    )
    assert (
        contract["final_status_owner_contract"]["owner_surface"]
        == FINAL_STATUS_OWNER_SURFACE
    )
