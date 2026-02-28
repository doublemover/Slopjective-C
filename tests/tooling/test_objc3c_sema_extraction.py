from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PM_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.h"
SEMA_PM_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_sema_module_exists_and_pipeline_uses_api() -> None:
    assert SEMA_CONTRACT.exists()
    assert SEMA_HEADER.exists()
    assert SEMA_SOURCE.exists()
    assert SEMA_PM_HEADER.exists()
    assert SEMA_PM_SOURCE.exists()

    pipeline_cpp = _read(PIPELINE_CPP)
    assert '#include "sema/objc3_sema_pass_manager.h"' in pipeline_cpp
    assert "RunObjc3SemaPassManager(sema_input)" in pipeline_cpp


def test_pipeline_uses_sema_contract_types() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    assert '#include "sema/objc3_sema_contract.h"' in pipeline_types
    assert '#include "sema/objc3_semantic_passes.h"' not in pipeline_types
    assert "struct FunctionInfo" not in pipeline_types
    assert "struct Objc3SemanticIntegrationSurface" not in pipeline_types


def test_sema_contract_exports_explicit_type_metadata_handoff_surface() -> None:
    contract = _read(SEMA_CONTRACT)
    sema_header = _read(SEMA_HEADER)
    sema_source = _read(SEMA_SOURCE)

    assert "kObjc3SemaBoundaryContractVersionMajor" in contract
    assert "struct Objc3SemanticFunctionTypeMetadata {" in contract
    assert "struct Objc3SemanticTypeMetadataHandoff {" in contract
    assert "std::vector<std::string> global_names_lexicographic;" in contract
    assert "std::vector<Objc3SemanticFunctionTypeMetadata> functions_lexicographic;" in contract
    assert "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(" in contract
    assert "bool IsDeterministicSemanticTypeMetadataHandoff(" in contract

    assert "BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);" in sema_header
    assert "IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);" in sema_header

    assert "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(" in sema_source
    assert "std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());" in sema_source
    assert "std::sort(function_names.begin(), function_names.end());" in sema_source
    assert "metadata.param_types.size() == metadata.arity" in sema_source
    assert "metadata.param_has_invalid_type_suffix.size() == metadata.arity" in sema_source


def test_cmake_registers_sema_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_sema STATIC" in cmake
    assert "src/sema/objc3_sema_pass_manager.cpp" in cmake
    assert "src/sema/objc3_semantic_passes.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_sema" in cmake
