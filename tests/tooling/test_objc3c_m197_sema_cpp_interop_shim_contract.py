from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
C_API_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.h"
C_API_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.cpp"
CLI_FRONTEND_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
CMAKE_SOURCE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m197_sema_cpp_interop_shim_strategy_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    assert "## M197 sema/type C++ interop shim strategy" in fragment


def test_m197_sema_cpp_interop_shim_strategy_source_anchor_mapping() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "interop shim packet 1.1 deterministic sema/type C++ interop architecture anchors",
        "m197_sema_type_cpp_interop_shim_architecture_packet",
        "interop shim packet 1.2 deterministic sema/type C++ interop isolation anchors",
        "m197_sema_type_cpp_interop_shim_isolation_packet",
        "Optional C ABI shim for non-C++ embedding environments.",
        "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;",
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
        "extern \"C\" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(",
        "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(",
        "product.pipeline_result = RunObjc3FrontendPipeline(source, options);",
        "product.artifact_bundle = BuildObjc3FrontendArtifacts(input_path, product.pipeline_result, options);",
        "Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);",
        "Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "result.parity_surface.ready =",
        "sema_input.program = &result.program;",
        "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "result.sema_parity_surface = sema_result.parity_surface;",
        "add_library(objc3c_sema_type_system INTERFACE)",
        "target_link_libraries(objc3c_lower PUBLIC",
        "target_link_libraries(objc3c_ir PUBLIC",
        "add_library(objc3c_runtime_abi STATIC",
        "target_link_libraries(objc3c_frontend PUBLIC",
        "objc3c_runtime_abi",
        "python -m pytest tests/tooling/test_objc3c_m197_sema_cpp_interop_shim_contract.py -q",
    ):
        assert text in fragment

    c_api_header = _read(C_API_HEADER)
    c_api_source = _read(C_API_SOURCE)
    cli_frontend_source = _read(CLI_FRONTEND_SOURCE)
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    cmake_source = _read(CMAKE_SOURCE)

    for marker in (
        "Optional C ABI shim for non-C++ embedding environments.",
        "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;",
    ):
        assert marker in c_api_header

    for marker in (
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
        "extern \"C\" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(",
    ):
        assert marker in c_api_source

    for marker in (
        "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(",
        "product.pipeline_result = RunObjc3FrontendPipeline(source, options);",
        "product.artifact_bundle = BuildObjc3FrontendArtifacts(input_path, product.pipeline_result, options);",
    ):
        assert marker in cli_frontend_source

    for marker in (
        "Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);",
        "Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);",
    ):
        assert marker in frontend_anchor_source

    for marker in (
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "Objc3SemaDiagnosticsBus diagnostics_bus;",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.deterministic_type_metadata_handoff =",
        "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        "result.parity_surface.ready =",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        "sema_input.program = &result.program;",
        "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "result.sema_parity_surface = sema_result.parity_surface;",
    ):
        assert marker in pipeline_source

    for marker in (
        "add_library(objc3c_sema_type_system INTERFACE)",
        "target_link_libraries(objc3c_lower PUBLIC",
        "target_link_libraries(objc3c_ir PUBLIC",
        "add_library(objc3c_runtime_abi STATIC",
        "target_link_libraries(objc3c_frontend PUBLIC",
        "objc3c_runtime_abi",
    ):
        assert marker in cmake_source
