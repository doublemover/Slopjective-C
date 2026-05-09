from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
PASS_MANAGER_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.h"
PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SEMA_STAGE_RUNNER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_sema_stage_runner.cpp"
SEMA_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "sema" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def _read(path: Path, seen: set[Path] | None = None) -> str:
    if seen is None:
        seen = set()
    if path in seen:
        return ""
    seen.add(path)
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
            continue
        include_path = stripped.split('"', 2)[1]
        target = ROOT / "native" / "objc3c" / "src" / include_path
        if target.exists():
            expanded.append(_read(target, seen))
    return "\n".join(expanded)


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_pass_manager_contract_exposes_pass_order_and_diagnostics_bus() -> None:
    contract = _read(PASS_MANAGER_CONTRACT)
    assert "kObjc3SemaPassManagerContractVersionMajor" in contract
    assert "enum class Objc3SemaPassId {" in contract
    assert "enum class Objc3SemaLanguageProfile : std::uint8_t {" in contract
    assert "Legacy = 1" not in contract
    assert "struct Objc3SemaCanonicalLiteralRejectionCounts {" in contract
    assert "BuildIntegrationSurface" in contract
    assert "ValidateBodies" in contract
    assert "ValidatePureContract" in contract
    assert "kObjc3SemaPassOrder" in contract
    assert "IsMonotonicObjc3SemaDiagnosticsAfterPass(" in contract
    assert "struct Objc3SemaDiagnosticsBus {" in contract
    assert "PublishBatch(const std::vector<std::string> &batch) const" in contract
    assert "std::size_t Count() const" in contract
    assert "Objc3SemaLanguageProfile language_profile = Objc3SemaLanguageProfile::Canonical;" in contract
    assert "Objc3SemaCanonicalLiteralRejectionCounts canonical_literal_rejection_counts;" in contract
    assert "std::vector<std::string> diagnostics;" in contract
    assert "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};" in contract
    assert "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;" in contract
    assert "bool deterministic_semantic_diagnostics = false;" in contract
    assert "bool deterministic_type_metadata_handoff = false;" in contract
    assert "struct Objc3SemaParityContractSurface {" in contract
    assert "bool IsReadyObjc3SemaParityContractSurface(" in contract
    assert "Objc3SemaParityContractSurface parity_surface;" in contract

    _assert_in_order(
        contract,
        [
            "Objc3SemaPassId::BuildIntegrationSurface,",
            "Objc3SemaPassId::ValidateBodies,",
            "Objc3SemaPassId::ValidatePureContract,",
        ],
    )

    _assert_in_order(
        contract,
        [
            "std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};",
            "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};",
            "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;\n  bool deterministic_semantic_diagnostics = false;",
            "Objc3SemaParityContractSurface parity_surface;",
        ],
    )


def test_pass_manager_module_exists_and_orchestrates_semantic_passes() -> None:
    header = _read(PASS_MANAGER_HEADER)
    source = _read(PASS_MANAGER_SOURCE)
    assert "RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input);" in header
    assert "RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input)" in source
    assert "BuildSemanticIntegrationSurface(" in source
    assert "*input.program," in source
    assert "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);" in source
    assert "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);" in source
    assert "AppendMigrationAssistDiagnostics" not in source
    assert "O3S216" not in source
    assert "result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());" in source
    assert "input.diagnostics_bus.PublishBatch(pass_diagnostics);" in source
    assert "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();" in source
    assert "result.diagnostics_emitted_by_pass[pass_index] = pass_diagnostics.size();" in source
    assert "CanonicalizePassDiagnostics(pass_diagnostics);" in source
    assert "result.deterministic_semantic_diagnostics =" in source
    assert "deterministic_semantic_diagnostics &&" in source
    assert "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);" in source
    assert "result.deterministic_type_metadata_handoff =" in source
    assert "result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;" in source
    assert "result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;" in source
    assert "result.parity_surface.diagnostics_after_pass_monotonic =" in source
    assert "result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;" in source
    assert "result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;" in source
    assert "result.parity_surface.ready =" in source

    _assert_in_order(
        source,
        [
            "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {",
            "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
            "CanonicalizePassDiagnostics(pass_diagnostics);",
            "result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());",
            "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
            "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();",
            "result.diagnostics_emitted_by_pass[pass_index] = pass_diagnostics.size();",
            "result.deterministic_semantic_diagnostics =",
            "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
            "result.deterministic_type_metadata_handoff =",
            "result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;",
            "result.parity_surface.diagnostics_after_pass_monotonic =",
            "result.parity_surface.ready =",
        ],
    )

def test_pipeline_uses_pass_manager_and_diagnostics_bus() -> None:
    pipeline = _read(PIPELINE_SEMA_STAGE_RUNNER)
    assert '#include "sema/objc3_sema_pass_manager.h"' in pipeline
    assert "Objc3SemaPassManagerInput sema_input;" in pipeline
    assert "sema_input.language_profile = Objc3SemaLanguageProfile::Canonical;" in pipeline
    assert (
        "sema_input.canonical_literal_rejection_counts.yes_literal_sites ="
        in pipeline
    )
    assert "result.canonical_literal_rejection_counts.yes_literal_sites;" in pipeline
    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline
    assert "RunObjc3SemaPassManager(sema_input)" in pipeline
    assert "BuildSemanticIntegrationSurface(result.program, result.stage_diagnostics.semantic);" not in pipeline
    assert "ValidateSemanticBodies(result.program, result.integration_surface, semantic_options," not in pipeline
    assert "ValidatePureContractSemanticDiagnostics(result.program, result.integration_surface.functions," not in pipeline


def test_build_surfaces_register_pass_manager_source() -> None:
    cmake = _read(SEMA_CMAKE_FILE)
    build_script = _read(BUILD_SCRIPT)
    assert "objc3_sema_pass_manager.cpp" in cmake
    assert "objc3c_diag" in cmake
    assert "add_library(objc3c_sema_type_system INTERFACE)" in cmake
    assert "target_link_libraries(objc3c_sema_type_system INTERFACE" in cmake
    assert "objc3c_sema_type_system" in cmake

    _assert_in_order(
        cmake,
        [
            "add_library(objc3c_sema STATIC",
            "objc3_sema_pass_manager_contract_flow.h",
            "objc3_sema_pass_manager.cpp",
            "target_link_libraries(objc3c_sema PUBLIC",
            "add_library(objc3c_sema_type_system INTERFACE)",
            "target_link_libraries(objc3c_sema_type_system INTERFACE",
        ],
    )

    assert '"native/objc3c/src/sema/objc3_sema_pass_manager.cpp"' in build_script
