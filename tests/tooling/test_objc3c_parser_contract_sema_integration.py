from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PARSER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DIAG_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.h"
DRIVER_OBJC3_PATH = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_parser_contract_exports_parsed_program_aliases() -> None:
    contract = _read(PARSER_CONTRACT)
    assert "struct Objc3ParsedProgram {" in contract
    assert "Objc3Program ast;" in contract
    assert "using Objc3ParsedGlobalDecl = GlobalDecl;" in contract
    assert "using Objc3ParsedFunctionDecl = FunctionDecl;" in contract
    assert "MutableObjc3ParsedProgramAst(" in contract
    assert "Objc3ParsedProgramAst(" in contract


def test_parser_uses_ast_builder_scaffold() -> None:
    parser = _read(PARSER_SOURCE)
    assert '#include "parse/objc3_ast_builder.h"' in parser
    assert "Objc3AstBuilder ast_builder_;" in parser
    assert "ast_builder_.BeginProgram()" in parser
    assert "ast_builder_.AddGlobalDecl(program, std::move(*decl));" in parser
    assert "ast_builder_.AddFunctionDecl(program, std::move(*fn));" in parser


def test_sema_header_consumes_parser_contract_outputs() -> None:
    sema = _read(SEMA_HEADER)
    assert '#include "sema/objc3_sema_contract.h"' in sema
    assert '#include "parse/objc3_parser_contract.h"' not in sema
    assert '#include "ast/objc3_ast.h"' not in sema
    assert "BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program," in sema
    assert "ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program," in sema
    assert "ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface," in sema


def test_parser_to_sema_type_metadata_handoff_contract_is_explicit() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_header = _read(SEMA_HEADER)
    pass_manager_contract = _read(PASS_MANAGER_CONTRACT)

    assert "struct Objc3SemanticTypeMetadataHandoff {" in sema_contract
    assert "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(" in sema_contract
    assert "bool IsDeterministicSemanticTypeMetadataHandoff(" in sema_contract

    assert "BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);" in sema_header
    assert "IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);" in sema_header

    assert "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;" in pass_manager_contract
    assert "bool deterministic_semantic_diagnostics = false;" in pass_manager_contract
    assert "bool deterministic_type_metadata_handoff = false;" in pass_manager_contract
    assert "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};" in pass_manager_contract


def test_ast_builder_scaffold_is_registered_in_build_surfaces() -> None:
    cmake = _read(CMAKE_FILE)
    build_script = _read(BUILD_SCRIPT)
    assert "src/parse/objc3_ast_builder.cpp" in cmake
    assert "target_link_libraries(objc3c_parse PUBLIC" in cmake
    assert "target_link_libraries(objc3c_sema PUBLIC" in cmake
    assert "add_library(objc3c_sema_type_system INTERFACE)" in cmake
    assert "target_link_libraries(objc3c_sema_type_system INTERFACE" in cmake
    assert "target_link_libraries(objc3c_lower PUBLIC" in cmake
    assert "target_link_libraries(objc3c_ir PUBLIC" in cmake
    assert "objc3c_sema_type_system" in cmake

    _assert_in_order(
        cmake,
        [
            "add_library(objc3c_parse STATIC",
            "target_link_libraries(objc3c_parse PUBLIC",
            "add_library(objc3c_sema STATIC",
            "target_link_libraries(objc3c_sema PUBLIC",
            "add_library(objc3c_sema_type_system INTERFACE)",
            "target_link_libraries(objc3c_sema_type_system INTERFACE",
            "add_library(objc3c_lower STATIC",
            "target_link_libraries(objc3c_lower PUBLIC",
            "add_library(objc3c_ir STATIC",
            "target_link_libraries(objc3c_ir PUBLIC",
        ],
    )

    assert '"native/objc3c/src/parse/objc3_ast_builder.cpp"' in build_script


def test_frontend_pipeline_artifact_boundary_uses_diagnostics_bus_contract() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    pipeline_source = _read(PIPELINE_SOURCE)
    artifacts_header = _read(ARTIFACTS_HEADER)
    artifacts_source = _read(ARTIFACTS_SOURCE)
    diag_header = _read(DIAG_ARTIFACTS_HEADER)
    driver_source = _read(DRIVER_OBJC3_PATH)

    assert "Objc3FrontendDiagnosticsBus stage_diagnostics;" in pipeline_types
    assert "std::array<std::size_t, 3> sema_diagnostics_after_pass = {0, 0, 0};" in pipeline_types
    assert "Objc3SemaParityContractSurface sema_parity_surface;" in pipeline_types
    assert "result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;" in pipeline_source
    assert "result.sema_parity_surface = sema_result.parity_surface;" in pipeline_source
    assert "sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy" in pipeline_source
    assert "sema_input.migration_assist = options.migration_assist;" in pipeline_source
    assert "sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;" in pipeline_source
    assert "sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;" in pipeline_source
    assert "sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;" in pipeline_source
    assert "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);" in pipeline_source

    assert "Objc3FrontendDiagnosticsBus stage_diagnostics;" in artifacts_header
    assert "std::vector<std::string> post_pipeline_diagnostics;" in artifacts_header
    assert "bundle.stage_diagnostics = pipeline_result.stage_diagnostics;" in artifacts_source
    assert "bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);" in artifacts_source
    assert '\\"language_version\\":' in artifacts_source
    assert '\\"compatibility_mode\\":\\"' in artifacts_source
    assert '\\"migration_assist\\":' in artifacts_source
    assert "CompatibilityModeName(options.compatibility_mode)" in artifacts_source
    _assert_in_order(
        artifacts_source,
        [
            'manifest << "  \\"frontend\\": {\\n";',
            'manifest << "    \\"language_version\\":"',
            'manifest << "    \\"compatibility_mode\\":\\""',
            'manifest << "    \\"migration_assist\\":"',
            'manifest << "    \\"max_message_send_args\\":"',
        ],
    )
    assert "sema_pass_manager" in artifacts_source
    assert "diagnostics_after_build" in artifacts_source
    assert "diagnostics_emitted_by_build" in artifacts_source
    assert "diagnostics_monotonic" in artifacts_source
    assert "deterministic_semantic_diagnostics" in artifacts_source
    assert "deterministic_type_metadata_handoff" in artifacts_source
    assert "parity_ready" in artifacts_source

    assert "const Objc3FrontendDiagnosticsBus &stage_diagnostics" in diag_header
    assert "artifacts.stage_diagnostics" in driver_source
    assert "artifacts.post_pipeline_diagnostics" in driver_source
