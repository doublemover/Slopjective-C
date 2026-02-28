from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"
PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_sema_module_exists_and_main_uses_api() -> None:
    assert SEMA_HEADER.exists()
    assert SEMA_SOURCE.exists()

    main_cpp = _read(MAIN_CPP)
    assert '#include "sema/objc3_semantic_passes.h"' in main_cpp
    assert "static Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(" not in main_cpp
    assert "static void ValidateSemanticBodies(" not in main_cpp
    assert "static void ValidatePureContractSemanticDiagnostics(" not in main_cpp
    assert "ValidateSemanticBodies(result.program, result.integration_surface, semantic_options," in main_cpp


def test_pipeline_uses_sema_contract_types() -> None:
    pipeline_types = _read(PIPELINE_TYPES)
    assert '#include "sema/objc3_semantic_passes.h"' in pipeline_types
    assert "struct FunctionInfo" not in pipeline_types
    assert "struct Objc3SemanticIntegrationSurface" not in pipeline_types


def test_cmake_registers_sema_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_sema STATIC" in cmake
    assert "src/sema/objc3_semantic_passes.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_sema" in cmake
