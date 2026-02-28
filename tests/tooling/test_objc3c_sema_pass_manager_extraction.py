from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
PASS_MANAGER_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.h"
PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_pass_manager_contract_exposes_pass_order_and_diagnostics_bus() -> None:
    contract = _read(PASS_MANAGER_CONTRACT)
    assert "kObjc3SemaPassManagerContractVersionMajor" in contract
    assert "enum class Objc3SemaPassId {" in contract
    assert "BuildIntegrationSurface" in contract
    assert "ValidateBodies" in contract
    assert "ValidatePureContract" in contract
    assert "kObjc3SemaPassOrder" in contract
    assert "IsMonotonicObjc3SemaDiagnosticsAfterPass(" in contract
    assert "struct Objc3SemaDiagnosticsBus {" in contract
    assert "PublishBatch(const std::vector<std::string> &batch) const" in contract
    assert "std::size_t Count() const" in contract
    assert "std::vector<std::string> diagnostics;" in contract
    assert "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};" in contract
    assert "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;" in contract
    assert "bool deterministic_type_metadata_handoff = false;" in contract


def test_pass_manager_module_exists_and_orchestrates_semantic_passes() -> None:
    header = _read(PASS_MANAGER_HEADER)
    source = _read(PASS_MANAGER_SOURCE)
    assert "RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input);" in header
    assert "RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input)" in source
    assert "BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);" in source
    assert "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);" in source
    assert "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);" in source
    assert "result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());" in source
    assert "input.diagnostics_bus.PublishBatch(pass_diagnostics);" in source
    assert "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();" in source
    assert "result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();" in source
    assert "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);" in source
    assert "result.deterministic_type_metadata_handoff =" in source


def test_pipeline_uses_pass_manager_and_diagnostics_bus() -> None:
    pipeline = _read(PIPELINE_SOURCE)
    assert '#include "sema/objc3_sema_pass_manager.h"' in pipeline
    assert "Objc3SemaPassManagerInput sema_input;" in pipeline
    assert "sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;" in pipeline
    assert "RunObjc3SemaPassManager(sema_input)" in pipeline
    assert "BuildSemanticIntegrationSurface(result.program, result.stage_diagnostics.semantic);" not in pipeline
    assert "ValidateSemanticBodies(result.program, result.integration_surface, semantic_options," not in pipeline
    assert "ValidatePureContractSemanticDiagnostics(result.program, result.integration_surface.functions," not in pipeline


def test_build_surfaces_register_pass_manager_source() -> None:
    cmake = _read(CMAKE_FILE)
    build_script = _read(BUILD_SCRIPT)
    assert "src/sema/objc3_sema_pass_manager.cpp" in cmake
    assert '"native/objc3c/src/sema/objc3_sema_pass_manager.cpp"' in build_script
