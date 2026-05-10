from runtime_runnable_closure_contracts_support import (
    RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES,
    runtime_closure_forbidden_claims_by_shape,
)


def runtime_closure_forbidden_claim_contracts_are_owner_typed() -> None:
    contracts, by_shape = runtime_closure_forbidden_claims_by_shape()

    assert set(by_shape) == set(RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES)
    assert by_shape["fallback-runtime-behavior"]["policy_field"] == (
        "fallback_runtime_semantics_allowed"
    )
    assert by_shape["report-only-runtime-closure"]["policy_field"] == (
        "report_only_executable_proof_claims_allowed"
    )
    assert by_shape["compatibility-shim-runtime-closure"]["policy_field"] == (
        "compatibility_runtime_semantics_allowed"
    )
    assert by_shape["wrapper-only-runnable-action"]["policy_field"] == (
        "wrapper_only_runnable_actions_allowed"
    )
    assert by_shape["public-runtime-abi-widening-without-source-owner"][
        "policy_field"
    ] == "public_claims_require_executable_proof"
    assert by_shape["generated-report-only-source-truth"]["policy_field"] == (
        "generated_reports_are_source"
    )
    for contract in contracts:
        assert contract["owner"] == "runtime-closure-owner-contract"
        assert contract["failure_mode"] == "fail-closed"
