from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RUNNER_SOURCE_ROOT = ROOT / "native" / "objc3c" / "src" / "tools"
RUNNER_CPP = RUNNER_SOURCE_ROOT / "objc3c_frontend_c_api_runner.cpp"
C_API_RUNNER_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "frontend_c_api_runner_contract.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _runner_sources() -> list[Path]:
    return sorted(
        path
        for path in RUNNER_SOURCE_ROOT.glob("objc3c_frontend_c_api_runner*.*")
        if path.suffix in {".cpp", ".h"}
    )


def _runner_source_text() -> str:
    sources = _runner_sources()
    assert RUNNER_CPP in sources
    assert sources
    return "\n".join(_read(source) for source in sources)


def test_c_api_runner_uses_c_shim_compile_path() -> None:
    source = _runner_source_text()

    assert '#include "libobjc3c_frontend/c_api.h"' in source
    assert "objc3c_frontend_c_context_create()" in source
    assert "session.status = objc3c_frontend_c_compile_file(" in source
    assert "compile_invocation.compile_options()" in source
    assert "&session.result" in source
    assert "objc3c_frontend_c_copy_last_error(context, nullptr, 0)" in source
    assert "objc3c_frontend_c_context_destroy(context_);" in source
    assert "FrontendCApiCompileResultGuard result_guard;" in source
    assert ": result_guard{&result} {}" in source
    assert "objc3c_frontend_c_result_destroy(result);" in source
    assert "ValidateFrontendCApiResultAccessors(" in source
    assert '"frontend C API accessor contract fail-closed: "' in source


def test_c_api_runner_reports_summary_and_cli_contract() -> None:
    source = _runner_source_text()

    assert '\\"mode\\": \\"objc3c-frontend-c-api-runner-v1\\"' in source
    assert 'std::filesystem::path("tmp") / "artifacts" / "compilation" /' in source
    assert '"objc3c-native";' in source
    assert "wrote summary: " in source
    assert "--llc <path>" in source
    assert "--objc3-ir-object-backend <clang|llvm-direct>" in source
    assert "--objc3-compat-mode <canonical|legacy>" not in source
    assert '#include "diagnostics/modes/objc3_removed_mode_options.h"' in source
    assert "BuildRemovedModeOptionDiagnostic(" in source
    assert "arg, error" in source
    assert "--objc3-max-message-args" in source
    assert "--objc3-runtime-dispatch-symbol" in source
    assert "invalid --objc3-compat-mode (expected canonical|legacy): " not in source
    assert '\\"compatibility_mode\\": \\"' not in source
    assert '\\"migration_assist\\": ' not in source
    assert "compile_options_.llc_path =" in source
    assert "compile_options.compatibility_mode = options.compatibility_mode;" not in source
    assert "compile_options.migration_assist = options.migration_assist ? 1u : 0u;" not in source
    assert "compile_options_.ir_object_backend = runner_options_.ir_object_backend;" in source
    assert "FrontendCApiExitCodeFromStatus" in source
    assert '\\"result_error_message\\": \\"' in source
    assert '\\"c_api_ownership\\": ' in source
    assert '"result_storage_owner"' in source
    assert '"result_release_function"' in source
    assert '"result_release_timing"' in source
    assert '"context_lifetime"' in source
    assert '"compile_options_lifetime"' in source
    assert '"standalone_string_release_function"' in source
    assert '"error_message"' in source
    assert '"artifacts"' in source
    assert '"required_by_runner"' in source
    assert '"borrowed-until-result-destroy"' in source
    assert "FrontendCApiResultArtifactPathSnapshot(" in source
    assert "OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS" in source
    assert "OBJC3C_FRONTEND_ARTIFACT_MANIFEST" in source
    assert "OBJC3C_FRONTEND_ARTIFACT_IR" in source
    assert "OBJC3C_FRONTEND_ARTIFACT_OBJECT" in source
    assert "OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA" in source


def test_c_api_runner_reports_observability_surface() -> None:
    source = _runner_source_text()

    assert '\\"observability\\": ' in source
    assert '\\"status_name\\": \\"' in source
    assert '\\"last_attempted_stage\\": \\"' in source
    assert '\\"blocking_stage\\": \\"' in source
    assert '\\"highest_diagnostic_severity\\": \\"' in source
    assert '\\"artifact_presence\\": {' in source
    assert '\\"dump_commands\\": {' in source
    assert "BuildFrontendCApiDiagnosticTotals" in source
    assert "BuildFrontendCApiRunnerReadCommand" in source
    assert "Get-Content -Raw " in source


def test_c_api_runner_fails_closed_on_stage_and_result_accessor_drift() -> None:
    source = _runner_source_text()

    assert "FrontendCApiStageSummaryShapeReady(result.lex," in source
    assert "OBJC3C_FRONTEND_STAGE_LEX" in source
    assert "FrontendCApiStageSummaryShapeReady(result.parse," in source
    assert "OBJC3C_FRONTEND_STAGE_PARSE" in source
    assert "FrontendCApiStageSummaryShapeReady(result.sema," in source
    assert "OBJC3C_FRONTEND_STAGE_SEMA" in source
    assert "FrontendCApiStageSummaryShapeReady(result.lower," in source
    assert "OBJC3C_FRONTEND_STAGE_LOWER" in source
    assert "FrontendCApiStageSummaryShapeReady(result.emit," in source
    assert "OBJC3C_FRONTEND_STAGE_EMIT" in source
    assert '"compile status does not match result.status"' in source
    assert '"successful compile did not set result.success"' in source
    assert '"failing compile left result.success set"' in source
    assert '"successful compile published a result-owned error message"' in source
    assert '"successful compile published a context last_error"' in source
    assert '"failing compile published no result-owned error message"' in source
    assert '"result-owned error message is present but empty"' in source
    assert '"result-owned error_message snapshot differs from accessor text"' in source
    assert '"context last_error and result-owned error_message differ"' in source
    assert '"required result-owned "' in source
    assert '"diagnostics output path contract fail-closed: result-owned "' in source
    assert "const bool stage_report_output_contract_ready =" in source
    assert "FrontendCApiStageReportShapeReady(result);" in source
    assert "stage_report_output_contract_ready);" in source
    assert "return 2;" in source
    assert "return compile_session.exit_code;" in source


def test_c_api_runner_contract_fixture_tracks_summary_ownership_fields() -> None:
    source = _runner_source_text()
    contract = json.loads(_read(C_API_RUNNER_CONTRACT))

    assert contract["contract_id"] == "objc3c.frontend.c_api.runner.contract.v1"
    assert contract["runner"] == "artifacts/bin/objc3c-frontend-c-api-runner.exe"
    assert contract["source_glob"] == "native/objc3c/src/tools/objc3c_frontend_c_api_runner*"
    assert contract["summary_mode"] == "objc3c-frontend-c-api-runner-v1"
    for field in contract["required_summary_fields"]:
        assert f'\\"{field}\\"' in source or f'"{field}"' in source
    for field in contract["required_ownership_fields"]:
        assert f'\\"{field}\\"' in source or f'"{field}"' in source
    for field in contract["required_path_fields"]:
        assert f'\\"{field}\\"' in source or f'"{field}"' in source
    for stage in contract["required_stage_fields"]:
        assert f'WriteFrontendCApiRunnerStageSummaryJson(out, "{stage}", result.{stage}' in source
    for field in contract["required_stage_summary_fields"]:
        assert f'\\"{field}\\"' in source
    for message in contract["fail_closed_reasons"]:
        assert message in source


def test_c_api_runner_reports_runtime_inspector_and_dump_flags() -> None:
    source = _runner_source_text()

    assert "--dump-summary-json" in source
    assert "--dump-observability-json" in source
    assert "--dump-playground-repro-json" in source
    assert "--dump-runtime-inspector-json" in source
    assert "--dump-stage-trace-json" in source
    assert '\\"runtime_inspector\\": ' in source
    assert '\\"section_inventory_command\\": \\"' in source
    assert '\\"symbol_inventory_command\\": \\"' in source
    assert '\\"arc_debug_state_snapshot_symbol\\": \\"' in source
    assert "BuildFrontendCApiRunnerObjectInspectionCommand" in source
    assert "kObjc3RuntimeMetadataObjectInspectionContractId" in source
    assert "kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel" in source
    assert '\\"mode\\": \\"objc3c-frontend-stage-trace-v1\\"' in source
    assert "BuildStageTraceJson" in source


def test_c_api_runner_reports_bonus_experience_boundary_surface() -> None:
    source = _runner_source_text()

    assert '\\"bonus_experiences\\": ' in source
    assert "WriteFrontendCApiRunnerBonusExperiencesJson" in source
    assert "objc3c.bonus.experiences.boundary.v1" in source
    assert '\\"product_boundary_model\\": ' in source
    assert '\\"runtime_boundary_model\\": ' in source
    assert '\\"playground\\": {' in source
    assert '\\"runtime_inspector_and_capability_explorer\\": {' in source
    assert '\\"template_and_demo_harness\\": {' in source
    assert "materialize-playground-workspace" in source
    assert "tmp/artifacts/playground" in source
    assert "tmp/reports/playground" in source
    assert "inspect-compile-observability" in source
    assert "benchmark-runtime-inspector" in source
    assert "inspect-capability-explorer" in source
    assert "inspect-runtime-inspector" in source
    assert "validate-showcase" in source


def test_c_api_runner_reports_playground_repro_surface() -> None:
    source = _runner_source_text()

    assert "WriteFrontendCApiRunnerPlaygroundReproJson" in source
    assert "BuildFrontendCApiRunnerReproCommand" in source
    assert "objc3c.playground.repro.surface.v1" in source
    assert 'EscapeJsonString(options.out_dir.generic_string())' in source
    assert '\\"compile_profile\\": {' in source
    assert '\\"showcase_examples\\": [' in source
    assert '\\"repro_runner\\": \\"' in source
    assert "materialize-playground-workspace" in source
    assert "inspect-playground-repro" in source
