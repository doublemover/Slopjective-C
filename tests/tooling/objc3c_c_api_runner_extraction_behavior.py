from __future__ import annotations

from typing import Any

from objc3c_c_api_runner_extraction_assertions import (
    assert_contract_fields_are_in_source,
    assert_contract_identity,
    assert_escaped_json_fields_are_in_source,
    assert_source_contains,
    assert_source_excludes,
    assert_stage_fields_are_in_source,
)


def assert_c_gate_compile_path(source: str) -> None:
    assert_source_contains(
        source,
        [
            '#include "libobjc3c_frontend/c_api.h"',
            "objc3c_frontend_c_context_create()",
            "session.status = objc3c_frontend_c_compile_file_owned(",
            "compile_invocation.compile_options()",
            "session.compile_result.out_param()",
            "objc3c_frontend_c_copy_last_error(context, nullptr, 0)",
            "objc3c_frontend_c_context_destroy(context_);",
            "objc3c_frontend_c_owned_result_destroy(result_);",
            "ValidateFrontendCApiResultAccessors(",
            '"frontend C API accessor contract fail-closed: "',
        ],
    )


def assert_summary_and_cli_contract(source: str) -> None:
    assert_source_contains(
        source,
        [
            '\\"mode\\": \\"objc3c-frontend-c-api-runner-v1\\"',
            'std::filesystem::path("tmp") / "artifacts" / "compilation" /',
            '"objc3c-native";',
            "wrote summary: ",
            "--llc <path>",
            "--objc3-ir-object-backend <clang|llvm-direct>",
            '#include "diagnostics/modes/canonical_rejections.h"',
            "BuildCanonicalModeRejectionDiagnostic(",
            "arg, error",
            "--objc3-max-message-args",
            "--objc3-runtime-dispatch-symbol",
            "--objc3-enable-live-error-runtime-surface",
            "compile_options.llc_path =",
            "compile_options.ir_object_backend = runner_options.ir_object_backend;",
            "compile_options.allow_live_error_runtime_surface =",
            "FrontendCApiExitCodeFromStatus",
            '\\"result_error_message\\": \\"',
            '\\"c_api_ownership\\": ',
            '"result_handle_owner"',
            '"result_release_function"',
            '"result_release_timing"',
            '"context_lifetime"',
            '"compile_options_lifetime"',
            '"standalone_string_release_function"',
            '"error_message"',
            '"artifacts"',
            '"required_by_runner"',
            '"borrowed-until-owned-result-destroy"',
            "CaptureFrontendCApiRunnerResultArtifactSnapshot(",
            "OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS",
            "OBJC3C_FRONTEND_ARTIFACT_MANIFEST",
            "OBJC3C_FRONTEND_ARTIFACT_IR",
            "OBJC3C_FRONTEND_ARTIFACT_OBJECT",
            "OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA",
        ],
    )
    assert_source_excludes(
        source,
        [
            "--objc3-retired-mode <canonical|legacy>",
            "invalid --objc3-retired-mode (expected canonical|legacy): ",
            '\\"retired_mode\\": \\"',
            '\\"retired_mode_assist\\": ',
            "compile_options.retired_mode = options.retired_mode;",
            "compile_options.retired_mode_assist = options.retired_mode_assist ? 1u : 0u;",
        ],
    )


def assert_observability_surface(source: str) -> None:
    assert_source_contains(
        source,
        [
            '\\"observability\\": ',
            '\\"status_name\\": \\"',
            '\\"last_attempted_stage\\": \\"',
            '\\"blocking_stage\\": \\"',
            '\\"highest_diagnostic_severity\\": \\"',
            '\\"artifact_presence\\": {',
            '\\"dump_commands\\": {',
            "BuildFrontendCApiDiagnosticTotals",
            "BuildFrontendCApiRunnerReadCommand",
            "Get-Content -Raw ",
        ],
    )


def assert_fail_closed_stage_and_result_accessor_drift(source: str) -> None:
    assert_source_contains(
        source,
        [
            "FrontendCApiStageSummaryShapeReady(result.lex,",
            "OBJC3C_FRONTEND_STAGE_LEX",
            "FrontendCApiStageSummaryShapeReady(result.parse,",
            "OBJC3C_FRONTEND_STAGE_PARSE",
            "FrontendCApiStageSummaryShapeReady(result.sema,",
            "OBJC3C_FRONTEND_STAGE_SEMA",
            "FrontendCApiStageSummaryShapeReady(result.lower,",
            "OBJC3C_FRONTEND_STAGE_LOWER",
            "FrontendCApiStageSummaryShapeReady(result.emit,",
            "OBJC3C_FRONTEND_STAGE_EMIT",
            '"compile status does not match result.status"',
            '"successful compile did not set result.success"',
            '"failing compile left result.success set"',
            '"successful compile published a result-owned error message"',
            '"successful compile published a context last_error"',
            '"failing compile published no result-owned error message"',
            '"result-owned error message is present but empty"',
            '"result-owned error_message snapshot differs from accessor text"',
            '"context last_error and result-owned error_message differ"',
            '"required result-owned "',
            '"diagnostics output path contract fail-closed: result-owned "',
            "const bool stage_report_output_contract_ready =",
            "FrontendCApiStageReportShapeReady(result);",
            "stage_report_output_contract_ready);",
            "return 2;",
            "return compile_session.exit_code;",
        ],
    )


def assert_contract_tracks_summary_ownership_fields(
    source: str,
    contract: dict[str, Any],
) -> None:
    assert_contract_identity(contract)
    assert_contract_fields_are_in_source(source, contract["required_summary_fields"])
    assert_contract_fields_are_in_source(source, contract["required_ownership_fields"])
    assert_contract_fields_are_in_source(source, contract["required_path_fields"])
    assert_stage_fields_are_in_source(source, contract["required_stage_fields"])
    assert_escaped_json_fields_are_in_source(
        source,
        contract["required_stage_summary_fields"],
    )
    assert_source_contains(source, contract["fail_closed_reasons"])


def assert_runtime_inspector_and_dump_flags(source: str) -> None:
    assert_source_contains(
        source,
        [
            "--dump-summary-json",
            "--dump-observability-json",
            "--dump-playground-repro-json",
            "--dump-runtime-inspector-json",
            "--dump-stage-trace-json",
            '\\"runtime_inspector\\": ',
            '\\"section_inventory_command\\": \\"',
            '\\"symbol_inventory_command\\": \\"',
            '\\"arc_debug_state_snapshot_symbol\\": \\"',
            "BuildFrontendCApiRunnerObjectInspectionCommand",
            "kObjc3RuntimeMetadataObjectInspectionContractId",
            "kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel",
            '\\"mode\\": \\"objc3c-frontend-stage-trace-v1\\"',
            "BuildFrontendCApiRunnerStageTraceJson",
        ],
    )


def assert_bonus_experience_boundary_surface(source: str) -> None:
    assert_source_contains(
        source,
        [
            '\\"bonus_experiences\\": ',
            "WriteFrontendCApiRunnerBonusExperiencesJson",
            "objc3c.bonus.experiences.boundary.v1",
            '\\"product_boundary_model\\": ',
            '\\"runtime_boundary_model\\": ',
            '\\"playground\\": {',
            '\\"runtime_inspector_and_capability_explorer\\": {',
            '\\"template_and_demo_harness\\": {',
            "materialize-playground-workspace",
            "tmp/artifacts/playground",
            "tmp/reports/playground",
            "inspect-compile-observability",
            "benchmark-runtime-inspector",
            "inspect-capability-explorer",
            "inspect-runtime-inspector",
            "validate-showcase",
        ],
    )


def assert_playground_repro_surface(source: str) -> None:
    assert_source_contains(
        source,
        [
            "WriteFrontendCApiRunnerPlaygroundReproJson",
            "BuildFrontendCApiRunnerReproCommand",
            "objc3c.playground.repro.surface.v1",
            'EscapeJsonString(options.out_dir.generic_string())',
            '\\"compile_profile\\": {',
            '\\"showcase_examples\\": [',
            '\\"repro_runner\\": \\"',
            "materialize-playground-workspace",
            "inspect-playground-repro",
        ],
    )
