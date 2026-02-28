from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
C_API_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.h"
C_API_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.cpp"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PIPELINE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
CMAKE_SOURCE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m196_sema_c_interop_headers_and_abi_alignment_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M196 sema/type C interop headers and ABI alignment",
        "c interop packet 1.1 deterministic sema/type C interop headers ABI architecture anchors",
        "m196_sema_type_c_interop_headers_abi_architecture_packet",
        "c interop packet 1.2 deterministic sema/type C interop headers ABI isolation anchors",
        "m196_sema_type_c_interop_headers_abi_isolation_packet",
        "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u",
        "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;",
        "typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;",
        "static_assert(std::is_same_v<objc3c_frontend_c_context_t, objc3c_frontend_context_t>,",
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_result_t, objc3c_frontend_compile_result_t>,",
        "Objc3FrontendOptions BuildFrontendOptions(const objc3c_frontend_compile_options_t &options) {",
        "frontend_options.compatibility_mode =",
        "frontend_options.migration_assist = options.migration_assist != 0;",
        "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
        "inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;",
        "inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;",
        "inline constexpr const char *kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
        "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =",
        "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
        "CanonicalizePassDiagnostics(pass_diagnostics);",
        "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
        "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
        "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();",
        "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        "result.parity_surface.ready =",
        "sema_input.program = &result.program;",
        "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.sema_parity_surface = sema_result.parity_surface;",
        "add_library(objc3c_sema_type_system INTERFACE)",
        "add_library(objc3c_runtime_abi STATIC",
        "target_link_libraries(objc3c_frontend PUBLIC",
        "objc3c_runtime_abi",
        "python -m pytest tests/tooling/test_objc3c_m196_sema_c_interop_headers_abi_contract.py -q",
    ):
        assert text in fragment


def test_m196_sema_c_interop_headers_and_abi_alignment_source_anchor_mapping() -> None:
    c_api_header = _read(C_API_HEADER)
    c_api_source = _read(C_API_SOURCE)
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    pipeline_contract_source = _read(PIPELINE_CONTRACT_SOURCE)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    pipeline_source = _read(PIPELINE_SOURCE)
    cmake_source = _read(CMAKE_SOURCE)

    for marker in (
        "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u",
        "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;",
        "typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;",
    ):
        assert marker in c_api_header

    for marker in (
        "static_assert(std::is_same_v<objc3c_frontend_c_context_t, objc3c_frontend_context_t>,",
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_result_t, objc3c_frontend_compile_result_t>,",
    ):
        assert marker in c_api_source

    for marker in (
        "Objc3FrontendOptions BuildFrontendOptions(const objc3c_frontend_compile_options_t &options) {",
        "frontend_options.compatibility_mode =",
        "frontend_options.migration_assist = options.migration_assist != 0;",
        "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
    ):
        assert marker in frontend_anchor_source

    for marker in (
        "inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;",
        "inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;",
        "inline constexpr const char *kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
    ):
        assert marker in pipeline_contract_source

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
        "result.parity_surface.ready =",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        "sema_input.program = &result.program;",
        "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy",
        "sema_input.migration_assist = options.migration_assist;",
        "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;",
        "Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);",
        "result.sema_parity_surface = sema_result.parity_surface;",
    ):
        assert marker in pipeline_source

    for marker in (
        "add_library(objc3c_sema_type_system INTERFACE)",
        "add_library(objc3c_runtime_abi STATIC",
        "target_link_libraries(objc3c_frontend PUBLIC",
        "objc3c_runtime_abi",
    ):
        assert marker in cmake_source
