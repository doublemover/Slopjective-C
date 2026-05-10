from __future__ import annotations

from .paths import (
    CONFORMANCE_MANIFEST,
    CONFORMANCE_NEGATIVE,
    CONFORMANCE_POSITIVE,
    CONFORMANCE_README,
    CONTRACT_ID,
    DASHBOARD_BLOCKERS,
    DETERMINISTIC_ARTIFACTS,
    IR_EMITTER,
    ISSUE,
    JSON_OUT,
    LOWERING_CONTRACT_CPP,
    LOWERING_CONTRACT_H,
    MD_OUT,
    NEGATIVE_FIXTURE,
    POSITIVE_FIXTURE,
    PUBLIC_CLAIM_DRIFT,
    REQUIRED_ARTIFACTS,
    REQUIRED_IR_TOKENS,
    REQUIRED_MANIFEST_KEYS,
    REQUIRED_OBJECT_SECTIONS,
    REQUIRED_OBJECT_SYMBOLS,
    SCRATCH,
    SEMA_PASS_MANAGER,
    SEMANTIC_PASSES,
    STATIC_ANALYSIS,
    SUPPORT_CLASSIFICATION,
    rel,
)
from .source_diagnostics import check_conformance, check_source_tokens
from .truth_models import build_negative_runs, build_positive_runs, inspect_object


def build_summary() -> dict:
    positive_runs = build_positive_runs()
    negative_runs = build_negative_runs()
    object_inspection = inspect_object(SCRATCH / "positive" / "run1" / "module.obj")
    source_checks = check_source_tokens()
    conformance = check_conformance()
    claim_gate_reports = {
        "support_classification": SUPPORT_CLASSIFICATION.is_file(),
        "public_claim_drift": PUBLIC_CLAIM_DRIFT.is_file(),
        "dashboard_release_blockers": DASHBOARD_BLOCKERS.is_file(),
    }

    run1_hashes = positive_runs["run1"]["hashes"]
    run2_hashes = positive_runs["run2"]["hashes"]
    deterministic_artifacts = {
        name: name in run1_hashes and run1_hashes.get(name) == run2_hashes.get(name)
        for name in DETERMINISTIC_ARTIFACTS
    }
    negative_diagnostics_deterministic = (
        negative_runs["run1"]["diagnostics_hash"]
        == negative_runs["run2"]["diagnostics_hash"]
        and bool(negative_runs["run1"]["diagnostics_hash"])
    )
    no_source_truth_under_tmp = all(
        not rel(path).startswith("tmp/")
        for path in [
            POSITIVE_FIXTURE,
            NEGATIVE_FIXTURE,
            LOWERING_CONTRACT_H,
            LOWERING_CONTRACT_CPP,
            IR_EMITTER,
            SEMA_PASS_MANAGER,
            SEMANTIC_PASSES,
            STATIC_ANALYSIS,
            CONFORMANCE_MANIFEST,
            CONFORMANCE_README,
            CONFORMANCE_POSITIVE,
            CONFORMANCE_NEGATIVE,
            SUPPORT_CLASSIFICATION,
            PUBLIC_CLAIM_DRIFT,
            DASHBOARD_BLOCKERS,
            JSON_OUT,
            MD_OUT,
        ]
    )

    checks = {
        "positive_runs_compiled": all(run["compiled"] for run in positive_runs.values()),
        "positive_artifacts_present": all(
            value for run in positive_runs.values() for value in run["artifacts"].values()
        ),
        "deterministic_artifact_hashes": all(deterministic_artifacts.values()),
        "positive_ir_tokens": all(
            value for run in positive_runs.values() for value in run["ir_tokens"].values()
        ),
        "positive_manifest_keys": all(
            value for run in positive_runs.values() for value in run["manifest_keys"].values()
        ),
        "positive_conformance_reports_ready": all(
            run["conformance_report"]["ready"]
            and run["conformance_report"]["runtime_capability_ready"]
            and run["conformance_report"]["schema_id"] == "objc3c-versioned-conformance-report-v1"
            and run["conformance_report"]["public_schema_id"] == "objc3-conformance-report/v1"
            for run in positive_runs.values()
        ),
        "object_backend_llvm_direct": all(
            run["object_backend"] == "llvm-direct" for run in positive_runs.values()
        ),
        "object_sections_present": object_inspection["tools_available"]
        and all(object_inspection["sections"].values()),
        "object_symbols_present": object_inspection["tools_available"]
        and all(object_inspection["symbols"].values()),
        "negative_runs_rejected": all(
            run["rejected"] and run["expected_codes_present"]
            for run in negative_runs.values()
        ),
        "negative_no_manifest_ir_object": all(
            value
            for run in negative_runs.values()
            for value in run["emitted_artifacts_absent"].values()
        ),
        "negative_diagnostics_deterministic": negative_diagnostics_deterministic,
        "source_tokens": all(source_checks.values()),
        "conformance": all(conformance.values()),
        "claim_gate_reports": all(claim_gate_reports.values()),
        "no_source_truth_under_tmp": no_source_truth_under_tmp,
    }

    return {
        "schema_version": "1.0.0",
        "issue": ISSUE,
        "contract_id": CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "counts": {
            "required_artifact_count": len(REQUIRED_ARTIFACTS),
            "deterministic_artifact_count": len(DETERMINISTIC_ARTIFACTS),
            "required_ir_token_count": len(REQUIRED_IR_TOKENS),
            "required_manifest_key_count": len(REQUIRED_MANIFEST_KEYS),
            "required_object_section_count": len(REQUIRED_OBJECT_SECTIONS),
            "required_object_symbol_count": len(REQUIRED_OBJECT_SYMBOLS),
            "positive_run_count": len(positive_runs),
            "negative_run_count": len(negative_runs),
        },
        "checks": checks,
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "positive_runs": positive_runs,
        "negative_runs": negative_runs,
        "deterministic_artifacts": deterministic_artifacts,
        "object_inspection": object_inspection,
        "source_checks": source_checks,
        "conformance": conformance,
        "claim_gate_reports": claim_gate_reports,
        "scratch_directory": rel(SCRATCH),
        "scratch_is_not_source_truth": True,
    }
